#### Rules to build the VU library ######
include $(CLEAR_VARS)
include ${LOCAL_PATH}/Visualizers/VU/Android.mk
LOCAL_LDLIBS    := -llog
LOCAL_MODULE    := VU
LOCAL_SRC_FILES := ${VU_SRC_FILES}
include $(BUILD_SHARED_LIBRARY)

#### Rules to build the VI20Band library ######
include $(CLEAR_VARS)
#include ${LOCAL_PATH}/Visualizers/VU/Android.mk
include ${LOCAL_PATH}/Visualizers/VI20Band/Android.mk
LOCAL_SHARED_LIBRARIES := VU
LOCAL_LDLIBS    := -llog
LOCAL_MODULE    := VI20Band
LOCAL_SRC_FILES := ${VI20BAND_SRC_FILES}
include $(BUILD_SHARED_LIBRARY)

#### Rules to build the VI10BandLeds library ######
include $(CLEAR_VARS)
include ${LOCAL_PATH}/Visualizers/VI10BandLeds/Android.mk
LOCAL_SHARED_LIBRARIES := VU
LOCAL_LDLIBS    := -llog
LOCAL_MODULE    := VI10BandLeds
LOCAL_SRC_FILES := ${VI10BANDLEDS_SRC_FILES}
include $(BUILD_SHARED_LIBRARY)