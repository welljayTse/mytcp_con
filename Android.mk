ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES  := libcutils   libutils  



ifeq ($(SPRD_HOSTAP_SUPPORT),true)
LOCAL_CFLAGS += -DHOSTAPD_SUPPORT
endif

LOCAL_SRC_FILES     :=\
			tcp_con.c  \
		       iot_util.c 

ifeq ($(PLATFORM_VERSION),$(filter $(PLATFORM_VERSION),4.4 4.4.1 4.4.2 4.4.3 4.4.4))
include external/stlport/libstlport.mk
endif

LOCAL_MODULE := tcp_con
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
