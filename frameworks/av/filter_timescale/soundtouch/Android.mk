LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

########################################
# LG OMX Sound Post-Processing Library #
########################################

LOCAL_C_INCLUDES:=$(LOCAL_PATH) \
					$(LOCAL_PATH)/src \
					$(LOCAL_PATH)/inc

#LOCAL_CFLAGS:=$(COMMON_CFLAGS)
LOCAL_CFLAGS+=-DSOUNDTOUCH_INTEGER_SAMPLES -DST_NO_EXCEPTION_HANDLING

LOCAL_SRC_FILES:= \
	src/AAFilter.cpp \
	src/BPMDetect.cpp \
	src/FIFOSampleBuffer.cpp \
	src/FIRFilter.cpp \
	src/PeakFinder.cpp \
	src/RateTransposer.cpp \
	src/SoundTouch.cpp \
	src/TDStretch.cpp \
	src/cpu_detect_x86_gcc.cpp

LOCAL_PRELINK_MODULE:=false
#LOCAL_MODULE_TAGS:=optional

#LOCAL_SHARED_LIBRARIES:=$(COMMON_SHARED_LIBRARIES)

LOCAL_MODULE:=libSoundTouch

#LOCAL_PREBUILT_LIBS:=libSoundTouch.a
#include $(BUILD_MULTI_PREBUILT)
include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)
