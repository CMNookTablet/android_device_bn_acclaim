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

DEVICE_PACKAGE_OVERLAYS += device/bn/acclaim/overlay

# copy all kernel modules under the "modules" directory to system/lib/modules
#PRODUCT_COPY_FILES += $(shell \
#    find device/bn/acclaim/modules -name '*.ko' \
#    | sed -r 's/^\/?(.*\/)([^/ ]+)$$/\1\2:system\/lib\/modules\/\2/' \
#    | tr '\n' ' ')

PRODUCT_COPY_FILES += \
	$(LOCAL_KERNEL):kernel \
	device/bn/acclaim/root/init.acclaim.rc:root/init.acclaim.rc \
	device/bn/acclaim/root/init.acclaim.usb.rc:root/init.acclaim.usb.rc \
	device/bn/acclaim/root/ueventd.acclaim.rc:root/ueventd.acclaim.rc \

# Graphics
PRODUCT_COPY_FILES += \
    device/bn/acclaim/prebuilt/sgx/gralloc.omap4430.so:/system/vendor/lib/hw/gralloc.omap4.so \
    device/bn/acclaim/prebuilt/sgx/libEGL_POWERVR_SGX540_120.so:/system/vendor/lib/egl/libEGL_POWERVR_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libGLESv1_CM_POWERVR_SGX540_120.so:/system/vendor/lib/egl/libGLESv1_CM_POWERVR_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libGLESv2_POWERVR_SGX540_120.so:/system/vendor/lib/egl/libGLESv2_POWERVR_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libglslcompiler_SGX540_120.so:/system/vendor/lib/libglslcompiler_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libIMGegl_SGX540_120.so:/system/vendor/lib/libIMGegl_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libpvr2d_SGX540_120.so:/system/vendor/lib/libpvr2d_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libpvrANDROID_WSEGL_SGX540_120.so:/system/vendor/lib/libpvrANDROID_WSEGL_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libPVRScopeServices_SGX540_120.so:/system/vendor/lib/libPVRScopeServices_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libsrv_init_SGX540_120.so:/system/vendor/lib/libsrv_init_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libsrv_um_SGX540_120.so:/system/vendor/lib/libsrv_um_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/libusc_SGX540_120.so:/system/vendor/lib/libusc_SGX540_120.so \
    device/bn/acclaim/prebuilt/sgx/pvrsrvinit_SGX540_120:/system/vendor/bin/pvrsrvinit_SGX540_120 \
    device/bn/acclaim/prebuilt/sgx/pvrsrvinit:/system/vendor/bin/pvrsrvinit \
    device/bn/acclaim/prebuilt/sgx/pvrsrvctl:/system/bin/pvrsrvctl \
    device/bn/acclaim/prebuilt/sgx/powervr.ini:/system/etc/powervr.ini \

# Art
PRODUCT_COPY_FILES += \
    device/bn/acclaim/prebuilt/poetry/poem.txt:root/sbin/poem.txt

# gfx. This needs http://git.omapzoom.org/?p=device/ti/proprietary-open.git;a=commit;h=47a8187f2d8a08f7210b3c964b3b8e50f3b0da66
#PRODUCT_PACKAGES += \
#	ti_omap4_sgx_libs \
#	ti_omap4_ducati_libs

PRODUCT_COPY_FILES += \
	device/bn/acclaim/firmware/ducati-m3.bin:/system/vendor/firmware/ducati-m3.bin

# Input
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/usr/idc/ft5x06-i2c.idc:system/usr/idc/ft5x06-i2c.idc \
	device/bn/acclaim/prebuilt/usr/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl\
	device/bn/acclaim/prebuilt/usr/idc/twl6030_pwrbutton.idc:system/usr/idc/twl6030_pwrbutton.idc \
	device/bn/acclaim/prebuilt/usr/keylayout/twl6030_pwrbutton.kl:system/usr/keylayout/twl6030_pwrbutton.kl

# Temporarily use prebuilt DOMX
# Prebuilts /system/lib
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/lib/libdomx.so:/system/lib/libdomx.so \
	device/bn/acclaim/prebuilt/lib/libmm_osal.so:/system/lib/libmm_osal.so \
	device/bn/acclaim/prebuilt/lib/libOMX.TI.DUCATI1.MISC.SAMPLE.so:/system/lib/libOMX.TI.DUCATI1.MISC.SAMPLE.so \
	device/bn/acclaim/prebuilt/lib/libOMX.TI.DUCATI1.VIDEO.CAMERA.so:/system/lib/libOMX.TI.DUCATI1.VIDEO.CAMERA.so \
	device/bn/acclaim/prebuilt/lib/libOMX.TI.DUCATI1.VIDEO.DECODER.secure.so:/system/lib/libOMX.TI.DUCATI1.VIDEO.DECODER.secure.so \
	device/bn/acclaim/prebuilt/lib/libOMX.TI.DUCATI1.VIDEO.DECODER.so:/system/lib/libOMX.TI.DUCATI1.VIDEO.DECODER.so \
	device/bn/acclaim/prebuilt/lib/libOMX.TI.DUCATI1.VIDEO.H264E.so:/system/lib/libOMX.TI.DUCATI1.VIDEO.H264E.so \
	device/bn/acclaim/prebuilt/lib/libOMX.TI.DUCATI1.VIDEO.MPEG4E.so:/system/lib/libOMX.TI.DUCATI1.VIDEO.MPEG4E.so \
	device/bn/acclaim/prebuilt/lib/libOMX_Core.so:/system/lib/libOMX_Core.so \
	
# fwram
#PRODUCT_COPY_FILES += \
#	device/bn/acclaim/prebuilt/fwram.ko:/system/lib/modules/fwram.ko

# Vold
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/etc/vold.acclaim.fstab:system/etc/vold.fstab

# Media Profile
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/etc/media_profiles.xml:system/etc/media_profiles.xml

# Clears the boot counter
PRODUCT_COPY_FILES += \
	device/bn/acclaim/clear_bootcnt.sh:/system/bin/clear_bootcnt.sh

# update the battery log info
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/bin/log_battery_data.sh:/system/bin/log_battery_data.sh \
	device/bn/acclaim/prebuilt/bin/fix-mac.sh:/system/bin/fix-mac.sh

PRODUCT_PACKAGES += \
	hwprops \
	CMStats \
	lights.acclaim

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

# postrecoveryboot for recovery
PRODUCT_COPY_FILES += \
    device/bn/acclaim/recovery/postrecoveryboot.sh:recovery/root/sbin/postrecoveryboot.sh

# Wifi
PRODUCT_PACKAGES += \
	dhcpcd.conf \
	TQS_D_1.7.ini \
	calibrator

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_AAPT_CONFIG := normal mdpi

# we have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise

#$(call inherit-product, hardware/ti/omap4xxx/omap4.mk)
# dont use omap4.mk. We have to drop hwcomposer.omap4, camera.omap4 for now.
# Just include rest directly here.
#PRODUCT_PACKAGES += \
#	libdomx \
#	libOMX_Core \
#	libOMX.TI.DUCATI1.VIDEO.H264E \
#	libOMX.TI.DUCATI1.VIDEO.MPEG4E \
#	libOMX.TI.DUCATI1.VIDEO.DECODER \
#	libOMX.TI.DUCATI1.VIDEO.DECODER.secure \
#	libOMX.TI.DUCATI1.VIDEO.CAMERA \
#	libOMX.TI.DUCATI1.MISC.SAMPLE
#PRODUCT_PACKAGES += \
#	libstagefrighthw \
#        libI420colorconvert \
#	libtiutils \
#	libcamera \
#	libion \
#	libomxcameraadapter
PRODUCT_PACKAGES += \
	hwcpmposer.omap4 \
	hwcomposer.default \
	smc_pa_ctrl \
	tf_daemon\
	audio.primary.omap4 \
	audio_policy.default \
	libaudioutils \
	Music \
	tinyplay \
	tinymix \
	tinycap

PRODUCT_PROPERTY_OVERRIDES := \
	ro.sf.lcd_density=160 \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=45

# TI-Connectivity
PRODUCT_COPY_FILES += \
        device/bn/acclaim/firmware/wl1271-nvs_127x.bin:system/etc/firmware/ti-connectivity/wl1271-nvs.bin.orig \
        device/bn/acclaim/firmware/wl127x-fw-4-mr.bin:system/etc/firmware/ti-connectivity/wl127x-fw-4-mr.bin \
        device/bn/acclaim/firmware/wl127x-fw-4-sr.bin:system/etc/firmware/ti-connectivity/wl127x-fw-4-sr.bin \

# enable Google-specific location features,
# like NetworkLocationProvider and LocationCollector
PRODUCT_PROPERTY_OVERRIDES += \
    ro.com.google.locationfeatures=1 \
    ro.com.google.networklocation=1

# we have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise


$(call inherit-product, frameworks/base/build/tablet-dalvik-heap.mk)

$(call inherit-product-if-exists, vendor/bn/acclaim/device-vendor.mk)
$(call inherit-product-if-exists, vendor/bn/acclaim/device-vendor-blobs.mk)

#$(call inherit-product, device/bn/acclaim/wl12xx/ti-wl12xx-vendor.mk)
#$(call inherit-product, device/bn/acclaim/wl12xx/ti-wpan-products.mk)
