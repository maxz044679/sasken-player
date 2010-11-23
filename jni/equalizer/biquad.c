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
 *
 *
 */

#include "biquad.h"

/**
 * Computes a BiQuad filter on a sample
*/

double pre;

void *process(void *vp)
{
	jdouble res;
	jint i, off;

	package *p = (package*)vp;
	off = (mode == 1) ? 2 : 4;
	pre = pow(10, (0.24 * preg - 12) / 40);

	/**
	 * Process the buffer
	 */
	for (i = p->offset; i < p->len; i+= off) {
		res = processample(p->ch, *(jshort *)&p->buffer[i]);
		*(jshort *)&p->buffer[i] = (jshort)res;
	}
	return 0;
}

jdouble processample(biquad *b[], jshort sample) {

	jdouble res, s, out;
	int i, max = nPEQ>>1;

	s = sample * pre;

	for (i = 0; i < max; i++) {

		res = b[i]->a0 * s + b[i]->a1 * b[i]->x1 + b[i]->a2 * b[i]->x2 -
			b[i]->a3 * b[i]->y1 - b[i]->a4 * b[i]->y2;

		/**
		 * shift x1 to x2, sample to x1
		 */
		b[i]->x2 = b[i]->x1;
		b[i]->x1 = s;

		/**
		 * shift y1 to y2, result to y1
		 */
		b[i]->y2 = b[i]->y1;
		b[i]->y1 = res;
		s = res;
	}
	return res;
}

/**
 * sets up a BiQuad Filter
 */
void setup(
	biquad *b,
	int type,
	smp_type dbGain,
	smp_type freq,
	smp_type srate,
	smp_type bandwidth)
{
	smp_type A, omega, sn, cs, alpha, beta;
	smp_type a0, a1, a2, b0, b1, b2;

	/**
	 * setup variables
	 */
	A = pow(10, dbGain / 40);
	omega = 2 * M_PI * freq / srate;
	sn = sin(omega);
	cs = cos(omega);
	alpha = sn * sinh(M_LN2 / 2 * bandwidth * omega / sn);
	beta = sqrt(A + A);

	switch (type) {
	case LPF:
		b0 = (1 - cs) /2;
		b1 = 1 - cs;
		b2 = (1 - cs) /2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case HPF:
		b0 = (1 + cs) /2;
		b1 = -(1 + cs);
		b2 = (1 + cs) /2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case BPF:
		b0 = alpha;
		b1 = 0;
		b2 = -alpha;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case NOTCH:
		b0 = 1;
		b1 = -2 * cs;
		b2 = 1;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case PEQ:
		b0 = 1 + alpha * A;
		b1 = -2 * cs;
		b2 = 1 - alpha * A;
		a0 = 1 + alpha / A;
		a1 = -2 * cs;
		a2 = 1 - alpha / A;
		break;
	case LSH:
		b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
		b1 = 2 * A * ((A - 1) - (A + 1) * cs);
		b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
		a0 = (A + 1) + (A - 1) * cs + beta * sn;
		a1 = -2 * ((A - 1) + (A + 1) * cs);
		a2 = (A + 1) + (A - 1) * cs - beta * sn;
		break;
	case HSH:
		b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
		b1 = -2 * A * ((A - 1) + (A + 1) * cs);
		b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
		a0 = (A + 1) - (A - 1) * cs + beta * sn;
		a1 = 2 * ((A - 1) - (A + 1) * cs);
		a2 = (A + 1) - (A - 1) * cs - beta * sn;
	}

	/**
	 * precompute the coefficients
	 */
	b->a0 = b0 / a0;
	b->a1 = b1 / a0;
	b->a2 = b2 / a0;
	b->a3 = a1 / a0;
	b->a4 = a2 / a0;

	/**
	 * zero initial samples
	 */
	b->x1 = b->x2 = 0;
	b->y1 = b->y2 = 0;
}
