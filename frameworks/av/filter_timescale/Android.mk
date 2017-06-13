ROOT_DIR := $(call my-dir)
LOCAL_PATH:= $(ROOT_DIR)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_C_INCLUDES 	:= $(COMMON_INCLUDES) \
						$(LOCAL_PATH)/interface \
						$(LOCAL_PATH)/soundtouch/inc \
						frameworks/base/include \
						bionic \

LOCAL_CFLAGS 		:= $(COMMON_CFLAGS)

LOCAL_SRC_FILES 	:= timescalefilter.cpp

LOCAL_MODULE 		:= libtimescale_filter
LOCAL_MODULE_TAGS	:= optional

LOCAL_STATIC_LIBRARIES := libSoundTouch
LOCAL_SHARED_LIBRARIES := $(COMMON_SHARED_LIBRARIES) \
							libutils \

include $(BUILD_SHARED_LIBRARY)
include $(call all-makefiles-under, $(LOCAL_PATH))
