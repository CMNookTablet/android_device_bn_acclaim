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

TARGET_BOARD_OMAP_CPU := 4430

# inherit from common
-include $(COMMON_FOLDER)/BoardConfigCommon.mk

# Use the non-open-source parts, if they're present
-include vendor/bn/acclaim/BoardConfigVendor.mk

PRODUCT_VENDOR_KERNEL_HEADERS := $(DEVICE_FOLDER)/kernel-headers

# camera
TI_OMAP4_CAMERAHAL_VARIANT := false
USE_CAMERA_STUB := true

# bluetooth
BOARD_HAVE_BLUETOOTH := false
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_FOLDER)/bluetooth

# kernel/boot
BOARD_KERNEL_PAGESIZE := 4096
BOARD_KERNEL_CMDLINE := androidboot.console=ttyO0 console=ttyO0,115200n8 def_disp=lcd2
TARGET_BOOTLOADER_BOARD_NAME := acclaim

# filesystem
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
BOARD_HOSTAPD_DRIVER := NL80211
PRODUCT_WIRELESS_TOOLS := true
BOARD_WLAN_DEVICE := wl12xx_mac80211
BOARD_SOFTAP_DEVICE := wl12xx_mac80211
WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wl12xx_sdio.ko"
WIFI_DRIVER_MODULE_NAME := "wl12xx_sdio"
WIFI_FIRMWARE_LOADER := ""
COMMON_GLOBAL_CFLAGS += -DUSES_TI_MAC80211
endif

# Vold
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/virtual/android_usb/android0/f_mass_storage/lun%d/file"

# Graphics
BOARD_EGL_CFG := $(DEVICE_FOLDER)/prebuilt/etc/egl.cfg

# OMAP Custom DOMX
TI_CUSTOM_DOMX_PATH := $(DEVICE_FOLDER)/domx
DOMX_PATH := $(DEVICE_FOLDER)/domx

# Recovery
BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../$(DEVICE_FOLDER)/recovery/recovery_ui.c
TARGET_RECOVERY_INITRC := $(DEVICE_FOLDER)/recovery/init.rc
TARGET_RECOVERY_PRE_COMMAND := "echo 'recovery' > /bootdata/BCB; sync"
BOARD_ALWAYS_INSECURE := true

# boot.img creation
BOARD_CUSTOM_BOOTIMG_MK := $(DEVICE_FOLDER)/boot.mk
TARGET_NO_BOOTLOADER := true
TARGET_PROVIDES_RELEASETOOLS := true

# hack the ota
TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT := $(DEVICE_FOLDER)/releasetools/acclaim_ota_from_target_files
# not tested at all
TARGET_RELEASETOOL_IMG_FROM_TARGET_SCRIPT := $(DEVICE_FOLDER)/releasetools/acclaim_img_from_target_files

TARGET_KERNEL_CONFIG := cyanogenmod_acclaim_defconfig
TARGET_KERNEL_SOURCE := kernel/bn/acclaim

# wlan build
WLAN_MODULES:
	make clean -C hardware/ti/wlan/mac80211/compat_wl12xx
	make -j8 -C hardware/ti/wlan/mac80211/compat_wl12xx KERNEL_DIR=$(KERNEL_OUT) KLIB=$(KERNEL_OUT) KLIB_BUILD=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE="arm-eabi-"
	mv hardware/ti/wlan/mac80211/compat_wl12xx/compat/compat.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/net/mac80211/mac80211.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/net/wireless/cfg80211.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx_spi.ko $(KERNEL_MODULES_OUT)
	mv hardware/ti/wlan/mac80211/compat_wl12xx/drivers/net/wireless/wl12xx/wl12xx_sdio.ko $(KERNEL_MODULES_OUT)

TARGET_KERNEL_MODULES := WLAN_MODULES

# External SGX Module
SGX_MODULES:
	make clean -C $(COMMON_FOLDER)/pvr-source/eurasiacon/build/linux2/omap4430_android
	cp $(TARGET_KERNEL_SOURCE)/drivers/video/omap2/omapfb/omapfb.h $(KERNEL_OUT)/drivers/video/omap2/omapfb/omapfb.h
	make -j8 -C $(COMMON_FOLDER)/pvr-source/eurasiacon/build/linux2/omap4430_android ARCH=arm KERNEL_CROSS_COMPILE=arm-eabi- CROSS_COMPILE=arm-eabi- KERNELDIR=$(KERNEL_OUT) TARGET_PRODUCT="blaze_tablet" BUILD=release TARGET_SGX=540 PLATFORM_VERSION=4.0
	mv $(KERNEL_OUT)/../../target/kbuild/pvrsrvkm_sgx540_120.ko $(KERNEL_MODULES_OUT)

TARGET_KERNEL_MODULES += SGX_MODULES

# TWRP
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
