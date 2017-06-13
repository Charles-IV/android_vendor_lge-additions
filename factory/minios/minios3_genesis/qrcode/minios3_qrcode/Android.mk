ifeq ($(PERFORM_SELF_D), 1)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../../minios3_config/minios3_config.mk

LOCAL_C_INCLUDES += external/libpng \
                    external/zlib \
                    $(LOCAL_PATH)/../ \
                    $(LOCAL_PATH)/../../../allautotest_skia/include \
                    $(LOCAL_PATH)/../../../../atd/lgftmitem \
                    $(LOCAL_PATH)/../../../../atd/lib/core \
                    $(LOCAL_PATH)/../../../allautotest_skia/SelfDiagnosis/include \

ifeq ($(PERFORM_AIO), 1)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../H3_Prj/All-In-One-Test/Common
endif

ifeq ($(PERFORM_H2_PRJ), 1)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../H2_Prj/SelfDExt/include
endif

LOCAL_SRC_FILES += minios3_qrcode.c \

LOCAL_SHARED_LIBRARIES += libutils \
                          libcutils \
                          libpng \
                          libz \
                          libqrcode \
                          libaat_manager \
                          liblgftmitem

ifeq ($(PERFORM_AIO),1)
LOCAL_SHARED_LIBRARIES += lib_T_Common
endif

LOCAL_STATIC_LIBRARIES := libaat_common

LOCAL_MODULE := minios3_qrcode
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := lge
ifeq (1,$(SUPPORT_64BIT))
LOCAL_MULTILIB := 32
endif
LOCAL_MODULE_PATH := $(TARGET_FACTORY_RAMDISK_OUT)/sbin

include $(BUILD_EXECUTABLE)
endif