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
#include "biquad.h"

biquad mEq[62], *peq[62];
int nPEQ, mode, preg = 50;

const double EQ_Frecs[][40] = {
	{20, 1000, 5000, 20000}, // EQ of 3 bands
	{20, 80, 320, 1280, 5120, 16000}, // EQ of 5 bands
	{20, 200, 600, 1000, 3000, 5000, 10000, 16000}, // EQ of 7 bands
	{15.75, 31.5, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000}, // EQ of 10 bands
	{15.75, 25, 40, 63, 100, 160, 250, 400, 630, 1000, 1600, 2500,
		4000, 6300, 10000, 16000}, // EQ of 15 bands
	{15.84, 22.4, 31.5, 45, 63, 90, 125, 180, 250, 355, 500, 710,
		1000, 1400, 2000, 2800, 4000, 5600, 8000, 11200, 16000}, // EQ of 20 bands
	{15.87, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315,
		400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000,
		6300, 8000, 10000, 12500, 16000, 20000}, // EQ of 31 bands using 31 peaking EQ filters
};

const int nBands[] = {3, 5, 7, 10, 15, 20, 31};

void SaskenEqualizer_init(jint chs, jint id, jdouble sfrec)
{
	jint i;
	mode = chs;

	for (i = 0; i < 62; i++) {
		peq[i] = &mEq[i];
		memset(peq[i],'0', sizeof(biquad));
	}

	nPEQ = nBands[id] << 1;

	for (i = 0; i < nBands[id]; i++) {
		/**
		 * Here we make the calculation of the center frecuency of the peaking filter
		 */
		peq[i]->cf = peq[(nPEQ >> 1) + i]->cf = pow(EQ_Frecs[id][i] * EQ_Frecs[id][i + 1], 0.5);

		/**
		 * Here we make the calculation of the bandwidth
		 */
		peq[i]->wb = peq[(nPEQ >> 1) + i]->wb = log(EQ_Frecs[id][i + 1]/EQ_Frecs[id][i])/log(2);
	}

	for (i = 0; i < nPEQ; i++) {
		peq[i]->sfrec = sfrec;
		setup(peq[i], PEQ, 0, peq[i]->cf, peq[i]->sfrec, peq[i]->wb);
		peq[i]->chmode = chs;
	}
}

void SaskenEqualizer_configure(const jint* buffer)
{
	if (!buffer) {
		/**
		 * This case is almost imposible, but anyway.
		 */
		LOGF("Not buffer provided!!");
		return;
	}

	int i, pre, mid = nPEQ>>1;
	preg = *buffer;

	for (i = 0; i < nPEQ; i++) {
		/**
		 * Here We make the calculation of the new gains (Coefficients)
		 * of the new buffer to be processed next time
		 */
		setup(peq[i], PEQ, (0.24 * buffer[(i % mid) + 1] - 12.0), peq[i]->cf, peq[i]->sfrec, peq[i]->wb);
	}
}

void SaskenEqualizer_reset()
{
	int i;

	for (i = 0; i < 62; i++)
		memset(peq[i],'0', sizeof(biquad));
}

void SaskenEqualizer_process(jbyte* buffer, int len)
{
	package mpackage;
	int e;

	memset(&mpackage, '0', sizeof(package));
	e = nPEQ>>1;

	if (!buffer) {
		LOGF("Failure: No buffer available");
		return;
	}

	mpackage.ch = &peq[0];
	mpackage.buffer = buffer;
	mpackage.len = len;
	mpackage.offset = 0;

	if (mode == 1) {
		process(&mpackage);
	} else {
		process(&mpackage);
		mpackage.ch = &peq[e];
		mpackage.offset = 2;
		process(&mpackage);
	}
}
