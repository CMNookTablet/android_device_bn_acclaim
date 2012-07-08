# Copyright (C) 2007 The Android Open Source Project
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

# BoardConfig.mk
#
# Product-specific compile-time definitions.
#

USE_CAMERA_STUB := true

# audio
BOARD_USES_GENERIC_AUDIO := false

# GPS
#BOARD_HAVE_FAKE_GPS := true
BOARD_HAVE_BLUETOOTH := false
# inherit from the proprietary version
-include vendor/bn/acclaim/BoardConfigVendor.mk

# Processor 
TARGET_BOARD_PLATFORM := omap4
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH_VARIANT := armv7-a-neon
ARCH_ARM_HAVE_TLS_REGISTER := true
TARGET_GLOBAL_CFLAGS += -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
TARGET_BOOTLOADER_BOARD_NAME := acclaim

# Kernel/Boot
BOARD_KERNEL_CMDLINE := androidboot.console=ttyO0 console=ttyO0,115200n8 init=/init rootwait vram=16M,82000000 omapfb.vram=0:5M@82000000 def_disp=lcd2

BOARD_KERNEL_BASE := 0x80080000
BOARD_KERNEL_PAGESIZE := 4096
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

# Filesystem
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 16777216
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 15728640
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 641728512
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12949893120
BOARD_FLASH_BLOCK_SIZE := 131072

# Wifi
BOARD_WPA_SUPPLICANT_DRIVER      := NL80211
WPA_SUPPLICANT_VERSION           := VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_wl12xx
BOARD_WLAN_DEVICE                := wl12xx_mac80211
BOARD_SOFTAP_DEVICE              := wl12xx_mac80211
WIFI_DRIVER_MODULE_PATH          := "/system/lib/modules/wl12xx_sdio.ko"
WIFI_DRIVER_MODULE_NAME          := "wl12xx_sdio"
WIFI_FIRMWARE_LOADER             := ""
COMMON_GLOBAL_CFLAGS += -DUSES_TI_MAC80211

# Vold
BOARD_VOLD_MAX_PARTITIONS := 20
BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/virtual/android_usb/android0/f_mass_storage/lun%d/file"

# Graphics
USE_OPENGL_RENDERER := true
BOARD_EGL_CFG := device/bn/acclaim/prebuilt/etc/egl.cfg

OMAP_ENHANCEMENT := true
COMMON_GLOBAL_CFLAGS += -DOMAP_ENHANCEMENT -DTARGET_OMAP4

# Recovery
BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../device/bn/acclaim/recovery/recovery_ui.c
TARGET_RECOVERY_PRE_COMMAND := "echo 'recovery' > /bootdata/BCB; sync"
BOARD_HAS_LARGE_FILESYSTEM := true

# adb has root
ADDITIONAL_DEFAULT_PROPERTIES += ro.secure=0

# boot.img creation
BOARD_CUSTOM_BOOTIMG_MK := device/bn/acclaim/boot.mk
TARGET_NO_BOOTLOADER := true
TARGET_PROVIDES_RELEASETOOLS := true

# hack the ota
TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT := ./device/bn/acclaim/releasetools/acclaim_ota_from_target_files
# not tested at all
TARGET_RELEASETOOL_IMG_FROM_TARGET_SCRIPT := ./device/bn/acclaim/releasetools/acclaim_img_from_target_files

TARGET_KERNEL_CONFIG := cyanogenmod_acclaim_defconfig
TARGET_KERNEL_SOURCE := kernel/bn/acclaim

SGX_MODULES:
	cp kernel/bn/acclaim/drivers/video/omap2/omapfb/omapfb.h $(KERNEL_OUT)/drivers/video/omap2/omapfb/omapfb.h
	make ARCH="arm" -C kernel/bn/acclaim/external/sgx/src/eurasia_km/eurasiacon/build/linux2/omap4430_android CROSS_COMPILE=arm-eabi- TARGET_PRODUCT="blaze_tablet" BUILD=release TARGET_SGX=540 PLATFORM_VERSION=4.0  KERNEL_CROSS_COMPILE=arm-eabi- KERNELDIR=$(KERNEL_OUT)
	mv $(KERNEL_OUT)/../../target/kbuild/omaplfb_sgx540_120.ko $(KERNEL_MODULES_OUT)
	mv $(KERNEL_OUT)/../../target/kbuild/pvrsrvkm_sgx540_120.ko $(KERNEL_MODULES_OUT)

WIFI_MODULES:
	make -C kernel/bn/acclaim/external/wlan/mac80211/compat_wl12xx KERNEL_DIR=$(KERNEL_OUT) KLIB=$(KERNEL_OUT) KLIB_BUILD=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=arm-eabi-
	mv $(KERNEL_OUT)/lib/crc7.ko $(KERNEL_MODULES_OUT)
	mv kernel/bn/acclaim/external/wlan/mac80211/compat_wl12xx/compat/compat.ko $(KERNEL_MODULES_OUT)
	mv kernel/bn/acclaim/external/wlan/mac80211/compat_wl12xx/net/mac80211/mac80211.ko $(KERNEL_MODULES_OUT)
	mv kernel/bn/acclaim/external/wlan/mac80211/compat_wl12xx/net/wireless/cfg80211.ko $(KERNEL_MODULES_OUT)
	mv kernel/bn/acclaim/external/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx.ko $(KERNEL_MODULES_OUT)
	mv kernel/bn/acclaim/external/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx_sdio.ko $(KERNEL_MODULES_OUT)

TARGET_KERNEL_MODULES := SGX_MODULES WIFI_MODULES

# Keep this as a fallback
TARGET_PREBUILT_KERNEL := device/bn/acclaim/kernel

#Config for building TWRP
DEVICE_RESOLUTION := 1024x600
RECOVERY_TOUCHSCREEN_SWAP_XY := true
RECOVERY_TOUCHSCREEN_FLIP_Y := true
TW_NO_REBOOT_BOOTLOADER := true
TW_NO_REBOOT_RECOVERY := true
TW_INTERNAL_STORAGE_PATH := "/emmc"
TW_INTERNAL_STORAGE_MOUNT_POINT := "emmc"
TW_EXTERNAL_STORAGE_PATH := "/sdcard"
TW_EXTERNAL_STORAGE_MOUNT_POINT := "sdcard"
TW_DEFAULT_EXTERNAL_STORAGE := true

