/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       pid.c/h
  * @brief      pid???????????????????PID????????
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ???
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include "pid.h"
#include "main.h"
#include "math.h"
#include "stdlib.h"
#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }
		
/* �����ϵ����������ʵ��һ�����0��1֮��ķֶ����Ժ���            */
/* ��ƫ��ľ���ֵС����Сֵʱ�����Ϊ1����ƫ��ľ���ֵ�������ֵʱ�����Ϊ0   */
/* ��ƫ��ľ���ֵ�������ֵ����Сֵ֮��ʱ�������0��1֮�����б仯    */
/* float error����ǰ�����ƫ��ֵ                                         */
/* float absmax��ƫ�����ֵ�����ֵ                                      */
/* float absmin��ƫ�����ֵ����Сֵ                                      */
static float VariableIntegralCoefficient(float error,float absmax,float absmin)
{

  float factor=0.0;

  if(abs(error)<=absmin)
  {

    factor=1.0;

  }
  else if(abs(error)>absmax)
  {

    factor=0.0;

  }
  else

  {

    factor=(absmax-abs(error))/(absmax-absmin);

  }

  return factor;

}

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
  * @param[out]     pid: PID?????????
  * @param[in]      mode: PID_POSITION:???PID
  *                 PID_DELTA: ???PID
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid??????
  * @param[in]      max_iout: pid?????????
  * @retval         none
  */
void PID_init(pid_type_def *pid, uint8_t mode, const fp32 PID[3], fp32 max_out, fp32 max_iout)
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
    pid->mode = mode;
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}

/**
  * @brief          pid calculate 
  * @param[out]     pid: PID struct data point
  * @param[in]      ref: feedback data 
  * @param[in]      set: set point
  * @retval         pid out
  */
/**
  * @brief          pid????
  * @param[out]     pid: PID?????????
  * @param[in]      ref: ????????
  * @param[in]      set: ???
  * @retval         pid???
  */
fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set)
{
    if (pid == NULL)
    {
        return 0.0f;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = ref;
    pid->error[0] = set - ref;
    if (pid->mode == PID_POSITION)
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    else if (pid->mode == PID_DELTA)
    {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}

/**
  * @brief          pid out clear
  * @param[out]     pid: PID struct data point
  * @retval         none
  */
/**
  * @brief          pid ??????
  * @param[out]     pid: PID?????????
  * @retval         none
  */
void PID_clear(pid_type_def *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}

//��������λ��ʽ��ʼ��
void PID_Cal_Limt_Init(_PID *PID, const fp32 PID_l[3], fp32 max_out, fp32 max_iout)
{
	
	PID->P=PID_l[0];
	PID->I=PID_l[1];
	PID->D=PID_l[2];
	PID->OUT_LIMIT = max_out ;
	PID->I_LIMIT = max_iout;

}

//��������λ��ʽ
float PID_Cal_Limt(_PID *PID, float limit, float get, float set )//PID�����޸�
{
	PID->set = set ;
	PID->get = get ;
	PID->err = PID->set - PID->get ;
	PID->err_err = PID->err - PID->err_old ;
	
	
	
	PID->P_OUT  = PID->P * PID->err;
	PID->I_OUT += PID->I * PID->err;
	PID->D_OUT  = PID->D * PID->err_err;
	
	PID->I_OUT = (PID->I_OUT > PID->I_LIMIT)?(PID->I_LIMIT):((PID->I_OUT < -PID->I_LIMIT)?(-PID->I_LIMIT):(PID->I_OUT));
	
	PID->OUT = PID->P_OUT + PID->I_OUT + PID->D_OUT;
	
	PID->OUT = (PID->OUT > PID->OUT_LIMIT)?(PID->OUT_LIMIT):((PID->OUT < -PID->OUT_LIMIT)?(-PID->OUT_LIMIT):(PID->OUT));
	
	if(abs(PID->err) <= abs(limit))
	{
	  PID->I_OUT=0;
	  PID->OUT=0;
	}
	
	PID->err_old = PID->err;
	
	return PID->OUT;
}


//�������������ֱ��͡����λ��֡�������㷨�Լ�����ȫ΢���㷨��������PID������

void PIDRegulator( CLASSICPID *vPID , float P , float I , float D , float set , float pv , float alp , float max_out , float min_out)
 {
   float thisError;
   float result;
   float factor;
	 
	 
	 vPID->maximum = max_out;
	 vPID->minimum = min_out;
   vPID->alpha = alp;
	 vPID->setpoint = set;
	 vPID->kp = P;
	 vPID->ki = I;
	 vPID->kd = D;
	 
   thisError=vPID->setpoint-pv; //�õ�ƫ��ֵ
   result=vPID->result;
 
   if (fabs(thisError)>vPID->deadband)
   {
     vPID-> integralValue= vPID-> integralValue+ thisError;
 
     //�����ϵ����ȡ
     factor=VariableIntegralCoefficient(thisError,vPID->errorabsmax,vPID->errorabsmin);
    
     //����΢��������������ȫ΢��
     vPID-> derivative =vPID->kd*(1-vPID->alpha)* (thisError-vPID->lasterror +vPID->alpha*vPID-> derivative) ;
        
     result=vPID->kp*thisError+vPID->ki*vPID-> integralValue +vPID-> derivative;
   }
   else
   {
     if((abs(vPID->setpoint-vPID->minimum)<vPID->deadband)&&(abs(pv-vPID->minimum)<vPID->deadband))
     {
       result=vPID->minimum;
     }
   }
  
   /*�������ֵ�����ⳬ���ͻ��ֱ�������*/
   if(result>=vPID->maximum)
   {
     result=vPID->maximum;
   }
 
   if(result<=vPID->minimum)
   {
     result=vPID->minimum;
   } 
  
   vPID->preerror=vPID->lasterror;  //���ƫ�������´�����
   vPID->lasterror=thisError;
   vPID->result=result;
 
   vPID->output=((result-vPID->minimum)/(vPID->maximum-vPID->minimum))*100.0;
 }

