LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	memmgr_rpc.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../inc \
	$(LOCAL_PATH)/../../ \
	$(LOCAL_PATH)/../../omx_rpc/inc \
	$(LOCAL_PATH)/../../../domx

LOCAL_CFLAGS += -D_Android

LOCAL_SHARED_LIBRARIES := \
	libOMX_CoreOsal \
	libipcutils \
	libsysmgr \
	libipc \
	librcm \
	libnotify \
	libc \
	liblog    \
	libd2cmap

LOCAL_MODULE:= libmemmgr_rpc

include $(BUILD_HEAPTRACKED_SHARED_LIBRARY)
