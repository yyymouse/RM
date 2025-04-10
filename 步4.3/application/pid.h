/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       pid.c/h
  * @brief      pidʵ�ֺ�����������ʼ����PID���㺯����
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */
#ifndef PID_H
#define PID_H
#include "struct_typedef.h"
enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA
};

typedef struct
{
    uint8_t mode;
    //PID ������
    fp32 Kp;
    fp32 Ki;
    fp32 Kd;

    fp32 max_out;  //������
    fp32 max_iout; //���������

    fp32 set;
    fp32 fdb;

    fp32 out;
    fp32 Pout;
    fp32 Iout;
    fp32 Dout;
    fp32 Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
    fp32 error[3]; //����� 0���� 1��һ�� 2���ϴ�

} pid_type_def;


typedef struct   //PID�����޸�
{
    uint8_t mode;
    //PID ������
    fp32 P;
    fp32 I;
    fp32 D;

    fp32 I_LIMIT;  //������
    fp32 OUT_LIMIT; //���������

    fp32 set;
    fp32 get;

    fp32 OUT;
    fp32 err;
    fp32 err_err;
	  fp32 err_old;
    fp32 P_OUT;
    fp32 I_OUT;  //΢���� 0���� 1��һ�� 2���ϴ�
    fp32 D_OUT; //����� 0���� 1��һ�� 2���ϴ�

} _PID;


/*������PID*/
 
 typedef struct
 {
   float setpoint;               /*�趨ֵ*/
   float kp;                     /*����ϵ��*/
   float ki;                     /*����ϵ��*/
   float kd;                     /*΢��ϵ��*/
   float lasterror;              /*ǰһ��ƫ��*/
   float preerror;               /*ǰ����ƫ��*/
   float deadband;               /*����*/
   float result;                 /*PID������������*/
   float output;                 /*���ֵ0-100%*/
   float maximum;                /*���ֵ����*/
   float minimum;                /*���ֵ����*/
   float errorabsmax;            /*ƫ�����ֵ���ֵ*/
   float errorabsmin;            /*ƫ�����ֵ��Сֵ*/
   float alpha;                  /*����ȫ΢��ϵ��*/
   float derivative;              /*΢����*/
   float integralValue;          /*�����ۼ���*/
 }CLASSICPID;
 
/**
  * @brief          pid struct data init
  * @param[out]     pid: PID struct data point
  * @param[in]      mode: PID_POSITION: normal pid
  *                 PID_DELTA: delta pid
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid max out
  * @param[in]      max_iout: pid max iout
  * @retval         none
  */
/**
  * @brief          pid struct data init
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      mode: PID_POSITION:��ͨPID
  *                 PID_DELTA: ���PID
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid������
  * @param[in]      max_iout: pid���������
  * @retval         none
  */
extern void PID_init(pid_type_def *pid, uint8_t mode, const fp32 PID[3], fp32 max_out, fp32 max_iout);

/**
  * @brief          pid calculate 
  * @param[out]     pid: PID struct data point
  * @param[in]      ref: feedback data 
  * @param[in]      set: set point
  * @retval         pid out
  */
/**
  * @brief          pid����
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      ref: ��������
  * @param[in]      set: �趨ֵ
  * @retval         pid���
  */
extern fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set);

/**
  * @brief          pid out clear
  * @param[out]     pid: PID struct data point
  * @retval         none
  */
/**
  * @brief          pid ������
  * @param[out]     pid: PID�ṹ����ָ��
  * @retval         none
  */
extern void PID_clear(pid_type_def *pid);

extern float PID_Cal_Limt(_PID *PID, float limit, float get, float set);//PID�����޸�
extern void PID_Cal_Limt_Init(_PID *PID, const fp32 PID_l[3], fp32 max_out, fp32 max_iout);
//�������������ֱ��͡����λ��֡�������㷨�Լ�����ȫ΢���㷨��������PID������
extern void PIDRegulator( CLASSICPID *vPID , float P , float I , float D , float set , float pv , float alp , float max_out , float min_out);

#endif




