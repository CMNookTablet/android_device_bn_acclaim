#
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
$(call inherit-product, device/bn/acclaim/full_acclaim.mk)

PRODUCT_NAME := cm_acclaim
PRODUCT_BRAND := Android
PRODUCT_DEVICE := acclaim
PRODUCT_MANUFACTURER := Barnes & Noble

# Release name and versioning
PRODUCT_RELEASE_NAME := NookTablet

TARGET_BOOTANIMATION_NAME := vertical-600x1024

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_tablet_wifionly.mk)

# DEVICE_PACKAGE_OVERLAYS += device/bn/acclaim/overlay/cm

