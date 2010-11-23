/*
 * File:   VUVisualizer.c
 * Author: Antonio Brenlla
 *
 * Created on Oct 14, 2010, 5:05 PM
 *
 */

#include <jni.h>
#include <math.h>
#include <android/log.h>
#include <string.h>
#include "VUVisualizer.h"


#define DEBUG_TAG "JNI"



/*typedef struct {
	int leftPower;
	int rightPower;
} vu_data;*/

int debug = 0;
int barHeight = 0;
int channels = 2;
//vu_data *data = NULL;
volatile double temp1[] = {-50.0, -50.0};

static jclass classRef;
static jobject objectRef;

JNIEXPORT jlong JNICALL
	Java_com_sasken_player_visualizations_VU_Init
	(JNIEnv* env, jobject thiz)
{
	vu_data *data = (vu_data *)malloc(sizeof(vu_data));
	data->leftPower = -50;
	data->rightPower = -50;

	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Init to left = %d - right = %d", data->leftPower, data->rightPower);

	return (jlong)data;

}

/*vu_data *init()
{
	data = (vu_data *)malloc(sizeof(vu_data));
	data->leftPower = -15.0;
	data->rightPower = -15.0;

	//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Init to left = %.2f - right = %.2f", data->leftPower, data->rightPower);

	return data;
}*/

double *getValues(double *input){
	input = temp1;
	return input;
}

/*void release(vu_data *handle)
{
	free(handle);
}*/

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_setNumOfChannels
		(JNIEnv* env, jobject thiz, jint numOfChannels)
{

	channels = numOfChannels;
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Number of Channels set = %d", channels);
}

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_Release
	(JNIEnv* env, jobject thiz, jlong handle)
{
	vu_data *data = (vu_data *)handle;

	free (data);

}

JNIEXPORT jintArray JNICALL
	Java_com_sasken_player_visualizations_VU_getPowVolumeData
	(JNIEnv* env, jobject thiz, jlong handle)
{
	vu_data *data = (vu_data *)handle;
	int powAvgVolume[2];

	jintArray powToReturn = (*env)->NewIntArray(env,2);
	if (powToReturn == NULL){
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return 0;
	}

	if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Calculating left = %d - right = %d", data->leftPower, data->rightPower);

	data->leftPower += 500;
	if (data->leftPower > 6000){
		data->leftPower = 0;
	}

	data->rightPower -= 500;
	if (data->rightPower < 0){
		data->rightPower = 6000;
	}

	powAvgVolume[0] = data->leftPower;
	powAvgVolume[1] = data->rightPower;

	if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Calculating Done left = %d - right = %d", data->leftPower, data->rightPower);


	if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Returning new power");
	(*env)->SetIntArrayRegion(env, powToReturn, 0, 2, powAvgVolume);
	if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Returning Done");
	return powToReturn;

}

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_getPowVolume
	(JNIEnv* env, jobject thiz, jbyteArray input, jdoubleArray output)
{
	double normalizeFactor = 1.0/32768.0;
	int i = 0;
	short *buffer16 = NULL;
	double powAvgVolume[2];

	if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Normalized factor = %.15f", normalizeFactor);

	//Get buffer length
	jsize inputLength = (*env)->GetArrayLength(env, input);

	//Get buffer
	jbyte* inputData = (*env)->GetPrimitiveArrayCritical(env, input, 0);
	if (inputData == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Cannot access the PrimitiveArray");
		return;
	}

	//Creates buffer char to hold input
	char* bufferChar = (char*) malloc(sizeof(char)*inputLength);
	if (bufferChar == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return;
	}
	for (i = 0; i < inputLength; i++){
		bufferChar[i] = inputData[i];
	}

	//Release input buffer
	(*env)->ReleasePrimitiveArrayCritical(env, input, inputData, 0);

	//Pass char buffer to short buffer
	buffer16 = bufferChar;

	//free char buffer, no longer needed
	free(bufferChar);

	//Prints content of buffer16
	if(debug){
		for (i = 0; i < inputLength/2; i++){
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "buffer16 %d = %d", i, buffer16[i]);
		}
	}

	//Normalize buffer
	float* bufferNormalized = (float*) malloc (sizeof(float)*inputLength/2);
	if (bufferNormalized == NULL) {
		if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return;
	}
	for (i = 0; i < inputLength/2; i++){
		bufferNormalized[i] = normalizeFactor*buffer16[i];
		if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "bufferNormalized %d = %.15f", i, bufferNormalized[i]);
	}

	//buffer16 no longer needed
	buffer16 = NULL;
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Number of Channels = %d", channels);
	if (channels == 1){
	//*********************
	//|  For mono files:  |
	//*********************
		powAvgVolume[0] = calculatePowerDb(bufferNormalized, inputLength/4, 0);
	}
	else if (channels ==2 ){
	//*********************
	//| For stereo files: *
	//*********************
		powAvgVolume[0] = calculatePowerDb(bufferNormalized, inputLength/4, 0);
		powAvgVolume[1] = calculatePowerDb(bufferNormalized, inputLength/4, 1);
	}
	//free Normalized buffer
	free(bufferNormalized);

	//Get output array
	jdouble* powToReturn = (*env)->GetPrimitiveArrayCritical(env, output, 0);
	if (powToReturn == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Cannot access the PrimitiveArray");
		return;
	}

	for (i = 0; i < 2; i++) {
		powToReturn[i] = powAvgVolume[i];
	}

	temp1[0] = powAvgVolume[0];
	temp1[1] = powAvgVolume[1];



	//Release output buffer
	(*env)->ReleasePrimitiveArrayCritical(env, output, powToReturn, 0);

	//====================  Callback Test  =======================

	jclass vuClass = (*env)->GetObjectClass(env, thiz);
	jmethodID mid = (*env)->GetMethodID(env, vuClass, "callback", "()V");
	if(mid == NULL){
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Method not found");
		return;
	}
	(*env)->CallVoidMethod(env, thiz, mid);
	//============================================================

}

double calculatePowerDb (float *buffer, jsize samples, int offset){
	double sum = 0;
	double sqsum = 0;
	double power = 0;
	int i = 0;
	int bal = 0;

	//if offset is 0, then is channel left, 1 is channel right
	for (i = 0; i < samples; i++){
		sum += buffer[i*2+offset];
		if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "sum %d = %.15f", i, sum);
		sqsum += buffer[i*2+offset]*buffer[i*2+offset];
		if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "sqsum %d = %.15f", i, sqsum);

	}
	bal = samples;
	power = (sqsum-sum*sum/bal)/bal;
	power = log10(power)*10.0;
	if(debug)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "power = %.15f", power);

	return power;
}

void VU_process_Buffer(JNIEnv* env, jobject thiz, jbyte* buffer, int len){
	double normalizeFactor = 1.0/32768.0;
	short *buffer16 = NULL;
	double powAvgVolume[2];
	int i = 0;

	buffer16 = buffer;
	//Normalize buffer
	float* bufferNormalized = (float*) malloc (sizeof(float)*len/2);
	if (bufferNormalized == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return;
	}
	for (i = 0; i < len/2; i++){
		bufferNormalized[i] = normalizeFactor*buffer16[i];
	}
	//buffer16 no longer needed
	buffer16 = NULL;
	if (channels == 1){
	//*********************
	//|  For mono files:  |
	//*********************
		powAvgVolume[0] = calculatePowerDb(bufferNormalized, len/4, 0);
	}
	else if (channels ==2 ){
	//*********************
	//| For stereo files: *
	//*********************
		powAvgVolume[0] = calculatePowerDb(bufferNormalized, len/4, 0);
		powAvgVolume[1] = calculatePowerDb(bufferNormalized, len/4, 1);
	}
	//free Normalized buffer
	free(bufferNormalized);
	temp1[0] = powAvgVolume[0];
	temp1[1] = powAvgVolume[1];

	//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "powAvgVolume[0] = %.15f", powAvgVolume[0]);
	//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "powAvgVolume[1] = %.15f", powAvgVolume[1]);
	//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "I want to process the buffer!!");
}

JNIEXPORT jdoubleArray JNICALL
	Java_com_sasken_player_visualizations_VU_readPowValues
	(JNIEnv* env, jobject thiz)
{
	jdoubleArray powToReturn = (*env)->NewDoubleArray(env,2);;
	(*env)->SetDoubleArrayRegion(env, powToReturn, 0, 2, temp1);
	return powToReturn;
}

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_Start
	(JNIEnv* env, jobject thiz)
{
	jclass ref = 0;

	ref = (*env)->FindClass(env, "com/sasken/player/visualizations/VU");

	classRef = (*env)->NewGlobalRef(env, ref);
	objectRef = (*env)->NewGlobalRef(env, thiz);

	jmethodID mid = (*env)->GetMethodID(env, classRef, "callbackStart", "()V");

	(*env)->CallVoidMethod(env, objectRef, mid);

	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Global References created");
}

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_Stop
	(JNIEnv* env, jobject thiz)
{
	(*env)->DeleteGlobalRef(env, classRef);
	(*env)->DeleteGlobalRef(env, objectRef);
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Global References deleted");
}

