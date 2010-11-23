#ifndef _AUDIOTRACK_H_
#define _AUDIOTRACK_H_

#include <jni.h>

#define STREAM_MUSIC 3
#define CHANNEL_CONFIGURATION_MONO 2
#define CHANNEL_CONFIGURATION_STEREO 3
#define ENCODING_PCM_8BIT 3
#define ENCODING_PCM_16BIT 2
#define MODE_STREAM 1
#define PLAYSTATE_PLAYING 3

#if defined(__cplusplus)
extern "C" {
#endif

extern JavaVM* javaVM;
extern jclass cAudioTrack;
extern jclass cSaskenDecoder;
extern jclass cEqualizer;
extern jfieldID mChange;
extern jfieldID mEqID;
extern jfieldID mEnable;
extern jfieldID mEq;
extern jmethodID mGetG;
extern jmethodID mAudioTrack;
extern jmethodID mGetMinBufferSize;
extern jmethodID mPlay;
extern jmethodID mPause;
extern jmethodID mStop;
extern jmethodID mRelease;
extern jmethodID mWrite;
extern jmethodID mGetPlayState;
extern jmethodID mCallBack;

/**
 * The following list of methods will be used by the
 * decoder library to equalize the decoded buffer
 * just when the equalizer is enabled
 */
extern void SaskenEqualizer_reset();
extern void SaskenEqualizer_init(jint chs, jint EQid, jdouble sfrec);
extern void SaskenEqualizer_process(jbyte* buffer, int len);
extern void SaskenEqualizer_configure(const int* buffer);

#if defined(__cplusplus)
}  /* extern "C" */
#endif

#endif
