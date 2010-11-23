/*
 * File:   VVisualizer.H
 * Author: Antonio Brenlla
 *
 * Created on Nov 11, 2010, 12:13 PM
 *
 */

#ifndef VUVISUALIZER_H
#define VUVISUALIZER_H

#ifdef __cplusplus
extern "C" {
#endif
void VU_process_Buffer(JNIEnv* env, jobject thiz, jbyte* buffer, int len);
#ifdef __cplusplus
}
#endif

typedef struct {
	double leftPower;
	double rightPower;
} vu_data;

JNIEXPORT jlong JNICALL Java_com_sasken_player_visualizations_VU_Init
	(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL Java_com_sasken_player_visualizations_VU_setNumOfChannels
		(JNIEnv* env, jobject thiz, jint numOfChannels);

JNIEXPORT void JNICALL Java_com_sasken_player_visualizations_VU_Release
	(JNIEnv* env, jobject thiz, jlong handle);

JNIEXPORT jintArray JNICALL Java_com_sasken_player_visualizations_VU_getPowVolumeData
	(JNIEnv* env, jobject thiz, jlong handle);

JNIEXPORT void JNICALL Java_com_sasken_player_visualizations_VU_getPowVolume
	(JNIEnv* env, jobject thiz, jbyteArray input, jdoubleArray output);

JNIEXPORT jdoubleArray JNICALL Java_com_sasken_player_visualizations_VU_readPowValues
	(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL Java_com_sasken_player_visualizations_VU_Start
	(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL Java_com_sasken_player_visualizations_VU_Stop
	(JNIEnv* env, jobject thiz);

double calculatePowerDb(float *, jsize, int);
//extern vu_data *init();
extern double *getValues(double *input);
//extern void release(vu_data*);

extern int debug;
extern volatile double temp1[];

#endif  /* VUVISUALIZER_H */
