/*
 * File:   VI20BandVisualizer.H
 * Author: Antonio Brenlla
 *
 * Created on Nov 18, 2010, 10:20 AM
 *
 */

#ifndef VI10BANDLEDS_H
#define VI10BANDLEDS_H

/*#ifdef __cplusplus
extern "C" {
#endif
void Vis_process_Buffer();
#ifdef __cplusplus
}
#endif*/

JNIEXPORT jdoubleArray JNICALL Java_com_sasken_player_visualizations_VI10BandLeds_getPowValues
	(JNIEnv* env, jobject thiz);

#endif /* VI10BANDLEDS_H */
