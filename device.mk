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

DEVICE_PACKAGE_OVERLAYS += device/bn/acclaim/overlay

$(call inherit-product, frameworks/base/build/tablet-dalvik-heap.mk)
TARGET_PREBUILT_KERNEL = device/bn/acclaim/kernel
ifeq ($(TARGET_PREBUILT_KERNEL),)
	LOCAL_KERNEL := device/bn/acclaim/kernel
else
	LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES += \
	$(LOCAL_KERNEL):kernel \
	device/bn/acclaim/init.acclaim.rc:root/init.acclaim.rc \
	device/bn/acclaim/init.acclaim.usb.rc:root/init.acclaim.usb.rc \
	device/bn/acclaim/ueventd.acclaim.rc:root/ueventd.acclaim.rc

# Art
PRODUCT_COPY_FILES += \
    device/bn/acclaim/prebuilt/poetry/poem.txt:root/sbin/poem.txt

# gfx. This needs http://git.omapzoom.org/?p=device/ti/proprietary-open.git;a=commit;h=47a8187f2d8a08f7210b3c964b3b8e50f3b0da66
PRODUCT_PACKAGES += \
	ti_omap4_sgx_libs \
	ti_omap4_ducati_libs

# Input
PRODUCT_COPY_FILES += \
	device/bn/acclaim/ft5x06-i2c.idc:system/usr/idc/ft5x06-i2c.idc \
	device/bn/acclaim/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl

# Misc
PRODUCT_COPY_FILES += \
	device/bn/acclaim/clear_bootcnt.sh:system/bin/clear_bootcnt.sh

# Wifi
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/wifi/tiwlan_drv.ko:/system/lib/modules/tiwlan_drv.ko \
	device/bn/acclaim/prebuilt/wifi/tiwlan.ini:/system/etc/wifi/tiwlan.ini \
	device/bn/acclaim/prebuilt/wifi/firmware.bin:/system/etc/wifi/firmware.bin 

# Other modules
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/tun.ko:/system/lib/modules/tun.ko \
	device/bn/acclaim/prebuilt/cifs.ko:/system/lib/modules/cifs.ko

# Bluetooth
PRODUCT_COPY_FILES += \
    device/bn/acclaim/prebuilt/TIInit_7.2.31.bts:/system/etc/firmware/TIInit_7.2.31.bts

# fwram
PRODUCT_COPY_FILES += \
	device/bn/acclaim/prebuilt/fwram.ko:/system/lib/modules/fwram.ko

# Vold
PRODUCT_COPY_FILES += \
	device/bn/acclaim/etc/vold.acclaim.fstab:system/etc/vold.fstab

# Media Profile
PRODUCT_COPY_FILES += \
	device/bn/acclaim/etc/media_profiles.xml:system/etc/media_profiles.xml

# Clears the boot counter
PRODUCT_COPY_FILES += \
	device/bn/acclaim/clear_bootcnt.sh:/system/bin/clear_bootcnt.sh

# update the battery log info
PRODUCT_COPY_FILES += \
	device/bn/acclaim/log_battery_data.sh:/system/bin/log_battery_data.sh

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

# Bluetooth
PRODUCT_PACKAGES += \
	uim-sysfs

# Filesystem management tools
PRODUCT_PACKAGES += \
	make_ext4fs \
	setup_fs

# postrecoveryboot for cwm
PRODUCT_COPY_FILES += \
    device/bn/acclaim/recovery/postrecoveryboot.sh:recovery/root/sbin/postrecoveryboot.sh

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

#$(call inherit-product, hardware/ti/omap4xxx/omap4.mk)
# dont use omap4.mk. We have to drop hwcomposer.omap4, camera.omap4 for now.
# Just include rest directly here.
PRODUCT_PACKAGES += \
	libdomx \
	libOMX_Core \
	libOMX.TI.DUCATI1.VIDEO.H264E \
	libOMX.TI.DUCATI1.VIDEO.MPEG4E \
	libOMX.TI.DUCATI1.VIDEO.DECODER \
	libOMX.TI.DUCATI1.VIDEO.DECODER.secure \
	libOMX.TI.DUCATI1.VIDEO.CAMERA \
	libOMX.TI.DUCATI1.MISC.SAMPLE \
	libstagefrighthw \
        libI420colorconvert \
	libtiutils \
	libcamera \
	libion \
	iontest \
	libomxcameraadapter \
	smc_pa_ctrl \
	tf_daemon

$(call inherit-product-if-exists, vendor/bn/acclaim/device-vendor.mk)
$(call inherit-product-if-exists, vendor/bn/acclaim/device-vendor-blobs.mk)
