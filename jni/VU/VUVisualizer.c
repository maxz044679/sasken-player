/*
 * File:   values.cpp
 * Author: Antonio Brenlla
 *
 * Created on Oct 14, 2010, 5:05 PM
 *
 */

#include <jni.h>
#include <math.h>
#include <android/log.h>

#define DEBUG_TAG "JNI"




int barHeight = 0;
int channels = 0;

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_setBuffer
		(JNIEnv* env, jobject thiz, jint inputBuffer)
{
	barHeight = inputBuffer;

}

jint Java_com_sasken_player_visualizations_VU_getPowerOld
	(JNIEnv* env, jobject thiz)
{

	barHeight ++;

	if (barHeight >= 110){
		barHeight = 0;
	}

	return barHeight;
}

jint Java_com_sasken_player_visualizations_VU_getPower
	(JNIEnv* env, jobject thiz)
{

	barHeight ++;

	if (barHeight >= 110){
		barHeight = 0;
	}

	return barHeight;
}

JNIEXPORT jint JNICALL
	Java_com_sasken_player_visualizations_VU_getAvgVolume
	(JNIEnv* env, jobject thiz, jshortArray input)
{
	int avgVolume = 0;
	int i = 0;
	jsize inputLength = (*env)->GetArrayLength(env, input);
	jshort *buffer = (*env)->GetShortArrayElements(env, input, 0);
	for (i = 0; i < inputLength; i++){
		if(buffer[i]<0){
			buffer[i] *= -1;
		}
		avgVolume += buffer[i];
	}
	avgVolume = avgVolume/inputLength;
	//avgVolume = buffer[0];
	return avgVolume;
}

JNIEXPORT void JNICALL
	Java_com_sasken_player_visualizations_VU_setNumOfChannels
		(JNIEnv* env, jobject thiz, jint numOfChannels)
{
	channels = numOfChannels;

}

//JNIEXPORT jint JNICALL
JNIEXPORT jlong JNICALL
	Java_com_sasken_player_visualizations_VU_getPowVolumeOld
	(JNIEnv* env, jobject thiz, jshortArray input)
{
	int powAvgVolume = 0;
	long long temp = 0;
	int i = 0;
	jsize inputLength = (*env)->GetArrayLength(env, input);
	jshort *buffer = (*env)->GetShortArrayElements(env, input, 0);
	int powVolume[inputLength];
	//Obtain the pow of 2 for every element in the buffer
	for (i = 0; i < inputLength; i++){
		powVolume[i] = pow(buffer[i],2);
	}
	//Sum all the elements in the buffer
	for (i = 0; i < inputLength; i++){
		temp += powVolume[i];
	}
	//Obtain the avarage of the buffer
	powAvgVolume = temp/inputLength;
	return powAvgVolume;
}

JNIEXPORT jlong JNICALL
	Java_com_sasken_player_visualizations_VU_getPowVolume
	(JNIEnv* env, jobject thiz, jbyteArray input)
{
	long long powAvgVolume = 0;
	long long powLeftAvgVolume = 0;
	long long powRightAvgVolume = 0;
	long long temp = 0;
	int i = 0;
	char *data;
	long long *powVolume;
	long long *powLeftVolume;
	long long *powRightVolume;
	short *leftChannelSamples;
	short *rightChannelSamples;
	jbyte *buffer;
	short *buffer16;
	short *buffer16Left;
	short *buffer16Right;

	jsize inputLength = (*env)->GetArrayLength(env, input);

	data = (char*) malloc (sizeof(char)*inputLength);
	if (data == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return 0;
	}

	buffer16 = (short*) malloc (sizeof(short)*inputLength/2);
	if (buffer16 == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
		return 0;
	}

	buffer = (*env)->GetPrimitiveArrayCritical(env, input, 0);
	if (buffer == NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Cannot access the PrimitiveArray");
		free(data);
		return 0;
	}

	//Puts byte array into a char array
	for(i = 0; i < inputLength; i++){
		data[i] = buffer[i];
	}

	//Release input buffer
	(*env)->ReleasePrimitiveArrayCritical(env, input, buffer, 0);

	//Puts the char array into a short array
	buffer16 = data;
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "buffer16 filled");

	if (channels == 1){
		//*********************
		//|  For mono files:  |
		//*********************

		powVolume = (long long*) malloc (sizeof(long long)*inputLength/2);
		if (powVolume == NULL) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
			return 0;
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "powVolume created");

		//Obtain the pow of 2 for every element in the buffer
		for (i = 0; i < inputLength/2; i++){
			powVolume[i] = pow(buffer16[i],2);
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "pow done");

		//Sum all the elements in the buffer
		for (i = 0; i < inputLength/2; i++){
			temp += powVolume[i];
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "sum done");

		//Obtain the avarage of the buffer
		powAvgVolume = temp/inputLength;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "average done");

		free(powVolume);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "free-ed powVolume");
	}
	else if (channels ==2 ){
		//*********************
		//| For stereo files: *
		//*********************

		powLeftVolume = (long long*) malloc (sizeof(long long)*inputLength/2);
		if (powLeftVolume == NULL) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
			return 0;
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "powLeftVolume created");

		powRightVolume = (long long*) malloc (sizeof(long long)*inputLength/2);
		if (powRightVolume == NULL) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
			return 0;
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "powRightVolume created");

		buffer16Left = (short*) malloc (sizeof(short)*inputLength/2);
		if (buffer16Left == NULL) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
			return 0;
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "buffer16Left created");

		buffer16Right = (short*) malloc (sizeof(short)*inputLength/2);
		if (buffer16Right == NULL) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Out of memory");
			return 0;
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "buffer16Right created");

		//Separate buffers
		for (i = 0; i < inputLength/2; i++){
			buffer16Left[i] = buffer16[i*2];
			buffer16Right[i] = buffer16[i*2+1];
		}

		//Obtain the pow of 2 for every element in the left channel
		for (i = 0; i < inputLength/2; i++){
			powLeftVolume[i] = pow(buffer16Left[i],2);
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "pow left done");

		//Obtain the pow of 2 for every element in the right channel
		for (i = 0; i < inputLength/2; i++){
			powRightVolume[i] = pow(buffer16Right[i],2);
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "pow right done");

		//Sum all the elements in the left buffer
		for (i = 0; i < inputLength/2; i++){
			temp += powLeftVolume[i];
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "sum left done");

		//Obtain the avarage of the left buffer
		powLeftAvgVolume = temp/(inputLength/2);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "average left done");

		//Sum all the elements in the right buffer
		temp = 0;
		for (i = 0; i < inputLength/2; i++){
			temp += powRightVolume[i];
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "sum right done");

		//Obtain the avarage of the left buffer
		powRightAvgVolume = temp/(inputLength/2);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "average right done");

		free(powLeftVolume);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "free-ed powLeftVolume");
		free(powRightVolume);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "free-ed powRightVolume");
		free(buffer16Left);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "free-ed buffer16Left");
		free(buffer16Right);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "free-ed buffer16Right");
		powAvgVolume = powRightAvgVolume;
	}

	free(data);
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "free-ed data");
	//free(buffer16);
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "free-ed buffer16");
	return powAvgVolume;
}

/*void copyByteArrayToShortArrayLE(jbyte *buffer8, short* buffer16, jsize inputLength) {
	int i = 0;
	for(i = 0; i < 35280; i += 2){
		buffer16[i] = (buffer8[i] | (buffer8[i + 1] << 8));
	}
}

void divideSamples( short samples[], short rightChannel[], short leftChannel[], int inputLength){
	int i = 0;
	for (i = 0; i < inputLength; i++){
		rightChannel[i] = samples[i*2];
		leftChannel[i] = samples[i*2+1];
	}
}*/
