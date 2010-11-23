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

#include "prototypes.h"

void Java_com_sasken_player_equalizers_SaskenEqualizer_init(JNIEnv* env, jobject obj,
	jint channelMode, jint lowfrec, jint highfrec,
	jint mixfrec /*Sampling Frec, E.g 48KHz*/)
{
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_changeChannelMode(JNIEnv* env,
	jobject obj, jint channelMode)
{
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_configure(JNIEnv* env,
	jobject obj, jdoubleArray eqParams)
{
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_reset(JNIEnv* env,
	jobject obj)
{
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_process(JNIEnv* env,
	jobject obj, jshortArray buffer)
{
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_getBandLimits(JNIEnv* env, jobject obj,
	jdoubleArray bands)
{
}
