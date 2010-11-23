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
#include "../mp3_dec/Mpg123Decoder.h"
#include "../vorbis_dec/VorbisDecoder.h"
#include <android/log.h>
#include <stdio.h>

#define DEBUG_TAG "NDK_SaskenDecoder"

JNIEXPORT jlong JNICALL Java_com_sasken_player_decoders_SaskenDecoder_OpenFile(JNIEnv* env, jobject this, jstring file) {

	jboolean isCopy;
	jlong    ref;

	// Obtain the file name
    const char * szFile = (*env)->GetStringUTFChars(env, file, &isCopy);

    // log the name
    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Open [%s]", szFile);
    
//    ref = (jlong)Mpg123Decoder_openFile((char *)szFile);
    ref = (jlong)VorbisDecoder_openFile((char *)szFile);
    
    // release the filename
    (*env)->ReleaseStringUTFChars(env, file, szFile);
    
    return ref;

}

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_ReadSamples(JNIEnv* env, jobject this, jlong file, jbyteArray buffer) {

	jint len;
	char *data;
	int  samples;
	int  i;
	jbyte *buf;
	
	len = (*env)->GetArrayLength(env, buffer);
	samples = len/2;

	data = (char*) malloc (sizeof(char)*len);
	if (data == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return 0;
	}
	
//	samples = Mpg123Decoder_readSamples(file, data, samples);
	samples = VorbisDecoder_readSamples(file, data, samples);
	
	// log the name
//    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Decoded [%d samples]", samples);
	
	
	len = samples * 2;
	
	int bytes = fwrite(data, sizeof(char), len, out);
    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "written %d bytes", bytes);

   /**
	 * Note: Here we are trying to get a critical region and so if any other running thread
	 * in your dalvik/VM or any other place tries to acquire this region, your
	 * application could fall in a deadlock.
	 * */
	buf = (*env)->GetPrimitiveArrayCritical(env, buffer, 0);

	if (buf == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Cannot access the PrimitiveArray");
		free(data);
		return 0;
	}
	
	for (i=0; i<len; i++) {
		buf[i] = (char)data[i];
	}
	
   	/**
	 * We need to make sure releasing previous acquired resources,
	 * to avoid deadlock scenarios.
	 */
	(*env)->ReleasePrimitiveArrayCritical(env, buffer, buf, 0);
	
	free(data);

	return len;

}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_CloseFile(JNIEnv* env, jobject this, jlong file) {
//	Mpg123Decoder_closeFile(file);
	VorbisDecoder_closeFile(file);
	
}

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_GetRate(JNIEnv* env, jobject this, jlong file){
//	return Mpg123Decoder_getRate(file);
	return VorbisDecoder_getRate(file);
}

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_GetNumChannels(JNIEnv* env, jobject this, jlong file){
//	return Mpg123Decoder_getNumChannels(file);
	return VorbisDecoder_getNumChannels(file);
}

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_seekTo(JNIEnv* env, jobject this, jlong file, jint msec){
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Seek To %d ms", msec);
}

