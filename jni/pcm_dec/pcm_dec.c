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

#include "../decoder.h"
#include <stdio.h>
#include <android/log.h>

#define DEBUG_TAG "NDK_Pcm_Dec"
#define HEADER_SIZE 40

// pointer to the file
static FILE* pFile;
static char  channels;
static long  rate;


unsigned long GetValue(const unsigned char *ptr, size_t num) {
	unsigned long value = 0;
	int i;
	for (i=num-1; i>=0; i--) {
		value = value<< 8 | ((long)ptr[i] & 0xFF);
	}
	
	return value;
}

int CheckHeader(char *buffer){

	// Verify the Chunk descriptor
	if (memcmp(buffer, "RIFF", 4) != 0) {
		buffer[4] = 0;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Wrong chunk descriptor [%s]", buffer);
		return 0;
	}
	
	// Check the file format
	if (memcmp(buffer + 8, "WAVE", 4) != 0) {
		buffer[12] = 0;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Wrong file format [%s]", buffer+8);
		return 0;
	}
	
	// Check the fmt subchunk
	if (memcmp(buffer+12, "fmt ", 4) != 0) {
		buffer[16] = 0;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Wrong fmt subchunk [%s]", buffer+8);
		return 0;
	}

	// Check the data subchunk
	if (memcmp(buffer+36, "data", 4) != 0) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Wrong data subchunk [%c%c%c%c]", buffer+36, buffer+37, buffer+38, buffer+39);
		return 0;
	}
	
	// the subchunk size should be 16
	if (GetValue(buffer + 16,4) != 16) {
		buffer[16] = 0;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "invalid subchunk1Size [%d]", GetValue(buffer + 16,4));
		return 0;
	}

	// the audio format should be 1
	if (GetValue(buffer + 20, 2) != 1) {
		buffer[16] = 0;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "invalid Audio Format [%d]", GetValue(buffer + 20, 2));
		return 0;
	}
	
	// the bytes per sample should be 16
	if (GetValue(buffer + 34, 2) != 16) {
		buffer[16] = 0;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "invalid BitsPerSample [%d]", GetValue(buffer + 34, 2));
		return 0;
	}
	
	// Read the NumChannels
	channels = GetValue(buffer + 22, 2);
	if (channels < 1 || channels > 2) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "invalid number of channels [%d]", channels);
		return 0;
	}
	
	// Read the SampleRate
	rate = GetValue(buffer + 24, 4);
	if (rate < 8000 || rate > 48000) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "invalid sample rate [%d]", rate);
		return 0;
	}
	
	// The Block Align should be twice the number of channels 
	if (GetValue(buffer + 32, 2) != channels*2) {
		buffer[16] = 0;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "invalid Block Align [%d]", GetValue(buffer + 32, 2));
		return 0;
	}
	
	// looks like a valid pcm header
	return 1;

}

jboolean Java_com_sasken_player_decoders_NativeDecoder_Init(JNIEnv* env, jobject this, jstring file) {

	jboolean isCopy;
	long lSize = 0;
	char *buffer;
	size_t result;
	long audio_format;
	long num_channels;
	long sample_rate;
	int valid_file;
	
	
	// Obtain the file name
    const char * szFile = (*env)->GetStringUTFChars(env, file, &isCopy);

    // log the name
    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Open [%s]", szFile);
    
    // open the file
    pFile = fopen(szFile, "rb");
    
    // release the filename
    (*env)->ReleaseStringUTFChars(env, file, szFile);
    
    if (pFile == NULL) {
        __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "file open failure");
        return 0;
    }
    
    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "file open success");
       
	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "file size [%d]", lSize);
    if (lSize < HEADER_SIZE) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "file too small to be a valid wav file");
		fclose(pFile);
		pFile = NULL;
		return 0;
    }
    
    // rewind the to the beginning
	rewind (pFile);
       	
    // allocate memory to contain the header:
  	buffer = (char*) malloc (HEADER_SIZE);
  	if (buffer == NULL) {
  		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
  		fclose(pFile);
		pFile = NULL;
  		return 0;
  	}

	// load the header to the memory
	result = fread(buffer,1,HEADER_SIZE,pFile);
	if (result != HEADER_SIZE) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Reading error");
		free(buffer);
		fclose(pFile);
		return 0;
	}
	
	valid_file = CheckHeader(buffer);
	
	free(buffer);
	
	if (valid_file == 0) {
		fclose(pFile);
		pFile = NULL;
	}
    
	return valid_file;
	
}

jint Java_com_sasken_player_decoders_NativeDecoder_Decode(JNIEnv* env, jobject this, jbyteArray buffer) {
   
	jbyte *buf;
	jint len;
	int  read;
	
	int i;
	
	char  *data;

	len = (*env)->GetArrayLength(env, buffer);

	data = (char*) malloc (sizeof(char)*len);

	if (data == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return 0;
	}

	read = fread(data, sizeof(char), len, pFile);

   
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
	
	for (i=0; i<read; i++) {
		buf[i] = (char)data[i];
	}
	
   	/**
	 * We need to make sure releasing previous acquired resources,
	 * to avoid deadlock scenarios.
	 */
	(*env)->ReleasePrimitiveArrayCritical(env, buffer, buf, 0);
	
	free(data);
   
   return read;
}

void Java_com_sasken_player_decoders_NativeDecoder_Release(JNIEnv* env, jobject this) {
	if (pFile != NULL) {
		fclose(pFile);
	}
}

jint Java_com_sasken_player_decoders_NativeDecoder_GetSampleRateInHz(JNIEnv* env, jobject this) {
	return (int)rate;
}

jint Java_com_sasken_player_decoders_NativeDecoder_GetNumberOfChannels(JNIEnv* env, jobject this){
	return (int)channels;
}

void Java_com_sasken_player_decoders_NativeDecoder_seekTo(JNIEnv* env, jobject this, jint msec) {

	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Seek To %d ms", msec);

	fseek ( pFile , (long)HEADER_SIZE , SEEK_SET );
	
}

