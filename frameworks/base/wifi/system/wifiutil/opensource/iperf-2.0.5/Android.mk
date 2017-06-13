ifeq ($(BOARD_HAS_QCOM_WLAN), true)
LOCAL_PATH := $(call my-dir)

COMPILE_IPERF_SRC := false
COMPILE_IPERF_SRC = $(shell if [ -d $(TOPDIR)vendor/lge/frameworks/base/wifi/system/proprietary/iperf-2.0.5 ] ; then echo true; fi)

ifneq ($(strip $(COMPILE_IPERF_SRC)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := iperf
LOCAL_SRC_FILES := iperf-2.0.5
LOCAL_MODULE_TAGS := eng debug
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
include $(BUILD_PREBUILT)
endif # COMPILE_IPERF_SRC
# Include subdirectory makefiles
# ============================================================
include $(call first-makefiles-under,$(LOCAL_PATH))
endif # BOARD_HAS_QCOM_WLAN