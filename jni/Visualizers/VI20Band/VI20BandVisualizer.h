/*
 * File:   VI20BandVisualizer.H
 * Author: Antonio Brenlla
 *
 * Created on Nov 18, 2010, 10:20 AM
 *
 */

#ifndef VI20BANDVISUALIZER_H
#define VI20BANDVISUALIZER_H

#ifdef __cplusplus
extern "C" {
#endif
void Vis_process_Buffer();
#ifdef __cplusplus
}
#endif

JNIEXPORT jdoubleArray JNICALL Java_com_sasken_player_visualizations_VI10BandLeds_getPowValues
	(JNIEnv* env, jobject thiz);

JNIEXPORT jdoubleArray JNICALL Java_com_sasken_player_visualizations_VI20Band_readPowValues
	(JNIEnv* env, jobject thiz);

#endif /* VI20BANDVISUALIZER_H */
