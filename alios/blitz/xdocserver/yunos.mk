LOCAL_PATH := $(call my-dir)

OFFICEKIT_DIR := $(LOCAL_PATH)/../../..
POPPLER_DIR := $(OFFICEKIT_DIR)/workdir/UnpackedTarball/poppler

include $(CLEAR_VARS)
LOCAL_MODULE := xdocserver
LOCAL_SRC_FILES:= \
    $(LOCAL_PATH)/main.cpp \
    $(LOCAL_PATH)/PdfService.cpp \
    $(LOCAL_PATH)/DocumentManager.cpp \

LOCAL_C_INCLUDES:= \
    $(base-includes) \
    $(jsoncpp-includes) \
    $(POPPLER_DIR)/cpp \

LOCAL_SHARED_LIBRARIES += \
    libbase \
    libaudio \
    libjsoncpp \
    libfreetype \
    libpng16 \
    libjpeg \
    libfontconfig \
    liblog

LOCAL_CXXFLAGS := -std=c++11 -g -O0
LOCAL_CXXFLAGS += \
    -DLOG_TAG=\"xpdf\" \
    -DHAVE_POSIX \
    -DHAVE_PRCTL

LOCAL_LDFLAGS := -lstdc++ \
                 -lpthread \
                 $(POPPLER_DIR)/cpp/.libs/libpoppler-cpp.a \
                 $(POPPLER_DIR)/poppler/.libs/libpoppler.a \

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := xdocclient
LOCAL_SRC_FILES:= \
    $(LOCAL_PATH)/client.cpp \

LOCAL_C_INCLUDES:= \
    $(base-includes) \
    $(jsoncpp-includes) \

LOCAL_SHARED_LIBRARIES += \
    libbase \
    libaudio \
    libjsoncpp \
    liblog

LOCAL_CXXFLAGS := -std=c++11 -g -O0
LOCAL_CXXFLAGS += \
    -DLOG_TAG=\"xpdfclient\" \
    -DHAVE_POSIX \
    -DHAVE_PRCTL

include $(BUILD_EXECUTABLE)
