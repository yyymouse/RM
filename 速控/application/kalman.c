/** *****************************************************************************************
  * @file    kalman.c                                                                                                                                      *
  * @author  Liu heng                                                                                                                                    *
  * @version V1.0.0                                                                                                                                      *
  * @date    27-August-2013                                                                                                                         *
  * @brief   一维卡尔曼滤波器的具体实现。实现过程完全与硬件无关，   *
  *   可直接调用，任意移植。                                                                                               *
  *   使用时先定义一个kalman指针，然后调用kalmanCreate()创建一个滤波器?*
  *   每次读取到传感器数据后即可调用KalmanFilter()来对数据进行滤波。               *
  *****************************************************************************************
  *                          使用示例                                                     *
  *          kalman p;                                                                   *
  *          float SersorData;                                                            *
  *          kalmanCreate(&p,20,200);                                                  *
  *          while(1)                                                                     *
  *          {                                                                            *
  *             SersorData = sersor();                                                    *
  *             SersorData = KalmanFilter(&p,SersorData);                                  *
  *             printf("%2.2f",SersorData);                                               *
  *          }                                                                            *
  *****************************************************************************************
  *          MPU6050的卡尔曼滤波器参考参数 Q：10 R：400                                   *
  *****************************************************************************************/

#include "kalman.h"

kalman mouse_x_fillter;
kalman mouse_y_fillter;
kalman mouse_z_fillter;

kalman visual_x_fillter;
kalman visual_y_fillter;

/**
  * @name   kalmanCreate
  * @brief  创建一个卡尔曼滤波器
  * @param  p:  滤波器
  *         T_Q:系统噪声协方差
  *         T_R:测量噪声协方差
  *         
  * @retval none
  */
void kalmanCreate(kalman *p,float T_Q,float T_R)
{
    //kalman* p = ( kalman*)malloc(sizeof( kalman));
    p->X_last = (float)0;
    p->P_last = 0;
    p->Q = T_Q;
    p->R = T_R;
    p->A = 1;
    p->H = 1;
    p->X_mid = p->X_last;
    //return p;
}

/**
  * @name   KalmanFilter
  * @brief  卡尔曼滤波器
  * @param  p:  滤波器
  *         dat:待滤波数据
  * @retval 滤波后的数据
  */

float KalmanFilter(kalman* p,float dat)
{
    p->X_mid =p->A*p->X_last;                     //x(k|k-1) = AX(k-1|k-1)+BU(k)
    p->P_mid = p->A*p->P_last+p->Q;               //p(k|k-1) = Ap(k-1|k-1)A'+Q
    p->kg = p->P_mid/(p->P_mid+p->R);             //kg(k) = p(k|k-1)H'/(Hp(k|k-1)'+R)
    p->X_now = p->X_mid+p->kg*(dat-p->X_mid);     //x(k|k) = X(k|k-1)+kg(k)(Z(k)-HX(k|k-1))
    p->P_now = (1-p->kg)*p->P_mid;                //p(k|k) = (I-kg(k)H)P(k|k-1)
    p->P_last = p->P_now;                         //状态更新
    p->X_last = p->X_now;
    return p->X_now;
}


//int main() {

//    kalman p;                                                                   
//        float SersorData;      

//        kalmanCreate(&p, 20, 200);                                                  
//        while (1)                                                                     
//        {
//        
//            SersorData = sersor();                                                    
//            SersorData = KalmanFilter(&p, SersorData);                                 
//            printf("%2.2f", SersorData);                                               
//            
//          }                                                                            
//    return 0;
//}

