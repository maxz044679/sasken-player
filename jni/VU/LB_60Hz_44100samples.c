/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 10 -a 1.3605442177e-03 0.0000000000e+00 -l */

#include <jni.h>
#include <math.h>

int NZEROS = 10;
int NPOLES = 10;
float GAIN = 2.673441015e+16;
float xv[NZEROS+1], yv[NPOLES+1];

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_filterloop(JNIEnv* env, jobject thiz, jshortArray inputBuffer)
	{

	jsize inputLength = (*env)->GetArrayLength(env, inputBuffer);
	jshort *buffer = (*env)->GetShortArrayElements(env, inputBuffer, 0);

	short outputBuffer[inputLength];

	for (;;){
		xv[0] = xv[1];
		xv[1] = xv[2];
		xv[2] = xv[3];
		xv[3] = xv[4];
		xv[4] = xv[5];
		xv[5] = xv[6];
		xv[6] = xv[7];
		xv[7] = xv[8];
		xv[8] = xv[9];
		xv[9] = xv[10];
		xv[10] = 'next input value' / GAIN;

		yv[0] = yv[1];
		yv[1] = yv[2];
		yv[2] = yv[3];
		yv[3] = yv[4];
		yv[4] = yv[5];
		yv[5] = yv[6];
		yv[6] = yv[7];
		yv[7] = yv[8];
		yv[8] = yv[9];
		yv[9] = yv[10];

		yv[10] = (xv[0] + xv[10]) + 10 * (xv[1] + xv[9]) + 45 * (xv[2] + xv[8]) + 120 * (xv[3] + xv[7]) + 210 * (xv[4] + xv[6]) + 252 * xv[5]
                     + ( -0.9468198528 * yv[0]) + (  9.5199386383 * yv[1])
                     + (-43.0739687230 * yv[2]) + (115.4923664800 * yv[3])
                     + (-203.2181180300 * yv[4]) + (245.1975949300 * yv[5])
                     + (-205.4513199500 * yv[6]) + (118.0446492800 * yv[7])
                     + (-44.5096765800 * yv[8]) + (  9.9453537960 * yv[9]);
        'next output value' = yv[10];
      }
  }
