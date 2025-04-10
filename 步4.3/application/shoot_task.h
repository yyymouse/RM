#ifndef SHOOT_TASK_H
#define SHOOT_TASK_H

#include "MOTOR_ANGLE_PID.h"
#include "pid.h"
#include "main.h"
#include "remote_control.h"
#include "chassic_task.h"

#define upgrade_shoot_level          KEY_PRESSED_OFFSET_B    //���ٵ����ȼ�����

#define cover_switch                 KEY_PRESSED_OFFSET_R //�����ո�

#define OPEN_SERVO    2420
#define CLOSE_SERVO   670  //670

#define cooling_heat_spend 10    //��������ֵ

#define Trigger_speed_PID_P 50.2f
#define Trigger_speed_PID_I 0.00f
#define Trigger_speed_PID_D 0.0f 

#define Trigger_angle_PID_P 50.0f
#define Trigger_angle_PID_I 0
#define Trigger_angle_PID_D 0.5f 

#define motor1_PID_P  43.0f         //Ħ����
#define motor1_PID_I  0.0f
#define motor1_PID_D  15.0f    

#define motor2_PID_P  43.0f
#define motor2_PID_I  0.0f
#define motor2_PID_D  15.0f

#define motor1_speed1 7000      //Ħ����ת��15m/s//4850
#define motor2_speed1 7000

#define motor1_speed2 7000          //Ħ����ת��18m/s  5220
#define motor2_speed2 7000

#define motor1_speed3 7200          //Ħ����ת��30m/s
#define motor2_speed3 7200

#define level_max 3    //���������λ����

#define Trigger_speed_PID_MAX_OUT   6000
#define Trigger_speed_PID_MAX_I_OUT 3000

#define shooting_frequency     90   //����Ƶ������  80
 
#define KEY_input 659             //�����ߵ�ƽ��Ч

#define division_angle   51.0f     //һ�������̷ֶȽǶ�   51�̲���
#define tigger_direction -1     //������ת����

#define set_angle_err  500.0f        //���ýǶȺ�ʵ�ʽǶȲ�ֵ����ֵ

#define tigger_blocked_count_judgment 30

#define speed_task 2000//����

typedef enum
{
	tigger_playing,       //��������ת
	tigger_over,          //��������ָ���趨�Ƕ�
	tigger_blocked        //�����̶�ת
	
}tigger_mode_en;
	
typedef enum
{
		shoot_down,           //ֹͣ����
    shoot_start,
		visual_shoot
}shoot_mode_en;

typedef struct
{
	uint8_t       Init_count;
	uint16_t      key_run_num;      //������������ֵ
	uint16_t      KEY_outcome;      //�������ʵ��ֵ
	float         set_angle;			//Ŀ��Ƕ�
	float         set_angle_old;    //�ϴ����õĽǶ�
	_PID          MOTOR_ANGLE;      
  _PID          MOTOR_SPEED;     
	pid_type_def  MOTOR_ANGLE1;     //���̵���ǶȻ�
	pid_type_def  MOTOR_SPEED1;     //���̵���ٶȻ�
	pid_type_def  motor1;           //Ħ�����ٶȻ�
	pid_type_def  motor2;
	CLASSICPID    MOTOR_ANGLE2;     //������pid
	shoot_mode_en shoot_mode;       //���ģʽ
	tigger_mode_en tigger_mode;      //����ģʽ
	const motor_measure_t *motor_data1;	//��������������ṹ��ָ��
	const motor_measure_t *motor1_data;	//Ħ������������ṹ��ָ��
	const motor_measure_t *motor2_data;	//Ħ������������ṹ��ָ��
	int xLastWakeTime;
	uint32_t tigger_blocked_count;      //��ת�жϼ���ֵ
	uint32_t last_count;                //��ת�ϴμ���ֵ
	const RC_ctrl_t *shoot_RC;             //ң����ֵ     
	float LAST_ANGLE;                   //�ϴνǶȴ�ֵ
  int KEY;
	uint16_t *shoot_cooling_rate;        //�����ȴ�ٶ�
	uint16_t *shoot_cooling_limit;       //�����ȴ����
	uint16_t *shoot_speed_limit;         //��������
	uint16_t *cooling_heat;              //ʵʱǹ������
	
	uint16_t speed_level;                //ǹ�����ٵȼ�
	
//	first_order_filter_type_t shoot1_filter;//�������˲�Ħ���ֵ��1
//	first_order_filter_type_t shoot2_filter;
//	
  ramp_function_source_t shoot1_filter;
	ramp_function_source_t shoot2_filter;
	float last_angle;
}shoot_all;

extern uint8_t  mag_flag;

extern shoot_all shoot;

extern  uint32_t xTaskGetTickCount(void);
extern void shoot_task(void const * pvParameters);

#endif

