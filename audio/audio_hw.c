/*
 * Copyright (C) 2011 The Android Open Source Project
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

#define LOG_TAG "audio_hw_primary"
/*#define LOG_NDEBUG 0*/

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
#include <audio_utils/resampler.h>
#include <audio_utils/echo_reference.h>
#include <hardware/audio_effect.h>
#include <audio_effects/effect_aec.h>

#include "ril_interface.h"

#define F_ALOG ALOGV("%s, line: %d", __FUNCTION__, __LINE__);

/* Mixer control names */
#define MIXER_DL2_LEFT_EQUALIZER            "DL2 Left Equalizer"
#define MIXER_DL2_RIGHT_EQUALIZER           "DL2 Right Equalizer"
#define MIXER_DL1_MEDIA_PLAYBACK_VOLUME     "DL1 Media Playback Volume"
#define MIXER_DL1_VOICE_PLAYBACK_VOLUME     "DL1 Voice Playback Volume"
#define MIXER_DL2_MEDIA_PLAYBACK_VOLUME     "DL2 Media Playback Volume"
#define MIXER_DL2_VOICE_PLAYBACK_VOLUME     "DL2 Voice Playback Volume"
#define MIXER_SDT_DL_VOLUME                 "SDT DL Volume"
#define MIXER_SDT_UL_VOLUME                 "SDT UL Volume"

#define MIXER_DL1_CAPTURE_PLAYBACK_VOLUME   "DL1 Capture Playback Volume"
#define MIXER_DL2_CAPTURE_PLAYBACK_VOLUME   "DL2 Capture Playback Volume"
#define MIXER_HEADSET_PLAYBACK_VOLUME       "Headset Playback Volume"
#define MIXER_HANDSFREE_PLAYBACK_VOLUME     "Handsfree Playback Volume"
#define MIXER_EARPHONE_PLAYBACK_VOLUME      "Earphone Playback Volume"
#define MIXER_BT_UL_VOLUME                  "BT UL Volume"

#define MIXER_DL1_EQUALIZER                 "DL1 Equalizer"
#define MIXER_DL1_MIXER_MULTIMEDIA          "DL1 Mixer Multimedia"
#define MIXER_DL1_MIXER_CAPTURE             "DL1 Mixer Capture"
#define MIXER_DL1_MIXER_VOICE               "DL1 Mixer Voice"
#define MIXER_DL1_MONO_MIXER                "DL1 Mono Mixer"
#define MIXER_DL2_MIXER_MULTIMEDIA          "DL2 Mixer Multimedia"
#define MIXER_DL2_MIXER_VOICE               "DL2 Mixer Voice"
#define MIXER_DL2_MONO_MIXER                "DL2 Mono Mixer"
#define MIXER_SIDETONE_MIXER_PLAYBACK       "Sidetone Mixer Playback"
#define MIXER_SIDETONE_MIXER_CAPTURE        "Sidetone Mixer Capture"
#define MIXER_DL1_PDM_SWITCH                "DL1 PDM Switch"
#define MIXER_DL1_PDM2_SWITCH               "DL1 PDM_DL2 Switch"
#define MIXER_DL1_BT_VX_SWITCH              "DL1 BT_VX Switch"
#define MIXER_DL1_MM_EXT_SWITCH             "DL1 MM_EXT Switch"
#define MIXER_VOICE_CAPTURE_MIXER_CAPTURE   "Voice Capture Mixer Capture"

#define MIXER_HS_LEFT_PLAYBACK              "HS Left Playback"
#define MIXER_HS_RIGHT_PLAYBACK             "HS Right Playback"
#define MIXER_HF_LEFT_PLAYBACK              "HF Left Playback"
#define MIXER_HF_RIGHT_PLAYBACK             "HF Right Playback"
#define MIXER_EARPHONE_ENABLE_SWITCH        "Earphone Enable Switch"
#define MIXER_HS_POWER_MODE                 "Headset Power Mode"
#define MIXER_HS_LOW_POWER_MODE             "Low-Power"

#define MIXER_ANALOG_LEFT_CAPTURE_ROUTE     "Analog Left Capture Route"
#define MIXER_ANALOG_RIGHT_CAPTURE_ROUTE    "Analog Right Capture Route"
#define MIXER_CAPTURE_PREAMPLIFIER_VOLUME   "Capture Preamplifier Volume"
#define MIXER_CAPTURE_VOLUME                "Capture Volume"
#define MIXER_AMIC_UL_VOLUME                "AMIC UL Volume"
#define MIXER_AUDUL_MONO_MIXER              "AUDUL Mono Mixer"
#define MIXER_AUDUL_VOICE_UL_VOLUME         "AUDUL Voice UL Volume"
#define MIXER_DMIC1_UL_VOLUME               "DMIC1 UL Volume"
#define MIXER_DMIC2_UL_VOLUME               "DMIC2 UL Volume"
#define MIXER_DMIC3_UL_VOLUME               "DMIC3 UL Volume"
#define MIXER_MUX_VX0                       "MUX_VX0"
#define MIXER_MUX_VX1                       "MUX_VX1"
#define MIXER_MUX_UL10                      "MUX_UL10"
#define MIXER_MUX_UL11                      "MUX_UL11"

#define MIXER_CAPTURE_MIXER_VOICE_CAPTURE   "Capture Mixer Voice Capture"
#define MIXER_CAPTURE_MIXER_VOICE_PLAYBACK  "Capture Mixer Voice Playback"
#define MIXER_VXREC_VOICE_UL_VOLUME         "VXREC Voice UL Volume"
#define MIXER_VXREC_VOICE_DL_VOLUME         "VXREC Voice DL Volume"

/* Mixer control gain and route values */
#define MIXER_ABE_GAIN_0DB                  120
#define MIXER_PLAYBACK_HS_DAC               "HS DAC"
#define MIXER_PLAYBACK_HF_DAC               "HF DAC"
#define MIXER_MAIN_MIC                      "Main Mic"
#define MIXER_SUB_MIC                       "Sub Mic"
#define MIXER_HS_MIC                        "Headset Mic"
#define MIXER_AMIC0                         "AMic0"
#define MIXER_AMIC1                         "AMic1"
#define MIXER_DMIC0L                        "DMic0L"
#define MIXER_DMIC0R                        "DMic0R"
#define MIXER_BT_LEFT                       "BT Left"
#define MIXER_BT_RIGHT                      "BT Right"
#define MIXER_MMEXTR			    "MMExt Right"
#define MIXER_MMEXTL			    "MMExt Left"
#define MIXER_450HZ_HIGH_PASS		    "450Hz High-pass"
#define MIXER_0DB_HIGH_PASS		    "High-pass 0dB"
#define MIXER_FLAT_RESPONSE		    "Flat Response"
#define MIXER_4KHZ_LPF_0DB		    "4Khz LPF   0dB"
#define MIXER_VX_RIGHT			    "VX Right"
#define MIXER_VX_LEFT			    "VX Left"

/* Codec controls */
#define SP_DRIVER_MUTE 			    "SP driver mute"
#define SP_ANALOG_GAIN			    "SP Analog Gain"
#define DAC_PLAYBACK_VOLUME		    "DAC Playback Volume"
#define RIGHT_DAC_INPUT_SELECTION	    "Right DAC input selection"
#define LEFT_DAC_INPUT_SELECTION	    "Left DAC input selection"
#define DAC_L_TO_LEFT_OUTPUT_MIXER	    "Left Output Mixer From DAC_L"
#define DAC_R_TO_RIGHT_OUTPUT_MIXER	    "Right Output Mixer From DAC_R"
#define HP_DRIVER_MUTE 			    "HP driver mute"
#define HP_ANALOG_GAIN			    "HP Analog Gain"
#define HP_CM_VOLTAGE_CTL		    "HP Output common - mode voltage control"
#define M_INPUT_MIXER			    "M_Input_Mixer CM_PGA_CNTL"
#define P_INPUT_MIXER			    "P_Input_Mixer MIC1LM_PGA_CNTL"
#define MIC_PGA_GAIN			    "ADC MIC_PGA GAIN"
#define ADC_COARSE_GAIN			    "ADC COARSE GAIN"
#define DL1_MM_EXT_SWITCH		    "DL1 MM_EXT Switch"

#define RIGHT_DATA			    "right data"
#define LEFT_DATA			    "left data"
#define VOLTAGE				    "5 V "

/* ALSA cards for OMAP */
#define CARD_OMAP_ABE 	0
#define CARD_OMAP_USB 	2
#define CARD_OMAP_DEFAULT CARD_OMAP_ABE

/* ALSA ports for OMAP */
#define PORT_MM_LP	0
#define PORT_MM 	1
#define PORT_MM2_UL 	2
#define PORT_MM_LEGACY 	3

#define PORT_VX 	0
#define PORT_TONES 	0
#define PORT_VIBRA 	0
#define PORT_MODEM 	0
#define PORT_SPDIF 	0
#define PORT_HDMI 	0

/* constraint imposed by ABE for CBPr mode: all period sizes must be multiples of 24 */
#define ABE_BASE_FRAME_COUNT 24
/* number of base blocks in a short period (low latency) */
#define SHORT_PERIOD_MULTIPLIER 80  /* 40 ms */
/* number of frames per short period (low latency) */
#define SHORT_PERIOD_SIZE (ABE_BASE_FRAME_COUNT * SHORT_PERIOD_MULTIPLIER)
/* number of short periods in a long period (low power) */
#define LONG_PERIOD_MULTIPLIER 1  /* 40 ms */
/* number of frames per long period (low power) */
#define LONG_PERIOD_SIZE (SHORT_PERIOD_SIZE * LONG_PERIOD_MULTIPLIER)
/* number of periods for playback */
#define PLAYBACK_PERIOD_COUNT 4
/* number of periods for capture */
#define CAPTURE_PERIOD_COUNT 2
/* minimum sleep time in out_write() when write threshold is not reached */
#define MIN_WRITE_SLEEP_US 5000

#define RESAMPLER_BUFFER_FRAMES (SHORT_PERIOD_SIZE * 2)
#define RESAMPLER_BUFFER_SIZE (4 * RESAMPLER_BUFFER_FRAMES)

#define DEFAULT_OUT_SAMPLING_RATE 44100

/* sampling rate when using MM low power port */
#define MM_LOW_POWER_SAMPLING_RATE 44100
/* sampling rate when using MM full power port */
#define MM_FULL_POWER_SAMPLING_RATE 48000
/* sampling rate when using VX port for narrow band */
#define VX_NB_SAMPLING_RATE 8000
/* sampling rate when using VX port for wide band */
#define VX_WB_SAMPLING_RATE 16000

/* conversions from dB to ABE and codec gains */
#define DB_TO_ABE_GAIN(x) ((x) + MIXER_ABE_GAIN_0DB)
#define DB_TO_CAPTURE_PREAMPLIFIER_VOLUME(x) (((x) + 6) / 6)
#define DB_TO_CAPTURE_VOLUME(x) (((x) - 6) / 6)
#define DB_TO_HEADSET_VOLUME(x) (((x) + 15) / 2) // was 30
#define DB_TO_SPEAKER_VOLUME(x) (((x) + 52) / 2)
#define DB_TO_EARPIECE_VOLUME(x) (((x) + 24) / 2)

/* use-case specific mic volumes, all in dB */
#define CAPTURE_DIGITAL_MIC_VOLUME            26
#define CAPTURE_MAIN_MIC_VOLUME               15
#define CAPTURE_SUB_MIC_VOLUME                15
#define CAPTURE_HEADSET_MIC_VOLUME            15

#define VOICE_RECOGNITION_MAIN_MIC_VOLUME     25
#define VOICE_RECOGNITION_SUB_MIC_VOLUME      25
#define VOICE_RECOGNITION_HEADSET_MIC_VOLUME  25

#define CAMCORDER_MAIN_MIC_VOLUME             15
#define CAMCORDER_SUB_MIC_VOLUME              15
#define CAMCORDER_HEADSET_MIC_VOLUME          15

#define VOIP_MAIN_MIC_VOLUME                  15
#define VOIP_SUB_MIC_VOLUME                   15
#define VOIP_HEADSET_MIC_VOLUME               15

#define VOICE_CALL_MAIN_MIC_VOLUME            15
#define VOICE_CALL_SUB_MIC_VOLUME             15
#define VOICE_CALL_HEADSET_MIC_VOLUME         15
#define VOICE_CALL_OMAP5_SEVM_MAIN_MIC_VOLUME     -4
#define VOICE_CALL_OMAP5_SEVM_SUB_MIC_VOLUME      -4
#define VOICE_CALL_OMAP5_SEVM_HEADSET_MIC_VOLUME  -4

#define VOICE_CALL_REC_UL_VOLUME              0
#define VOICE_CALL_REC_DL_VOLUME              0

/* use-case specific output volumes */
#define NORMAL_SPEAKER_VOLUME                 0
#define VOICE_CALL_SPEAKER_VOLUME             0

#define HEADSET_VOLUME                        0
#define HEADPHONE_VOLUME                      0 /* allow louder output for headphones */

/* product-specific defines */
#define PRODUCT_DEVICE_PROPERTY 	"ro.product.device"
#define PRODUCT_DEVICE_BLAZE    	"blaze"
#define PRODUCT_DEVICE_TABLET   	"blaze_tablet"
#define PRODUCT_DEVICE_OMAP5_SEVM       "omap5sevm"
#define PRODUCT_DEVICE_ACCLAIM          "acclaim"


enum supported_boards {
    OMAP4_BLAZE,
    OMAP4_TABLET,
    OMAP5_SEVM,
    OMAP4_ACCLAIM,
};

enum tty_modes {
    TTY_MODE_OFF,
    TTY_MODE_VCO,
    TTY_MODE_HCO,
    TTY_MODE_FULL
};

struct pcm_config pcm_config_mm = {
    .channels = 2,
    .rate = MM_FULL_POWER_SAMPLING_RATE,
    .period_size = LONG_PERIOD_SIZE,
    .period_count = PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_mm_ul = {
    .channels = 2,
    .rate = MM_FULL_POWER_SAMPLING_RATE,
    .period_size = SHORT_PERIOD_SIZE,
    .period_count = CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_vx = {
    .channels = 2,
    .rate = VX_WB_SAMPLING_RATE,
    .period_size = 160,
    .period_count = 2,
    .format = PCM_FORMAT_S16_LE,
};

#define MIN(x, y) ((x) > (y) ? (y) : (x))

struct route_setting {
    char *ctl_name;
    int intval;
    char *strval;
};

/* These are values that never change */
struct route_setting defaults[] = {
    /* general */
    {
        .ctl_name = MIXER_DL2_LEFT_EQUALIZER,
        .strval = MIXER_0DB_HIGH_PASS,
    },
    {
        .ctl_name = MIXER_DL2_RIGHT_EQUALIZER,
        .strval = MIXER_0DB_HIGH_PASS,
    },
    {
        .ctl_name = MIXER_DL1_EQUALIZER,
        .strval = MIXER_0DB_HIGH_PASS,
    },
    {
        .ctl_name = MIXER_DL1_MEDIA_PLAYBACK_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB - 2,
    },
    {
        .ctl_name = MIXER_DL2_MEDIA_PLAYBACK_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB - 2,
    },
    {
        .ctl_name = MIXER_DL1_VOICE_PLAYBACK_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB,
    },
    {
        .ctl_name = MIXER_DL2_VOICE_PLAYBACK_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB,
    },
    {
        .ctl_name = MIXER_SDT_DL_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB,
    },
    {
        .ctl_name = MIXER_EARPHONE_PLAYBACK_VOLUME,
        .intval = DB_TO_EARPIECE_VOLUME(6),
    },
    {
        .ctl_name = MIXER_AUDUL_VOICE_UL_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB,
    },
    {
        .ctl_name = MIXER_CAPTURE_PREAMPLIFIER_VOLUME,
        .intval = DB_TO_CAPTURE_PREAMPLIFIER_VOLUME(0),
    },
    {
        .ctl_name = MIXER_CAPTURE_VOLUME,
        .intval = DB_TO_CAPTURE_VOLUME(30),
    },
    {
        .ctl_name = MIXER_SDT_UL_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB - 19,
    },
    {
        .ctl_name = MIXER_SIDETONE_MIXER_CAPTURE,
        .intval = 0,
    },

    /* headset */
    {
        .ctl_name = MIXER_SIDETONE_MIXER_PLAYBACK,
        .intval = 1,
    },
    {
        .ctl_name = MIXER_DL1_PDM_SWITCH,
        .intval = 1,
    },
    {
        .ctl_name = MIXER_HS_POWER_MODE,
        .strval = MIXER_HS_LOW_POWER_MODE,
    },

    /* bt */
    {
        .ctl_name = MIXER_BT_UL_VOLUME,
        .intval = DB_TO_ABE_GAIN(6),
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting hf_dl1[] = {
    {
        .ctl_name = MIXER_DL1_PDM2_SWITCH,
        .intval = 1,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting hf_dl2[] = {
    {
        .ctl_name = MIXER_DL2_LEFT_EQUALIZER,
        .strval = MIXER_0DB_HIGH_PASS,
    },
    {
        .ctl_name = MIXER_DL2_RIGHT_EQUALIZER,
        .strval = MIXER_0DB_HIGH_PASS,
    },
    {
        .ctl_name = MIXER_DL2_MEDIA_PLAYBACK_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB - 2,
    },
    {
        .ctl_name = MIXER_DL2_VOICE_PLAYBACK_VOLUME,
        .intval = MIXER_ABE_GAIN_0DB,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting hf_output[] = {
    {
        .ctl_name = MIXER_HF_LEFT_PLAYBACK,
        .strval = MIXER_PLAYBACK_HF_DAC,
    },
    {
        .ctl_name = MIXER_HF_RIGHT_PLAYBACK,
        .strval = MIXER_PLAYBACK_HF_DAC,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting hs_output[] = {
    {
        .ctl_name = MIXER_HS_LEFT_PLAYBACK,
        .strval = MIXER_PLAYBACK_HS_DAC,
    },
    {
        .ctl_name = MIXER_HS_RIGHT_PLAYBACK,
        .strval = MIXER_PLAYBACK_HS_DAC,
    },
    {
        .ctl_name = NULL,
    },
};

/* MM UL front-end paths */
struct route_setting mm_ul2_bt[] = {
    {
        .ctl_name = MIXER_MUX_UL10,
        .strval = MIXER_BT_LEFT,
    },
    {
        .ctl_name = MIXER_MUX_UL11,
        .strval = MIXER_BT_RIGHT,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting mm_ul2_amic_left[] = {
    {
        .ctl_name = MIXER_MUX_UL10,
        .strval = MIXER_AMIC0,
    },
    {
        .ctl_name = MIXER_MUX_UL11,
        .strval = MIXER_AMIC0,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting mm_ul2_amic_right[] = {
    {
        .ctl_name = MIXER_MUX_UL10,
        .strval = MIXER_AMIC1,
    },
    {
        .ctl_name = MIXER_MUX_UL11,
        .strval = MIXER_AMIC1,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting mm_ul2_dmic0[] = {
    {
        .ctl_name = MIXER_MUX_UL10,
        .strval = MIXER_MMEXTL,
    },
    {
        .ctl_name = MIXER_MUX_UL11,
        .strval = MIXER_MMEXTR,
    },
    {
        .ctl_name = NULL,
    },
};

/* VX UL front-end paths */
struct route_setting vx_ul_amic_left[] = {
    {
        .ctl_name = MIXER_MUX_VX0,
        .strval = MIXER_AMIC0,
    },
    {
        .ctl_name = MIXER_MUX_VX1,
        .strval = MIXER_AMIC0,
    },
    {
        .ctl_name = MIXER_AUDUL_MONO_MIXER,
        .intval = 0,
    },
    {
        .ctl_name = MIXER_VOICE_CAPTURE_MIXER_CAPTURE,
        .intval = 1,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting vx_ul_amic_right[] = {
    {
        .ctl_name = MIXER_MUX_VX0,
        .strval = MIXER_AMIC1,
    },
    {
        .ctl_name = MIXER_MUX_VX1,
        .strval = MIXER_AMIC1,
    },
    {
        .ctl_name = MIXER_AUDUL_MONO_MIXER,
        .intval = 0,
    },
    {
        .ctl_name = MIXER_VOICE_CAPTURE_MIXER_CAPTURE,
        .intval = 1,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting vx_ul_dmic0[] = {
    {
        .ctl_name = MIXER_MUX_VX0,
        .strval = MIXER_DMIC0L,
    },
    {
        .ctl_name = MIXER_MUX_VX1,
        .strval = MIXER_DMIC0L,
    },
    {
        .ctl_name = MIXER_AUDUL_MONO_MIXER,
        .intval = 0,
    },
    {
        .ctl_name = MIXER_VOICE_CAPTURE_MIXER_CAPTURE,
        .intval = 1,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting vx_ul_bt[] = {
    {
        .ctl_name = MIXER_MUX_VX0,
        .strval = MIXER_BT_LEFT,
    },
    {
        .ctl_name = MIXER_MUX_VX1,
        .strval = MIXER_BT_RIGHT,
    },
    {
        .ctl_name = MIXER_AUDUL_MONO_MIXER,
        .intval = 1,
    },
    {
        .ctl_name = MIXER_VOICE_CAPTURE_MIXER_CAPTURE,
        .intval = 1,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting vx_rec_default[] = {
    {
        .ctl_name = MIXER_MUX_UL10,
        .strval = MIXER_VX_LEFT,
    },
    {
        .ctl_name = MIXER_MUX_UL11,
        .strval = MIXER_VX_RIGHT,
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting vx_rec_ul[] = {
    {
        .ctl_name = MIXER_CAPTURE_MIXER_VOICE_CAPTURE,
        .intval = 1,
    },
    {
        .ctl_name = MIXER_VXREC_VOICE_UL_VOLUME,
        .intval = DB_TO_ABE_GAIN(VOICE_CALL_REC_UL_VOLUME),
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting vx_rec_dl[] = {
    {
        .ctl_name = MIXER_CAPTURE_MIXER_VOICE_PLAYBACK,
        .intval = 1,
    },
    {
        .ctl_name = MIXER_VXREC_VOICE_DL_VOLUME,
        .intval = DB_TO_ABE_GAIN(VOICE_CALL_REC_DL_VOLUME),
    },
    {
        .ctl_name = NULL,
    },
};

struct route_setting codec_output_controls[] = {
    { .ctl_name = SP_DRIVER_MUTE ,               .intval = 1, },
    { .ctl_name = SP_ANALOG_GAIN,                .intval = 127, },
    { .ctl_name = DAC_PLAYBACK_VOLUME,           .intval = 140, },
    { .ctl_name = RIGHT_DAC_INPUT_SELECTION,     .strval = RIGHT_DATA, },
    { .ctl_name = LEFT_DAC_INPUT_SELECTION,      .strval = LEFT_DATA, },
    { .ctl_name = DAC_L_TO_LEFT_OUTPUT_MIXER,    .intval = 1, },
    { .ctl_name = DAC_R_TO_RIGHT_OUTPUT_MIXER,   .intval = 1, },
    { .ctl_name = HP_DRIVER_MUTE,                .intval = 1, },
    { .ctl_name = HP_ANALOG_GAIN,                .intval = 127, },
    { .ctl_name = HP_CM_VOLTAGE_CTL,             .strval = VOLTAGE, },
    { .ctl_name = MIXER_DL1_MIXER_MULTIMEDIA,    .intval = 1, },
    { .ctl_name = MIXER_SIDETONE_MIXER_PLAYBACK, .intval = 1, },
    { .ctl_name = DL1_MM_EXT_SWITCH,             .intval = 1, },
    { .ctl_name = NULL, },
};

struct route_setting codec_input_controls[] = {
    { .ctl_name = M_INPUT_MIXER,                .intval = 2, },
    { .ctl_name = P_INPUT_MIXER,                .intval = 2, },
    { .ctl_name = MIC_PGA_GAIN,                 .intval = 50, },
    { .ctl_name = ADC_COARSE_GAIN,              .intval = 48, },
    { .ctl_name = MIXER_SIDETONE_MIXER_CAPTURE, .intval = 1, },
    { .ctl_name = MIXER_AUDUL_VOICE_UL_VOLUME,  .intval = MIXER_ABE_GAIN_0DB, },
    { .ctl_name = MIXER_SDT_UL_VOLUME,          .intval = MIXER_ABE_GAIN_0DB, },
    { .ctl_name = NULL, },
};

struct buffer_remix;

/* buffer_remix: functor for doing in-place buffer manipulations.
 *
 * NB. When remix_func is called, the memory at `buf` must be at least
 * as large as frames * sample_size * MAX(in_chans, out_chans).
 */
struct buffer_remix {
    void (*remix_func)(struct buffer_remix *data, void *buf, size_t frames);
    size_t sample_size; /* size of one audio sample, in bytes */
    size_t in_chans;    /* number of input channels */
    size_t out_chans;   /* number of output channels */
};

struct mixer_ctls
{
    /* Digital uplink controls */
    struct mixer_ctl *amic_ul_volume;
    struct mixer_ctl *dmic1_ul_volume;
    struct mixer_ctl *dmic2_ul_volume;
    struct mixer_ctl *dmic3_ul_volume;
    struct mixer_ctl *voice_ul_volume;
    struct mixer_ctl *bt_ul_volume;

    /* Digital downlink controls */
    struct mixer_ctl *dl1_eq;
    struct mixer_ctl *mm_dl1;
    struct mixer_ctl *vx_dl1;
    struct mixer_ctl *mm2_dl1_capture;
    struct mixer_ctl *dl1_mono;
    struct mixer_ctl *mm_dl2;
    struct mixer_ctl *vx_dl2;
    struct mixer_ctl *dl2_mono;
    struct mixer_ctl *dl1_handsfree;
    struct mixer_ctl *dl1_headset;
    struct mixer_ctl *dl1_bt;
    struct mixer_ctl *sidetone_capture;

    /* Analog uplink controls */
    struct mixer_ctl *left_capture;
    struct mixer_ctl *right_capture;
    /* Analog downlink controls */
    struct mixer_ctl *earpiece_enable;
    struct mixer_ctl *headset_volume;
    struct mixer_ctl *speaker_volume;
};

struct audio_devices {
    int out_devices;
    int in_devices;
};

struct omap_audio_device {
    struct audio_hw_device hw_device;

    pthread_mutex_t lock;       /* see note below on mutex acquisition order */
    struct mixer *mixer;
    struct mixer_ctls mixer_ctls;
    int mode;
    struct audio_devices devices;
    struct audio_devices cur_devices;
    struct pcm *pcm_modem_dl;
    struct pcm *pcm_modem_ul;
    int in_call;
    float voice_volume;
    struct omap_stream_in *active_input;
    struct omap_stream_out *active_output;
    bool mic_mute;
    int tty_mode;
    int sidetone_capture;
    int board_type;
    struct echo_reference_itfe *echo_reference;
    int input_requires_stereo;
    bool low_power;
    bool bluetooth_nrec;
    bool vx_rec_on;
    bool dl2_support;

    /* For modem purpose */
    bool modem;
    int wb_amr;

    /* RIL */
    struct ril_handle ril;
};

struct omap_stream_out {
    struct audio_stream_out stream;

    pthread_mutex_t lock;       /* see note below on mutex acquisition order */
    struct pcm_config config;
    struct pcm *pcm;
    struct resampler_itfe *resampler;
    char *buffer;
    int standby;
    struct echo_reference_itfe *echo_reference;
    int write_threshold;
    bool low_power;

    struct omap_audio_device *dev;
};

#define MAX_PREPROCESSORS 3 /* maximum one AGC + one NS + one AEC per input stream */

struct omap_stream_in {
    struct audio_stream_in stream;

    pthread_mutex_t lock;       /* see note below on mutex acquisition order */
    struct pcm_config config;
    struct pcm *pcm;
    int device;
    struct resampler_itfe *resampler;
    struct resampler_buffer_provider buf_provider;
    int16_t *buffer;
    size_t frames_in;
    unsigned int requested_rate;
    int standby;
    int source;
    struct echo_reference_itfe *echo_reference;
    bool need_echo_reference;
    effect_handle_t preprocessors[MAX_PREPROCESSORS];
    int num_preprocessors;
    int16_t *proc_buf;
    size_t proc_buf_size;
    size_t proc_frames_in;
    int16_t *ref_buf;
    size_t ref_buf_size;
    size_t ref_frames_in;
    int read_status;
    struct buffer_remix *remix_at_driver; /* adapt hw chan count to client */

    struct omap_audio_device *dev;
};

/**
 * NOTE: when multiple mutexes have to be acquired, always respect the following order:
 *        hw device > in stream > out stream
 */

static void select_output_device(struct omap_audio_device *adev);
static void select_input_device(struct omap_audio_device *adev);
static int adev_set_voice_volume(struct audio_hw_device *dev, float volume);
static int set_voice_volume(struct audio_hw_device *dev, float volume);
static int do_input_standby(struct omap_stream_in *in);
static int do_output_standby(struct omap_stream_out *out);

/* Implementation of buffer_remix::remix_func that removes
 * channels in place without doing any other processing.  The
 * extra channels are truncated.
 */
static void remove_channels_from_buf(struct buffer_remix *data, void *buf, size_t frames)
{
    size_t samp_size, in_frame, out_frame;
    size_t N, c;
    char *s, *d;

    if (frames == 0)
        return;

    samp_size = data->sample_size;
    in_frame = data->in_chans * samp_size;
    out_frame = data->out_chans * samp_size;

    if (out_frame >= in_frame) {
        ALOGE("BUG: remove_channels_from_buf() can not add channels to a buffer.\n");
        return;
    }

    N = frames - 1;
    d = (char*)buf + out_frame;
    s = (char*)buf + in_frame;

    /* take the first several channels and
     * truncate the rest
     */
    while (N--) {
        for (c = 0; c < out_frame; ++c)
            d[c] = s[c];
        d += out_frame;
        s += in_frame;
    }
}

static void setup_stereo_to_mono_input_remix(struct omap_stream_in *in)
{
    struct buffer_remix *br = (struct buffer_remix *)malloc(sizeof(struct buffer_remix));

    if (br) {
        br->remix_func = remove_channels_from_buf;
        br->sample_size = audio_stream_frame_size(&in->stream.common) / in->config.channels;
        br->in_chans = 2;
        br->out_chans = 1;
    } else
        ALOGE("Could not allocate memory for struct buffer_remix\n");

    if (in->buffer) {
        size_t chans = (br->in_chans > br->out_chans) ? br->in_chans : br->out_chans;
        free(in->buffer);
        in->buffer = malloc(in->config.period_size * br->sample_size * chans);
        if (!in->buffer)
            ALOGE("Could not reallocate memory for input buffer\n");
    }

    if (in->remix_at_driver)
        free(in->remix_at_driver);
    in->remix_at_driver = br;
}

static int get_boardtype(struct omap_audio_device *adev)
{
    char board[PROPERTY_VALUE_MAX];
    int status = 0;
    int board_type = 0;

    property_get(PRODUCT_DEVICE_PROPERTY, board, "UNKNOWN");
    if (!strcmp(board, "UNKNOWN")) {
         return -ENODEV;
    }

    /* return true if the property matches the given value */
    if (!strcmp(board, PRODUCT_DEVICE_BLAZE)) {
            adev->board_type = OMAP4_BLAZE;
          /*true on devices that must use sidetone capture */
            adev->sidetone_capture = 1;
    }
    else if (!strcmp(board, PRODUCT_DEVICE_TABLET)) {
            adev->board_type = OMAP4_TABLET;
            adev->sidetone_capture = 0;
    }
    else if (!strcmp(board, PRODUCT_DEVICE_OMAP5_SEVM)) {
            adev->board_type = OMAP5_SEVM;
            adev->sidetone_capture = 1;
    }
    else if (!strcmp(board, PRODUCT_DEVICE_ACCLAIM)) {
            adev->board_type = OMAP4_ACCLAIM;
            adev->sidetone_capture = 0;
    }
    else
        return -EINVAL;

    ALOGI("boardtype used: %s(%d)", board, adev->board_type);

    return 0;
}

/* The enable flag when 0 makes the assumption that enums are disabled by
 * "Off" and integers/booleans by 0 */
static int set_route_by_array(struct mixer *mixer, struct route_setting *route,
                              int enable)
{
    struct mixer_ctl *ctl;
    unsigned int i, j;

    /* Go through the route array and set each value */
    i = 0;
    while (route[i].ctl_name) {
        ctl = mixer_get_ctl_by_name(mixer, route[i].ctl_name);
        if (!ctl)
            return -EINVAL;

        if (route[i].strval) {
            if (enable)
                mixer_ctl_set_enum_by_string(ctl, route[i].strval);
            else
                mixer_ctl_set_enum_by_string(ctl, "Off");
        } else {
            /* This ensures multiple (i.e. stereo) values are set jointly */
            for (j = 0; j < mixer_ctl_get_num_values(ctl); j++) {
                if (enable)
                    mixer_ctl_set_value(ctl, j, route[i].intval);
                else
                    mixer_ctl_set_value(ctl, j, 0);
            }
        }
        i++;
    }

    return 0;
}

static int start_call(struct omap_audio_device *adev)
{
    ALOGE("Opening modem PCMs");

    if (adev->modem) {
        pcm_config_vx.rate = adev->wb_amr ? VX_WB_SAMPLING_RATE : VX_NB_SAMPLING_RATE;
    } else {
        pcm_config_vx.rate = VX_NB_SAMPLING_RATE;
    }

    /* Open modem PCM channels */
    if (adev->pcm_modem_dl == NULL) {
        adev->pcm_modem_dl = pcm_open(0, PORT_MODEM, PCM_OUT, &pcm_config_vx);
        if (!pcm_is_ready(adev->pcm_modem_dl)) {
            ALOGE("cannot open PCM modem DL stream: %s", pcm_get_error(adev->pcm_modem_dl));
            goto err_open_dl;
        }
    }

    if (adev->pcm_modem_ul == NULL) {
        adev->pcm_modem_ul = pcm_open(0, PORT_MODEM, PCM_IN, &pcm_config_vx);
        if (!pcm_is_ready(adev->pcm_modem_ul)) {
            ALOGE("cannot open PCM modem UL stream: %s", pcm_get_error(adev->pcm_modem_ul));
            goto err_open_ul;
        }
    }

    pcm_start(adev->pcm_modem_dl);
    pcm_start(adev->pcm_modem_ul);

    return 0;

err_open_ul:
    pcm_close(adev->pcm_modem_ul);
    adev->pcm_modem_ul = NULL;
err_open_dl:
    pcm_close(adev->pcm_modem_dl);
    adev->pcm_modem_dl = NULL;

    return -ENOMEM;
}

static void end_call(struct omap_audio_device *adev)
{
    ALOGE("Closing modem PCMs");

    pcm_stop(adev->pcm_modem_dl);
    pcm_stop(adev->pcm_modem_ul);
    pcm_close(adev->pcm_modem_dl);
    pcm_close(adev->pcm_modem_ul);
    adev->pcm_modem_dl = NULL;
    adev->pcm_modem_ul = NULL;
}

static void set_eq_filter(struct omap_audio_device *adev)
{
    if (false == adev->modem) {
        return;
    }

    /* DL1_EQ can't be used for bt */
    int dl1_eq_applicable = adev->devices.out_devices & (AUDIO_DEVICE_OUT_WIRED_HEADSET |
                    AUDIO_DEVICE_OUT_WIRED_HEADPHONE | AUDIO_DEVICE_OUT_EARPIECE);

    /* 4Khz LPF is used only in NB-AMR voicecall */
    if ((adev->mode == AUDIO_MODE_IN_CALL) && dl1_eq_applicable &&
            (adev->tty_mode == TTY_MODE_OFF) && !adev->wb_amr)
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.dl1_eq, MIXER_4KHZ_LPF_0DB);
    else
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.dl1_eq, MIXER_FLAT_RESPONSE);
}

void audio_set_wb_amr_callback(void *data, int enable)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)data;
    int trylock;

    /* audio_set_wb_amr_callback callback can be called
     * in the same thread context than
     * the audio_hw in case audio RIL is not in seperate thread.
     * In this case, deadlock needs to be avoided.
     * In case the same function is called in other thread,
     * mutex needs to be used. */
    trylock = pthread_mutex_trylock(&adev->lock);
    if (EDEADLK == trylock)
        ALOGV("%s: WB AMR callback calls in a deadlock situation", __FUNCTION__);
    if (EBUSY == trylock) {
        pthread_mutex_lock(&adev->lock);
    }
    if (adev->wb_amr != enable) {
        adev->wb_amr = enable;

        /* reopen the modem PCMs at the new rate */
        if (adev->in_call) {
            end_call(adev);
            set_eq_filter(adev);
            start_call(adev);
        }
    }
    if (EDEADLK != trylock) {
        pthread_mutex_unlock(&adev->lock);
    }
}

static void set_incall_device(struct omap_audio_device *adev)
{
    int device_type;
    char prop[PROPERTY_VALUE_MAX];

    if (false == adev->modem) {
        return;
    }

    switch (adev->devices.out_devices & AUDIO_DEVICE_OUT_ALL) {
        case AUDIO_DEVICE_OUT_EARPIECE:
            device_type = SOUND_AUDIO_PATH_HANDSET;
            break;
        case AUDIO_DEVICE_OUT_SPEAKER:
            device_type = SOUND_AUDIO_PATH_SPEAKER;
            break;
        case AUDIO_DEVICE_OUT_WIRED_HEADSET:
            device_type = SOUND_AUDIO_PATH_HEADSET;
            break;
        case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
            device_type = SOUND_AUDIO_PATH_HEADPHONE;
            break;
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
            if (adev->bluetooth_nrec)
                device_type = SOUND_AUDIO_PATH_BLUETOOTH;
            else
                device_type = SOUND_AUDIO_PATH_BLUETOOTH_NO_NR;
            break;
        default:
            device_type = SOUND_AUDIO_PATH_HANDSET;
            break;
    }

    /* if output device isn't supported, open modem side with handset acoustic
     * profile, or with no acoustic profile if omap.modem.no_acoustic property
     * is set to "Yes" */
    property_get("omap.modem.no_acoustic", prop, "");
    if (!strcmp(prop, "Yes")) {
        ril_set_call_audio_path(&adev->ril, SOUND_AUDIO_PATH_NO_ACOUSTICS);
    } else {
        ril_set_call_audio_path(&adev->ril, device_type);
    }
}

static void set_input_volumes(struct omap_audio_device *adev, int main_mic_on,
                              int headset_mic_on, int sub_mic_on)
{
    unsigned int channel;
    int volume = MIXER_ABE_GAIN_0DB;

    if (adev->mode == AUDIO_MODE_IN_CALL) {
        /* special case: don't look at input source for IN_CALL state */
        if (adev->board_type == OMAP5_SEVM) {
            volume = DB_TO_ABE_GAIN(main_mic_on ? VOICE_CALL_OMAP5_SEVM_MAIN_MIC_VOLUME :
                    (headset_mic_on ? VOICE_CALL_OMAP5_SEVM_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? VOICE_CALL_OMAP5_SEVM_MAIN_MIC_VOLUME : 0)));
        } else {
            volume = DB_TO_ABE_GAIN(main_mic_on ? VOICE_CALL_MAIN_MIC_VOLUME :
                    (headset_mic_on ? VOICE_CALL_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? VOICE_CALL_SUB_MIC_VOLUME : 0)));
        }
    } else if (adev->active_input) {
        /* determine input volume by use case */
        switch (adev->active_input->source) {
        case AUDIO_SOURCE_MIC: /* general capture */
            if ((adev->board_type == OMAP4_BLAZE) ||
               (adev->board_type == OMAP5_SEVM)) {
                volume = DB_TO_ABE_GAIN(main_mic_on ? CAPTURE_MAIN_MIC_VOLUME :
                    (headset_mic_on ? CAPTURE_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? CAPTURE_SUB_MIC_VOLUME : 0)));
            } else if (adev->board_type == OMAP4_TABLET) {
                volume = DB_TO_ABE_GAIN(main_mic_on ? CAPTURE_DIGITAL_MIC_VOLUME :
                    (headset_mic_on ? CAPTURE_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? CAPTURE_SUB_MIC_VOLUME : 0)));
            } else if (adev->board_type == OMAP4_ACCLAIM) {
                volume = DB_TO_ABE_GAIN(main_mic_on ? CAPTURE_DIGITAL_MIC_VOLUME :
                    (headset_mic_on ? CAPTURE_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? CAPTURE_SUB_MIC_VOLUME : 0)));
            }
            break;

        case AUDIO_SOURCE_CAMCORDER:
            volume = DB_TO_ABE_GAIN(main_mic_on ? CAMCORDER_MAIN_MIC_VOLUME :
                    (headset_mic_on ? CAMCORDER_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? CAMCORDER_SUB_MIC_VOLUME : 0)));
            break;

        case AUDIO_SOURCE_VOICE_RECOGNITION:
            volume = DB_TO_ABE_GAIN(main_mic_on ? VOICE_RECOGNITION_MAIN_MIC_VOLUME :
                    (headset_mic_on ? VOICE_RECOGNITION_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? VOICE_RECOGNITION_SUB_MIC_VOLUME : 0)));
            break;

        case AUDIO_SOURCE_VOICE_COMMUNICATION: /* VoIP */
            volume = DB_TO_ABE_GAIN(main_mic_on ? VOIP_MAIN_MIC_VOLUME :
                    (headset_mic_on ? VOIP_HEADSET_MIC_VOLUME :
                    (sub_mic_on ? VOIP_SUB_MIC_VOLUME : 0)));
            break;

        default:
            /* nothing to do */
            break;
        }
    }

    for (channel = 0; channel < 2; channel++) {
        if ((adev->board_type == OMAP4_BLAZE) ||
           (adev->board_type == OMAP5_SEVM)) {
            mixer_ctl_set_value(adev->mixer_ctls.amic_ul_volume, channel, volume);
        } else if ((adev->board_type == OMAP4_TABLET) ||
                 (adev->board_type == OMAP4_ACCLAIM)) {
            if (headset_mic_on)
                mixer_ctl_set_value(adev->mixer_ctls.amic_ul_volume, channel, volume);
            else
                mixer_ctl_set_value(adev->mixer_ctls.dmic1_ul_volume, channel, volume);
        }
    }
}

static void set_output_volumes(struct omap_audio_device *adev)
{
    unsigned int channel;
    int speaker_volume;
    int headset_volume;

    speaker_volume = adev->mode == AUDIO_MODE_IN_CALL ? VOICE_CALL_SPEAKER_VOLUME :
                                                        NORMAL_SPEAKER_VOLUME;
    headset_volume = adev->devices.out_devices & AUDIO_DEVICE_OUT_WIRED_HEADSET ?
                                                        HEADSET_VOLUME :
                                                        HEADPHONE_VOLUME;

    for (channel = 0; channel < 2; channel++) {
        mixer_ctl_set_value(adev->mixer_ctls.speaker_volume, channel,
            DB_TO_SPEAKER_VOLUME(speaker_volume));
        mixer_ctl_set_value(adev->mixer_ctls.headset_volume, channel,
            DB_TO_HEADSET_VOLUME(headset_volume));
    }
}

static void force_all_standby(struct omap_audio_device *adev)
{
    struct omap_stream_in *in;
    struct omap_stream_out *out;

    if (adev->active_output) {
        out = adev->active_output;
        pthread_mutex_lock(&out->lock);
        do_output_standby(out);
        pthread_mutex_unlock(&out->lock);
    }
    if (adev->active_input) {
        in = adev->active_input;
        pthread_mutex_lock(&in->lock);
        do_input_standby(in);
        pthread_mutex_unlock(&in->lock);
    }
}

static void select_mode(struct omap_audio_device *adev)
{
    if (adev->mode == AUDIO_MODE_IN_CALL) {
        ALOGE("Entering IN_CALL state, in_call=%d", adev->in_call);
        if (!adev->in_call) {
            force_all_standby(adev);
            /* force earpiece route for in call state if speaker is the
            only currently selected route. This prevents having to tear
            down the modem PCMs to change route from speaker to earpiece
            after the ringtone is played, but doesn't cause a route
            change if a headset or bt device is already connected. If
            speaker is not the only thing active, just remove it from
            the route. We'll assume it'll never be used initially during
            a call. This works because we're sure that the audio policy
            manager will update the output device after the audio mode
            change, even if the device selection did not change. */
            if ((adev->devices.out_devices & AUDIO_DEVICE_OUT_ALL) == AUDIO_DEVICE_OUT_SPEAKER) {
                adev->devices.out_devices = AUDIO_DEVICE_OUT_EARPIECE;
                adev->devices.in_devices = AUDIO_DEVICE_IN_BUILTIN_MIC & ~AUDIO_DEVICE_BIT_IN;
            } else
                adev->devices.out_devices &= ~AUDIO_DEVICE_OUT_SPEAKER;

            select_output_device(adev);

            if (adev->modem) {
                ril_set_call_clock_sync(&adev->ril, SOUND_CLOCK_START);
            }
            start_call(adev);
            set_voice_volume(&adev->hw_device, adev->voice_volume);
            adev->in_call = 1;
        }
    } else {
        ALOGE("Leaving IN_CALL state, in_call=%d, mode=%d",
             adev->in_call, adev->mode);
        if (adev->in_call) {
            adev->in_call = 0;
            end_call(adev);
            if (adev->modem) {
                ril_set_call_clock_sync(&adev->ril, SOUND_CLOCK_STOP);
            }
            force_all_standby(adev);
            select_output_device(adev);
            select_input_device(adev);
        }
    }
}

static void select_output_device(struct omap_audio_device *adev)
{
    int headset_on;
    int headphone_on;
    int speaker_on;
    int earpiece_on;
    int bt_on;
    int dl1_on;
    int sidetone_capture_on = 0;
    unsigned int channel, voice_ul_volume[2] = {0, 0};

    /* Mute VX_UL to avoid pop noises in the tx path
     * during call before switch changes.
     */
    if (adev->mode == AUDIO_MODE_IN_CALL) {
        /* no sidetone while re-routing uplink path to avoid
           propagating pops/glitches to downlink */
        mixer_ctl_set_value(adev->mixer_ctls.sidetone_capture, 0, 0);

        for (channel = 0; channel < 2; channel++)
            voice_ul_volume[channel] =
                mixer_ctl_get_value(adev->mixer_ctls.voice_ul_volume, channel);
            mixer_ctl_set_value(adev->mixer_ctls.voice_ul_volume, channel, 0);
            /* Mute voice before setting new path and audio acoustic profile */
            set_voice_volume(&adev->hw_device, 0);
    }

    headset_on |= adev->devices.out_devices & AUDIO_DEVICE_OUT_WIRED_HEADSET;
    headphone_on |= adev->devices.out_devices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
    speaker_on |= adev->devices.out_devices & AUDIO_DEVICE_OUT_SPEAKER;
    earpiece_on |= adev->devices.out_devices & AUDIO_DEVICE_OUT_EARPIECE;
    bt_on |= adev->devices.out_devices & AUDIO_DEVICE_OUT_ALL_SCO;

    /* force rx path according to TTY mode when in call */
    if (adev->mode == AUDIO_MODE_IN_CALL && !bt_on) {
        switch(adev->tty_mode) {
            case TTY_MODE_FULL:
            case TTY_MODE_VCO:
                /* rx path to headphones */
                headphone_on = 1;
                headset_on = 0;
                speaker_on = 0;
                earpiece_on = 0;
                break;
            case TTY_MODE_HCO:
                /* rx path to device speaker */
                headphone_on = 0;
                headset_on = 0;
                speaker_on = 1;
                earpiece_on = 0;
                break;
            case TTY_MODE_OFF:
            default:
                break;
        }
    }

    dl1_on = headset_on | headphone_on | earpiece_on| speaker_on;

    /* Select front end */
    mixer_ctl_set_value(adev->mixer_ctls.mm_dl1, 0, dl1_on);
    mixer_ctl_set_value(adev->mixer_ctls.vx_dl1, 0,
                        dl1_on && (adev->mode == AUDIO_MODE_IN_CALL));
    /* Select back end */
    mixer_ctl_set_value(adev->mixer_ctls.dl1_headset, 0,
                        headset_on | headphone_on | earpiece_on);
    mixer_ctl_set_value(adev->mixer_ctls.dl1_bt, 0, bt_on);
    mixer_ctl_set_value(adev->mixer_ctls.earpiece_enable, 0, earpiece_on);

    /* select output stage */
    set_route_by_array(adev->mixer, hs_output, headset_on | headphone_on);
    set_route_by_array(adev->mixer, hf_output, speaker_on);
    set_route_by_array(adev->mixer, codec_output_controls, headset_on | headphone_on | speaker_on);

    /* Set mono mixer for headset, headphone, bluetooth and speaker during call */
    mixer_ctl_set_value(adev->mixer_ctls.dl1_mono, 0,
                        ((headset_on | headphone_on) &&
                         (adev->mode == AUDIO_MODE_IN_CALL)) || bt_on);
    if (adev->dl2_support) {
        mixer_ctl_set_value(adev->mixer_ctls.dl2_mono, 0,
                            speaker_on && (adev->mode == AUDIO_MODE_IN_CALL));
    }
    set_eq_filter(adev);
    set_output_volumes(adev);
    /* Special case: select input path if in a call, otherwise
       in_set_parameters is used to update the input route
       todo: use sub mic for handsfree case */
    if (adev->mode == AUDIO_MODE_IN_CALL) {
        if (bt_on)
            set_route_by_array(adev->mixer, vx_ul_bt, bt_on);
        else {
            /* force tx path according to TTY mode when in call */
            switch(adev->tty_mode) {
                case TTY_MODE_FULL:
                case TTY_MODE_HCO:
                    /* tx path from headset mic */
                    headphone_on = 0;
                    headset_on = 1;
                    speaker_on = 0;
                    earpiece_on = 0;
                    break;
                case TTY_MODE_VCO:
                    /* tx path from device sub mic */
                    headphone_on = 0;
                    headset_on = 0;
                    speaker_on = 1;
                    earpiece_on = 0;
                    break;
                case TTY_MODE_OFF:
                default:
                    break;
            }

            if (headset_on || headphone_on || earpiece_on)
                set_route_by_array(adev->mixer, vx_ul_amic_left, 1);
            else if (speaker_on) {
                if ((adev->board_type == OMAP4_BLAZE) ||
                   (adev->board_type == OMAP5_SEVM))
                    set_route_by_array(adev->mixer, vx_ul_amic_right, 1);
                else if (adev->board_type == OMAP4_TABLET)
                    set_route_by_array(adev->mixer, vx_ul_dmic0, 1);
                else if (adev->board_type == OMAP4_ACCLAIM)
                    set_route_by_array(adev->mixer, vx_ul_dmic0, 1);
            }
            else {
                if ((adev->board_type == OMAP4_BLAZE) ||
                   (adev->board_type == OMAP5_SEVM))
                    set_route_by_array(adev->mixer, vx_ul_amic_left, 0);
                else if (adev->board_type == OMAP4_TABLET)
                    set_route_by_array(adev->mixer, vx_ul_dmic0, 0);
                else if (adev->board_type == OMAP4_ACCLAIM) 
                    set_route_by_array(adev->mixer, vx_ul_dmic0, 0);
            }
            if ((adev->board_type == OMAP4_BLAZE) ||
               (adev->board_type == OMAP5_SEVM)) {
                mixer_ctl_set_enum_by_string(adev->mixer_ctls.left_capture,
                        (earpiece_on || headphone_on) ? MIXER_MAIN_MIC :
                        (headset_on ? MIXER_HS_MIC : "Off"));
                mixer_ctl_set_enum_by_string(adev->mixer_ctls.right_capture,
                        speaker_on ? MIXER_SUB_MIC : "Off");

            } else if (adev->board_type == OMAP4_TABLET) {
                mixer_ctl_set_enum_by_string(adev->mixer_ctls.left_capture,
                        (headset_on ? MIXER_HS_MIC : "Off"));
                mixer_ctl_set_enum_by_string(adev->mixer_ctls.right_capture, "Off");
            } else if (adev->board_type == OMAP4_ACCLAIM) {
                mixer_ctl_set_enum_by_string(adev->mixer_ctls.left_capture,
                        (headset_on ? MIXER_HS_MIC : "Off"));
                mixer_ctl_set_enum_by_string(adev->mixer_ctls.right_capture, "Off");
            }

            set_input_volumes(adev, earpiece_on || headphone_on,
                    headset_on, speaker_on);

            /* enable sidetone mixer capture if needed */
            sidetone_capture_on = earpiece_on && adev->sidetone_capture;
        }
        set_incall_device(adev);

        /* Unmute VX_UL after the switch */
        for (channel = 0; channel < 2; channel++) {
            mixer_ctl_set_value(adev->mixer_ctls.voice_ul_volume,
                                channel, voice_ul_volume[channel]);
        }
        /* Unmute voice after new path and audio acoustic profile applied */
        set_voice_volume(&adev->hw_device, adev->voice_volume);
    }

    mixer_ctl_set_value(adev->mixer_ctls.sidetone_capture, 0, sidetone_capture_on);
    adev->cur_devices = adev->devices;
}

static void select_input_device(struct omap_audio_device *adev)
{
    int headset_on = 0;
    int main_mic_on = 0;
    int sub_mic_on = 0;
    int bt_on = 0;

    if (!bt_on) {
        if ((adev->mode != AUDIO_MODE_IN_CALL) && (adev->active_input != 0)) {
            /* sub mic is used for camcorder or VoIP on speaker phone */
            sub_mic_on = (adev->active_input->source == AUDIO_SOURCE_CAMCORDER) ||
            	((adev->devices.out_devices & AUDIO_DEVICE_OUT_SPEAKER) &&
            	(adev->active_input->source == AUDIO_SOURCE_VOICE_COMMUNICATION));
        }
        if (!sub_mic_on) {
            headset_on = adev->devices.in_devices & AUDIO_DEVICE_IN_WIRED_HEADSET;
            main_mic_on = adev->devices.in_devices & AUDIO_DEVICE_IN_BUILTIN_MIC;
        }
    }

    /* TODO: check how capture is possible during voice calls or if
     * both use cases are mutually exclusive.
     */
    if (bt_on)
        set_route_by_array(adev->mixer, mm_ul2_bt, 1);

    if ((adev->board_type == OMAP4_BLAZE) ||
       (adev->board_type == OMAP5_SEVM)) {
        /* Select front end */
        if (main_mic_on || headset_on)
            set_route_by_array(adev->mixer, mm_ul2_amic_left, 1);
        else if (sub_mic_on)
            set_route_by_array(adev->mixer, mm_ul2_amic_right, 1);
        else
            set_route_by_array(adev->mixer, mm_ul2_amic_left, 0);
        /* Select back end */
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.right_capture,
                sub_mic_on ? MIXER_SUB_MIC : "Off");
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.left_capture,
                main_mic_on ? MIXER_MAIN_MIC :
                (headset_on ? MIXER_HS_MIC : "Off"));
    } else if (adev->board_type == OMAP4_TABLET) {
        /* Select front end */
        if (headset_on)
            set_route_by_array(adev->mixer, mm_ul2_amic_left, 1);
        else if (main_mic_on || sub_mic_on) {
            set_route_by_array(adev->mixer, mm_ul2_dmic0, 1);
        } else {
            set_route_by_array(adev->mixer, mm_ul2_dmic0, 0);
        }

        /* Select back end */
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.right_capture, "off");
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.left_capture,
                main_mic_on ? "off" :
                (headset_on ? MIXER_HS_MIC : "Off"));
    } else if (adev->board_type == OMAP4_ACCLAIM) {
        /* Select front end */
        if (main_mic_on || sub_mic_on || headset_on)
            set_route_by_array(adev->mixer, mm_ul2_dmic0, 1);

        /* Select back end */
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.right_capture, "off");
        mixer_ctl_set_enum_by_string(adev->mixer_ctls.left_capture,
                main_mic_on ? "off" :
                (headset_on ? MIXER_HS_MIC : "Off"));
        set_route_by_array(adev->mixer, codec_input_controls, 1);
    }

    set_input_volumes(adev, main_mic_on, headset_on, sub_mic_on);
    adev->cur_devices = adev->devices;
}

/* must be called with hw device and output stream mutexes locked */
static int start_output_stream(struct omap_stream_out *out)
{
    struct omap_audio_device *adev = out->dev;
    unsigned int card = CARD_OMAP_DEFAULT;
    unsigned int port = PORT_MM_LP;

    if (adev->board_type == OMAP5_SEVM)
        port = PORT_MM;

    adev->active_output = out;

    out->config.rate = MM_FULL_POWER_SAMPLING_RATE;

    if (adev->mode != AUDIO_MODE_IN_CALL) {
        /* FIXME: only works if only one output can be active at a time */
        select_output_device(adev);
    }

    if ((adev->devices.out_devices & AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET) ||
        (adev->devices.out_devices & AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET)) {
        card = CARD_OMAP_USB;
        port = PORT_MM;
    }
    /* default to low power:
     *  NOTE: PCM_NOIRQ mode is required to dynamically scale avail_min
     */
    out->write_threshold = PLAYBACK_PERIOD_COUNT * LONG_PERIOD_SIZE;
    out->config.start_threshold = SHORT_PERIOD_SIZE * 2;
    out->config.avail_min = LONG_PERIOD_SIZE;
    out->low_power = 1;

    out->pcm = pcm_open(card, port, PCM_OUT | PCM_MMAP, &out->config);

    if (!pcm_is_ready(out->pcm)) {
        ALOGE("cannot open pcm_out driver: %s", pcm_get_error(out->pcm));
        pcm_close(out->pcm);
        adev->active_output = NULL;
        return -ENOMEM;
    }

    if (adev->echo_reference != NULL)
        out->echo_reference = adev->echo_reference;
    if (out->resampler)
        out->resampler->reset(out->resampler);

    return 0;
}

static int check_input_parameters(uint32_t sample_rate, int format, int channel_count)
{
    if (format != AUDIO_FORMAT_PCM_16_BIT)
        return -EINVAL;

    if ((channel_count < 1) || (channel_count > 2))
        return -EINVAL;


    switch(sample_rate) {
    case 8000:
    case 11025:
    case 16000:
    case 22050:
    case 24000:
    case 32000:
    case 44100:
    case 48000:
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

static size_t get_input_buffer_size(uint32_t sample_rate, int format, int channel_count)
{
    size_t size;
    size_t device_rate;

    if (check_input_parameters(sample_rate, format, channel_count) != 0)
        return 0;

    /* take resampling into account and return the closest majoring
    multiple of 16 frames, as audioflinger expects audio buffers to
    be a multiple of 16 frames */
    size = (pcm_config_mm.period_size * sample_rate) / pcm_config_mm.rate;
    size = ((size + 15) / 16) * 16;

    return size * channel_count * sizeof(short);
}

static void add_echo_reference(struct omap_stream_out *out,
                               struct echo_reference_itfe *reference)
{
    pthread_mutex_lock(&out->lock);
    out->echo_reference = reference;
    pthread_mutex_unlock(&out->lock);
}

static void remove_echo_reference(struct omap_stream_out *out,
                                  struct echo_reference_itfe *reference)
{
    pthread_mutex_lock(&out->lock);
    if (out->echo_reference == reference) {
        /* stop writing to echo reference */
        reference->write(reference, NULL);
        out->echo_reference = NULL;
    }
    pthread_mutex_unlock(&out->lock);
}

static void put_echo_reference(struct omap_audio_device *adev,
                          struct echo_reference_itfe *reference)
{
    if (adev->echo_reference != NULL &&
            reference == adev->echo_reference) {
        if (adev->active_output != NULL)
            remove_echo_reference(adev->active_output, reference);
        release_echo_reference(reference);
        adev->echo_reference = NULL;
    }
}

static struct echo_reference_itfe *get_echo_reference(struct omap_audio_device *adev,
                                               audio_format_t format,
                                               uint32_t channel_count,
                                               uint32_t sampling_rate)
{
    put_echo_reference(adev, adev->echo_reference);
    if (adev->active_output != NULL) {
        struct audio_stream *stream = &adev->active_output->stream.common;
        uint32_t wr_channel_count = popcount(stream->get_channels(stream));
        uint32_t wr_sampling_rate = stream->get_sample_rate(stream);

        int status = create_echo_reference(AUDIO_FORMAT_PCM_16_BIT,
                                           channel_count,
                                           sampling_rate,
                                           AUDIO_FORMAT_PCM_16_BIT,
                                           wr_channel_count,
                                           wr_sampling_rate,
                                           &adev->echo_reference);
        if (status == 0)
            add_echo_reference(adev->active_output, adev->echo_reference);
    }
    return adev->echo_reference;
}

static int get_playback_delay(struct omap_stream_out *out,
                       size_t frames,
                       struct echo_reference_buffer *buffer)
{
    size_t kernel_frames;
    int status;

    status = pcm_get_htimestamp(out->pcm, &kernel_frames, &buffer->time_stamp);
    if (status < 0) {
        buffer->time_stamp.tv_sec  = 0;
        buffer->time_stamp.tv_nsec = 0;
        buffer->delay_ns           = 0;
        ALOGV("get_playback_delay(): pcm_get_htimestamp error,"
                "setting playbackTimestamp to 0");
        return status;
    }

    kernel_frames = pcm_get_buffer_size(out->pcm) - kernel_frames;

    /* adjust render time stamp with delay added by current driver buffer.
     * Add the duration of current frame as we want the render time of the last
     * sample being written. */
    buffer->delay_ns = (long)(((int64_t)(kernel_frames + frames)* 1000000000)/
                            MM_FULL_POWER_SAMPLING_RATE);

    return 0;
}

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    return DEFAULT_OUT_SAMPLING_RATE;
}

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    return 0;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    struct omap_stream_out *out = (struct omap_stream_out *)stream;

    /* take resampling into account and return the closest majoring
    multiple of 16 frames, as audioflinger expects audio buffers to
    be a multiple of 16 frames */
    size_t size = (SHORT_PERIOD_SIZE * DEFAULT_OUT_SAMPLING_RATE) / out->config.rate;
    size = ((size + 15) / 16) * 16;
    return size * audio_stream_frame_size((struct audio_stream *)stream);
}

static audio_channel_mask_t out_get_channels(const struct audio_stream *stream)
{
    return AUDIO_CHANNEL_OUT_STEREO;
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int out_set_format(struct audio_stream *stream, audio_format_t format)
{
    return 0;
}

/* must be called with hw device and output stream mutexes locked */
static int do_output_standby(struct omap_stream_out *out)
{
    struct omap_audio_device *adev = out->dev;

    if (!out->standby) {
        pcm_close(out->pcm);
        out->pcm = NULL;

        adev->active_output = 0;

        /* if in call, don't turn off the output stage. This will
        be done when the call is ended */
        if (adev->mode != AUDIO_MODE_IN_CALL) {
            /* FIXME: only works if only one output can be active at a time */
            set_route_by_array(adev->mixer, hs_output, 0);
            set_route_by_array(adev->mixer, hf_output, 0);
        }

        /* stop writing to echo reference */
        if (out->echo_reference != NULL) {
            out->echo_reference->write(out->echo_reference, NULL);
            out->echo_reference = NULL;
        }
        out->standby = 1;
    }

    return 0;
}

static int out_standby(struct audio_stream *stream)
{
    struct omap_stream_out *out = (struct omap_stream_out *)stream;
    int status;

    pthread_mutex_lock(&out->dev->lock);
    pthread_mutex_lock(&out->lock);
    status = do_output_standby(out);
    pthread_mutex_unlock(&out->lock);
    pthread_mutex_unlock(&out->dev->lock);
    return status;
}

static int out_dump(const struct audio_stream *stream, int fd)
{
    return 0;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    struct omap_stream_out *out = (struct omap_stream_out *)stream;
    struct omap_audio_device *adev = out->dev;
    struct omap_stream_in *in;
    struct str_parms *parms;
    char *str;
    char value[32];
    int ret, val = 0;
    bool force_input_standby = false;

    parms = str_parms_create_str(kvpairs);

    ret = str_parms_get_str(parms, AUDIO_PARAMETER_STREAM_ROUTING, value, sizeof(value));
    if (ret >= 0) {
        val = atoi(value);
        pthread_mutex_lock(&adev->lock);
        pthread_mutex_lock(&out->lock);
        if (((adev->devices.out_devices & AUDIO_DEVICE_OUT_ALL) != val) && (val != 0)) {
            if (out == adev->active_output) {
                do_output_standby(out);
                /* a change in output device may change the microphone selection */
                if (adev->active_input &&
                        adev->active_input->source == AUDIO_SOURCE_VOICE_COMMUNICATION) {
                    force_input_standby = true;
                }
            }
            adev->devices.out_devices = val;
            select_output_device(adev);
        }

        pthread_mutex_unlock(&out->lock);
        if (force_input_standby) {
            in = adev->active_input;
            pthread_mutex_lock(&in->lock);
            do_input_standby(in);
            pthread_mutex_unlock(&in->lock);
        }
        pthread_mutex_unlock(&adev->lock);
    }

    str_parms_destroy(parms);
    return ret;
}

static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    return strdup("");
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    struct omap_stream_out *out = (struct omap_stream_out *)stream;

    return (SHORT_PERIOD_SIZE * PLAYBACK_PERIOD_COUNT * 1000) / out->config.rate;
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    return -ENOSYS;
}

static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
                         size_t bytes)
{
    int ret;
    struct omap_stream_out *out = (struct omap_stream_out *)stream;
    struct omap_audio_device *adev = out->dev;
    size_t frame_size = audio_stream_frame_size(&out->stream.common);
    size_t in_frames = bytes / frame_size;
    size_t out_frames = RESAMPLER_BUFFER_SIZE / frame_size;
    bool force_input_standby = false;
    struct omap_stream_in *in;
    int kernel_frames;
    void *buf = (void *)0xDEADBAAD;

do_over:
    if (out->standby) {
        /* adev must be locked when manipulating standby state.
         * However, only lock it if we really need it.  Otherwise we
         * could be delayed by an input stream shutting down, causing
         * an xrun in the middle of this stream.
         */
        pthread_mutex_lock(&adev->lock);
        pthread_mutex_lock(&out->lock);
        if (out->standby) {
            ret = start_output_stream(out);
            if (ret != 0) {
                pthread_mutex_unlock(&adev->lock);
                goto exit;
            }
            out->standby = 0;
            /* a change in output device may change the microphone selection */
            if (adev->active_input &&
                    adev->active_input->source == AUDIO_SOURCE_VOICE_COMMUNICATION) {
                force_input_standby = true;
            }
        }
        pthread_mutex_unlock(&adev->lock);
    } else {
        pthread_mutex_lock(&out->lock);
        /* re-check */
        if (out->standby) {
            pthread_mutex_unlock(&out->lock);
            goto do_over;
        }
    }

    /* only use resampler if required */
    if (out->config.rate != DEFAULT_OUT_SAMPLING_RATE) {
        if (!out->resampler) {
            ret = create_resampler(DEFAULT_OUT_SAMPLING_RATE,
                    MM_FULL_POWER_SAMPLING_RATE,
                    2,
                    RESAMPLER_QUALITY_DEFAULT,
                    NULL,
                    &out->resampler);
            if (ret != 0)
                goto exit;
            out->buffer = malloc(RESAMPLER_BUFFER_SIZE); /* todo: allow for reallocing */
            if (!out->buffer) {
                ret = -ENOMEM;
                goto exit;
            }
        }

        out->resampler->resample_from_input(out->resampler,
                (int16_t *)buffer,
                &in_frames,
                (int16_t *)out->buffer,
                &out_frames);
        buf = out->buffer;

    } else {
        out_frames = in_frames;
        buf = (void *)buffer;
    }
    if (out->echo_reference != NULL) {
        struct echo_reference_buffer b;
        b.raw = (void *)buffer;
        b.frame_count = in_frames;

        get_playback_delay(out, out_frames, &b);
        out->echo_reference->write(out->echo_reference, &b);
    }

    /* do not allow more than out->write_threshold frames in kernel pcm driver buffer */
    do {
        struct timespec time_stamp;

        if (pcm_get_htimestamp(out->pcm, (unsigned int *)&kernel_frames, &time_stamp) < 0)
            break;
        kernel_frames = pcm_get_buffer_size(out->pcm) - kernel_frames;
        if (kernel_frames > out->write_threshold) {
            unsigned long time = (unsigned long)
                    (((int64_t)(kernel_frames - out->write_threshold) * 1000000) /
                            MM_FULL_POWER_SAMPLING_RATE);
            if (time < MIN_WRITE_SLEEP_US)
                time = MIN_WRITE_SLEEP_US;
            usleep(time);
        }
    } while (kernel_frames > out->write_threshold);

    ret = pcm_mmap_write(out->pcm, (void *)buf, out_frames * frame_size);

exit:

    if (ret != 0) {
        unsigned int usecs = bytes * 1000000 / audio_stream_frame_size(&stream->common) /
            out_get_sample_rate(&stream->common);
        if (usecs >= 1000000L) {
            usecs = 999999L;
        }
        usleep(usecs);
    }

    pthread_mutex_unlock(&out->lock);

    if (ret == -EPIPE) {
	    /* Recover from an underrun */
	    ALOGE("XRUN detected");
	    pthread_mutex_lock(&adev->lock);
	    pthread_mutex_lock(&out->lock);
	    do_output_standby(out);
	    pthread_mutex_unlock(&out->lock);
	    pthread_mutex_unlock(&adev->lock);
	    goto do_over;
    }

    if (force_input_standby) {
        pthread_mutex_lock(&adev->lock);
        if (adev->active_input) {
            in = adev->active_input;
            pthread_mutex_lock(&in->lock);
            do_input_standby(in);
            pthread_mutex_unlock(&in->lock);
        }
        pthread_mutex_unlock(&adev->lock);
    }

    return bytes;
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    return -EINVAL;
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

/** audio_stream_in implementation **/

/* must be called with hw device and input stream mutexes locked */
static int start_input_stream(struct omap_stream_in *in)
{
    int ret = 0;
    unsigned int card = CARD_OMAP_DEFAULT;
    unsigned int device = PORT_MM2_UL;
    struct omap_audio_device *adev = in->dev;
    unsigned int vx_rec_ul_on = (in->source == AUDIO_SOURCE_VOICE_UPLINK) ||
            (in->source == AUDIO_SOURCE_VOICE_CALL);
    unsigned int vx_rec_dl_on = (in->source == AUDIO_SOURCE_VOICE_DOWNLINK) ||
            (in->source == AUDIO_SOURCE_VOICE_CALL);

    adev->active_input = in;

    if (adev->mode != AUDIO_MODE_IN_CALL) {
        adev->devices.in_devices &= ~AUDIO_DEVICE_IN_ALL;
        adev->devices.in_devices |= in->device & ~AUDIO_DEVICE_BIT_IN;
        select_input_device(adev);
        adev->vx_rec_on = false;
    } else {
        /* Route for voicecall record */
        set_route_by_array(adev->mixer, vx_rec_default,
                                            vx_rec_ul_on || vx_rec_dl_on);
        set_route_by_array(adev->mixer, vx_rec_ul, vx_rec_ul_on);
        set_route_by_array(adev->mixer, vx_rec_dl, vx_rec_dl_on);
        adev->vx_rec_on = true;
    }

    if (adev->input_requires_stereo && (in->config.channels == 1))
        setup_stereo_to_mono_input_remix(in);

    if (in->need_echo_reference && in->echo_reference == NULL)
        in->echo_reference = get_echo_reference(adev,
                                        AUDIO_FORMAT_PCM_16_BIT,
                                        in->config.channels,
                                        in->requested_rate);

    /* this assumes routing is done previously */
    if (in->remix_at_driver)
        in->config.channels = in->remix_at_driver->in_chans;

    in->pcm = pcm_open(card, device, PCM_IN, &in->config);
    if (in->remix_at_driver)
        in->config.channels = in->remix_at_driver->out_chans;
    if (!pcm_is_ready(in->pcm)) {
        ALOGE("cannot open pcm_in driver: %s", pcm_get_error(in->pcm));
        pcm_close(in->pcm);
        adev->active_input = NULL;
        return -ENOMEM;
    }

    /* if no supported sample rate is available, use the resampler */
    if (in->resampler) {
        in->resampler->reset(in->resampler);
        in->frames_in = 0;
    }
    return 0;
}

static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;

    return in->requested_rate;
}

static int in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    return 0;
}

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;

    return get_input_buffer_size(in->requested_rate,
                                 AUDIO_FORMAT_PCM_16_BIT,
                                 in->config.channels);
}

static audio_channel_mask_t in_get_channels(const struct audio_stream *stream)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;

    if (in->config.channels == 1) {
        return AUDIO_CHANNEL_IN_MONO;
    } else {
        return AUDIO_CHANNEL_IN_STEREO;
    }
}

static audio_format_t in_get_format(const struct audio_stream *stream)
{
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int in_set_format(struct audio_stream *stream, audio_format_t format)
{
    return 0;
}

/* must be called with hw device and input stream mutexes locked */
static int do_input_standby(struct omap_stream_in *in)
{
    struct omap_audio_device *adev = in->dev;

    if (!in->standby) {
        pcm_close(in->pcm);
        in->pcm = NULL;

        adev->active_input = 0;
        if (adev->mode != AUDIO_MODE_IN_CALL) {
            adev->devices.in_devices &= ~AUDIO_DEVICE_IN_ALL;
            select_input_device(adev);
        }

        if (adev->mode == AUDIO_MODE_IN_CALL || !adev->vx_rec_on) {
            set_route_by_array(adev->mixer, vx_rec_ul, 0);
            set_route_by_array(adev->mixer, vx_rec_dl, 0);
            set_route_by_array(adev->mixer, vx_rec_default, 0);
        }

        if (in->echo_reference != NULL) {
            /* stop reading from echo reference */
            in->echo_reference->read(in->echo_reference, NULL);
            put_echo_reference(adev, in->echo_reference);
            in->echo_reference = NULL;
        }
        in->standby = 1;
    }
    return 0;
}

static int in_standby(struct audio_stream *stream)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;
    int status;

    pthread_mutex_lock(&in->dev->lock);
    pthread_mutex_lock(&in->lock);
    status = do_input_standby(in);
    pthread_mutex_unlock(&in->lock);
    pthread_mutex_unlock(&in->dev->lock);
    return status;
}

static int in_dump(const struct audio_stream *stream, int fd)
{
    return 0;
}

static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;
    struct omap_audio_device *adev = in->dev;
    struct str_parms *parms;
    char *str;
    char value[32];
    int ret, val = 0;
    bool do_standby = false;

    parms = str_parms_create_str(kvpairs);

    ret = str_parms_get_str(parms, AUDIO_PARAMETER_STREAM_INPUT_SOURCE, value, sizeof(value));

    pthread_mutex_lock(&adev->lock);
    pthread_mutex_lock(&in->lock);
    if (ret >= 0) {
        val = atoi(value);
        /* no audio source uses val == 0 */
        if ((in->source != val) && (val != 0)) {
            in->source = val;
            do_standby = true;
        }
    }

    ret = str_parms_get_str(parms, AUDIO_PARAMETER_STREAM_ROUTING, value, sizeof(value));
    if (ret >= 0) {
        val = atoi(value);
        if ((in->device != val) && (val != 0)) {
            in->device = val;
            do_standby = true;
        }
    }

    if (do_standby)
        do_input_standby(in);

    pthread_mutex_unlock(&in->lock);
    pthread_mutex_unlock(&adev->lock);

    str_parms_destroy(parms);
    return ret;
}

static char * in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
    return strdup("");
}

static int in_set_gain(struct audio_stream_in *stream, float gain)
{
    return 0;
}

static void get_capture_delay(struct omap_stream_in *in,
                       size_t frames,
                       struct echo_reference_buffer *buffer)
{

    /* read frames available in kernel driver buffer */
    size_t kernel_frames;
    struct timespec tstamp;
    long buf_delay;
    long rsmp_delay;
    long kernel_delay;
    long delay_ns;

    if (pcm_get_htimestamp(in->pcm, &kernel_frames, &tstamp) < 0) {
        buffer->time_stamp.tv_sec  = 0;
        buffer->time_stamp.tv_nsec = 0;
        buffer->delay_ns           = 0;
        ALOGW("read get_capture_delay(): pcm_htimestamp error");
        return;
    }

    /* read frames available in audio HAL input buffer
     * add number of frames being read as we want the capture time of first sample
     * in current buffer */
    buf_delay = (long)(((int64_t)(in->frames_in + in->proc_frames_in) * 1000000000)
                                    / in->config.rate);
    /* add delay introduced by resampler */
    rsmp_delay = 0;
    if (in->resampler) {
        rsmp_delay = in->resampler->delay_ns(in->resampler);
    }

    kernel_delay = (long)(((int64_t)kernel_frames * 1000000000) / in->config.rate);

    delay_ns = kernel_delay + buf_delay + rsmp_delay;

    buffer->time_stamp = tstamp;
    buffer->delay_ns   = delay_ns;
    ALOGV("get_capture_delay time_stamp = [%ld].[%ld], delay_ns: [%d],"
         " kernel_delay:[%ld], buf_delay:[%ld], rsmp_delay:[%ld], kernel_frames:[%d], "
         "in->frames_in:[%d], in->proc_frames_in:[%d], frames:[%d]",
         buffer->time_stamp.tv_sec , buffer->time_stamp.tv_nsec, buffer->delay_ns,
         kernel_delay, buf_delay, rsmp_delay, kernel_frames,
         in->frames_in, in->proc_frames_in, frames);

}

static int32_t update_echo_reference(struct omap_stream_in *in, size_t frames)
{
    struct echo_reference_buffer b;
    b.delay_ns = 0;

    ALOGV("update_echo_reference, frames = [%d], in->ref_frames_in = [%d],  "
          "b.frame_count = [%d]",
         frames, in->ref_frames_in, frames - in->ref_frames_in);
    if (in->ref_frames_in < frames) {
        if (in->ref_buf_size < frames) {
            in->ref_buf_size = frames;
            in->ref_buf = (int16_t *)realloc(in->ref_buf,
                                             in->ref_buf_size *
                                                 in->config.channels * sizeof(int16_t));
        }

        b.frame_count = frames - in->ref_frames_in;
        b.raw = (void *)(in->ref_buf + in->ref_frames_in * in->config.channels);

        get_capture_delay(in, frames, &b);

        if (in->echo_reference->read(in->echo_reference, &b) == 0)
        {
            in->ref_frames_in += b.frame_count;
            ALOGV("update_echo_reference: in->ref_frames_in:[%d], "
                    "in->ref_buf_size:[%d], frames:[%d], b.frame_count:[%d]",
                 in->ref_frames_in, in->ref_buf_size, frames, b.frame_count);
        }
    } else
        ALOGW("update_echo_reference: NOT enough frames to read ref buffer");
    return b.delay_ns;
}

static int set_preprocessor_param(effect_handle_t handle,
                           effect_param_t *param)
{
    uint32_t size = sizeof(int);
    uint32_t psize = ((param->psize - 1) / sizeof(int) + 1) * sizeof(int) +
                        param->vsize;

    int status = (*handle)->command(handle,
                                   EFFECT_CMD_SET_PARAM,
                                   sizeof (effect_param_t) + psize,
                                   param,
                                   &size,
                                   &param->status);
    if (status == 0)
        status = param->status;

    return status;
}

static int set_preprocessor_echo_delay(effect_handle_t handle, int32_t delay_us)
{
    uint32_t buf[sizeof(effect_param_t) / sizeof(uint32_t) + 2];
    effect_param_t *param = (effect_param_t *)buf;

    param->psize = sizeof(uint32_t);
    param->vsize = sizeof(uint32_t);
//    uint32_t ed = AEC_PARAM_ECHO_DELAY ; memcpy(&param->data, &ed, sizeof(uint32_t));
//    memcpy((void*)(&param->data) + sizeof(int32_t), &delay_us, sizeof(int32_t));
    *(uint32_t *)param->data = AEC_PARAM_ECHO_DELAY;
    *((int32_t *)param->data + 1) = delay_us;

    return set_preprocessor_param(handle, param);
}

static int push_echo_reference(struct omap_stream_in *in, size_t frames)
{
    /* read frames from echo reference buffer and update echo delay
     * in->ref_frames_in is updated with frames available in in->ref_buf */
    int32_t delay_us = update_echo_reference(in, frames)/1000;
    int i;
    audio_buffer_t buf;

    if (in->ref_frames_in < frames)
        frames = in->ref_frames_in;

    buf.frameCount = frames;
    buf.raw = in->ref_buf;

    for (i = 0; i < in->num_preprocessors; i++) {
        if ((*in->preprocessors[i])->process_reverse == NULL)
            continue;

        (*in->preprocessors[i])->process_reverse(in->preprocessors[i],
                                               &buf,
                                               NULL);
        set_preprocessor_echo_delay(in->preprocessors[i], delay_us);
    }

    in->ref_frames_in -= buf.frameCount;
    if (in->ref_frames_in) {
        memcpy(in->ref_buf,
               in->ref_buf + buf.frameCount * in->config.channels,
               in->ref_frames_in * in->config.channels * sizeof(int16_t));
    }
    // return the number of frames written to the preprocessor in this call
    return buf.frameCount;
}

static int get_next_buffer(struct resampler_buffer_provider *buffer_provider,
                                   struct resampler_buffer* buffer)
{
    struct omap_stream_in *in;
    struct buffer_remix *remix;
    size_t hw_frame_size;

    if (buffer_provider == NULL || buffer == NULL)
        return -EINVAL;

    in = (struct omap_stream_in *)((char *)buffer_provider -
                                   offsetof(struct omap_stream_in, buf_provider));
    remix = in->remix_at_driver;

    if (in->pcm == NULL) {
        buffer->raw = NULL;
        buffer->frame_count = 0;
        in->read_status = -ENODEV;
        return -ENODEV;
    }

    if (remix)
        hw_frame_size = remix->in_chans * remix->sample_size;
    else
        hw_frame_size = audio_stream_frame_size(&in->stream.common);

    if (in->frames_in == 0) {
        in->read_status = pcm_read(in->pcm,
                                   (void*)in->buffer,
                                   in->config.period_size * hw_frame_size);
        if (in->read_status != 0) {
            ALOGE("get_next_buffer() pcm_read error %d", in->read_status);
            buffer->raw = NULL;
            buffer->frame_count = 0;
            return in->read_status;
        }
        in->frames_in = in->config.period_size;

        if (remix)
            remix->remix_func(remix, in->buffer, in->frames_in);
    }

    buffer->frame_count = (buffer->frame_count > in->frames_in) ?
                                in->frames_in : buffer->frame_count;
    buffer->i16 = in->buffer + (in->config.period_size - in->frames_in) *
                                                in->config.channels;

    return in->read_status;
}

static void release_buffer(struct resampler_buffer_provider *buffer_provider,
                                  struct resampler_buffer* buffer)
{
    struct omap_stream_in *in;

    if (buffer_provider == NULL || buffer == NULL)
        return;

    in = (struct omap_stream_in *)((char *)buffer_provider -
                                   offsetof(struct omap_stream_in, buf_provider));

    in->frames_in -= buffer->frame_count;
}

/* read_frames() reads frames from kernel driver, down samples to capture rate
 * if necessary and output the number of frames requested to the buffer specified */
static ssize_t read_frames(struct omap_stream_in *in, void *buffer, ssize_t frames)
{
    ssize_t frames_wr = 0;
    size_t frame_size;

    if (in->remix_at_driver)
        frame_size = in->remix_at_driver->out_chans * in->remix_at_driver->sample_size;
    else
        frame_size = audio_stream_frame_size(&in->stream.common);

    while (frames_wr < frames) {
        size_t frames_rd = frames - frames_wr;
        if (in->resampler != NULL) {
            in->resampler->resample_from_provider(in->resampler,
                    (int16_t *)((char *)buffer + frames_wr * frame_size),
                    &frames_rd);
        } else {
            struct resampler_buffer buf = {
                    { raw : NULL, },
                    frame_count : frames_rd,
            };
            get_next_buffer(&in->buf_provider, &buf);
            if (buf.raw != NULL) {
                memcpy((char *)buffer +
                        frames_wr * frame_size,
                        buf.raw,
                        buf.frame_count * frame_size);
                frames_rd = buf.frame_count;
            }
            release_buffer(&in->buf_provider, &buf);
        }
        /* in->read_status is updated by getNextBuffer() also called by
         * in->resampler->resample_from_provider() */
        if (in->read_status != 0)
            return in->read_status;

        frames_wr += frames_rd;
    }
    return frames_wr;
}

/* process_frames() reads frames from kernel driver (via read_frames()),
 * calls the active audio pre processings and output the number of frames requested
 * to the buffer specified */
static ssize_t process_frames(struct omap_stream_in *in, void* buffer, ssize_t frames)
{
    ssize_t frames_wr = 0;
    audio_buffer_t in_buf;
    audio_buffer_t out_buf;
    int i;
    ssize_t prepro_wr_cnt;

    while (frames_wr < frames) {
        /* first reload enough frames at the end of process input buffer */
        if (in->proc_frames_in < (size_t)frames) {
            ssize_t frames_rd;

            if (in->proc_buf_size < (size_t)frames) {
                in->proc_buf_size = (size_t)frames;
                in->proc_buf = (int16_t *)realloc(in->proc_buf,
                                         in->proc_buf_size *
                                             in->config.channels * sizeof(int16_t));
                ALOGV("process_frames(): in->proc_buf %p size extended to %d frames",
                     in->proc_buf, in->proc_buf_size);
            }
            frames_rd = read_frames(in,
                                    in->proc_buf +
                                        in->proc_frames_in * in->config.channels,
                                    frames - in->proc_frames_in);
            if (frames_rd < 0) {
                frames_wr = frames_rd;
                break;
            }
            in->proc_frames_in += frames_rd;
        }

        /* make sure that the number of mic signal frames written to the
         * preprocessor is the same as the number of playback signal frames
         * sent */
        if (in->echo_reference != NULL) {
            prepro_wr_cnt = push_echo_reference(in, in->proc_frames_in);
        } else {
            prepro_wr_cnt = in->proc_frames_in;
        }

        /* in_buf.frameCount and out_buf.frameCount indicate respectively the
         * maximum number of frames to be consumed and produced by process() */
        in_buf.frameCount = prepro_wr_cnt;
        in_buf.s16 = in->proc_buf;
        out_buf.frameCount = frames - frames_wr;
        out_buf.s16 = (int16_t *)buffer + frames_wr * in->config.channels;

        for (i = 0; i < in->num_preprocessors; i++)
            (*in->preprocessors[i])->process(in->preprocessors[i],
                                               &in_buf,
                                               &out_buf);

        /* process() has updated the number of frames consumed and produced in
         * in_buf.frameCount and out_buf.frameCount respectively
         * move remaining frames to the beginning of in->proc_buf */
        in->proc_frames_in -= in_buf.frameCount;
        if (in->proc_frames_in) {
            memcpy(in->proc_buf,
                   in->proc_buf + in_buf.frameCount * in->config.channels,
                   in->proc_frames_in * in->config.channels * sizeof(int16_t));
        }

        /* if not enough frames were passed to process(), read more and retry. */
        if (out_buf.frameCount == 0)
            continue;

        frames_wr += out_buf.frameCount;
    }
    return frames_wr;
}

static ssize_t in_read(struct audio_stream_in *stream, void* buffer, size_t bytes)
{
    int ret = 0;
    struct omap_stream_in *in = (struct omap_stream_in *)stream;
    struct omap_audio_device *adev = in->dev;
    size_t frames_rq = bytes / audio_stream_frame_size(&stream->common);

do_over:
    if (in->standby) {
        /* adev must be locked when manipulating standby state.
         * However, only lock it if we really need it.  Otherwise we
         * could be delayed by an output stream shutting down, causing
         * an xrun in the middle of this stream.
         */
        pthread_mutex_lock(&adev->lock);
        pthread_mutex_lock(&in->lock);
        if (in->standby) {
            ret = start_input_stream(in);
            if (ret == 0)
                in->standby = 0;
        }
        pthread_mutex_unlock(&adev->lock);
    } else {
        pthread_mutex_lock(&in->lock);
        /* re-check */
        if (in->standby) {
            pthread_mutex_unlock(&in->lock);
            goto do_over;
        }
    }

    if (ret < 0)
        goto exit;

    if (in->num_preprocessors != 0)
        ret = process_frames(in, buffer, frames_rq);
    else if (in->resampler != NULL || in->remix_at_driver)
        ret = read_frames(in, buffer, frames_rq);
    else
        ret = pcm_read(in->pcm, buffer, bytes);

    if (ret > 0)
        ret = 0;

    if (ret == 0 && adev->mic_mute)
        memset(buffer, 0, bytes);

exit:
    if (ret < 0)
        usleep(bytes * 1000000 / audio_stream_frame_size(&stream->common) /
               in_get_sample_rate(&stream->common));

    pthread_mutex_unlock(&in->lock);
    return bytes;
}

static uint32_t in_get_input_frames_lost(struct audio_stream_in *stream)
{
    return 0;
}

static int in_add_audio_effect(const struct audio_stream *stream,
                               effect_handle_t effect)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;
    int status;
    effect_descriptor_t desc;

    pthread_mutex_lock(&in->dev->lock);
    pthread_mutex_lock(&in->lock);
    if (in->num_preprocessors >= MAX_PREPROCESSORS) {
        status = -ENOSYS;
        goto exit;
    }

    status = (*effect)->get_descriptor(effect, &desc);
    if (status != 0)
        goto exit;

    in->preprocessors[in->num_preprocessors++] = effect;

    if (memcmp(&desc.type, FX_IID_AEC, sizeof(effect_uuid_t)) == 0) {
        in->need_echo_reference = true;
        do_input_standby(in);
    }

exit:

    pthread_mutex_unlock(&in->lock);
    pthread_mutex_unlock(&in->dev->lock);
    return status;
}

static int in_remove_audio_effect(const struct audio_stream *stream,
                                  effect_handle_t effect)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;
    int i;
    int status = -EINVAL;
    bool found = false;
    effect_descriptor_t desc;

    pthread_mutex_lock(&in->dev->lock);
    pthread_mutex_lock(&in->lock);
    if (in->num_preprocessors <= 0) {
        status = -ENOSYS;
        goto exit;
    }

    for (i = 0; i < in->num_preprocessors; i++) {
        if (found) {
            in->preprocessors[i - 1] = in->preprocessors[i];
            continue;
        }
        if (in->preprocessors[i] == effect) {
            in->preprocessors[i] = NULL;
            status = 0;
            found = true;
        }
    }

    if (status != 0)
        goto exit;

    in->num_preprocessors--;

    status = (*effect)->get_descriptor(effect, &desc);
    if (status != 0)
        goto exit;
    if (memcmp(&desc.type, FX_IID_AEC, sizeof(effect_uuid_t)) == 0) {
        in->need_echo_reference = false;
        do_input_standby(in);
    }

exit:

    pthread_mutex_unlock(&in->lock);
    pthread_mutex_unlock(&in->dev->lock);
    return status;
}

static int adev_open_output_stream(struct audio_hw_device *dev,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   struct audio_stream_out **stream_out)
{
    struct omap_audio_device *ladev = (struct omap_audio_device *)dev;
    struct omap_stream_out *out;
    int ret;

    out = (struct omap_stream_out *)calloc(1, sizeof(struct omap_stream_out));
    if (!out)
        return -ENOMEM;
    if (devices & AUDIO_DEVICE_OUT_ALL_SCO) {
        ret = create_resampler(DEFAULT_OUT_SAMPLING_RATE,
                MM_FULL_POWER_SAMPLING_RATE,
                2,
                RESAMPLER_QUALITY_DEFAULT,
                NULL,
                &out->resampler);
        if (ret != 0)
            goto err_open;
        out->buffer = malloc(RESAMPLER_BUFFER_SIZE); /* todo: allow for reallocing */
    } else
       out->resampler = NULL;

    out->stream.common.get_sample_rate = out_get_sample_rate;
    out->stream.common.set_sample_rate = out_set_sample_rate;
    out->stream.common.get_buffer_size = out_get_buffer_size;
    out->stream.common.get_channels = out_get_channels;
    out->stream.common.get_format = out_get_format;
    out->stream.common.set_format = out_set_format;
    out->stream.common.standby = out_standby;
    out->stream.common.dump = out_dump;
    out->stream.common.set_parameters = out_set_parameters;
    out->stream.common.get_parameters = out_get_parameters;
    out->stream.common.add_audio_effect = out_add_audio_effect;
    out->stream.common.remove_audio_effect = out_remove_audio_effect;
    out->stream.get_latency = out_get_latency;
    out->stream.set_volume = out_set_volume;
    out->stream.write = out_write;
    out->stream.get_render_position = out_get_render_position;

    out->config = pcm_config_mm;

    out->dev = ladev;
    out->standby = 1;

    /* FIXME: when we support multiple output devices, we will want to
     * do the following:
     * adev->out_device = out->device;
     * select_output_device(adev);
     * This is because out_set_parameters() with a route is not
     * guaranteed to be called after an output stream is opened. */

    config->format = out_get_format(&out->stream.common);
    config->channel_mask = out_get_channels(&out->stream.common);
    config->sample_rate = out_get_sample_rate(&out->stream.common);

    *stream_out = &out->stream;
    return 0;

err_open:
    free(out);
    *stream_out = NULL;
    return ret;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
                                     struct audio_stream_out *stream)
{
    struct omap_stream_out *out = (struct omap_stream_out *)stream;

    out_standby(&stream->common);
    if (out->buffer)
        free(out->buffer);
    if (out->resampler)
        release_resampler(out->resampler);
    free(stream);
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)dev;
    struct str_parms *parms;
    char *str;
    char value[32];
    int ret;

    parms = str_parms_create_str(kvpairs);
    ret = str_parms_get_str(parms, AUDIO_PARAMETER_KEY_TTY_MODE, value, sizeof(value));
    if (ret >= 0) {
        int tty_mode;

        if (strcmp(value, AUDIO_PARAMETER_VALUE_TTY_OFF) == 0)
            tty_mode = TTY_MODE_OFF;
        else if (strcmp(value, AUDIO_PARAMETER_VALUE_TTY_VCO) == 0)
            tty_mode = TTY_MODE_VCO;
        else if (strcmp(value, AUDIO_PARAMETER_VALUE_TTY_HCO) == 0)
            tty_mode = TTY_MODE_HCO;
        else if (strcmp(value, AUDIO_PARAMETER_VALUE_TTY_FULL) == 0)
            tty_mode = TTY_MODE_FULL;
        else
            return -EINVAL;

        pthread_mutex_lock(&adev->lock);
        if (tty_mode != adev->tty_mode) {
            adev->tty_mode = tty_mode;
            if (adev->mode == AUDIO_MODE_IN_CALL)
                select_output_device(adev);
        }
        pthread_mutex_unlock(&adev->lock);
    }

    ret = str_parms_get_str(parms, AUDIO_PARAMETER_KEY_BT_NREC, value, sizeof(value));
    if (ret >= 0) {
        if (strcmp(value, AUDIO_PARAMETER_VALUE_ON) == 0)
            adev->bluetooth_nrec = true;
        else
            adev->bluetooth_nrec = false;

    }

    ret = str_parms_get_str(parms, "screen_state", value, sizeof(value));
    if (ret >= 0) {
        if (strcmp(value, AUDIO_PARAMETER_VALUE_ON) == 0)
            adev->low_power = false;
        else
            adev->low_power = true;
    }

    str_parms_destroy(parms);
    return ret;
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
    return strdup("");
}

static int adev_init_check(const struct audio_hw_device *dev)
{
    return 0;
}

static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)dev;

    pthread_mutex_lock(&adev->lock);

    if (adev->voice_volume != volume) {
        adev->voice_volume = volume;
        set_voice_volume(&adev->hw_device, volume);
    }
    pthread_mutex_unlock(&adev->lock);
    return 0;
}

static int set_voice_volume(struct audio_hw_device *dev, float volume)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)dev;

    if (false == adev->modem) {
        return 0;
    }
    enum ril_sound_type sound_type;

    /* in-call output devices are maintained in adev->devices */
    if (adev->mode == AUDIO_MODE_IN_CALL) {
        switch (adev->devices.out_devices & AUDIO_DEVICE_OUT_ALL) {
            case AUDIO_DEVICE_OUT_EARPIECE:
            default:
                sound_type = SOUND_TYPE_VOICE;
                break;
            case AUDIO_DEVICE_OUT_SPEAKER:
                sound_type = SOUND_TYPE_SPEAKER;
                break;
            case AUDIO_DEVICE_OUT_WIRED_HEADSET:
            case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
                sound_type = SOUND_TYPE_HEADSET;
                break;
            case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
            case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
            case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
                sound_type = SOUND_TYPE_BTVOICE;
                break;
        }
        ril_set_call_volume(&adev->ril, sound_type, volume);
    }

    return 0;
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev, audio_mode_t mode)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)dev;

    pthread_mutex_lock(&adev->lock);
    if (adev->mode != mode) {
        adev->mode = mode;
        select_mode(adev);
    }
    pthread_mutex_unlock(&adev->lock);

    return 0;
}

static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)dev;
    unsigned int channel;

    if (adev->mode == AUDIO_MODE_IN_CALL) {
        for (channel = 0; channel < mixer_ctl_get_num_values(
                adev->mixer_ctls.voice_ul_volume); channel++) {
            mixer_ctl_set_value(adev->mixer_ctls.voice_ul_volume,
                   channel, (state ? 0 : MIXER_ABE_GAIN_0DB));
        }
    }
    else {
        for (channel = 0; channel < mixer_ctl_get_num_values(
                adev->mixer_ctls.amic_ul_volume); channel++) {
            mixer_ctl_set_value(adev->mixer_ctls.amic_ul_volume,
                   channel, (state ? 0 : MIXER_ABE_GAIN_0DB));
        }
        for (channel = 0; channel < mixer_ctl_get_num_values(
                adev->mixer_ctls.dmic1_ul_volume); channel++) {
            mixer_ctl_set_value(adev->mixer_ctls.dmic1_ul_volume,
                   channel, (state ? 0 : MIXER_ABE_GAIN_0DB));
        }
        for (channel = 0; channel < mixer_ctl_get_num_values(
                adev->mixer_ctls.dmic2_ul_volume); channel++) {
            mixer_ctl_set_value(adev->mixer_ctls.dmic2_ul_volume,
                   channel, (state ? 0 : MIXER_ABE_GAIN_0DB));
        }
        for (channel = 0; channel < mixer_ctl_get_num_values(
                adev->mixer_ctls.dmic3_ul_volume); channel++) {
            mixer_ctl_set_value(adev->mixer_ctls.dmic3_ul_volume,
                   channel, (state ? 0 : MIXER_ABE_GAIN_0DB));
        }
        for (channel = 0; channel < mixer_ctl_get_num_values(
                adev->mixer_ctls.bt_ul_volume); channel++) {
            mixer_ctl_set_value(adev->mixer_ctls.bt_ul_volume,
                   channel, (state ? 0 : DB_TO_ABE_GAIN(6)));
        }
    }

    adev->mic_mute = state;

    return 0;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)dev;

    *state = adev->mic_mute;

    return 0;
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                         const struct audio_config *config)
{
    int channel_count = popcount(config->channel_mask);
    if (check_input_parameters(config->sample_rate, config->format, channel_count) != 0) {
        return 0;
    }

    return get_input_buffer_size(config->sample_rate, config->format, channel_count);
}

static int adev_open_input_stream(struct audio_hw_device *dev,
                                  audio_io_handle_t handle,
                                  audio_devices_t devices,
                                  struct audio_config *config,
                                  struct audio_stream_in **stream_in)
{
    struct omap_audio_device *ladev = (struct omap_audio_device *)dev;
    struct omap_stream_in *in;
    int ret;
    int channel_count = popcount(config->channel_mask);
    /*audioflinger expects return variable to be NULL incase of failure */
    *stream_in = NULL;

    if (check_input_parameters(config->sample_rate, config->format, channel_count) != 0)
        return -EINVAL;

    in = (struct omap_stream_in *)calloc(1, sizeof(struct omap_stream_in));
    if (!in)
        return -ENOMEM;

    in->stream.common.get_sample_rate = in_get_sample_rate;
    in->stream.common.set_sample_rate = in_set_sample_rate;
    in->stream.common.get_buffer_size = in_get_buffer_size;
    in->stream.common.get_channels = in_get_channels;
    in->stream.common.get_format = in_get_format;
    in->stream.common.set_format = in_set_format;
    in->stream.common.standby = in_standby;
    in->stream.common.dump = in_dump;
    in->stream.common.set_parameters = in_set_parameters;
    in->stream.common.get_parameters = in_get_parameters;
    in->stream.common.add_audio_effect = in_add_audio_effect;
    in->stream.common.remove_audio_effect = in_remove_audio_effect;
    in->stream.set_gain = in_set_gain;
    in->stream.read = in_read;
    in->stream.get_input_frames_lost = in_get_input_frames_lost;
    in->remix_at_driver = NULL;

    in->requested_rate = config->sample_rate;

    memcpy(&in->config, &pcm_config_mm, sizeof(pcm_config_mm));
    in->config.channels = channel_count;

    in->buffer = malloc(2 * in->config.period_size * audio_stream_frame_size(&in->stream.common));
    if (!in->buffer) {
        ret = -ENOMEM;
        goto err;
    }

    if (in->requested_rate != in->config.rate) {
        in->buf_provider.get_next_buffer = get_next_buffer;
        in->buf_provider.release_buffer = release_buffer;
        ret = create_resampler(in->config.rate,
                               in->requested_rate,
                               in->config.channels,
                               RESAMPLER_QUALITY_DEFAULT,
                               &in->buf_provider,
                               &in->resampler);
        if (ret != 0) {
            ret = -EINVAL;
            goto err;
        }
    }

    in->dev = ladev;
    in->standby = 1;
    in->device = devices & ~AUDIO_DEVICE_BIT_IN;

    *stream_in = &in->stream;
    return 0;

err:
    if (in->resampler)
        release_resampler(in->resampler);

    free(in);
    *stream_in = NULL;
    return ret;
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *stream)
{
    struct omap_stream_in *in = (struct omap_stream_in *)stream;
    struct omap_audio_device *adev = (struct omap_audio_device *)dev;

    in_standby(&stream->common);

    if (in->resampler) {
        free(in->buffer);
        release_resampler(in->resampler);
    }

    if (in->remix_at_driver)
        free(in->remix_at_driver);

    free(stream);
    return;
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
    return 0;
}

static int adev_close(hw_device_t *device)
{
    struct omap_audio_device *adev = (struct omap_audio_device *)device;

    /* RIL */
    if (adev->modem) {
        ril_close(&adev->ril);
    }
    mixer_close(adev->mixer);
    free(device);
    return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct omap_audio_device *adev;
    int ret;
    pthread_mutexattr_t mta;
    char modem[PROPERTY_VALUE_MAX];

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    adev = calloc(1, sizeof(struct omap_audio_device));
    if (!adev)
        return -ENOMEM;

    property_get("modem.audio", modem, "0");
    if (!strcmp(modem, "1")) {
        adev->modem = true;
    } else {
        adev->modem = false;
    }

    adev->hw_device.common.tag = HARDWARE_DEVICE_TAG;
    adev->hw_device.common.version = AUDIO_DEVICE_API_VERSION_CURRENT;
    adev->hw_device.common.module = (struct hw_module_t *) module;
    adev->hw_device.common.close = adev_close;

    adev->hw_device.init_check = adev_init_check;
    adev->hw_device.set_voice_volume = adev_set_voice_volume;
    adev->hw_device.set_master_volume = adev_set_master_volume;
    adev->hw_device.set_mode = adev_set_mode;
    adev->hw_device.set_mic_mute = adev_set_mic_mute;
    adev->hw_device.get_mic_mute = adev_get_mic_mute;
    adev->hw_device.set_parameters = adev_set_parameters;
    adev->hw_device.get_parameters = adev_get_parameters;
    adev->hw_device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->hw_device.open_output_stream = adev_open_output_stream;
    adev->hw_device.close_output_stream = adev_close_output_stream;
    adev->hw_device.open_input_stream = adev_open_input_stream;
    adev->hw_device.close_input_stream = adev_close_input_stream;
    adev->hw_device.dump = adev_dump;

    adev->mixer = mixer_open(0);
    if (!adev->mixer) {
        free(adev);
        ALOGE("Unable to open the mixer, aborting.");
        return -EINVAL;
    }

    adev->mixer_ctls.dl1_eq = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_EQUALIZER);
    adev->mixer_ctls.mm_dl1 = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_MIXER_MULTIMEDIA);
    adev->mixer_ctls.mm2_dl1_capture = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_MIXER_CAPTURE);
    adev->mixer_ctls.vx_dl1 = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_MIXER_VOICE);
    adev->mixer_ctls.mm_dl2 = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL2_MIXER_MULTIMEDIA);
    adev->mixer_ctls.vx_dl2 = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL2_MIXER_VOICE);
    adev->mixer_ctls.dl2_mono = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL2_MONO_MIXER);
    adev->mixer_ctls.dl1_handsfree = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_PDM2_SWITCH);
    adev->mixer_ctls.dl1_mono = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_MONO_MIXER);
    adev->mixer_ctls.dl1_headset = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_PDM_SWITCH);
    adev->mixer_ctls.dl1_bt = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_BT_VX_SWITCH);
    adev->mixer_ctls.earpiece_enable = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_EARPHONE_ENABLE_SWITCH);
    adev->mixer_ctls.left_capture = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_ANALOG_LEFT_CAPTURE_ROUTE);
    adev->mixer_ctls.right_capture = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_ANALOG_RIGHT_CAPTURE_ROUTE);
    adev->mixer_ctls.amic_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_AMIC_UL_VOLUME);
    adev->mixer_ctls.dmic1_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DMIC1_UL_VOLUME);
    adev->mixer_ctls.dmic2_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DMIC2_UL_VOLUME);
    adev->mixer_ctls.dmic3_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DMIC3_UL_VOLUME);
    adev->mixer_ctls.bt_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_BT_UL_VOLUME);
    adev->mixer_ctls.voice_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_AUDUL_VOICE_UL_VOLUME);
    adev->mixer_ctls.sidetone_capture = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_SIDETONE_MIXER_CAPTURE);
    adev->mixer_ctls.headset_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_HEADSET_PLAYBACK_VOLUME);
    adev->mixer_ctls.speaker_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_HANDSFREE_PLAYBACK_VOLUME);

    if (adev->mixer_ctls.mm_dl2 && adev->mixer_ctls.vx_dl2 &&
        adev->mixer_ctls.dl2_mono) {
        adev->dl2_support = true;
    } else if (adev->mixer_ctls.dl1_handsfree) {
        adev->dl2_support = false;
    } else {
        mixer_close(adev->mixer);
        free(adev);
        ALOGE("Unable to identify ABE downlink capabilities, aborting.");
        return -EINVAL;
    }

    if (!adev->mixer_ctls.mm_dl1 ||
        !adev->mixer_ctls.vx_dl1 ||
        !adev->mixer_ctls.dl1_mono ||
        !adev->mixer_ctls.dl1_headset ||
        !adev->mixer_ctls.dl1_bt ||
        !adev->mixer_ctls.earpiece_enable ||
        !adev->mixer_ctls.left_capture ||
        !adev->mixer_ctls.right_capture ||
        !adev->mixer_ctls.amic_ul_volume ||
        !adev->mixer_ctls.sidetone_capture ||
        !adev->mixer_ctls.headset_volume ||
        !adev->mixer_ctls.speaker_volume ||
        !adev->mixer_ctls.dmic1_ul_volume ||
        !adev->mixer_ctls.dl1_eq ||
        !adev->mixer_ctls.voice_ul_volume ||
        !adev->mixer_ctls.mm2_dl1_capture) {
#if 0
        mixer_close(adev->mixer);
        free(adev);
        ALOGE("Unable to locate all mixer controls, aborting.");
        return -EINVAL;
#endif
    }

    pthread_mutexattr_init(&mta);
    if (adev->modem) {
        /* need to set attribute to Error check to detect deadlock usefull
        * for the callback function audio_set_wb_amr_callback */
        pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_ERRORCHECK);
    } else {
        pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_NORMAL);
    }
    pthread_mutex_init(&adev->lock, &mta);
    pthread_mutexattr_destroy(&mta);

    /* Set the default route before the PCM stream is opened */
    pthread_mutex_lock(&adev->lock);

    set_route_by_array(adev->mixer, defaults, 1);
    if (adev->dl2_support) {
        set_route_by_array(adev->mixer, hf_dl2, 1);
    } else {
        set_route_by_array(adev->mixer, hf_dl1, 1);
    }
    adev->mode = AUDIO_MODE_NORMAL;
    adev->devices.out_devices = AUDIO_DEVICE_OUT_SPEAKER;
    adev->devices.in_devices = AUDIO_DEVICE_IN_BUILTIN_MIC & ~AUDIO_DEVICE_BIT_IN;
    select_output_device(adev);

    adev->pcm_modem_dl = NULL;
    adev->pcm_modem_ul = NULL;
    adev->voice_volume = 1.0f;
    adev->tty_mode = TTY_MODE_OFF;

    if (get_boardtype(adev)) {
        pthread_mutex_unlock(&adev->lock);
        mixer_close(adev->mixer);
        free(adev);
        ALOGE("Unsupported boardtype, aborting.");
        return -EINVAL;
    }

    adev->input_requires_stereo = 0;
    adev->bluetooth_nrec = true;
    adev->vx_rec_on = false;
    if (adev->modem) {
        adev->wb_amr = 0;

        /* RIL */
        ril_open(&adev->ril);
    }
    pthread_mutex_unlock(&adev->lock);
    if (adev->modem) {
        /* register callback for wideband AMR setting */
        ril_register_set_wb_amr_callback(audio_set_wb_amr_callback, (void *)adev);
    }
    *device = &adev->hw_device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "OMAP audio HW HAL",
        .author = "Texas Instruments",
        .methods = &hal_module_methods,
    },
};
