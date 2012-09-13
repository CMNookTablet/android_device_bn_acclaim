/* -*- mode: C; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2012 Texas Instruments
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "hdmi_audio_caps"
/* #define LOG_NDEBUG 0 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "hdmi_audio_hal.h"

/*
 * This program can be compiled standalone for debugging and
 * testing by defining HDMI_CAPS_STANDALONE
 */

#ifdef HDMI_CAPS_STANDALONE
#include <stdio.h>

#define lfmt(x) x
#define ALOGV(fmt, ...) printf(lfmt(fmt) "\n", ##__VA_ARGS__)
#define ALOGW(fmt, ...) printf(lfmt(fmt) "\n", ##__VA_ARGS__)
#define ALOGE(fmt, ...) printf(lfmt(fmt) "\n", ##__VA_ARGS__)
#define ALOGV_IF(cond, fmt, ...) { if (cond) { printf(lfmt(fmt) "\n", ##__VA_ARGS__); } }
#define ALOGE_IF(cond, fmt, ...) { if (cond) { printf(lfmt(fmt) "\n", ##__VA_ARGS__); } }
#else /* HDMI_CAPS_STANDALONE */

#include <cutils/log.h>

#endif /* HDMI_CAPS_STANDALONE */

/*****************************************************************
 * HDMI AUDIO CAPABILITIES (EDID PARSING)
 *
 * To maintain this section, you need these references:
 *
 *   - High-Definition Multimedia Interface Specification Version 1.4a
 *     Mar 4, 2010, Hitachi et al. (See Section 8.3 "E-EDID Data
 *     Structure")
 *
 *   - E-EDID, VESA Enhanced Extended Display Identification Data
 *     Standard, Release A, Revision 2, September 25. 2006,
 *     Video Electronics Standards Association (VESA)
 *
 *   - CEA-861-D, A DTV Profile for Uncompressed High Speed Digital
 *     Interfaces, July 2006 (Section 7.5)
 *
 *
 * Summary: The E-EDID is composed of 128-byte blocks.  The first byte
 * identifies the block.  HDMI devices are required to send a CEA
 * Extension block (version 3), and the first one is the one we /must/
 * parse.  (You can probably go far with just this info and the
 * CEA-861-D spec.)
 *
 *****************************************************************
 */

/* TODO: Figure this out dynamically, but ATM this is enforced
 * in the kernel.
 */
#define HDMI_MAX_EDID 512

#define EDID_BLOCK_SIZE       128
#define EDID_BLOCK_MAP_ID     0xF0
#define EDID_BLOCK_CEA_ID     0x02
#define EDID_BLOCK_CEA_REV_3  0x03

#define CEA_TAG_MASK  0xE0
#define CEA_SIZE_MASK 0x1F
#define CEA_TAG_AUDIO (1 << 5)
#define CEA_TAG_VIDEO (2 << 5)
#define CEA_TAG_SPKRS (4 << 5)
#define CEA_BIT_AUDIO (1 << 6)

static void hdmi_dump_short_audio_descriptor_block(unsigned char *mem)
{
    const unsigned char FORMAT_MASK = 0x78;
    const unsigned char MAX_CH_MASK = 0x07;
    int n, size;
    unsigned char *p, *end, byte, format, chs;
    const char* formats[] = {
        "Reserved (0)",
        "LPCM",
        "AC-3",
        "MPEG1 (Layers 1 & 2)",
        "MP3 (MPEG1 Layer 3)",
        "MPEG2 (multichannel)",
        "AAC",
        "DTS",
        "ATRAC",
        "One Bit Audio",
        "Dolby Digital +",
        "DTS-HD",
        "MAT (MLP)",
        "DST",
        "WMA Pro",
        "Reserved (15)",
    };

    size = *mem & CEA_SIZE_MASK;
    end = mem + 1 + size;

    for (p = mem + 1 ; p < end ; p += 3) {
        byte = p[0];
        format = (byte & FORMAT_MASK) >> 3;
        chs = byte & MAX_CH_MASK;

        ALOGV("Parsing Short Audio Descriptor:");
        ALOGV("  format: %s", formats[format]);
        ALOGV("  max channels: %d", chs + 1);
        ALOGV("  sample rates:");

        byte = p[1];
        ALOGV_IF(byte & (1 << 0), "    32.0 kHz");
        ALOGV_IF(byte & (1 << 1), "    44.1 kHz");
        ALOGV_IF(byte & (1 << 2), "    48.0 kHz");
        ALOGV_IF(byte & (1 << 3), "    88.2 kHz");
        ALOGV_IF(byte & (1 << 4), "    96.0 kHz");
        ALOGV_IF(byte & (1 << 5), "   176.4 kHz");
        ALOGV_IF(byte & (1 << 6), "   192.0 kHz");

        byte = p[2];
        if ((format >= 2) && (format <= 8)) {
            ALOGV("  max bit rate: %d kHz", ((int)byte) * 8);
        }
    }
}

int hdmi_query_audio_caps(const char* edid_path, hdmi_audio_caps_t *caps)
{
    int fd;
    unsigned char edid[HDMI_MAX_EDID];
    int status;
    int index, n;
    int nblocks;
    int edid_size;

    int has_audio = 0;
    int speaker_alloc = 0;
    int done = 0;

    memset(edid, 0, sizeof(edid));

    fd = open(edid_path, O_RDONLY);
    if (fd == -1) {
        return -errno;
    }

    status = read(fd, edid, sizeof(edid));
    close(fd);
    if (status == -1) {
        ALOGV("Error reading EDID");
        return -errno;
    } else {
        ALOGV("read %d bytes from edid file", status);
    }

    nblocks = edid[0x7E];
    if (edid[EDID_BLOCK_SIZE] == EDID_BLOCK_MAP_ID) {
        /* The block map is just an index... don't need it. */
        ++nblocks;
    }
    ALOGV("EDID contians %d additional extension block(s)", nblocks);
    edid_size = EDID_BLOCK_SIZE * (nblocks + 1);
    if (edid_size > HDMI_MAX_EDID) {
        edid_size = HDMI_MAX_EDID;
    }

    for (index = EDID_BLOCK_SIZE ; index < edid_size ; index += EDID_BLOCK_SIZE) {
        if ((edid[index] == EDID_BLOCK_CEA_ID)
                && (edid[index+1] == EDID_BLOCK_CEA_REV_3)) {
            /* CEA BLOCK */
            unsigned char d;
            unsigned char byte, tag, size;

            /* Parse CEA header for size and audio presence */
            d = edid[index + 2];
            if (edid[index + 3] & CEA_BIT_AUDIO) {
                has_audio = 1;
            } else {
                break;
            }

            n = 4;
            while (n < d) {
                byte = edid[index + n];
                tag = byte & CEA_TAG_MASK;
                size = byte & CEA_SIZE_MASK;
                ++n;

                switch (tag) {
                case CEA_TAG_AUDIO:
                    /* ignore for now, needed for bitstream. */
                    hdmi_dump_short_audio_descriptor_block(&edid[index + n - 1]);
                    break;
                case CEA_TAG_SPKRS: /* I think this fails... not sure why */
                    ALOGE_IF(size != 3, "CEA Speaker Allocation Block is wrong size "
                             "(got %d, expected 3)", size);
                    byte = edid[index + n];
                    speaker_alloc = byte;
                    break;
                }

                n += size;
            }

            /* Per spec, only need to parse the first block */
            break;
        }
    }

    caps->has_audio = has_audio;
    caps->speaker_alloc = speaker_alloc;

    return 0;
}


#ifdef HDMI_CAPS_STANDALONE
int main(int argc, char* argv[])
{
    const char prog_name[] = "hdmi_audio_caps";
    const char *edid_path;
    hdmi_audio_caps_t caps = {
        .has_audio = 0,
    };

    if (argc < 2) {
        printf("usage: %s <edid-file>\n", argc ? argv[0] : prog_name);
        return 0;
    }

    edid_path = argv[1];

    if (hdmi_query_audio_caps(edid_path, &caps)) {
        fprintf(stderr, "Fatal error: could not read EDID (%s)\n",
                strerror(errno));
        return 1;
    }

    printf("\n");
    printf("caps = {\n");
    printf("  .has_audio = %d\n", caps.has_audio);
    printf("  .speaker_alloc = 0x%02x\n", caps.speaker_alloc);
    printf("}\n");

    return 0;
}
#endif /* HDMI_CAPS_STANDALONE */
