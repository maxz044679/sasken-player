/*
 * File:   VI_20BandVisualizer.c
 * Author: Antonio Brenlla
 *
 * Created on Nov 11, 2010, 9:51 AM
 *
 */

#include <jni.h>
#include <math.h>
#include <android/log.h>
#include <string.h>
#include "../VU/VUVisualizer.h"
//#include "/home/x0083674/workspace/equalizer/jni/Visualizers/VU/VUVisualizer.h"


#define DEBUG_TAG "JNI_10BandLeds"

JNIEXPORT jdoubleArray JNICALL
	Java_com_sasken_player_visualizations_VI10BandLeds_getPowValues
	(JNIEnv* env, jobject thiz)
{
	int i = 0;
	double *values = NULL;

	values = getValues(values);

	jdoubleArray powToReturn = (*env)->NewDoubleArray(env,2);;
	(*env)->SetDoubleArrayRegion(env, powToReturn, 0, 2, values);
	return powToReturn;
}
