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
PRODUCT_VENDOR_KERNEL_HEADERS := device/bn/acclaim/kernel-headers
USE_CAMERA_STUB := true
BOARD_USES_GENERIC_AUDIO := false
#BOARD_HAVE_FAKE_GPS := true
BOARD_HAVE_BLUETOOTH := false
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/bn/acclaim/bluetooth

# inherit from the proprietary version
-include vendor/bn/acclaim/BoardConfigVendor.mk

# Processor 
TARGET_BOARD_PLATFORM := omap4
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
ARCH_ARM_HAVE_TLS_REGISTER := true
TARGET_GLOBAL_CFLAGS += -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
TARGET_BOOTLOADER_BOARD_NAME := acclaim

# Kernel/Boot
BOARD_KERNEL_BASE := 0x80080000
BOARD_KERNEL_PAGESIZE := 4096
BOARD_KERNEL_CMDLINE := androidboot.console=ttyO0 console=ttyO0,115200n8 def_disp=lcd2
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

OMAP_ENHANCEMENT := true
#OMAP_ENHANCEMENT_BURST_CAPTURE := true
#OMAP_ENHANCEMENT_S3D := true
#OMAP_ENHANCEMENT_CPCAM := true
#OMAP_ENHANCEMENT_VTC := true
OMAP_ENHANCEMENT_MULTIGPU := true
BOARD_USE_TI_ENHANCED_DOMX := true
BOARD_PROVIDES_CUSTOM_DOMX := true

# Filesystem
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 16777216
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 15728640
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 641728512
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12949893120
BOARD_FLASH_BLOCK_SIZE := 131072

# Connectivity - Wi-Fi
USES_TI_MAC80211 := true
ifdef USES_TI_MAC80211
WPA_SUPPLICANT_VERSION := VER_0_8_X_TI
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_wl12xx
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_wl12xx
PRODUCT_WIRELESS_TOOLS := true
BOARD_WLAN_DEVICE := wl12xx_mac80211
BOARD_SOFTAP_DEVICE := wl12xx_mac80211
WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wl12xx_sdio.ko"
WIFI_DRIVER_MODULE_NAME := "wl12xx_sdio"
WIFI_FIRMWARE_LOADER := ""
COMMON_GLOBAL_CFLAGS += -DUSES_TI_MAC80211
endif

# Vold
BOARD_VOLD_MAX_PARTITIONS := 32
BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/virtual/android_usb/android0/f_mass_storage/lun%d/file"

# Graphics
USE_OPENGL_RENDERER := true
BOARD_EGL_CFG := device/bn/acclaim/prebuilt/etc/egl.cfg

# OMAP
#OMAP_ENHANCEMENT := false
OMAP_ENHANCEMENT_MULTIGPU := true
#ENHANCED_DOMX := false

# Recovery
BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../device/bn/acclaim/recovery/recovery_ui.c
TARGET_RECOVERY_INITRC := device/bn/acclaim/recovery/init.rc
TARGET_RECOVERY_PRE_COMMAND := "echo 'recovery' > /bootdata/BCB; sync"
TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"
BOARD_HAS_LARGE_FILESYSTEM := true

# adb has root
ADDITIONAL_DEFAULT_PROPERTIES += ro.secure=0
ADDITIONAL_DEFAULT_PROPERTIES += ro.allow.mock.location=1

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
	mv $(KERNEL_OUT)/../../target/kbuild/pvrsrvkm_sgx540_120.ko $(KERNEL_MODULES_OUT)

WIFI_MODULES:
	make -C hardware/ti/wlan/mac80211/compat_wl12xx KERNEL_DIR=$(KERNEL_OUT) KLIB=$(KERNEL_OUT) KLIB_BUILD=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=arm-eabi-
	mv hardware/ti/wlan/mac80211/compat_wl12xx/compat/compat.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/net/mac80211/mac80211.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/net/wireless/cfg80211.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx_spi.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx_sdio.ko $(KERNEL_MODULES_OUT)

TARGET_KERNEL_MODULES := SGX_MODULES WIFI_MODULES

# Keep this as a fallback
TARGET_PREBUILT_KERNEL := device/bn/acclaim/kernel
TARGET_SPECIFIC_HEADER_PATH := device/bn/acclaim/include

ifdef BOARD_USE_TI_ENHANCED_DOMX
    COMMON_GLOBAL_CFLAGS += -DENHANCED_DOMX
    TI_CUSTOM_DOMX_PATH := $(DEVICE_FOLDER)/domx
    DOMX_PATH := $(DEVICE_FOLDER)/domx
else
    DOMX_PATH := hardware/ti/omap4xxx/domx
endif

ifdef OMAP_ENHANCEMENT
    COMMON_GLOBAL_CFLAGS += -DOMAP_ENHANCEMENT -DTARGET_OMAP4
endif

ifdef OMAP_ENHANCEMENT_BURST_CAPTURE
    COMMON_GLOBAL_CFLAGS += -DOMAP_ENHANCEMENT_BURST_CAPTURE
endif

ifdef OMAP_ENHANCEMENT_S3D
    COMMON_GLOBAL_CFLAGS += -DOMAP_ENHANCEMENT_S3D
endif

ifdef OMAP_ENHANCEMENT_CPCAM
    COMMON_GLOBAL_CFLAGS += -DOMAP_ENHANCEMENT_CPCAM
    PRODUCT_MAKEFILES += $(LOCAL_DIR)/sdk_addon/ti_omap_addon.mk
endif

ifdef OMAP_ENHANCEMENT_VTC
    COMMON_GLOBAL_CFLAGS += -DOMAP_ENHANCEMENT_VTC
endif

ifdef USE_ITTIAM_AAC
    COMMON_GLOBAL_CFLAGS += -DUSE_ITTIAM_AAC
endif

ifdef OMAP_ENHANCEMENT_MULTIGPU
    COMMON_GLOBAL_CFLAGS += -DOMAP_ENHANCEMENT_MULTIGPU
endif

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

# Bootanimation
TARGET_BOOTANIMATION_PRELOAD := true
