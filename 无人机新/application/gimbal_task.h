#ifndef GIMBAL_TASK_H
#define GIMBAL_TASK_H

#include "arm_math.h"
#include "remote_control.h"
#include "CAN_receive.h"
#include "pid.h"
#include "FreeRTOS.h"
#include "user_lib.h"
#include "main.h"
#include "INS_task.h"
//#include "chassic_task.h"

#define pi  3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f

#define ballistic_compensation 0.0f    //弹道补偿值

#define PITCH_DIRECTION -1    //pitch方向
#define YAW_DIRECTION 1      //yaw方向 

#define MAX_ANGLE  0.33f   //
#define MIN_ANGLE -0.6f   //P轴

//pitch gyro angle close-loop PID params, max out and max iout
//pitch 角度环 角度由陀螺仪解算 PID参数以及 PID最大输出，积分输出
#define PITCH_GYRO_ABSOLUTE_PID_KP   6.6f      // 150.0f //26   //150    50
#define PITCH_GYRO_ABSOLUTE_PID_KI   0.0f
#define PITCH_GYRO_ABSOLUTE_PID_KD   1.5f           //-1.1f

#define PITCH_GYRO_ABSOLUTE_PID_MAX_OUT  6.0f
#define PITCH_GYRO_ABSOLUTE_PID_MAX_IOUT 1.0f
 
//pitch speed close-loop PID params, max out and max iout
//pitch 速度环 PID参数以及 PID最大输出，积分输出
#define PITCH_SPEED_PID_KP        2.0f// 8500.0f///////////////////////7000
#define PITCH_SPEED_PID_KI        0.2f//60.0f
#define PITCH_SPEED_PID_KD        1.0f//-170.0f  
#define PITCH_SPEED_PID_MAX_OUT    30.0f//30000.0f
#define PITCH_SPEED_PID_MAX_IOUT   1.5f//10000.0f

//yaw gyro angle close-loop PID params, max out and max iout
//yaw 角度环 角度由陀螺仪解算 PID参数以及 PID最大输出，积分输出
#define YAW_GYRO_ABSOLUTE_PID_KP        80.0f//35.0f //20.0f  //130   不开陀螺的自瞄       有预测
#define YAW_GYRO_ABSOLUTE_PID_KI        0.0f//3.0f
#define YAW_GYRO_ABSOLUTE_PID_KD        -4.0f//0.3
#define YAW_GYRO_ABSOLUTE_PID_MAX_OUT   50.0f
#define YAW_GYRO_ABSOLUTE_PID_MAX_IOUT  20.0f

#define YAW_SPEED_PID_KP        1400.0f//2900.0f //3600
#define YAW_SPEED_PID_KI        0.0f//20.0f
#define YAW_SPEED_PID_KD        20.0f
#define YAW_SPEED_PID_MAX_OUT   30000.0f
#define YAW_SPEED_PID_MAX_IOUT  5000.0f

//遥控器输入死区，因为遥控器存在差异，摇杆在中间，其值不一定为零
#define RC_DEADBAND   20 //原来的值10，yao

#define gimbal_pitch_CHANNEL  3
//pitch和yaw的遥控器通道号码
#define gimbal_yaw_CHANNEL    2

#define Motor_Angle_to_Rad_YAW    -0.00001f//自瞄跟随速度  -0.0001f
#define Motor_Angle_to_Rad_PITCH  -0.00001f//自瞄跟随速度  -0.0001f

//#define yaw_offset_ecd    4250    //yaw轴电机头正方向电机值   小绿2880    小黑150    4250   //小黑2号6000    全向轮3915

#define yaw_ecd         2056   //yaw轴电机头正方向电机值   小绿2880    小黑150    4250   //小黑2号6000    全向轮3915
#define yaw_back_ecd    6583   //2503

#define pitch_offset_ecd  8080      //pitch轴电机水平方向电机值   小绿2800    小黑6780
//#define YAW_MOUSE_SEN   0.0015f//0.00005f
//#define PITCH_MOUSE_SEN 0.002f//f0.00015f

#define HALF_ECD_RANGE  4096   //电机编码器中值
#define ECD_RANGE       8191   //电机编码器最大值

//电机编码值转化成角度值
#define MOTOR_ECD_TO_RAD 0.000766990394f //      2*  PI  /8192

//#define yaw_rc_multiple    0.000025f  //yaw轴旋转速度
//#define pitch_rc_multiple  0.000015f //pitch旋转速度

//视觉信息转换系数 1/pi * kp最大值
#define Visual_Transform 0.00007f//30.0f


#define GIMBAL_ACCEL_X_NUM 0.1666666667f
#define GIMBAL_ACCEL_Y_NUM 0.3333333333f

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
} gimbal_PID_t;

typedef enum
{
	  GIMBAL_ZERO, 
    GIMBAL_MOTOR_RAW = 0, //电机原始值控制
    GIMBAL_MOTOR_GYRO,    //陀螺仪自稳模式
    GIMBAL_MOTOR_ENCONDE, //电机编码值角度控制
		GIMBAL_MOTOR_VISUAL,  //电机视觉控制
	  GIMBAL_MOTOR_VISUAL_NULL//无视觉模式
	  
} gimbal_motor_mode_e;

typedef struct
{
    const motor_measure_t *gimbal_motor_measure;
    gimbal_PID_t gimbal_motor_absolute_angle_pid;
    gimbal_PID_t gimbal_motor_relative_angle_pid;
    gimbal_PID_t gimbal_motor_visual_angle_pid;
    pid_type_def gimbal_motor_gyro_pid;
    gimbal_motor_mode_e gimbal_motor_mode;
    gimbal_motor_mode_e last_gimbal_motor_mode;
    uint16_t offset_ecd;
    fp32 max_relative_angle; //rad
    fp32 min_relative_angle; //rad

	  fp32 goal_angle ;        //rad

    fp32 relative_angle;     //rad
    fp32 relative_angle_set; //rad
	  fp64 difference_angle;
	  fp32 absolute_angle_last;
    fp32 absolute_angle;     //rad
    fp32 absolute_angle_set; //rad
    fp32 motor_gyro;         //rad/s
    fp32 motor_gyro_set;
    fp32 motor_speed;
    fp32 raw_cmd_current;
    fp32 current_set;
    int16_t given_current;
		
   	fp32 pitch_angle;
    fp32 image_angle;
		
		fp64 visual_pixel;//视觉接收像素点
		fp64 visual_pixel_set;
		fp64 visual_angle;//视觉接收像素点
		fp64 visual_angle_set;
    

} gimbal_motor_t;

typedef struct
{
	
		 first_order_filter_type_t gimbal_cmd_slow_set_x;
		 first_order_filter_type_t gimbal_cmd_slow_set_y;
	
	    gimbal_motor_t gimbal_yaw_motor;
      gimbal_motor_t gimbal_pitch_motor;
    

//	  const motor_measure_t *gimbal_motor_measure[2];  //电机返回参数值指针
	  const fp32 *gimbal_INT_angle_point;  //角度
    const fp32 *gimbal_INT_gyro_point;	 //角速度 
	   const fp32 *gimbal_INT_accel_point;	 //加速度 
	  const fp32 *gimbal_INT_mag_point;

	
	
	  const fp32 *gimbal_INT_quat;	       //四元数
	  const RC_ctrl_t *gimbal_rc;        //遥控器指针
    pid_type_def gyro_yaw_motor_speed;      //陀螺仪yaw速度环pid
	  pid_type_def gyro_yaw_motor_angle;      //陀螺仪yaw角度环pid
	  pid_type_def yaw_motor_speed;           //编码器yaw速度环pid
		pid_type_def yaw_motor_angle;           //编码器yaw角度环pid
	
	
	  pid_type_def visual_pitch_motor_speed;    //视觉pitch速度环pid
	  pid_type_def visual_pitch_motor_angle;    //视觉pitch角度环pid

      pid_type_def pitch_motor_speed;    //pitch速度环pid
	  pid_type_def pitch_motor_angle;    //pitch角度环pid
      
	
	  fp32 first_angle_point;
	  bool_t yaw_first_angle_flag;
	  fp64 add_yaw;
	  fp64 add_pitch;
		fp64 yaw_old;
		fp64 flag_shoot ;
		uint8_t goal_logo ;
}gimbal;
extern  fp32 pitch_angle;

extern fp32 yaw_P;
extern fp32 pitch_P;

extern gimbal gimbal_ctrl;
extern void gimbal_task(void const *pvParameters);
extern const gimbal_motor_t *get_yaw_motor_point(void);//返回yaw云台电机的参数
extern float gimbal_yaw_calc(void);//去零点
extern void gimbal_init(gimbal *gimbal_init);
extern int yaw_offset_ecd;
extern void calculate_absolute_pitch(const float *gyro_data, const float *accel_data, const float *mag_data, float *pitch);
extern  void gimbal_PID_init(gimbal_PID_t *pid, fp32 maxout, fp32 max_iout, fp32 kp, fp32 ki, fp32 kd);
extern  fp32 gimbal_PID_calc(gimbal_PID_t *pid, fp32 get, fp32 set, fp32 error_delta);
#endif

