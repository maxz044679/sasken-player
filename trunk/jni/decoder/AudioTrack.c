#include <stdlib.h>
#include <jni.h>
#include <pthread.h>
#include <android/log.h>

#include "decoder/AudioTrack.h"

JavaVM* javaVM = NULL;
jclass cAudioTrack = NULL;
jclass cSaskenDecoder = NULL;
jclass cEqualizer = NULL;
jfieldID mChange;
jfieldID mEqID;
jfieldID mEnable;
jfieldID mEq;
jmethodID mGetG;
jmethodID mAudioTrack;
jmethodID mGetMinBufferSize;
jmethodID mPlay;
jmethodID mPause;
jmethodID mStop;
jmethodID mRelease;
jmethodID mWrite;
jmethodID mGetPlayState;
jmethodID mCallBack;

#define DEBUG_TAG "NDK_AudioTrack"

#ifndef LOGE
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, DEBUG_TAG, __VA_ARGS__)
#endif

#ifndef LOGI
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, __VA_ARGS__)
#endif

static JNIEnv* GetEnv()
{
	JNIEnv* env = NULL;
	if (javaVM) {
		if ((*javaVM)->GetEnv(javaVM, (void**)&env, JNI_VERSION_1_2) != JNI_OK)
		{
			env = NULL;
		}
	}
	return env;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	javaVM = vm;
	JNIEnv* env;
	jint result = -1;

	LOGI("JNI_OnLoad Running...");

	env = GetEnv();
	if (env == NULL) {
		LOGE("Error: GetEnv failed\n");
		return result;
	}

	if (cSaskenDecoder == NULL) {
		cSaskenDecoder = (*env)->FindClass(env, "com/sasken/player/decoders/SaskenDecoder");
		if (!cSaskenDecoder) {
			LOGE("com.sasken.player.decoders.SaskenDecoder class is not found.");
			return result;
		}
		cSaskenDecoder = (*env)->NewGlobalRef(env, cSaskenDecoder);
		mCallBack = (*env)->GetMethodID(env, cSaskenDecoder, "callBack", "(I)V");

		mEq = (*env)->GetStaticFieldID(env, cSaskenDecoder, "equalizer",
			"Lcom/sasken/player/equalizers/SaskenEqualizer;");
		if (!mEq) {
			LOGE(">=1 Fields not found, at file: %s, Line: %d", __FILE__, __LINE__);
			return result;
		}
	}

	if (!cEqualizer) {
		cEqualizer = (*env)->FindClass(env, "com/sasken/player/equalizers/SaskenEqualizer");
		if (!cEqualizer) {
			LOGE("Class com/sasken/player/equalizers/SaskenEqualizer not found");
			return result; // Exception thrown
		}
		cEqualizer = (*env)->NewGlobalRef(env, cEqualizer);
		if (!cEqualizer) {
			LOGE("Out of memory, at file: %s, Line: %d", __FILE__, __LINE__);
			return result;
		}
		mChange = (*env)->GetStaticFieldID(env, cEqualizer, "change", "Z");
		mEqID = (*env)->GetStaticFieldID(env, cEqualizer, "id", "I");
		mEnable = (*env)->GetStaticFieldID(env, cEqualizer, "enable", "Z");
		mGetG = (*env)->GetMethodID(env, cEqualizer, "getG", "()[I");
		if (!mChange || !mEqID || !mGetG || !mEnable) {
			LOGE(">=1 Fields not found, at file: %s, Line: %d", __FILE__, __LINE__);
			return result;
		}
	}

	/* Cache AudioTrack class and it's method id's
	 * And do this only once!
	 */
	if (cAudioTrack == NULL)
	{
		cAudioTrack = (*env)->FindClass(env, "android/media/AudioTrack");
		if (!cAudioTrack)
		{
			LOGE("android.media.AudioTrack class is not found. Are you running at least 1.5 version?");
			return result;
		}

		cAudioTrack = (*env)->NewGlobalRef(env, cAudioTrack);

		mAudioTrack = (*env)->GetMethodID(env, cAudioTrack, "<init>", "(IIIIII)V");
		mGetMinBufferSize = (*env)->GetStaticMethodID(env, cAudioTrack, "getMinBufferSize", "(III)I");
		mPlay = (*env)->GetMethodID(env, cAudioTrack, "play", "()V");
		mPause = (*env)->GetMethodID(env, cAudioTrack, "pause", "()V");
		mStop = (*env)->GetMethodID(env, cAudioTrack, "stop", "()V");
		mRelease = (*env)->GetMethodID(env, cAudioTrack, "release", "()V");
		mWrite = (*env)->GetMethodID(env, cAudioTrack, "write", "([BII)I");
		mGetPlayState = (*env)->GetMethodID(env, cAudioTrack, "getPlayState", "()I");
	}
	return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
	JNIEnv* env = GetEnv();

    LOGI("JNI_OnUnload Running...");

    if (env == NULL)
	{
    	LOGE("Error: GetEnv failed\n");
	}
	else
	{
		/* release cached AudioTrack class */
		(*env)->DeleteGlobalRef(env, cAudioTrack);
		(*env)->DeleteGlobalRef(env, cSaskenDecoder);
		(*env)->DeleteGlobalRef(env, cEqualizer);
	}
}
