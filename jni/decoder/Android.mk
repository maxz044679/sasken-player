include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/OpenAL/include

DECODER_SRC_FILES := decoder/SaskenDecoder.cpp \
					 decoder/decoder.cpp \
					 decoder/PcmDecoder.cpp \
					 decoder/Mpg123Decoder.cpp \
					 decoder/VorbisDecoder.cpp \
					 decoder/AudioTrack.c

include $(LOCAL_PATH)/decoder/mpg123/Android.mk
include $(LOCAL_PATH)/decoder/vorbis/Android.mk  
LOCAL_SHARED_LIBRARIES := equalizer \
						  VU \
						  VI20Band
LOCAL_LDLIBS           := -llog -Wl,-s
LOCAL_MODULE           := decoder
LOCAL_ARM_MODE         := arm
LOCAL_CFLAGS           := -O2 -Wall
LOCAL_SRC_FILES        := $(DECODER_SRC_FILES)
include $(BUILD_SHARED_LIBRARY)