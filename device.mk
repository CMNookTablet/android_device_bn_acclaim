#
# Copyright (C) 2011 The Android Open-Source Project
# Copyright (C) 2011 The CyanogenMod Project
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
#

# This file includes all definitions that apply to ALL tuna devices, and
# are also specific to tuna devices
#
# Everything in this directory will become public

ifeq ($(TARGET_PREBUILT_KERNEL),)
	LOCAL_KERNEL := device/bn/acclaim/kernel
else
	LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES := \
	$(LOCAL_KERNEL):kernel \
	device/bn/acclaim/init.acclaim.rc:root/init.acclaim.rc \
	device/bn/acclaim/init.acclaim.usb.rc:root/init.acclaim.usb.rc \
	device/bn/acclaim/ueventd.acclaim.rc:root/ueventd.acclaim.rc

# Graphics
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/egl/libEGL_POWERVR_SGX540_120.so:system/vendor/lib/egl/libEGL_POWERVR_SGX540_120.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/egl/libGLESv1_CM_POWERVR_SGX540_120.so:system/vendor/lib/egl/libGLESv1_CM_POWERVR_SGX540_120.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/egl/libGLESv2_POWERVR_SGX540_120.so:system/vendor/lib/egl/libGLESv2_POWERVR_SGX540_120.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/hw/gralloc.omap4.so:system/vendor/lib/hw/gralloc.omap4.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libIMGegl.so:system/vendor/lib/libIMGegl.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libPVRScopeServices.so:system/vendor/lib/libPVRScopeServices.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libglslcompiler.so:system/vendor/lib/libglslcompiler.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libpvr2d.so:system/vendor/lib/libpvr2d.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libpvrANDROID_WSEGL.so:system/vendor/lib/libpvrANDROID_WSEGL.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libsrv_init.so:system/vendor/lib/libsrv_init.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libsrv_um.so:system/vendor/lib/libsrv_um.so \
	device/bn/acclaim/prebuilt/imgtec/vendor/lib/libusc.so:system/vendor/lib/libusc.so \
	device/bn/acclaim/prebuilt/imgtec/bin/pvrsrvinit:system/vendor/bin/pvrsrvinit 

# Input
PRODUCT_COPY_FILES += \
	device/bn/acclaim/ft5x06-i2c.idc:system/usr/idc/ft5x06-i2c.idc \
	device/bn/acclaim/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl

# Misc
PRODUCT_COPY_FILES += \
	device/bn/acclaim/clrbootcount.sh:system/bin/clrbootcount.sh

# Wifi
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/wifi/tiwlan_drv.ko:/system/lib/modules/tiwlan_drv.ko \
	device/bn/acclaim/prebuilt/wifi/tiwlan.ini:/system/etc/wifi/tiwlan.ini \
	device/bn/acclaim/prebuilt/wifi/firmware.bin:/system/etc/wifi/firmware.bin \

# fwram
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/fwram.ko:/system/lib/modules/fwram.ko

# audio
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/audio/libaudio.so:/system/lib/libaudio.so \
	device/bn/acclaim/prebuilt/audio/alsa.omap4.so:/system/lib/hw/alsa.omap4.so \
	device/bn/acclaim/prebuilt/audio/libasound.so:/system/lib/libasound.so \
	device/bn/acclaim/prebuilt/audio/asound.conf:/system/etc/asound.conf \
	device/bn/acclaim/prebuilt/audio/alsa.conf:/system/usr/share/alsa/alsa.conf \
	device/bn/acclaim/prebuilt/audio/cards/aliases.conf:/system/usr/share/alsa/cards/aliases.conf \
	device/bn/acclaim/prebuilt/audio/pcm/center_lfe.conf:/system/usr/share/alsa/pcm/center_lfe.conf \
	device/bn/acclaim/prebuilt/audio/pcm/default.conf:/system/usr/share/alsa/pcm/default.conf \
	device/bn/acclaim/prebuilt/audio/pcm/dmix.conf:/system/usr/share/alsa/pcm/dmix.conf \
	device/bn/acclaim/prebuilt/audio/pcm/dpl.conf:/system/usr/share/alsa/pcm/dpl.conf \
	device/bn/acclaim/prebuilt/audio/pcm/dsnoop.conf:/system/usr/share/alsa/pcm/dsnoop.conf \
	device/bn/acclaim/prebuilt/audio/pcm/front.conf:/system/usr/share/alsa/pcm/front.conf \
	device/bn/acclaim/prebuilt/audio/pcm/iec958.conf:/system/usr/share/alsa/pcm/iec958.conf \
	device/bn/acclaim/prebuilt/audio/pcm/modem.conf:/system/usr/share/alsa/pcm/modem.conf \
	device/bn/acclaim/prebuilt/audio/pcm/rear.conf:/system/usr/share/alsa/pcm/rear.conf \
	device/bn/acclaim/prebuilt/audio/pcm/side.conf:/system/usr/share/alsa/pcm/side.conf \
	device/bn/acclaim/prebuilt/audio/pcm/surround40.conf:/system/usr/share/alsa/pcm/surround40.conf \
	device/bn/acclaim/prebuilt/audio/pcm/surround41.conf:/system/usr/share/alsa/pcm/surround41.conf \
	device/bn/acclaim/prebuilt/audio/pcm/surround50.conf:/system/usr/share/alsa/pcm/surround50.conf \
	device/bn/acclaim/prebuilt/audio/pcm/surround51.conf:/system/usr/share/alsa/pcm/surround51.conf \
	device/bn/acclaim/prebuilt/audio/pcm/surround71.conf:/system/usr/share/alsa/pcm/surround71.conf

PRODUCT_PACKAGES += \
	libaudioutils \
	hwprops \
	CMStats \
	libaudiohw_legacy \
	lights.acclaim \
	audio.primary.acclaim 

# Place permission files
PRODUCT_COPY_FILES += \
	frameworks/base/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
	frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
	frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
	frameworks/base/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
	frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
	frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
	frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
	frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/base/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
	packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml

# Device specific packages
PRODUCT_PACKAGES += \
	liblights.acclaim \
	sensors.acclaim

# Filesystem management tools
PRODUCT_PACKAGES += \
	make_ext4fs \
	setup_fs

# Wifi
PRODUCT_PACKAGES += \
	dhcpcd.conf \
	libCustomWifi \
	wlan_cu \
	wlan_loader \
	wpa_supplicant.conf

PRODUCT_PACKAGES += \
	hwcomposer.default

PRODUCT_CHARACTERISTICS := tablet

# Screen size is "large", density is "mdpi"
PRODUCT_AAPT_CONFIG := large mdpi

# we have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise

$(call inherit-product, frameworks/base/build/tablet-dalvik-heap.mk)
$(call inherit-product, hardware/ti/omap4xxx/omap4.mk)

$(call inherit-product-if-exists, vendor/bn/acclaim/device-vendor.mk)
$(call inherit-product-if-exists, vendor/bn/acclaim/device-vendor-blobs.mk)