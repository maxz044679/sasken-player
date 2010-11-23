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

#include <jni.h>

#ifndef _SASKEN_DECODER_H_
#define _SASKEN_DECODER_H_

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_CheckFile
	(JNIEnv*, jobject, jstring);

JNIEXPORT jlong JNICALL Java_com_sasken_player_decoders_SaskenDecoder_OpenFile
	(JNIEnv*, jobject, jstring);

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_ReadSamples
	(JNIEnv*, jobject, jlong, jbyteArray);

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_CloseFile
	(JNIEnv*, jobject, jlong);

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_GetRate
	(JNIEnv*, jobject, jlong);

JNIEXPORT jint JNICALL Java_com_sasken_player_decoders_SaskenDecoder_GetNumChannels
	(JNIEnv*, jobject, jlong);

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_seekTo
	(JNIEnv*, jobject, jlong, jint);
	
JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_SetBufferSize
	(JNIEnv*, jobject, jlong, jint);

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_Start
	(JNIEnv*, jobject, jlong);

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_Pause
	(JNIEnv*, jobject, jlong);

JNIEXPORT void JNICALL Java_com_sasken_player_decoders_SaskenDecoder_Stop
	(JNIEnv*, jobject, jlong);

JNIEXPORT jboolean JNICALL Java_com_sasken_player_decoders_SaskenDecoder_isPlaying
	(JNIEnv*, jobject, jlong);

JNIEXPORT jboolean JNICALL Java_com_sasken_player_decoders_SaskenDecoder_isPaused
	(JNIEnv*, jobject, jlong);

#ifdef __cplusplus
}
#endif

#endif /* _SASKEN_DECODER_H_ */
