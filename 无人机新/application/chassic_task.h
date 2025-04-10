#ifndef CHASSIC_TASK_H
#define CHASSIC_TASK_H

#include "remote_control.h"
#include "CAN_receive.h"
#include "pid.h"
#include "FreeRTOS.h"
#include "main.h"
#include "gimbal_task.h"
#include "arm_math.h"
#include "user_lib.h"
#include "cmd_ctr.h"

//m3508转化成底盘速度(m/s)的比例，
#define M3508_MOTOR_RPM_TO_VECTOR 0.00041553708677306599621414267730162f    
#define CHASSIS_MOTOR_RPM_TO_VECTOR_SEN M3508_MOTOR_RPM_TO_VECTOR

#define CHASSIS_ACCEL_X_NUM 0.1666666667f
#define CHASSIS_ACCEL_Y_NUM 0.3333333333f

//底盘任务控制间隔 0.002s
#define CHASSIS_CONTROL_TIME 0.002f

//前后的遥控器通道号码
#define CHASSIS_X_CHANNEL 1
//the channel num of controlling horizontal speed
//左右的遥控器通道号码
#define CHASSIS_Y_CHANNEL 0

//摇杆死区
#define CHASSIS_RC_DEADLINE 20

#define CHASSIS_VX_RC_SEN 0.005f
//rocker value (max 660) change to horizontal speed (m/s)
//遥控器左右摇杆（max 660）转化成车体左右速度（m/s）的比例
#define CHASSIS_VY_RC_SEN 0.005f

#define s_right 0
#define s_left  1

#define MOTOR_SPEED_TO_CHASSIS_SPEED_VX 0.05f     //姿态解算倍率
#define MOTOR_SPEED_TO_CHASSIS_SPEED_VY 0.05f
#define MOTOR_SPEED_TO_CHASSIS_SPEED_WZ 0.25f

#define M3508_rpm_to_v 0.000415809748903494517209f //m3508转化成底盘速度(m/s)的比例，


#define M3505_v_P 10000.0f
#define M3505_v_I 10.0f
#define M3505_v_D -10.0f

#define M3508_out_max  16384.0f
#define M3508_iout_max 2000.0f


#define M3505_angle_P 10.0f   //底盘角度环   8
#define M3505_angle_I 0.0f
#define M3505_angle_D -5.0f
#define M3508_angle_out_max  50.50f
#define M3508_angle_iout_max 100.2f

#define M3505_speed_P 10.0f    //底盘速度环   3
#define M3505_speed_I 0.0f
#define M3505_speed_D 0.0f
#define M3508_speed_out_max  3.0f     //7
#define M3508_speed_iout_max 0.2f

#define spin_speed_P 15.0f     //小陀螺恒速旋转
#define spin_speed_I 0.0f
#define spin_speed_D 10.0f
#define spin_speed_out_max 100.0f
#define spin_speed_iout_max 100.0f

#define spin_wz 10.0f  //目标旋转速度

#define chassis_x_ch  1
#define chassis_y_ch  0
#define chassis_wz_ch 2

#define CHASSIS_WZ_SET_SCALE 0.1f

#define TOP_SPIN_SPEEd 9.0f //小陀螺转速    9

//旧小黑x 330       y 390
//新小黑x 400       y 375

#define MOTOR_DISTANCE_TO_CENTER 0.3875f  //底盘旋转速度倍率   （x/2+y/2）*0.001


#define CHASSIS_OPEN_RC_SCALE 10 // in CHASSIS_OPEN mode, multiply the value. 在chassis_open 模型下，遥控器乘以该比例发送到can上

//电机编码值转化成角度值

#define MOTOR_ECD_TO_RAD 0.000766990394f //      2*  PI  /8192


//底盘摇摆按键
#define SWING_KEY KEY_PRESSED_OFFSET_CTRL
//chassi forward, back, left, right key
//底盘前后左右控制按键
#define CHASSIS_FRONT_KEY KEY_PRESSED_OFFSET_W
#define CHASSIS_BACK_KEY KEY_PRESSED_OFFSET_S
#define CHASSIS_LEFT_KEY KEY_PRESSED_OFFSET_A
#define CHASSIS_RIGHT_KEY KEY_PRESSED_OFFSET_D

#define SUPER_CAP KEY_PRESSED_OFFSET_C  //超级电容

#define DIVERSION KEY_PRESSED_OFFSET_Z  //掉头

//小陀螺开启键可以左旋和右旋
#define TOP_LEFT_SPIN KEY_PRESSED_OFFSET_SHIFT

//键盘操作加速比例
#define CHASSISS_ACCELERATION 0.005
#define SPIN_ACCELERATION 0.001

//底盘运动过程最大前进速度
#define NORMAL_MAX_CHASSIS_SPEED_X power_limit_X_5
//chassis left or right max speed
//底盘运动过程最大平移速度
#define NORMAL_MAX_CHASSIS_SPEED_Y power_limit_Y_5

//#define power_limit_X_5 1.5f           //50W         
//#define power_limit_Y_5 1.8f              

//#define power_limit_X_6 2.09f           //60
//#define power_limit_Y_6 1.8f
// 
//#define power_limit_X_7 2.29f           //70
//#define power_limit_Y_7 1.85f

//#define power_limit_X_8 2.39f           //80
//#define power_limit_Y_8 1.95f
// 
//#define power_limit_X_10 2.9f          //100
//#define power_limit_Y_10 2.8f
// 
//#define power_limit_X_12 2.50f          //120
//#define power_limit_Y_12 2.30f

//#define power_limit_X_CP 111.0f          //超级电容
//#define power_limit_Y_CP 111.5f

#define power_limit_X_5 1.1f           //45W   1.5\1.0        
#define power_limit_Y_5 1.0f

#define power_limit_X_6 1.3f           //50
#define power_limit_Y_6 1.6f 

#define power_limit_X_7 1.6f           //55
#define power_limit_Y_7 1.4f

#define power_limit_X_8 1.9f          //60
#define power_limit_Y_8 1.6f

#define power_limit_X_10 2.1f         //80
#define power_limit_Y_10 1.7f

#define power_limit_X_12 2.7f          //100
#define power_limit_Y_12 2.6f

#define power_limit_X_CP 4.0f           
#define power_limit_Y_CP 3.8f


typedef struct
{
    fp32 kp;
    fp32 ki;
    fp32 kd;

    fp32 set;
    fp32 get;
    fp32 err;

    fp32 max_out;
    fp32 max_iout;

    fp32 Pout;
    fp32 Iout;
    fp32 Dout;

    fp32 out;
} chassic_PID_t;

typedef enum
{
  chassic_follow_gim_yaw,           //小陀螺模式
	chassic_follow_gim_yaw_fanxiang,  //反向小陀螺
  chassic_follow_chassic_yaw,  //云台跟随底盘模式     
  chassic_zero,                //底盘停止模式
	division_mode               //一点掉头模式
	
} chassis_mode_e;

typedef enum
{
	remote_mode=0,//默认模式，遥控器控制
	auto_mod,//此模式为上位机控制运动模式
  
} cmd_mode;

typedef struct
{
  const motor_measure_t *chassis_motor_measure;
  fp32 accel;
  fp32 speed;
  fp32 speed_set;
  int16_t give_current;
} chassis_motor_t;

typedef struct
{
	first_order_filter_type_t chassis_cmd_slow_set_vx;  //use first order filter to slow set-point.使用一阶低通滤波减缓设定值
  first_order_filter_type_t chassis_cmd_slow_set_vy;  //use first order filter to slow set-point.使用一阶低通滤波减缓设定值
	chassis_mode_e chassis_mode;
	cmd_mode Sport_mode;
	const fp32 *chassis_INS_angle;             //底盘获取陀螺仪参数
	const fp32 *gimbal_INT_gyro_point;	       //角速度
	const gimbal_motor_t *chassis_yaw_motor;   //底盘使用到yaw云台电机的相对角度来计算底盘的欧拉角.
  const gimbal_motor_t *chassis_pitch_motor; //底盘使用到pitch云台电机的相对角度来计算底盘的欧拉角
	fp32 wheel_speed[4];                     //最后输出给4个电机对应的输出值
	chassis_motor_t chassis_motor[4];        //底盘电机数据
  const RC_ctrl_t *chassis_RC;             //遥控器值
//	const motor_measure_t *chassis_motor[4]; //底盘四个电机返回参数指针
  pid_type_def motor_speed_pid[4];         //四个电机的pid
	first_order_filter_type_t motor_speed_pid_slow[4];
	pid_type_def chassis_angle_pid;          //底盘跟随角度pid
	pid_type_def chassis_speed_pid;          //底盘跟随角度pid
	pid_type_def chassis_vx_speed_pid;       //底盘x速度环
	pid_type_def chassis_vy_speed_pid;      //底盘y速度环
	
	chassic_PID_t chassic_wz;     //地盘跟随云台pid角度环

	fp32 wz_speed_set;
	
	pid_type_def chassis_mode_spin_wz;      //小陀螺自旋速度闭环 ，以yaw轴编码器旋转速度为参照
  fp32 vx_max_speed;  //max forward speed, unit m/s.前进方向最大速度 单位m/s
  fp32 vx_min_speed;  //max backward speed, unit m/s.后退方向最大速度 单位m/s
  fp32 vy_max_speed;  //max letf speed, unit m/s.左方向最大速度 单位m/s
  fp32 vy_min_speed;  //max right speed, unit m/s.右方向最大速度 单位m/s
	fp32 vx;
	fp32 vy;
	fp32 wz;
	fp32 set_vx;
	fp32 set_vy;
	fp32 set_wz;
	fp32 chassis_yaw;   //陀螺仪和云台电机叠加的yaw角度
  fp32 chassis_pitch; //陀螺仪和云台电机叠加的pitch角度
  fp32 chassis_roll;  //陀螺仪和云台电机叠加的roll角度
	gimbal_motor_mode_e gimble_mode;
	uint16_t *power_limit;  //机器人底盘功率限制上限
	fp32 TOP_SPIN_SPEED;
} chassis;

extern float diversion_key_count;
extern chassis chassic_ctrl;//底盘所有参数存值结构体

extern void chassic_task(void const * pvParameters);

extern const chassis *get_chassic_date(void);

extern void chassis_init(chassis *chassis_init);
extern fp32 robot_color_hero  ;
#endif



