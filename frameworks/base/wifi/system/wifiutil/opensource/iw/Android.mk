ifeq ($(BOARD_HAS_QCOM_WLAN), true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := iw
LOCAL_SRC_FILES := iw
LOCAL_MODULE_TAGS := eng debug
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
include $(BUILD_PREBUILT)
# Include subdirectory makefiles
# ============================================================
include $(call first-makefiles-under,$(LOCAL_PATH))
endif # BOARD_HAS_QCOM_WLAN
