#ifndef SHOOT_TASK_H
#define SHOOT_TASK_H

#include "MOTOR_ANGLE_PID.h"
#include "pid.h"
#include "main.h"
#include "remote_control.h"
#include "chassic_task.h"

#include "bsp_usb.h"

#define deadarea 17

#define upgrade_shoot_level          KEY_PRESSED_OFFSET_B    //���ٵ����ȼ�����

#define cover_switch                 KEY_PRESSED_OFFSET_R //�����ո�

#define OPEN_SERVO    2420
#define CLOSE_SERVO   670  //670

#define cooling_heat_spend 10    //��������ֵ

#define Trigger_speed_PID_P 50.2f
#define Trigger_speed_PID_I 0.01f
#define Trigger_speed_PID_D 1.0f 

#define Trigger_angle_PID_P 50.0f
#define Trigger_angle_PID_I 0
#define Trigger_angle_PID_D 0.5f 

#define motor1_angle_P 0.0f
#define motor1_angle_I 0.0f
#define motor1_angle_D 0.0f

#define motor1_PID_P  10.0f         //Ħ����  23
#define motor1_PID_I  0.0f
#define motor1_PID_D  2.0f    

#define motor2_angle_P 0.0f
#define motor2_angle_I 0.0f
#define motor2_angle_D 0.0f

#define motor2_PID_P  10.0f
#define motor2_PID_I  0.05f
#define motor2_PID_D  2.0f

#define motor_speed 0.5f

#define level_max 3    //���������λ����

#define Trigger_speed_PID_MAX_OUT   6000
#define Trigger_speed_PID_MAX_I_OUT 3000

#define shooting_frequency     90   //����Ƶ������  80
 
#define KEY_input 659             //�����ߵ�ƽ��Ч

#define division_angle   75   //51.0f     //һ�������̷ֶȽǶ�   51�̲���
#define tigger_direction -1     //������ת����

#define set_angle_err  500.0f        //���ýǶȺ�ʵ�ʽǶȲ�ֵ����ֵ

#define tigger_blocked_count_judgment 50

#define length 100//�������ecdֵ

typedef enum
{
	tigger_playing,       //��������ת
	tigger_over,          //��������ָ���趨�Ƕ�
	tigger_blocked        //�����̶�ת
	
}tigger_mode_en;
	
typedef enum
{
	  shoot_start,
		shoot_zero,
		shoot_one,         //����ģʽ
    shoot_double			 //˫��ģʽ

}shoot_mode_en;

typedef struct
{
	uint8_t       Init_count;
	uint16_t      key_run_num;      //������������ֵ
	uint16_t      KEY_outcome;      //�������ʵ��ֵ
	float         set_angle  ;			//Ŀ��Ƕ�
	float         set_angle_old;    //�ϴ����õĽǶ�
	_PID          MOTOR_ANGLE;      
  _PID          MOTOR_SPEED;     
	pid_type_def  MOTOR_ANGLE1;     //���̵���ǶȻ�
	pid_type_def  MOTOR_SPEED1;     //���̵���ٶȻ�
	pid_type_def  motor1_angle;
	pid_type_def  motor1;           //Ħ�����ٶȻ�
	pid_type_def  motor2_angle;
	pid_type_def  motor2;
	CLASSICPID    MOTOR_ANGLE2;     //������pid
	shoot_mode_en shoot_mode;       //���ģʽ     
	const motor_measure_t *motor_data1;	//��������������ṹ��ָ��
	const motor_measure_t *motor1_data;	//Ħ������������ṹ��ָ��
	const motor_measure_t *motor2_data;	//Ħ������������ṹ��ָ��
	int xLastWakeTime;
	uint32_t tigger_blocked_count;      //��ת�жϼ���ֵ
	uint32_t last_count;                //��ת�ϴμ���ֵ
	const RC_ctrl_t *shoot_RC;             //ң����ֵ     
	float LAST_ANGLE;                   //�ϴνǶȴ�ֵ
  int KEY;
	int motor1_angle_out;									//�ǶȻ����
	int motor2_angle_out;									//
	uint16_t *shoot_cooling_rate;        //�����ȴ�ٶ�
	uint16_t *shoot_cooling_limit;       //�����ȴ����
	uint16_t *shoot_speed_limit;         //��������
	uint16_t *cooling_heat;              //ʵʱǹ������
  int shoot_motor1_angle;
  int shoot_motor2_angle;
	uint16_t speed_level;                //ǹ�����ٵȼ�
  ramp_function_source_t shoot1_filter;
	ramp_function_source_t shoot2_filter;
	
}shoot_all;

extern uint8_t  mag_flag;

extern shoot_all shoot;

extern  uint32_t xTaskGetTickCount(void);
extern void shoot_task(void const * pvParameters);
void positive_motion();
void negative_motion();
#endif

