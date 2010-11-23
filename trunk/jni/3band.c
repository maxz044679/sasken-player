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

#include <math.h>
#include "prototypes.h"
#include "3band.h"

/**
 * Some parameters for this EQ.
 */

/**
 * Very small amount (Denormal Fix)
 */
static jdouble vsa = (1.0/4294967295.0);

static EQSTATE eq, *p  = &eq;

// This macro calculates low and hight cutoff frecuencies for this specific EQ.
#define CUTOFFF(F, MIXF) ((2) * sin((M_PI) * ((F * 1.0)/(MIXF * 1.0))))

/**
 * Initialise EQ
 *
 * Recommended frecuencies are
 * Low frec: 880Hz
 * Hight frec: 5000
 * Set mixfrec to whatever rate your system is using (ex. 48KHz)
 *
 */

void Java_com_sasken_player_equalizers_SaskenEqualizer_init(JNIEnv* env, jobject obj,
	jint channelMode, jint lowfrec, jint highfrec,
	jint mixfrec /*Sampling Frec, E.g 48KHz*/)
{
	/*
	 * Making cleanup
	 */
	memset(p, '0', sizeof(EQSTATE));

	/**
	 * Set Low/Mid/Hight gains to unit
	 */
	eq.lg = 1.0;
	eq.mg = 1.0;
	eq.hg = 1.0;

	/**
	 * Calculate filter cutoff frecuencies
	 */
	eq.lf = CUTOFFF(lowfrec, mixfrec);
	eq.hf = CUTOFFF(highfrec, mixfrec);

	/**
	 * Channel Mode
	 */
	eq.mode = channelMode;
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_changeChannelMode(JNIEnv* env,
	jobject obj, jint channelMode)
{
	eq.mode = channelMode;
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_configure(JNIEnv* env,
	jobject obj, jdoubleArray gain)
{
	jdouble *buf;

	/**
	 * Note: Here we are trying to get a critical region and so if any other thread
	 * running in your dalvik/VM or any other place tries to acquire this region, your
	 * application could fall in a deadlock.
	 * */

	if (!gain)
		return;

	buf = (*env)->GetPrimitiveArrayCritical(env, gain, 0);

	if (!buf) {
		return;
	}

	eq.lg = buf[0];
	eq.mg = buf[1];
	eq.hg = buf[2];

	/**
	 * We need to make sure releasing previous acquired resources,
	 * to avoid deadlock scenarios.
	 */
	(*env)->ReleasePrimitiveArrayCritical(env, gain, buf, 0);
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_reset(JNIEnv* env,
	jobject obj)
{
	/**
	 * Making cleanup
	 */
	memset(p, '0', sizeof(EQSTATE));
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_process(JNIEnv* env,
	jobject obj, jshortArray buffer)
{
	/**
	 * buf: buffer pointer.
	 * Low/Mid/High sample values
	 */
	jshort *buf, l, m, h;
	jint len, i;

	len = (*env)->GetArrayLength(env, buffer);
	/**
	 * Note: Here we are trying to get a critical region and so if any other running thread
	 * in your dalvik/VM or any other place tries to acquire this region, your
	 * application could fall in a deadlock.
	 * */
	buf = (*env)->GetPrimitiveArrayCritical(env, buffer, 0);

	if (!buf) {
		return;
	}

	for (i = 0; i < len; i++) {
		// Filter 1 (Low pass)
		if (!i%2) {
			eq.f1P0 += (eq.lf * (buf[i] - eq.f1P0)) + vsa;
			eq.f1P1 += (eq.lf * (eq.f1P0 - eq.f1P1));
			eq.f1P2 += (eq.lf * (eq.f1P1 - eq.f1P2));
			eq.f1P3 += (eq.lf * (eq.f1P2 - eq.f1P3));
			l = eq.f1P3;

			// Filter #2 (highpass)
			eq.f2P0 += (eq.hf * (buf[i] - eq.f2P0)) + vsa;
			eq.f2P1 += (eq.hf * (eq.f2P0 - eq.f2P1));
			eq.f2P2 += (eq.hf * (eq.f2P1 - eq.f2P2));
			eq.f2P3 += (eq.hf * (eq.f2P2 - eq.f2P3));
			h = eq.sdm3 - eq.f2P3;

			// Calculate midrange (signal - (low + high))
			m = eq.f2P3 - eq.f1P3;

			// Scale, combine and store
			l *= eq.lg;
			m *= eq.mg;
			h *= eq.hg;

			// Shuffle history buffer
			eq.sdm3 = eq.sdm2;
			eq.sdm2 = eq.sdm1;
			eq.sdm1 = buf[i];

			// Store Result to the buffer
			buf[i] = (jshort)(l + m + h);
		} else {
		}
	}

	/**
	 * We need to make sure releasing previous acquired resources,
	 * to avoid deadlock scenarios.
	 */
	(*env)->ReleasePrimitiveArrayCritical(env, buffer, buf, 0);
}

void Java_com_sasken_player_equalizers_SaskenEqualizer_getBandLimits(JNIEnv* env, jobject obj,
	jdoubleArray bands)
{
}
