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
#ifndef TI_HDMI_AUDIO_HAL
#define TI_HDMI_AUDIO_HAL

typedef struct _hdmi_audio_caps {
    int has_audio;
    int speaker_alloc;
} hdmi_audio_caps_t;

/* Speaker allocation bits */
#define CEA_SPKR_FLFR   (1 << 0)
#define CEA_SPKR_LFE    (1 << 1)
#define CEA_SPKR_FC     (1 << 2)
#define CEA_SPKR_RLRR   (1 << 3)
#define CEA_SPKR_RC     (1 << 4)
#define CEA_SPKR_FLCFRC (1 << 5)
#define CEA_SPKR_RLCRRC (1 << 6)

/* Defined in file hdmi_audio_utils.c */
int hdmi_query_audio_caps(const char* edid_path, hdmi_audio_caps_t *caps);

#endif /* TI_HDMI_AUDIO_HAL */
