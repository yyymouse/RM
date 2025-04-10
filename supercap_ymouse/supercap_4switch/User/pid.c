#include "pid.h"

void pid_calculate(PID *pid, float fdb)
{
	pid->fdb = fdb;
	pid->error[2] = pid->error[1];       //上上次误差
  pid->error[1] = pid->error[0];       //上次误差
  pid->error[0] = pid->ref - pid->fdb; //本次误差
	if(pid->PID_Mode == PID_POSITION)
	{
		float error_delta=pid->error[0]-pid->error[1];
		pid->error_sum += pid->KI * pid->error[0];
		//积分上限控制
		if (pid->error_sum > pid->error_max)
			pid->error_sum = pid->error_max;
		else if (pid->error_sum < -pid->error_max)
			pid->error_sum = -pid->error_max;
		pid->output = pid->KP * pid->error[0] + pid->error_sum 
								+ pid->KD * error_delta + pid->feedforward;
	}
	else if (pid->PID_Mode == PID_DELTA)
	{
		pid->output += pid->KP * (pid->error[0] - pid->error[1]) + 
		pid->KD * (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]) + pid->KI * pid->error[0]+ pid->feedforward;
	}
	//输出上限控制
	if(pid->output > pid->outputmax)
		pid->output = pid->outputmax;
	else if(pid->output < pid->outputmin)
		pid->output = pid->outputmin;
	return;
}

void pid_init(PID *pid, PID_MODE PID_Mode,float KP,float KI,float KD,float error_max,float outputmax,float outputmin)
{
	pid->PID_Mode = PID_Mode;
	pid->KP = KP;
	pid->KI = KI;
	pid->KD = KD;
	pid->error_max = error_max;
	pid->outputmax = outputmax;
	pid->outputmin = outputmin;
	return;
}

void pid_reset(PID *pid)
{
	pid->error_sum = 0;
	pid->output = 0;
	for(int i=0;i<3;i++)
	{
		pid->error[i] = 0;
	}
}

void pid_setfeedforward(PID *pid ,float feedforward)
{
	pid->feedforward = feedforward;
}
