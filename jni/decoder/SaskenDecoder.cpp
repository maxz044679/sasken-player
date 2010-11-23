/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "SaskenDecoder.h"
#include "decoder.h"
#include "PcmDecoder.h"
#include "Mpg123Decoder.h"
#include "VorbisDecoder.h"
#include <android/log.h>
#include <stdio.h>

#include "decoder/AudioTrack.h"

#define DEBUG_TAG "NDK_SaskenDecoder"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, DEBUG_TAG, __VA_ARGS__)

enum format {
	UNKNOWN,
	PCM,
	MP3,
	VORBIS
};

format check_file(const char *file)
{
	FILE *fp;
	char buffer[5];

	format found = UNKNOWN;

	fp = fopen(file, "rb" );

	if (fp != NULL)
	{
		if (fread(buffer, sizeof(char), 4, fp) == 4) 
		{
			if (memcmp(buffer, "RIFF", 4) == 0)
				found = PCM;
			else if (memcmp(buffer, "ID3", 3) == 0)
				found = MP3;
			else if (memcmp(buffer, "OggS", 4) == 0)
				found = VORBIS;
		}
	}

	fclose(fp);

	return found;
}


JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_CheckFile
	(JNIEnv* env, jobject, jstring file)
{
	// Obtain the file name
	const char *szFile = env->GetStringUTFChars(file, NULL);
	format found = check_file(szFile);

	// release the filename
	env->ReleaseStringUTFChars(file, szFile);

	return (jint)found;
}

JNIEXPORT jlong JNICALL Java_com_sasken_player_decoders_SaskenDecoder_OpenFile
	(JNIEnv* env, jobject, jstring file)
{
	int      result = 0;

	decoder *dec = NULL;

	// Obtain the file name
	const char * szFile = env->GetStringUTFChars(file, NULL);

	switch (check_file(szFile))
	{
		case PCM:
			dec = new PcmDecoder();
			break;
		case MP3:
			dec = new Mpg123Decoder();
			break;
		case VORBIS:
			dec = new VorbisDecoder();
			break;
		default:
			break;
	}

	if (dec != NULL) {
		// try to open the file
		result = dec->open((char *)szFile);
	}

	// release the filename
	env->ReleaseStringUTFChars(file, szFile);

	if (result == 0 && dec != NULL) {
		delete dec;
		dec = NULL;
	}
	return (jlong)dec;
}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_Start
	(JNIEnv* env, jobject thiz, jlong handle)
{
	if (handle) {
		decoder *dec = (decoder *)handle;
		jobject ref = 0;
		if (!dec->isPlaying()) {
			ref = env->NewGlobalRef(thiz);
		}
		dec->start(ref);
	}
}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_Pause
	(JNIEnv* env, jobject, jlong handle)
{
	if (handle) {
		LOGI("--- Pause ---");
		decoder *dec = (decoder *)handle;
		dec->pause();
	}
}

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_ReadSamples
	(JNIEnv* env, jobject, jlong handle, jbyteArray buffer)
{
	jbyte *data;
	int  samples = 0;
	char *buf;

	if (handle) {
		decoder *dec = (decoder *)handle;
		samples = dec->read();

		if (samples > 0 ) {
			data = dec->getData();

			/**
			 * Note: Here we are trying to get a critical region and so if any other running thread
			 * in your dalvik/VM or any other place tries to acquire this region, your
			 * application might deadlock.
			 */
			buf = (char *)env->GetPrimitiveArrayCritical(buffer, 0);

			if (buf == NULL) {
				LOGE("Cannot access the PrimitiveArray");
				return 0;
			}

			memcpy(buf,data,samples);

			/**
			 * We need to make sure releasing previous acquired resources,
			 * to avoid deadlock scenarios.
			 */
			env->ReleasePrimitiveArrayCritical(buffer, buf, 0);

		}
	}
	
	return (jint)samples;
}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_CloseFile
	(JNIEnv* env, jobject, jlong handle)
{
	if (handle) {
		decoder *dec = (decoder *)handle;
		dec->close();
	}
}

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_GetRate
	(JNIEnv* env, jobject, jlong handle)
{
	int rate = 0;
	if (handle) {
		decoder *dec = (decoder *)handle;
		rate = dec->rate();
	}
	return (jint)rate;
}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_SetBufferSize
	(JNIEnv* env, jobject, jlong handle, jint size)
{
	if (handle) {
		decoder *dec = (decoder *)handle;
		dec->setBuffer(size);
	}
}

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_GetNumChannels
	(JNIEnv* env, jobject, jlong handle)
{
	int channels = 0;
	if (handle) {
		decoder *dec = (decoder *)handle;
		channels = dec->channels();
	}

	return (jint)channels;
}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_Stop
	(JNIEnv* env, jobject, jlong handle)
{
	if (handle) {
		decoder *dec = (decoder *)handle;
		dec->stop();
	}
}

JNIEXPORT jboolean JNICALL Java_com_sasken_player_decoders_SaskenDecoder_isPlaying
	(JNIEnv* env, jobject thiz, jlong handle)
{
	bool result = false;

	if (handle) {
		decoder *dec = (decoder *)handle;
		result = dec->isPlaying();
	}

	return (jboolean)result;
}

JNIEXPORT jboolean JNICALL Java_com_sasken_player_decoders_SaskenDecoder_isPaused
	(JNIEnv* env, jobject, jlong handle)
{
	bool result = false;
	if (handle) {
		decoder *dec = (decoder *)handle;
		result = dec->isPaused();
	}
	return (jboolean)result;
}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_seekTo
	(JNIEnv* env, jobject, jlong handle, jint msec)
{
	if (handle) {
		LOGI("Seek To %d ms", msec);

		decoder *dec = (decoder *)handle;

		if (msec == 0) {
			dec->rewind();
		}
	}
}
