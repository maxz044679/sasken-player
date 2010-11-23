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

#ifndef SASKEN_DECODER
#define SASKEN_DECODER

#include <string.h>
#include <jni.h>

extern jboolean Java_com_sasken_player_decoders_NativeDecoder_Init(JNIEnv*, jobject, jstring);

extern jint Java_com_sasken_player_decoders_NativeDecoder_Decode(JNIEnv*, jobject, jbyteArray);

extern void Java_com_sasken_player_decoders_NativeDecoder_Release(JNIEnv*, jobject);

extern jint Java_com_sasken_player_decoders_NativeDecoder_GetSampleRateInHz(JNIEnv*, jobject);

extern jint Java_com_sasken_player_decoders_NativeDecoder_GetNumberOfChannels(JNIEnv*, jobject);

extern void Java_com_sasken_player_decoders_NativeDecoder_seekTo(JNIEnv*, jobject, jint);

#endif
