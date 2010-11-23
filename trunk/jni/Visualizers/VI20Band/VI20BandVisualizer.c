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


#define DEBUG_TAG "JNI_20Band"

volatile double temp[] = {-50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0, -50.0};

JNIEXPORT jdoubleArray JNICALL
	Java_com_sasken_player_visualizations_VI10BandLeds_getPowValues
	(JNIEnv* env, jobject thiz)
{
	/**=========================================================================================+
	 *  																						|
	 * This Code was used when "process buffer" was called from the AudioPlayer in Java.		|
	 * Since the change from Java to JNI, this code was moved to the method "Vis_process_Buffer"|
	 * below and now the name of this method "getValues" is reuse.								|
	 *  																						|
	 * Old parameters -> (JNIEnv* env, jobject thiz, jdoubleArray values)						|
	 *  																						|
	 *==========================================================================================+

	int i = 0;
	int j = 1;
	double *values2 = NULL;

	jsize inputLength = (*env)->GetArrayLength(env, values);

	jdouble* valuesToReturn = (*env)->GetPrimitiveArrayCritical(env, values, 0);
	if (valuesToReturn == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Cannot access the PrimitiveArray");
		return;
	}

	values2 = getValues(values2);

	for (i = 0; i < inputLength; i += 2){
		//valuesToReturn[i] = -20;
		//valuesToReturn[i+1] = -10;
		//valuesToReturn[i] = temp1[0];
		//valuesToReturn[i+1] = temp1[1];
		valuesToReturn[i] = values2[0];
		valuesToReturn[i+1] = values2[1];
	}

	for (i = 10; i >= 0; i--){
		valuesToReturn[i] += -1*j;
		j++;
	}
	j = 1;
	for (i = 10; i <= 19; i++){
		valuesToReturn[i] += -1*j;
		j++;
	}

	for (i = 0; i <= 19; i++){
		valuesToReturn[i] += 1;
	}


	(*env)->ReleasePrimitiveArrayCritical(env, values, valuesToReturn, 0);

	jclass vuClass = (*env)->GetObjectClass(env, thiz);
	jmethodID mid = (*env)->GetMethodID(env, vuClass, "callback", "()V");
	if(mid == NULL){
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Method not found");
		return;
	}
	(*env)->CallVoidMethod(env, thiz, mid);
	*/
	int i = 0;
	double *values = NULL;

	values = getValues(values);

	jdoubleArray powToReturn = (*env)->NewDoubleArray(env,2);;
	(*env)->SetDoubleArrayRegion(env, powToReturn, 0, 2, values);
	return powToReturn;
}

void Vis_process_Buffer(){
	int i = 0;
	int j = 1;
	double *values2 = NULL;

	values2 = getValues(values2);

	for (i = 0; i < 10; i ++){
		temp[i] = values2[0];
	}
	for (i = 10; i < 20; i ++){
		temp[i] = values2[1];
	}

	for (i = 8; i >= 0; i--){
		temp[i] += -1*j;
		j++;
	}
	j = 1;
	for (i = 11; i <= 19; i++){
		temp[i] += -1*j;
		j++;
	}

	for (i = 0; i <= 19; i++){
		temp[i] += 1;
	}
	//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "temp[0] = %.15f", temp[0]);
	//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "temp[1] = %.15f", temp[1]);
}

JNIEXPORT jdoubleArray JNICALL
	Java_com_sasken_player_visualizations_VI20Band_readPowValues
	(JNIEnv* env, jobject thiz)
{
	jdoubleArray powToReturn = (*env)->NewDoubleArray(env,20);;
	(*env)->SetDoubleArrayRegion(env, powToReturn, 0, 20, temp);
	return powToReturn;
}
