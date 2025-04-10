#ifndef MOTOR_ANGLE_PID_H
#define MOTOR_ANGLE_PID_H
#include "struct_typedef.h"
#include "pid.h"
#include "CAN_receive.h"

#include "math.h"
#include "stdlib.h"

#define  motor_ratio 19           //��������ٱ�

//һЩ�Ƕȿ��ƵĽṹ�����
typedef struct
{
	float POS_GAOL;//Ŀ��λ��
	float POS_ABS;//����λ��0
	float POS_OFFSET;
	float eer;
	float eer_eer;
}ANGLE_TypeDef;


typedef struct
{
	ANGLE_TypeDef ANGLE[8];
	pid_type_def   PID_SPEED[8];
	pid_type_def   PID_ANGLE[8];
	
}MOTOR_TypeDef;

typedef struct
{
	const motor_measure_t *motor_data1;	//��������ṹ��ָ��
  const motor_measure_t *motor_data2;	//��������ṹ��ָ��

}shoot_move_t;

extern float ABS_ANGLE;

extern shoot_move_t motor_pid;

extern  float Motor_Angle_Cal(shoot_move_t *motor1_data);


#endif


