#include "MOTOR_ANGLE_PID.h"

uint8_t flag_cnt_calib=1;

MOTOR_TypeDef motor;

float pos[2], pos_old[2];

shoot_move_t motor_pid;

MOTOR_TypeDef motor;

uint8_t cnt = 1;
uint8_t cnt_old = 0;


float ABS_ANGLE;
float first_ABS_ANGLE=0;

/**
  * @brief         ���ٵ������ʵ�ʽǶ�ֵ����          
  * @param[in]     motor1_data������ṹ���ֵ 
	* @author        ���� 
  * @retval        ABS_ANGLE:����������ʵ����Ƕ�ֵ���ϵ翪����ʼ�ǶȲ�Ϊ�㣩
  */
float Motor_Angle_Cal(shoot_move_t *motor1_data)
{
	
	motor1_data->motor_data1 = get_trigger2_motor_measure_point();
	motor1_data->motor_data2 = get_chassis_motor_measure_point(1);
  
	float  res1, res2;
	static float pos[2], pos_old[2];
  uint16_t	motor_num=1;
	uint16_t	T=8190;//һȦ�ķֶ�ֵ

	
	pos[motor_num] =motor1_data->motor_data1->ecd;
	motor.ANGLE[motor_num].eer=pos[motor_num] - pos_old[motor_num];
	
	if(flag_cnt_calib&&cnt)
	{
		pos_old[motor_num]=motor1_data->motor_data1->ecd;
		cnt=0;
	}
	
	if(motor.ANGLE[motor_num].eer>0) 	
	{
		res1=motor.ANGLE[motor_num].eer-T;//��ת,�Լ�����T
		res2=motor.ANGLE[motor_num].eer;
	}
	else
	{
		res1=motor.ANGLE[motor_num].eer+T;//��ת���Լ�����T
		res2=motor.ANGLE[motor_num].eer;
	}
	
	if(fabs(res1)<fabs(res2)) //��������ת���϶���ת�ĽǶ�С���Ǹ������
	{
		motor.ANGLE[motor_num].eer_eer = res1;
	}
	else
	{
		motor.ANGLE[motor_num].eer_eer = res2;
	}
	
	motor.ANGLE[motor_num].POS_ABS =(motor.ANGLE[motor_num].POS_ABS+ motor.ANGLE[motor_num].eer_eer);

	pos_old[motor_num]  = pos[motor_num];
  ABS_ANGLE=motor.ANGLE[motor_num].POS_ABS/((8190*motor_ratio)/360);
  
	if(cnt_old==0)
	{
		first_ABS_ANGLE=ABS_ANGLE;
	  cnt_old=1;
	}
	
	ABS_ANGLE=ABS_ANGLE-first_ABS_ANGLE;
	return ABS_ANGLE;
	
}




