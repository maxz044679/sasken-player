/**
 * Simple implementation of Biquad filters -- Tom St Denis
 *
 * Based on the work
 *
 * Cookbook formulae for audio EQ biquad filter coefficients
 *
 * ---------------------------------------------------------
 * By Robert Bristow-Johnson, pbjrbj@viconet.com  a.k.a. robert@audioheads.com

 * Available on the web at:
 * http://www.smartelectronix.com/musicdsp/text/filters005.txt
 *
 * This work is hereby placed in the public domain for all purposes, whether
 * commercial, free [as in speech] or educational, etc.  Use the code and please
 * give me credit if you wish.
 *
 * Tom St Denis -- http://tomstdenis.home.dhs.org
*/

#include <math.h>
#include <stdlib.h>
#include <jni.h>
#include "../info.h"

#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern int nPEQ, mode, preg;

/**
 * Whatever sample type you want
 */
typedef double smp_type;

/**
 * This holds the data required to update samples thru a filter
 */
typedef struct {
	smp_type a0, a1, a2, a3, a4;
	smp_type x1, x2, y1, y2;
	smp_type sfrec, cf, wb;
	int chmode;
} biquad;

typedef struct {
	biquad **ch;
	jbyte *buffer;
	jint len, offset;
} package;

extern void *process(void *);
extern jdouble processample(biquad *b[], jshort);
extern void configure(JNIEnv* env, jobject obj);
extern biquad mEq[62], *peq[62];
extern void setup(biquad *p,
	int type,
	smp_type dbGain,
	smp_type cfreq,
	smp_type srate,
	smp_type bandwidth
);

/**
 * filter types
 */
enum {
	LPF   /* low pass filter */,
	HPF   /* High pass filter */,
	BPF   /* band pass filter */,
	NOTCH /* Notch Filter */,
	PEQ   /* Peaking band EQ filter */,
	LSH   /* Low shelf filter */,
	HSH   /* High shelf filter */,
};
