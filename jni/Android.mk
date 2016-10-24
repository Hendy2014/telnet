LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LIB_TELNET_SRC_DIR := $(LOCAL_PATH)/../lib
LOCAL_SRC_FILES += src/client/main.c \
					$(LIB_TELNET_SRC_DIR)/telnet.c \
					$(LIB_TELNET_SRC_DIR)/telnet_cmds.c \
					$(LIB_TELNET_SRC_DIR)/telnet_log.c \
					$(LIB_TELNET_SRC_DIR)/telnet_nvt.c \
					$(LIB_TELNET_SRC_DIR)/telnet_options.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

					
										
LOCAL_MODULE := telnet

LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_SHARED_LIBRARIES:= libcutils libutils liblog

LOCAL_LDLIBS :=-llog

include $(BUILD_EXECUTABLE)
