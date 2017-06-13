LOCAL_PATH := $(call my-dir)


##############################################
# generate aspectj libraries with ant library
.PHONY: aspectj_compile_libs
aspectj_compile_out_dir := $(call intermediates-dir-for,JAVA_LIBRARIES,aspectj_compile_libs,HOST,,COMMON)

aspectj_compile_java_libraries := \
	$(HOST_OUT_JAVA_LIBRARIES)/aspectjbuild-tools.jar \
	$(LOCAL_PATH)/lib/ant/lib/ant.jar \
	$(JAVA_HOME)/lib/tools.jar

aspectj_compile_main := org.aspectj.internal.tools.ant.taskdefs.Main
aspectj_compile_base := $(LOCAL_PATH)/

aspectj_compile_libs: $(aspectj_compile_java_libraries)
	( \
		java -classpath $(call normalize-path-list, $(aspectj_compile_java_libraries)) \
			$(aspectj_compile_main) \
			-verbose \
			-base $(aspectj_compile_base) \
			-out $(aspectj_compile_out_dir) \
	)

aspectj_compile_aspectjrt := $(aspectj_compile_out_dir)/dist/tools/lib/aspectjrt.jar
aspectj_compile_aspectjweaver := $(aspectj_compile_out_dir)/dist/tools/lib/aspectjweaver.jar
aspectj_compile_aspectjtools := $(aspectj_compile_out_dir)/dist/tools/lib/aspectjtools.jar
aspectj_compile_org_aspectj_matcher := $(aspectj_compile_out_dir)/dist/tools/lib/org.aspectj.matcher.jar

$(aspectj_compile_aspectjrt): aspectj_compile_libs
$(aspectj_compile_aspectjweaver): apsectj_compile_libs
$(aspectj_compile_aspectjtools): aspectj_compile_libs
$(aspectj_compile_org_aspectj_matcher): aspectj_compile_libs

##############################################
# copy static generated java libraries
$(TARGET_OUT_COMMON_INTERMEDIATES)/JAVA_LIBRARIES/aspectjrt_static_intermediates/javalib.jar: aspectj_compile_libs
	$(hide) mkdir -p $(dir $@)
	$(hide) cp $(aspectj_compile_aspectjrt) $@

$(HOST_OUT_COMMON_INTERMEDIATES)/JAVA_LIBRARIES/aspectjtools_static_intermediates/javalib.jar: aspectj_compile_libs
	$(hide) mkdir -p $(dir $@)
	$(hide) cp $(aspectj_compile_aspectjtools) $@

##############################################
# build aspectjbuild-tools for host library
include $(CLEAR_VARS)

LOCAL_MODULE := aspectjbuild-tools
LOCAL_JAVA_RESOURCE_DIRS := build/src/
LOCAL_SRC_FILES := $(call all-java-files-under, build/src)
LOCAL_JAVA_LIBRARIES := aspectj.ant

include $(BUILD_HOST_JAVA_LIBRARY)


##############################################
# use ant.jar library for build-tools
include $(CLEAR_VARS)
LOCAL_PREBUILT_JAVA_LIBRARIES := \
	aspectj.ant:lib/ant/lib/ant.jar
include $(BUILD_HOST_PREBUILT)


##############################################
# build target / host aspectj libraries
include $(CLEAR_VARS)
LOCAL_MODULE := aspectjrt
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_JARJAR_RULES := vendor/lge/build/tools/aspectj/rule.txt
LOCAL_JACK_ENABLED := disabled
LOCAL_STATIC_JAVA_LIBRARIES := aspectjrt_static
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := aspectjtools
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_JACK_ENABLED := disabled
LOCAL_STATIC_JAVA_LIBRARIES := aspectjtools_static
include $(BUILD_HOST_JAVA_LIBRARY)

