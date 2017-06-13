LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += external/libpng \
                    external/zlib \

LOCAL_SRC_FILES += bitstream.c \
                   mask.c \
                   mmask.c \
                   mqrspec.c \
                   qrencode.c \
                   qrinput.c \
                   qrspec.c \
                   rscode.c \
                   split.c \

LOCAL_SHARED_LIBRARIES += libutils \
                          libcutils \
                          libpng \
                          libz \

LOCAL_MODULE := libqrcode2
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := lge
ifeq (1,$(SUPPORT_64BIT))
LOCAL_MULTILIB := 32
endif
LOCAL_MODULE_PATH := $(TARGET_FACTORY_RAMDISK_OUT)/vendor/factory_lib

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under, $(LOCAL_PATH))
