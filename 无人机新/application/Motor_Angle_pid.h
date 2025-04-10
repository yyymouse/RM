#ifndef MOTOR_ANGLE_PID_H
#define MOTOR_ANGLE_PID_H
#include "struct_typedef.h"
#include "pid.h"
#include "CAN_receive.h"

#include "math.h"
#include "stdlib.h"

#define  motor_ratio 19           //减速箱减速比

//一些角度控制的结构体变量
typedef struct
{
	float POS_GAOL;//目标位置
	float POS_ABS;//绝对位置0
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
	const motor_measure_t *motor_data1;	//声明电机结构体指针
  const motor_measure_t *motor_data2;	//声明电机结构体指针

}shoot_move_t;

extern float ABS_ANGLE;

extern shoot_move_t motor_pid;

extern  float Motor_Angle_Cal(shoot_move_t *motor1_data);


#endif


