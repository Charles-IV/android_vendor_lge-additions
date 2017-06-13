ifeq ($(BOARD_HAS_QCOM_WLAN), true)
LOCAL_PATH := $(call my-dir)

COMPILE_WFA_STA_SRC := false
COMPILE_WFA_STA_SRC = $(shell if [ -d $(TOPDIR)vendor/lge/frameworks/base/wifi/system/proprietary/wfa_sta ] ; then echo true; fi)

COMPILE_PTT_APP_SRC := false
#COMPILE_PTT_APP_SRC = $(shell if [ -d vendor/qcom/proprietary/wlan/utils/ptt ] ; then echo true; fi)
#always use ptt_socket_app binary   not from source - bluetooth.kang

ifneq ($(strip $(COMPILE_WFA_STA_SRC)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := wfa_sta
LOCAL_SRC_FILES := wfa_sta
LOCAL_MODULE_TAGS := eng debug
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
include $(BUILD_PREBUILT)
endif # COMPILE_WFA_STA_SRC

ifneq ($(strip $(COMPILE_PTT_APP_SRC)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := ptt_socket_app
LOCAL_SRC_FILES := ptt_socket_app
LOCAL_MODULE_TAGS := eng debug
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
include $(BUILD_PREBUILT)
endif # COMPILE_PTT_APP_SRC

# Include subdirectory makefiles
# ============================================================
include $(call first-makefiles-under,$(LOCAL_PATH))
endif# BOARD_HAS_QCOM_WLAN
