#!/bin/bash

# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

DEVICE=acclaim
MANUFACTURER=bn

BN_ACC_PROP=../../../vendor/$MANUFACTURER/$DEVICE/proprietary

mkdir -p $BN_ACC_PROP

# gfx
mkdir -p $BN_ACC_PROP/gfx
mkdir -p $BN_ACC_PROP/gfx/egl
mkdir -p $BN_ACC_PROP/gfx/hw
mkdir -p $BN_ACC_PROP/gfx/bin

adb pull /system/vendor/lib/egl/libEGL_POWERVR_SGX540_120.so $BN_ACC_PROP/gfx/egl/libEGL_POWERVR_SGX540_120.so
adb pull /system/vendor/lib/egl/libGLESv1_CM_POWERVR_SGX540_120.so $BN_ACC_PROP/gfx/egl/libGLESv1_CM_POWERVR_SGX540_120.so
adb pull /system/vendor/lib/egl/libGLESv2_POWERVR_SGX540_120.so $BN_ACC_PROP/gfx/egl/libGLESv2_POWERVR_SGX540_120.so

adb pull /system/vendor/lib/hw/gralloc.omap4.so $BN_ACC_PROP/gfx/hw/gralloc.omap4.so

adb pull /system/vendor/lib/libIMGegl.so $BN_ACC_PROP/gfx/libIMGegl.so
adb pull /system/vendor/lib/libPVRScopeServices.so $BN_ACC_PROP/gfx/libPVRScopeServices.so
adb pull /system/vendor/lib/libglslcompiler.so $BN_ACC_PROP/gfx/libglslcompiler.so
adb pull /system/vendor/lib/libpvr2d.so $BN_ACC_PROP/gfx/libpvr2d.so
adb pull /system/vendor/lib/libpvrANDROID_WSEGL.so $BN_ACC_PROP/gfx/libpvrANDROID_WSEGL.so
adb pull /system/vendor/lib/libsrv_init.so $BN_ACC_PROP/gfx/libsrv_init.so
adb pull /system/vendor/lib/libsrv_um.so $BN_ACC_PROP/gfx/libsrv_um.so
adb pull /system/vendor/lib/libusc.so $BN_ACC_PROP/gfx/libusc.so

adb pull /system/bin/pvrsrvinit $BN_ACC_PROP/gfx/bin/pvrsrvinit

# sound libs
mkdir -p $BN_ACC_PROP/audio
mkdir -p $BN_ACC_PROP/audio/hw

adb pull /system/lib/libaudio.so $BN_ACC_PROP/audio/libaudio.so
adb pull /system/lib/libaudiopolicy.so $BN_ACC_PROP/audio/libaudiopolicy.so
adb pull /system/lib/hw/alsa.omap4.so $BN_ACC_PROP/audio/hw/alsa.omap4.so
adb pull /system/lib/libasound.so $BN_ACC_PROP/audio/libasound.so
adb pull /system/lib/liba2dp.so $BN_ACC_PROP/audio/liba2dp.so

# sound conf
BN_ACC_ALSA_CONF=$BN_ACC_PROP/audio/conf
mkdir -p $BN_ACC_ALSA_CONF
mkdir -p $BN_ACC_ALSA_CONF/etc
mkdir -p $BN_ACC_ALSA_CONF/cards
mkdir -p $BN_ACC_ALSA_CONF/pcm

adb pull /system/etc/asound.conf $BN_ACC_ALSA_CONF/etc/asound.conf
adb pull /system/usr/share/alsa/alsa.conf $BN_ACC_ALSA_CONF/alsa.conf
adb pull /system/usr/share/alsa/cards/aliases.conf $BN_ACC_ALSA_CONF/cards/aliases.conf
adb pull /system/usr/share/alsa/pcm/center_lfe.conf $BN_ACC_ALSA_CONF/pcm/center_lfe.conf
adb pull /system/usr/share/alsa/pcm/default.conf $BN_ACC_ALSA_CONF/pcm/default.conf
adb pull /system/usr/share/alsa/pcm/dmix.conf $BN_ACC_ALSA_CONF/pcm/dmix.conf
adb pull /system/usr/share/alsa/pcm/dpl.conf $BN_ACC_ALSA_CONF/pcm/dpl.conf
adb pull /system/usr/share/alsa/pcm/dsnoop.conf $BN_ACC_ALSA_CONF/pcm/dsnoop.conf
adb pull /system/usr/share/alsa/pcm/front.conf $BN_ACC_ALSA_CONF/pcm/front.conf
adb pull /system/usr/share/alsa/pcm/iec958.conf $BN_ACC_ALSA_CONF/pcm/iec958.conf
adb pull /system/usr/share/alsa/pcm/modem.conf $BN_ACC_ALSA_CONF/pcm/modem.conf
adb pull /system/usr/share/alsa/pcm/rear.conf $BN_ACC_ALSA_CONF/pcm/rear.conf
adb pull /system/usr/share/alsa/pcm/side.conf $BN_ACC_ALSA_CONF/pcm/side.conf
adb pull /system/usr/share/alsa/pcm/surround40.conf $BN_ACC_ALSA_CONF/pcm/surround40.conf
adb pull /system/usr/share/alsa/pcm/surround41.conf $BN_ACC_ALSA_CONF/pcm/surround41.conf
adb pull /system/usr/share/alsa/pcm/surround50.conf $BN_ACC_ALSA_CONF/pcm/surround50.conf
adb pull /system/usr/share/alsa/pcm/surround51.conf $BN_ACC_ALSA_CONF/pcm/surround51.conf
adb pull /system/usr/share/alsa/pcm/surround71.conf $BN_ACC_ALSA_CONF/pcm/surround71.conf


./setup-makefiles.sh