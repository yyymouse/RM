/** ******************************************************************************
  * @file    kalman.h                                                            *
  * @author  Liu heng                                                            *
  * @version V1.0.0                                                              *
  * @date    27-August-2013                                                      *
  * @brief   Hearder file for kalman filter                                      *
  *                                                                              *
  ********************************************************************************
  *          �˴�������⴫����ʹ�ã�����ע��������                              *
  ********************************************************************************/
#ifndef _KALMAN_H
#define _KALMAN_H

#include "stdlib.h"

typedef struct {
    float X_last; //��һʱ�̵����Ž��
    float X_mid;  //��ǰʱ�̵�Ԥ����
    float X_now;  //��ǰʱ�̵����Ž��
    float P_mid;  //��ǰʱ��Ԥ������Э����
    float P_now;  //��ǰʱ�����Ž����Э����
    float P_last; //��һʱ�����Ž����Э����
    float kg;     //kalman����
    float A;      //ϵͳ����
    float Q;
    float R;
    float H;
}kalman;

extern kalman mouse_x_fillter;
extern kalman mouse_y_fillter; 
extern kalman mouse_z_fillter;

extern kalman visual_x_fillter;
extern kalman visual_y_fillter;

void kalmanCreate(kalman* p, float T_Q, float T_R);
float KalmanFilter(kalman* p, float dat);

#endif
