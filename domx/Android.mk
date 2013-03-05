# Copyright (C) Texas Instruments - http://www.ti.com/
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

# DOMX not used in OMAP3 program
ifneq ($(TARGET_BOARD_PLATFORM),omap3)

    LOCAL_PATH:= $(call my-dir)
    HARDWARE_TI_OMAP4_BASE:= hardware/ti/omap4xxx
    OMAP4_DEBUG_MEMLEAK:= false

    BUILD_HEAPTRACKED_SHARED_LIBRARY:=$(BUILD_SHARED_LIBRARY)
    BUILD_HEAPTRACKED_EXECUTABLE:= $(BUILD_EXECUTABLE)

    include $(call first-makefiles-under,$(LOCAL_PATH))
endif # ifeq ($(TARGET_BOARD_PLATFORM),omap4)
