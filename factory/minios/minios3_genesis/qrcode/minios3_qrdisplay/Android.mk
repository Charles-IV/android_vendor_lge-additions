ifeq ($(PERFORM_SELF_D), 1)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../../minios3_config/minios3_config.mk

LOCAL_C_INCLUDES += system/core/include/cutils
LOCAL_C_INCLUDES += system/core/include/utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_include/aat_config/$(RESOLUTION_TYPE)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_displayinfo
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_event/v$(MINIOS_AAT_VERSION)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_service/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../allautotest_skia/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../atd/lgftmitem \
LOCAL_C_INCLUDES += $(minios3_atd_include)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../allautotest_skia/SelfDiagnosis/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_genesis/libwifimini
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_device/sdcard
ifeq ($(PERFORM_H2_PRJ), 1)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../H2_Prj/SelfDExt/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../H2_Prj/lib/H2_Common
LOCAL_C_INCLUDES += $(ATD_PATH)/lib/common
endif
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../common/brm/unified_sysfs/include

LOCAL_C_INCLUDES += \
  $(call include-path-for, libhardware)/hardware \
  $(call include-path-for, libhardware_legacy)/hardware_legacy \

LOCAL_SRC_FILES := \
    minios3_qrdisplay.cpp \

LOCAL_SHARED_LIBRARIES := \
    libaat_manager \
    libminios3_widget \
    libminios3_displayinfo \
    libminios3_interface \
    libminios3_event \
    libcutils \
    libutils \
    libbinder \
    liblog \
    liblgftmitem \
    libwifimini \
    libunified_sysfs_pm

LOCAL_SHARED_LIBRARIES += \
    libhardware \
    libhardware_legacy

ifeq ($(PERFORM_AIO), 1)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../H3_Prj/All-In-One-Test/Common
LOCAL_SHARED_LIBRARIES += lib_T_Common
endif

ifeq ($(PERFORM_H2_PRJ), 1)
LOCAL_SRC_FILES += \
    minios_KeyPressTest.cpp \
    KeyPress_Check.cpp

LOCAL_SHARED_LIBRARIES += \
    libH2_Common
endif

ifneq ($(PERFORM_SDCARD_COUNT), 0)
LOCAL_SHARED_LIBRARIES += libminios3_sdcard
endif

ifeq ($(PERFORM_QR_FLASH), 1)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../minios3_camera
LOCAL_SHARED_LIBRARIES += libminios3_camera
endif

LOCAL_SHARED_LIBRARIES += libatd_common

LOCAL_STATIC_LIBRARIES := libaat_common

ifeq ($(HAS_XML_SHARED), yes)
LOCAL_SHARED_LIBRARIES += libxml2  # QCT
else
LOCAL_STATIC_LIBRARIES += libxml2
endif
LOCAL_SHARED_LIBRARIES += libaat_xml_parser

LOCAL_CFLAGS += -DMINIOS3_SKIA_WIDGET

LOCAL_MODULE := minios_qrdisp
LOCAL_MODULE_OWNER := lge
LOCAL_MODULE_TAGS := optional
ifeq (1,$(SUPPORT_64BIT))
LOCAL_MULTILIB := 32
endif
LOCAL_MODULE_PATH := $(TARGET_FACTORY_RAMDISK_OUT)/sbin

include $(BUILD_EXECUTABLE)
endif
