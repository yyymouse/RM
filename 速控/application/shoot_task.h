#ifndef SHOOT_TASK_H
#define SHOOT_TASK_H

#include "MOTOR_ANGLE_PID.h"
#include "pid.h"
#include "main.h"
#include "remote_control.h"
#include "chassic_task.h"

#include "bsp_usb.h"

#define deadarea 17

#define upgrade_shoot_level          KEY_PRESSED_OFFSET_B    //射速调整等级按键

#define cover_switch                 KEY_PRESSED_OFFSET_R //开弹舱盖

#define OPEN_SERVO    2420
#define CLOSE_SERVO   670  //670

#define cooling_heat_spend 10    //热量限制值

#define Trigger_speed_PID_P 50.2f
#define Trigger_speed_PID_I 0.01f
#define Trigger_speed_PID_D 1.0f 

#define Trigger_angle_PID_P 50.0f
#define Trigger_angle_PID_I 0
#define Trigger_angle_PID_D 0.5f 

#define motor1_angle_P 0.0f
#define motor1_angle_I 0.0f
#define motor1_angle_D 0.0f

#define motor1_PID_P  10.0f         //摩擦轮  23
#define motor1_PID_I  0.0f
#define motor1_PID_D  2.0f    

#define motor2_angle_P 0.0f
#define motor2_angle_I 0.0f
#define motor2_angle_D 0.0f

#define motor2_PID_P  10.0f
#define motor2_PID_I  0.05f
#define motor2_PID_D  2.0f

#define motor_speed 0.5f

#define level_max 3    //最多三个挡位射速

#define Trigger_speed_PID_MAX_OUT   6000
#define Trigger_speed_PID_MAX_I_OUT 3000

#define shooting_frequency     90   //拨弹频率设置  80
 
#define KEY_input 659             //按键高电平有效

#define division_angle   75   //51.0f     //一个拨弹盘分度角度   51绿步兵
#define tigger_direction -1     //拨盘旋转方向

#define set_angle_err  500.0f        //设置角度和实际角度差值允许值

#define tigger_blocked_count_judgment 50

#define length 100//轨道长度ecd值

typedef enum
{
	tigger_playing,       //拨弹正在转
	tigger_over,          //拨弹到达指定设定角度
	tigger_blocked        //拨弹盘堵转
	
}tigger_mode_en;
	
typedef enum
{
	  shoot_start,
		shoot_zero,
		shoot_one,         //单发模式
    shoot_double			 //双发模式

}shoot_mode_en;

typedef struct
{
	uint8_t       Init_count;
	uint16_t      key_run_num;      //按键消抖计数值
	uint16_t      KEY_outcome;      //按键输出实际值
	float         set_angle  ;			//目标角度
	float         set_angle_old;    //上次设置的角度
	_PID          MOTOR_ANGLE;      
  _PID          MOTOR_SPEED;     
	pid_type_def  MOTOR_ANGLE1;     //拨盘电机角度环
	pid_type_def  MOTOR_SPEED1;     //拨盘电机速度环
	pid_type_def  motor1_angle;
	pid_type_def  motor1;           //摩擦轮速度环
	pid_type_def  motor2_angle;
	pid_type_def  motor2;
	CLASSICPID    MOTOR_ANGLE2;     //带死区pid
	shoot_mode_en shoot_mode;       //射击模式     
	const motor_measure_t *motor_data1;	//拨弹轮声明电机结构体指针
	const motor_measure_t *motor1_data;	//摩擦轮声明电机结构体指针
	const motor_measure_t *motor2_data;	//摩擦轮声明电机结构体指针
	int xLastWakeTime;
	uint32_t tigger_blocked_count;      //堵转判断计数值
	uint32_t last_count;                //堵转上次计数值
	const RC_ctrl_t *shoot_RC;             //遥控器值     
	float LAST_ANGLE;                   //上次角度存值
  int KEY;
	int motor1_angle_out;									//角度环输出
	int motor2_angle_out;									//
	uint16_t *shoot_cooling_rate;        //射击冷却速度
	uint16_t *shoot_cooling_limit;       //射击冷却上限
	uint16_t *shoot_speed_limit;         //射速上限
	uint16_t *cooling_heat;              //实时枪口热量
  int shoot_motor1_angle;
  int shoot_motor2_angle;
	uint16_t speed_level;                //枪口射速等级
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

