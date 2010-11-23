/* Copyright (C) 2010 The Android Open Source Project
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

#ifndef EQ_3BAND_H_
#define EQ_3BAND_H_

#include <jni.h>

/**
 * Structures
 */

typedef struct {

	/**
	 * Filter 1 (Low Band)
	 * lf: Low Frequency
	 * f1Px: Poles
	 */
	jdouble lf;
	jdouble f1P0;
	jdouble f1P1;
	jdouble f1P2;
	jdouble f1P3;

	/**
	 * Filter 2 (Hight Band)
	 * hf: Hight Frecuency
	 * f2Px: Poles
	 */
	jdouble hf;
	jdouble f2P0;
	jdouble f2P1;
	jdouble f2P2;
	jdouble f2P3;

	/**
	 * Sample history buffer
	 */
	jdouble sdm1;
	jdouble sdm2;
	jdouble sdm3;

	/**
	 * Gain controls
	 * lg: Low gain
	 * mg: Middle gain
	 * hg: Hight gain
	 */
	jdouble lg;
	jdouble mg;
	jdouble hg;

	/**
	 * Channel Mode
	 */
	jint mode;

} EQSTATE;

#endif /* EQ_3BAND_H_ */
