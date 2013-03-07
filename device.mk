# Copyright (C) 2013 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

DEVICE_FOLDER := device/bn/acclaim
COMMON_FOLDER := device/bn/omap4-common

$(call inherit-product, $(COMMON_FOLDER)/common.mk)

DEVICE_PACKAGE_OVERLAYS += $(DEVICE_FOLDER)/overlay/aosp

# rootfs
PRODUCT_COPY_FILES += \
	$(DEVICE_FOLDER)/root/init.acclaim.rc:root/init.acclaim.rc \
	$(DEVICE_FOLDER)/root/init.acclaim.usb.rc:root/init.acclaim.usb.rc \
	$(DEVICE_FOLDER)/root/ueventd.acclaim.rc:root/ueventd.acclaim.rc

# media
PRODUCT_COPY_FILES += \
	$(DEVICE_FOLDER)/prebuilt/etc/audio_policy.conf:/system/etc/audio_policy.conf \
	$(DEVICE_FOLDER)/prebuilt/etc/media_codecs.xml:/system/etc/media_codecs.xml \
	$(DEVICE_FOLDER)/prebuilt/etc/media_profiles.xml:system/etc/media_profiles.xml \
	$(DEVICE_FOLDER)/prebuilt/etc/mixer_paths.xml:/system/etc/mixer_paths.xml

# art
PRODUCT_COPY_FILES += \
	$(DEVICE_FOLDER)/prebuilt/poetry/poem.txt:root/sbin/poem.txt

# input
PRODUCT_COPY_FILES += \
	$(DEVICE_FOLDER)/prebuilt/usr/idc/ft5x06-i2c.idc:system/usr/idc/ft5x06-i2c.idc \
	$(DEVICE_FOLDER)/prebuilt/usr/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl\
	$(DEVICE_FOLDER)/prebuilt/usr/idc/twl6030_pwrbutton.idc:system/usr/idc/twl6030_pwrbutton.idc \
	$(DEVICE_FOLDER)/prebuilt/usr/keylayout/twl6030_pwrbutton.kl:system/usr/keylayout/twl6030_pwrbutton.kl

# vold
PRODUCT_COPY_FILES += \
	$(DEVICE_FOLDER)/prebuilt/etc/vold.acclaim.fstab:system/etc/vold.fstab

# wifi
PRODUCT_PACKAGES += \
	calibrator \
	crda \
	regulatory.bin \
	lib_driver_cmd_wl12xx 

# scripts to clear boot counter and update the battery log info
PRODUCT_COPY_FILES += \
	$(DEVICE_FOLDER)/prebuilt/bin/clear_bootcnt.sh:/system/bin/clear_bootcnt.sh \
	$(DEVICE_FOLDER)/prebuilt/bin/log_battery_data.sh:/system/bin/log_battery_data.sh \
	$(DEVICE_FOLDER)/prebuilt/bin/fix-mac.sh:/system/bin/fix-mac.sh \
	$(DEVICE_FOLDER)/prebuilt/bin/fix-serial-no.sh:/system/bin/fix-serial-no.sh

# permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml

# hardware HALs
PRODUCT_PACKAGES += \
	hwcomposer.acclaim \
	lights.acclaim \
	power.acclaim \
	sensors.acclaim \
	audio.primary.acclaim

# recovery: script to reset boot cmds
PRODUCT_COPY_FILES += \
	$(DEVICE_FOLDER)/recovery/postrecoveryboot.sh:recovery/root/sbin/postrecoveryboot.sh

PRODUCT_PACKAGES += \
	librs_jni \
	libjni_pinyinime \
	libwvm \
	TFF \
	setpropex\
	sdcard \
	com.android.future.usb.accessory \
	Superuser \
	su \
	make_ext4fs \
	setup_fs \
	tinyplay \
	tinymix \
	tinycap

# TI OMAP4
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
	libion_ti \
	libomxcameraadapter \
	smc_pa_ctrl \
	tf_daemon \
	libtf_crypto_sst

PRODUCT_CHARACTERISTICS := tablet
PRODUCT_AAPT_CONFIG := normal mdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi

PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=131072 \
	ro.sf.lcd_density=160 \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=45 \
	persist.sys.usb.config=mass_storage,adb \
	ro.crypto.state=unencrypted \
	persist.sys.root_access=3

$(call inherit-product-if-exists, vendor/bn/acclaim/acclaim-vendor.mk)
$(call inherit-product, frameworks/native/build/tablet-dalvik-heap.mk)