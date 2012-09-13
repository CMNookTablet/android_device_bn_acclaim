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

#define LOG_TAG "hdmi_audio_hw"
/* #define LOG_NDEBUG 0 */
/* #define LOG_TRACE_FUNCTION */

#ifndef LOG_TRACE_FUNCTION
#define TRACE() ((void)0)
#define TRACEM(fmt, ...) ((void)0)
#else
#define tfmt(x) x
#define TRACE() (ALOGV("%s() %s:%d", __func__, __FILE__, __LINE__))
#define TRACEM(fmt, ...) (ALOGV("%s() " tfmt(fmt) " %s:%d", __func__, ##__VA_ARGS__, __FILE__, __LINE__))
#endif

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>

#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <cutils/properties.h>

#include <hardware/hardware.h>
#include <system/audio.h>
#include <hardware/audio.h>

#include <tinyalsa/asoundlib.h>

#include "hdmi_audio_hal.h"

#define UNUSED(x) (void)(x)

/* XXX TODO: Dynamically detect the HDMI card
 * E.g. if a USB device is plugged in at boot time,
 * it sometimes takes the card #1 slot and puts us
 * on card #2.
 */
#define HDMI_PCM_DEV 0
#define HDMI_SAMPLING_RATE 44100
#define HDMI_PERIOD_SIZE 1920
#define HDMI_PERIOD_COUNT 4

#define HDMI_EDID_PATH "/sys/devices/omapdss/display1/edid"

typedef audio_hw_device_t hdmi_device_t;

typedef struct _hdmi_out {
    audio_stream_out_t stream_out;
    hdmi_device_t *dev;
    struct pcm_config config;
    struct pcm *pcm;
    audio_config_t android_config;
    int up;
} hdmi_out_t;

#define S16_SIZE sizeof(int16_t)

/*****************************************************************
 * UTILITY FUNCTIONS
 *****************************************************************
 */

/*****************************************************************
 * AUDIO STREAM OUT (hdmi_out_*) DEFINITION
 *****************************************************************
 */

uint32_t hdmi_out_get_sample_rate(const struct audio_stream *stream)
{
    hdmi_out_t *out = (hdmi_out_t*)stream;
    struct pcm_config *config = &out->config;
    TRACEM("stream=%p returning %d", stream, config->rate);
    return config->rate;
}

/* DEPRECATED API */
int hdmi_out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    TRACE();
    return -EINVAL;
}

/* Returns bytes for ONE PERIOD */
size_t hdmi_out_get_buffer_size(const struct audio_stream *stream)
{
    hdmi_out_t *out = (hdmi_out_t*)stream;
    struct pcm_config *config = &out->config;
    size_t ans;

    ans = audio_stream_frame_size((struct audio_stream*)stream) * config->period_size;

    TRACEM("stream=%p returning %u", stream, ans);

    return ans;
}

audio_channel_mask_t hdmi_out_get_channels(const struct audio_stream *stream)
{
    hdmi_out_t *out = (hdmi_out_t*)stream;
    TRACEM("stream=%p returning %x", stream, out->android_config.channel_mask);
    return out->android_config.channel_mask;
}

audio_format_t hdmi_out_get_format(const struct audio_stream *stream)
{
    hdmi_out_t *out = (hdmi_out_t*)stream;
    TRACEM("stream=%p returning %x", stream, out->android_config.format);
    return out->android_config.format;
}

/* DEPRECATED API */
int hdmi_out_set_format(struct audio_stream *stream, audio_format_t format)
{
    TRACE();
    return -EINVAL;
}

int hdmi_out_standby(struct audio_stream *stream)
{
    hdmi_out_t *out = (hdmi_out_t*)stream;

    TRACEM("stream=%p", stream);

    if (out->up && out->pcm) {
        out->up = 0;
        pcm_close(out->pcm);
        out->pcm = 0;
    }

    return 0;
}

int hdmi_out_dump(const struct audio_stream *stream, int fd)
{
    TRACE();
    return 0;
}

audio_devices_t hdmi_out_get_device(const struct audio_stream *stream)
{
    TRACEM("stream=%p", stream);
    return AUDIO_DEVICE_OUT_AUX_DIGITAL;
}

/* DEPRECATED API */
int hdmi_out_set_device(struct audio_stream *stream, audio_devices_t device)
{
    TRACE();
    return 0;
}

int hdmi_out_set_parameters(struct audio_stream *stream, const char *kv_pairs)
{
    TRACEM("stream=%p kv_pairs='%s'", stream, kv_pairs);
    return 0;
}

#define MASK_CEA_QUAD     ( CEA_SPKR_FLFR | CEA_SPKR_RLRR )
#define MASK_CEA_SURROUND ( CEA_SPKR_FLFR | CEA_SPKR_FC | CEA_SPKR_RC )
#define MASK_CEA_5POINT1  ( CEA_SPKR_FLFR | CEA_SPKR_FC | CEA_SPKR_LFE | CEA_SPKR_RLRR )
#define MASK_CEA_7POINT1  ( CEA_SPKR_FLFR | CEA_SPKR_FC | CEA_SPKR_LFE | CEA_SPKR_RLRR | CEA_SPKR_RLCRRC )
#define SUPPORTS_ARR(spkalloc, profile) (((spkalloc) & (profile)) == (profile))

char * hdmi_out_get_parameters(const struct audio_stream *stream,
			 const char *keys)
{
    struct str_parms *query = str_parms_create_str(keys);
    char *str;
    char value[256];
    struct str_parms *reply = str_parms_create();
    int status;
    hdmi_audio_caps_t caps;

    TRACEM("stream=%p keys='%s'", stream, keys);

    if (hdmi_query_audio_caps(HDMI_EDID_PATH, &caps)) {
        ALOGE("Unable to get the HDMI audio capabilities");
        str = calloc(1, 1);
        goto end;
    }

    status = str_parms_get_str(query, AUDIO_PARAMETER_STREAM_SUP_CHANNELS,
                                value, sizeof(value));
    if (status >= 0) {
        unsigned sa = caps.speaker_alloc;
        bool first = true;

        /* STEREO is intentionally skipped.  This code is only
         * executed for the 'DIRECT' interface, and we don't
         * want stereo on a DIRECT thread.
         */
        value[0] = '\0';
        if (SUPPORTS_ARR(sa, MASK_CEA_QUAD)) {
            if (!first) {
                strcat(value, "|");
            }
            first = false;
            strcat(value, "AUDIO_CHANNEL_OUT_QUAD");
        }
        if (SUPPORTS_ARR(sa, MASK_CEA_SURROUND)) {
            if (!first) {
                strcat(value, "|");
            }
            first = false;
            strcat(value, "AUDIO_CHANNEL_OUT_SURROUND");
        }
        if (SUPPORTS_ARR(sa, MASK_CEA_5POINT1)) {
            if (!first) {
                strcat(value, "|");
            }
            first = false;
            strcat(value, "AUDIO_CHANNEL_OUT_5POINT1");
        }
        if (SUPPORTS_ARR(sa, MASK_CEA_7POINT1)) {
            if (!first) {
                strcat(value, "|");
            }
            first = false;
            strcat(value, "AUDIO_CHANNEL_OUT_7POINT1");
        }
        str_parms_add_str(reply, AUDIO_PARAMETER_STREAM_SUP_CHANNELS, value);
        str = strdup(str_parms_to_str(reply));
    } else {
        str = strdup(keys);
    }

    ALOGV("%s() reply: '%s'", __func__, str);

end:
    str_parms_destroy(query);
    str_parms_destroy(reply);
    return str;
}
int hdmi_out_add_audio_effect(const struct audio_stream *stream,
			effect_handle_t effect)
{
    TRACE();
    return 0;
}
int hdmi_out_remove_audio_effect(const struct audio_stream *stream,
			   effect_handle_t effect)
{
    TRACE();
    return 0;
}

/* returns milliseconds */
uint32_t hdmi_out_get_latency(const struct audio_stream_out *stream)
{
    uint32_t latency;
    hdmi_out_t *out = (hdmi_out_t*)stream;
    struct pcm_config *config = &out->config;

    TRACEM("stream=%p", stream);

    return (1000 * config->period_size * config->period_count) / config->rate;
}

int hdmi_out_set_volume(struct audio_stream_out *stream, float left, float right)
{
    TRACE();
    return -ENOSYS;
}

static int hdmi_out_find_card(void)
{
    /* XXX TODO: Dynamically detect HDMI card
     * If another audio device is present at boot time (e.g.
     * USB Audio device) then it might take the card #1
     * position, putting HDMI on card #2.
     */
    return 1;
}

static int hdmi_out_open_pcm(hdmi_out_t *out)
{
    int card = hdmi_out_find_card();
    int dev = HDMI_PCM_DEV;
    int ret;

    TRACEM("out=%p", out);

    /* out->up must be 0 (down) */
    if (out->up) {
        ALOGE("Trying to open a PCM that's already up. "
             "This will probably deadlock... so aborting");
        return 0;
    }

    out->pcm = pcm_open(card, dev, PCM_OUT, &out->config);

    if(out->pcm && pcm_is_ready(out->pcm)) {
        out->up = 1;
        ret = 0;
    } else {
        ALOGE("cannot open HDMI pcm card %d dev %d error: %s",
              card, dev, pcm_get_error(out->pcm));
        pcm_close(out->pcm);
        out->pcm = 0;
        out->up = 0;
        ret = 1;
    }

    return ret;
}

ssize_t hdmi_out_write(struct audio_stream_out *stream, const void* buffer,
		 size_t bytes)
{
    hdmi_out_t *out = (hdmi_out_t*)stream;
    ssize_t ret;

    TRACEM("stream=%p buffer=%p bytes=%d", stream, buffer, bytes);

    if (!out->up) {
        if(hdmi_out_open_pcm(out)) {
            return -ENOSYS;
        }
    }

    ret = pcm_write(out->pcm, buffer, bytes);
    if (ret) {
        ALOGE("Error writing to HDMI pcm: %s", pcm_get_error(out->pcm));
        ret = (ret < 0) ? ret : -ret;
        hdmi_out_standby((struct audio_stream*)stream);
    } else {
        ret = bytes;
    }

    return ret;
}

int hdmi_out_get_render_position(const struct audio_stream_out *stream,
			   uint32_t *dsp_frames)
{
    TRACE();
    return -EINVAL;
}

int hdmi_out_get_next_write_timestamp(const struct audio_stream_out *stream,
				int64_t *timestamp)
{
    TRACE();
    return -EINVAL;
}



audio_stream_out_t hdmi_stream_out_descriptor = {
    .common = {
        .get_sample_rate = hdmi_out_get_sample_rate,
        .set_sample_rate = hdmi_out_set_sample_rate,
        .get_buffer_size = hdmi_out_get_buffer_size,
        .get_channels = hdmi_out_get_channels,
        .get_format = hdmi_out_get_format,
        .set_format = hdmi_out_set_format,
        .standby = hdmi_out_standby,
        .dump = hdmi_out_dump,
        .get_device = hdmi_out_get_device,
        .set_device = hdmi_out_set_device,
        .set_parameters = hdmi_out_set_parameters,
        .get_parameters = hdmi_out_get_parameters,
        .add_audio_effect = hdmi_out_add_audio_effect,
        .remove_audio_effect = hdmi_out_remove_audio_effect,
    },
    .get_latency = hdmi_out_get_latency,
    .set_volume = hdmi_out_set_volume,
    .write = hdmi_out_write,
    .get_render_position = hdmi_out_get_render_position,
    .get_next_write_timestamp = hdmi_out_get_next_write_timestamp,
};

/*****************************************************************
 * AUDIO DEVICE (hdmi_adev_*) DEFINITION
 *****************************************************************
 */

static int hdmi_adev_close(struct hw_device_t *device)
{
    TRACE();
    return 0;
}

static uint32_t hdmi_adev_get_supported_devices(const audio_hw_device_t *dev)
{
    TRACE();
    return AUDIO_DEVICE_OUT_AUX_DIGITAL;
}

static int hdmi_adev_init_check(const audio_hw_device_t *dev)
{
    TRACE();
    return 0;
}

static int hdmi_adev_set_voice_volume(audio_hw_device_t *dev, float volume)
{
    TRACE();
    return -ENOSYS;
}

static int hdmi_adev_set_master_volume(audio_hw_device_t *dev, float volume)
{
    TRACE();
    return -ENOSYS;
}

static int hdmi_adev_get_master_volume(audio_hw_device_t *dev, float *volume)
{
    TRACE();
    return -ENOSYS;
}

static int hdmi_adev_set_mode(audio_hw_device_t *dev, audio_mode_t mode)
{
    TRACE();
    return 0;
}

static int hdmi_adev_set_mic_mute(audio_hw_device_t *dev, bool state)
{
    TRACE();
    return -ENOSYS;
}

static int hdmi_adev_get_mic_mute(const audio_hw_device_t *dev, bool *state)
{
    TRACE();
    return -ENOSYS;
}

static int hdmi_adev_set_parameters(audio_hw_device_t *dev, const char *kv_pairs)
{
    TRACEM("dev=%p kv_pairss='%s'", dev, kv_pairs);
    return 0;
}

static char* hdmi_adev_get_parameters(const audio_hw_device_t *dev,
                                      const char *keys)
{
    TRACEM("dev=%p keys='%s'", dev, keys);
    return NULL;
}

static size_t hdmi_adev_get_input_buffer_size(const audio_hw_device_t *dev,
                                              const struct audio_config *config)
{
    TRACE();
    return 0;
}

#define DUMP_FLAG(flags, flag) {                \
        if ((flags) & (flag)) {                 \
            ALOGV("set:   " #flag);             \
        } else {                                \
            ALOGV("unset: " #flag);             \
        }                                       \
    }

static int hdmi_adev_open_output_stream(audio_hw_device_t *dev,
                                        audio_io_handle_t handle,
                                        audio_devices_t devices,
                                        audio_output_flags_t flags,
                                        struct audio_config *config,
                                        struct audio_stream_out **stream_out)
{
    hdmi_out_t *out = 0;
    struct pcm_config *pcm_config = 0;
    struct audio_config *a_config = 0;

    TRACE();

    out = calloc(1, sizeof(hdmi_out_t));
    if (!out) {
        return -ENOMEM;
    }

    out->dev = dev;
    memcpy(&out->stream_out, &hdmi_stream_out_descriptor,
           sizeof(audio_stream_out_t));
    memcpy(&out->android_config, config, sizeof(audio_config_t));

    pcm_config = &out->config;
    a_config = &out->android_config;

#if defined(LOG_NDEBUG) && (LOG_NDEBUG == 0)
    /* Analyze flags */
    if (flags) {
        DUMP_FLAG(flags, AUDIO_OUTPUT_FLAG_DIRECT)
        DUMP_FLAG(flags, AUDIO_OUTPUT_FLAG_PRIMARY)
        DUMP_FLAG(flags, AUDIO_OUTPUT_FLAG_FAST)
        DUMP_FLAG(flags, AUDIO_OUTPUT_FLAG_DEEP_BUFFER)
    } else {
        ALOGV("flags == AUDIO_OUTPUT_FLAG_NONE (0)");
    }
#endif /* defined(LOG_NDEBUG) && (LOG_NDEBUG == 0) */
    /* Initialize the PCM Configuration */
    pcm_config->period_size = HDMI_PERIOD_SIZE;
    pcm_config->period_count = HDMI_PERIOD_COUNT;

    if (a_config->sample_rate) {
        pcm_config->rate = config->sample_rate;
    } else {
        pcm_config->rate = HDMI_SAMPLING_RATE;
        a_config->sample_rate = HDMI_SAMPLING_RATE;
    }

    switch (a_config->format) {
    case AUDIO_FORMAT_DEFAULT:
        a_config->format = AUDIO_FORMAT_PCM_16_BIT;
        /* fall through */
    case AUDIO_FORMAT_PCM_16_BIT:
        pcm_config->format = PCM_FORMAT_S16_LE;
        break;
    default:
        ALOGE("HDMI rejecting format %x", config->format);
        goto fail;
    }

    a_config->channel_mask = config->channel_mask;
    switch (config->channel_mask) {
    case AUDIO_CHANNEL_OUT_STEREO:
        pcm_config->channels = 2;
        break;
    case AUDIO_CHANNEL_OUT_QUAD:
    case AUDIO_CHANNEL_OUT_SURROUND:
        pcm_config->channels = 4;
        break;
    case AUDIO_CHANNEL_OUT_5POINT1:
        pcm_config->channels = 6;
        break;
    case AUDIO_CHANNEL_OUT_7POINT1:
        pcm_config->channels = 8;
        break;
    default:
        ALOGE("HDMI setting a default channel_mask %x -> 8", config->channel_mask);
        config->channel_mask = AUDIO_CHANNEL_OUT_7POINT1;
        a_config->channel_mask = AUDIO_CHANNEL_OUT_7POINT1;
        pcm_config->channels = 8;
    }

    ALOGV("stream = %p", out);
    *stream_out = &out->stream_out;

    return 0;

fail:
    free(out);
    return -ENOSYS;
}

static void hdmi_adev_close_output_stream(audio_hw_device_t *dev,
                                          struct audio_stream_out* stream_out)
{
    TRACEM("dev=%p stream_out=%p", dev, stream_out);
    stream_out->common.standby((audio_stream_t*)stream_out);
    free(stream_out);
}

static int hdmi_adev_open_input_stream(audio_hw_device_t *dev,
                                       audio_io_handle_t handle,
                                       audio_devices_t devices,
                                       struct audio_config *config,
                                       struct audio_stream_in **stream_in)
{
    TRACE();
    return -ENOSYS;
}

static void hdmi_adev_close_input_stream(audio_hw_device_t *dev,
                                         struct audio_stream_in *stream_in)
{
    TRACE();
}

static int hdmi_adev_dump(const audio_hw_device_t *dev, int fd)
{
    TRACE();
    return 0;
}

static audio_hw_device_t hdmi_adev_descriptor = {
    .common = {
        .tag = HARDWARE_DEVICE_TAG,
        .version = AUDIO_DEVICE_API_VERSION_CURRENT,
        .module = NULL,
        .close = hdmi_adev_close,
    },
    .get_supported_devices = hdmi_adev_get_supported_devices,
    .init_check = hdmi_adev_init_check,
    .set_voice_volume = hdmi_adev_set_voice_volume,
    .set_master_volume = hdmi_adev_set_master_volume,
    .get_master_volume = hdmi_adev_get_master_volume,
    .set_mode = hdmi_adev_set_mode,
    .set_mic_mute = hdmi_adev_set_mic_mute,
    .get_mic_mute = hdmi_adev_get_mic_mute,
    .set_parameters = hdmi_adev_set_parameters,
    .get_parameters = hdmi_adev_get_parameters,
    .get_input_buffer_size = hdmi_adev_get_input_buffer_size,
    .open_output_stream = hdmi_adev_open_output_stream,
    .close_output_stream = hdmi_adev_close_output_stream,
    .open_input_stream =  hdmi_adev_open_input_stream,
    .close_input_stream = hdmi_adev_close_input_stream,
    .dump = hdmi_adev_dump,
};

static int hdmi_adev_open(const hw_module_t* module,
                          const char* name,
                          hw_device_t** device)
{
    TRACE();

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    hdmi_adev_descriptor.common.module = (struct hw_module_t *) module;
    *device = &hdmi_adev_descriptor.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = hdmi_adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "OMAP HDMI audio HW HAL",
        .author = "Texas Instruments",
        .methods = &hal_module_methods,
    },
};
