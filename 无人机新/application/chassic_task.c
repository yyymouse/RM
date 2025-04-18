#include "chassic_task.h"
#include "bsp_usb.h"
#include "referee.h"
#include "gimbal_task.h"
#include "remote_control.h"
#include "usb_task.h"
#include "cmd_ctr.h"
#include "stm32_flash.h"

#define rc_deadband_limit(input, output, dealine)        \
    {                                                    \
        if ((input) > (dealine) || (input) < -(dealine)) \
        {                                                \
            (output) = (input);                          \
        }                                                \
        else                                             \
        {                                                \
            (output) = 0;                                \
        }                                                \
    }


chassis chassic_ctrl;//底盘所有参数存值结构体
		
uint16_t power_1 = 55 ;
uint16_t power_2 = 60 ;
uint16_t power_3 = 65 ;	
uint32_t color_tick = 0;
fp32 robot_color_hero = 0 ;
void chassis_init(chassis *chassis_init);//初始化函数
static void chassis_ctrl_loop(chassis *chassic_ctrl_loop);//底盘电机输出循环
static void chassis_feedback_update(chassis *chassis_date_update);//底盘姿态更新
static void chassic_follow_gimbal(chassis *chassis_rc_vector);//底盘跟随云台角速度计算
static void chassis_vector_to_mecanum_wheel_speed(chassis *chassis_wheel_speed);
static void chassic_mode_set(chassis *mode);//状态机模式切换
static void chassic_mode_ctrl(chassis *mode_ctrl);//状态机模式对应执行
int xjb_direction = 1;
void chassic_task(void const * pvParameters)
{

	chassis_init(&chassic_ctrl); //初始化
	for(;;)
	{
		    if(chassic_ctrl.chassis_RC->key.v & KEY_PRESSED_OFFSET_Q)
		{
		chassic_ctrl.power_limit = &power_1;
		}
		else if (chassic_ctrl.chassis_RC->key.v & KEY_PRESSED_OFFSET_E)
		{
		chassic_ctrl.power_limit = &power_2;		 
		}
		else if (chassic_ctrl.chassis_RC->key.v & KEY_PRESSED_OFFSET_F)
		{
		chassic_ctrl.power_limit = &power_3;				
		}
//		chassic_ctrl.power_limit = &ext_game_robot_status.chassis_power_limit ; /*超级电容*/ 
	  CAN_cmd_supercap(((unsigned short)*chassic_ctrl.power_limit)*100);
		chassis_feedback_update(&chassic_ctrl);  //底盘状态数据计算以及更新
	  chassic_mode_set(&chassic_ctrl);//状态机模式切换
	  chassic_mode_ctrl(&chassic_ctrl);//状态机模式对应执行
		chassis_vector_to_mecanum_wheel_speed(&chassic_ctrl);  //运动分解

    vTaskDelay(5);
	}

}

const chassis *get_chassic_date(void)
{
	return &chassic_ctrl;
}


static void chassic_PID_init(chassic_PID_t *pid, fp32 maxout, fp32 max_iout, fp32 kp, fp32 ki, fp32 kd)
{
    if (pid == NULL)
    {
        return;
    }
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->err = 0.0f;
    pid->get = 0.0f;

    pid->max_iout = max_iout;
    pid->max_out = maxout;
}

static fp32 chassic_PID_calc(chassic_PID_t *pid, fp32 get, fp32 set, fp32 error_delta)
{
    fp32 err;
    if (pid == NULL)
    {
        return 0.0f;
    }
    pid->get = get;
    pid->set = set;

    err = set - get;
    pid->err = rad_format(err);
    pid->Pout = pid->kp * pid->err;
    pid->Iout += pid->ki * pid->err;
    pid->Dout = pid->kd * error_delta;
    abs_limit(&pid->Iout, pid->max_iout);
    pid->out = pid->Pout + pid->Iout + pid->Dout;
    abs_limit(&pid->out, pid->max_out);
    return pid->out;
}



void chassis_init(chassis *chassis_init)
{		

          
					const static fp32 chassis_x_order_filter[1] = {CHASSIS_ACCEL_X_NUM};    //速度设置的一阶滤波参数
					const static fp32 chassis_y_order_filter[1] = {CHASSIS_ACCEL_Y_NUM};
					
					const static fp32 chassis_filter[1] = {0.01};
					uint8_t i;
					chassis_init->chassis_RC = get_remote_control_point(); //获取遥控器指针
					chassis_init->chassis_yaw_motor = get_yaw_motor_point();//获取yaw轴数据
					chassis_init->chassis_INS_angle = get_INS_angle_point();//获取imu数据
					chassis_init->gimbal_INT_gyro_point = get_gyro_data_point();//获取角速度
					chassis_init->chassis_motor[0].chassis_motor_measure = get_chassis_motor_measure_point(0); //获取底盘四个电机返回值
					chassis_init->chassis_motor[1].chassis_motor_measure = get_chassis_motor_measure_point(1);
					chassis_init->chassis_motor[2].chassis_motor_measure = get_chassis_motor_measure_point(2);
					chassis_init->chassis_motor[3].chassis_motor_measure = get_chassis_motor_measure_point(3);
					chassis_init->chassis_mode = chassic_zero;  //初始化关闭底盘
					chassis_init->Sport_mode = remote_mode;//初始化为遥控器控制
		 
				 const static fp32 chassis_mode_spin_pid1[3] = {spin_speed_P, spin_speed_I, spin_speed_D};    //自旋速度
				 
				 
				 PID_init(&chassis_init->motor_speed_pid[0],PID_POSITION,mid_date.motor_speed_pid,M3508_out_max,M3508_iout_max);
				 PID_init(&chassis_init->motor_speed_pid[1],PID_POSITION,mid_date.motor_speed_pid,M3508_out_max,M3508_iout_max);
				 PID_init(&chassis_init->motor_speed_pid[2],PID_POSITION,mid_date.motor_speed_pid,M3508_out_max,M3508_iout_max);
				 PID_init(&chassis_init->motor_speed_pid[3],PID_POSITION,mid_date.motor_speed_pid,M3508_out_max,M3508_iout_max);	
				 
				 for( i =0;i<4;i++ )//初始化底盘每个电机速度环的pid
				 {
					 
//					 PID_init(&chassis_init->motor_speed_pid[i],PID_POSITION,mid_date.motor_speed_pid,M3508_out_max,M3508_iout_max);
					 first_order_filter_init(&chassis_init->motor_speed_pid_slow[i],CHASSIS_CONTROL_TIME,chassis_filter);
				 }
				 PID_init(&chassis_init->chassis_angle_pid , PID_POSITION , mid_date.chassis_angle_pid1 , M3508_angle_out_max,M3508_angle_iout_max);//底盘跟随云台pid
				 PID_init(&chassis_init->chassis_speed_pid , PID_POSITION , mid_date.chassis_speed_pid1 , M3508_speed_out_max,M3508_speed_iout_max);//
//				 chassic_PID_init(&chassis_init->chassic_wz,M3508_speed_out_max,M3508_angle_iout_max,M3505_angle_P, M3505_angle_I, M3505_angle_D);//底盘跟随云台pid（未使用）
				 				 
				 PID_init(&chassis_init->chassis_mode_spin_wz,PID_POSITION,chassis_mode_spin_pid1,spin_speed_out_max,spin_speed_iout_max);  //自旋恒速pid
				 
					//一阶滤波初始化
				 first_order_filter_init(&chassis_init->chassis_cmd_slow_set_vx, CHASSIS_CONTROL_TIME, chassis_x_order_filter);
				 first_order_filter_init(&chassis_init->chassis_cmd_slow_set_vy, CHASSIS_CONTROL_TIME, chassis_y_order_filter);

}


fp32 speed_date[7][2]={{power_limit_X_5 ,power_limit_Y_5},
                       {power_limit_X_6 ,power_limit_Y_6},
											 {power_limit_X_7 ,power_limit_Y_7},
											 {power_limit_X_8 ,power_limit_Y_8},
											 {power_limit_X_10,power_limit_Y_10},
											 {power_limit_X_12,power_limit_Y_12},
                       {power_limit_X_CP,power_limit_Y_CP}
                     };
                // 50    60     70    80   100   120    CP
fp32 PIN_SPEED[]={2.0,  2.2 ,  2.3,  2.4,  3.3,  3.5 ,  4};
fp32 spin_cp=4.8;
fp32 spin_old ;			 
int  prower_bank;
int  date_middle;

//根据裁判系统返回的底盘功率上限，调整底盘不开电容的移动最大功率
fp32 *speed_update(void)
{
	

	if(chassic_ctrl.chassis_RC->key.v & SUPER_CAP )
	{
    date_middle=65535;
		chassic_ctrl.TOP_SPIN_SPEED=spin_cp;
	  
	}
	else
	{
 		prower_bank = *chassic_ctrl.power_limit;
		date_middle =prower_bank;

	}
	

	
	    switch(date_middle)
			{
				case 45:
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[0]*spin_old;			
					return speed_date[0];
						
					break;				
				}
				case 50:
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[1]*spin_old;;
					return speed_date[1];
					break;
				
				}
				
				case 55:
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[2]*spin_old;;
					return speed_date[2];
					break;
				
				}
				
				case 60:
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[3]*spin_old;;
					return speed_date[3];
					break;
				
				}
				case 80:
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[4]*spin_old;;
					return speed_date[4];
					break;
				
				}
				case 100:
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[5]*spin_old;;
					return speed_date[5];
					break;
				
				}
				case 65535:
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[6];
          return speed_date[6];
					break;
				
				}
				default :
				{
					chassic_ctrl.TOP_SPIN_SPEED=PIN_SPEED[0]*spin_old;;
				  return speed_date[0];
				}break;
				
			}

}

#define bilixishu 1000.0

//遥控器（处理后）值生成底盘速度
static void chassis_control_vector(chassis *chassis_move_to_vector)
{   	  
    if(chassis_move_to_vector->Sport_mode==remote_mode)
		{
					//max and min speed
				//最大 最小速度X_speed_update(void)
				chassis_move_to_vector->vx_max_speed =  speed_update()[0];
				chassis_move_to_vector->vx_min_speed = -speed_update()[0];

				chassis_move_to_vector->vy_max_speed =  speed_update()[1];
				chassis_move_to_vector->vy_min_speed = -speed_update()[1];
			
				if (chassis_move_to_vector == NULL )
				{
						return;
				}
				
				static double vector_acceleration_1 = 0.1;
				static double vector_acceleration_2 = 0.1;
				static double vector_acceleration_3 = 0.1;
				static double vector_acceleration_4 = 0.1;
				int16_t vx_channel, vy_channel;
				fp32 vx_set_channel, vy_set_channel;
				//deadline, because some remote control need be calibrated,  the value of rocker is not zero in middle place,
				//死区限制，因为遥控器可能存在差异 摇杆在中间，其值不为0
				rc_deadband_limit(chassis_move_to_vector->chassis_RC->rc.ch[CHASSIS_X_CHANNEL], vx_channel, CHASSIS_RC_DEADLINE);
				rc_deadband_limit(chassis_move_to_vector->chassis_RC->rc.ch[CHASSIS_Y_CHANNEL], vy_channel, CHASSIS_RC_DEADLINE);

				vx_set_channel = vx_channel *  CHASSIS_VX_RC_SEN;
				vy_set_channel = vy_channel * -CHASSIS_VY_RC_SEN;
				
				 //keyboard set speed set-point
				//键盘控制,更新为键盘矢量控制
				
				if(!KEY_IS_DOWN(rc_ctrl,SHIFT))	
			{
				if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_FRONT_KEY)
				{
					if(vector_acceleration_1 >= 1)
					{
						vector_acceleration_1 = 1;
					}
					else if(vector_acceleration_1 < 1)
					{
						vector_acceleration_1 = vector_acceleration_1 + CHASSISS_ACCELERATION;
					}
						vx_set_channel = chassis_move_to_vector->vx_max_speed * vector_acceleration_1;
				}
				else if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_BACK_KEY)
				{
					if(vector_acceleration_2 >= 1)
					{
						vector_acceleration_2 = 1;
					}
					else if(vector_acceleration_2 < 1)
					{
						vector_acceleration_2 = vector_acceleration_2 + CHASSISS_ACCELERATION;
					}
						vx_set_channel = chassis_move_to_vector->vx_min_speed * vector_acceleration_2;
				}

				if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_LEFT_KEY)
				{
					if(vector_acceleration_3 >= 1)
					{
						vector_acceleration_3 = 1;
					}
					else if(vector_acceleration_3 < 1)
					{
						vector_acceleration_3 = vector_acceleration_3 + CHASSISS_ACCELERATION;
					}
						vy_set_channel = chassis_move_to_vector->vy_max_speed * vector_acceleration_3;
				}
				else if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_RIGHT_KEY)
				{
					if(vector_acceleration_4 >= 1)
					{
						vector_acceleration_4 = 1;
					}
					else if(vector_acceleration_4 < 1)
					{
						vector_acceleration_4 = vector_acceleration_4 + CHASSISS_ACCELERATION;
					}
						vy_set_channel = chassis_move_to_vector->vy_min_speed * vector_acceleration_4;
				}
			}
				
				
				
					 if (chassis_move_to_vector->chassis_RC->key.v & TOP_LEFT_SPIN)
			{
				 if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_FRONT_KEY)
				{

						vx_set_channel = chassis_move_to_vector->vx_max_speed * 0.6;
				}
				else if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_BACK_KEY)
				{

						vx_set_channel = chassis_move_to_vector->vx_min_speed * 0.6;
				}

				if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_LEFT_KEY)
				{

						vy_set_channel = chassis_move_to_vector->vy_max_speed * 0.6;
				}
				else if (chassis_move_to_vector->chassis_RC->key.v & CHASSIS_RIGHT_KEY)
				{

						vy_set_channel = chassis_move_to_vector->vy_min_speed * 0.6;
				}

			}
				
				
				
				//first order low-pass replace ramp function, calculate chassis speed set-point to improve control performance
				//一阶低通滤波代替斜波作为底盘速度输入
				first_order_filter_cali(&chassis_move_to_vector->chassis_cmd_slow_set_vx, vx_set_channel);
				first_order_filter_cali(&chassis_move_to_vector->chassis_cmd_slow_set_vy, vy_set_channel);
				//stop command, need not slow change, set zero derectly
				//停止信号，不需要缓慢加速，直接减速到零
				if (vx_set_channel < CHASSIS_RC_DEADLINE * CHASSIS_VX_RC_SEN && vx_set_channel > -CHASSIS_RC_DEADLINE * CHASSIS_VX_RC_SEN)
				{
						vector_acceleration_1 = 0.2;
						vector_acceleration_2 = 0.2;
						chassis_move_to_vector->chassis_cmd_slow_set_vx.out = 0.0f;
					vx_set_channel = 0;
				}

				if (vy_set_channel < CHASSIS_RC_DEADLINE * CHASSIS_VY_RC_SEN && vy_set_channel > -CHASSIS_RC_DEADLINE * CHASSIS_VY_RC_SEN)
				{
						vector_acceleration_3 = 0.2;
						vector_acceleration_4 = 0.2;
						chassis_move_to_vector->chassis_cmd_slow_set_vy.out = 0.0f;
					vy_set_channel=0;
				}
				   chassis_move_to_vector->set_vx = chassis_move_to_vector->chassis_cmd_slow_set_vx.out;
           chassis_move_to_vector->set_vy = chassis_move_to_vector->chassis_cmd_slow_set_vy.out;
		
		}
		else if(chassis_move_to_vector->Sport_mode==auto_mod)
		{
			    chassis_move_to_vector->set_vx = auto_set_v[0];
          chassis_move_to_vector->set_vy = auto_set_v[1];
		}

}

float chassic_yaw_calc(void)//去零点
{
	float set;	
	set = chassic_ctrl.chassis_yaw_motor->absolute_angle_set;
	float aset = (set +3.14)/3.14*2 * 360;
	float aget = (chassic_ctrl.chassis_yaw+3.14) / 3.14*2 * 360;

	float res = 0.0f;
		if(aset >= aget) {
		if((aset - aget) > 180){	//aset = 350 aget = 10
			res = aset - 360 - aget;
		} else {					//aset = 20 aget = 10
			res = aset - aget;
		}
	} else {
		if((aset - aget) < -180){ 	//aset = 10 aget = 350
			res = 360 + aset - aget;
		} else {					//aset = 10 aget = 20
			res = aset - aget;
		}
	}
	
	return PID_calc(&chassic_ctrl.chassis_angle_pid,aget, aget + res);//角度环
}
	
//底盘跟随云台计算角速度函数
static void chassic_follow_gimbal(chassis *chassis_rc_vector)
{
	    if(chassis_rc_vector->Sport_mode==remote_mode)
		{
				PID_calc(&chassis_rc_vector->chassis_angle_pid,-chassis_rc_vector->chassis_yaw_motor->relative_angle,0);//底盘跟随云台
				PID_calc(&chassis_rc_vector->chassis_speed_pid,chassis_rc_vector->wz,chassis_rc_vector->chassis_angle_pid.out);//速度环
				chassis_rc_vector->set_wz =  chassis_rc_vector->chassis_speed_pid.out*PITCH_DIRECTION;
		}
		else if(chassis_rc_vector->Sport_mode==auto_mod)
		{
			  chassis_rc_vector->set_wz = auto_set_v[2];
		}

}

char diversion_key;
float diversion_key_count=0;
char diversion_flag=0;


uint32_t division_tick = 0;

static void chassis_vector_to_mecanum_wheel_speed(chassis *chassis_wheel_speed)
{


	if(KEY_IS_DOWN(rc_ctrl,Z)&& HAL_GetTick() - division_tick > 500)
	{	 
		 division_tick = HAL_GetTick();
	 	 chassis_wheel_speed->chassis_mode = chassic_zero;     //按键按下则停止地盘运动
		 diversion_flag =1;  //转向之后，底盘跟随标志位置1，关闭地盘跟随
		 gimbal_ctrl.gimbal_yaw_motor.absolute_angle_set = gimbal_ctrl.gimbal_yaw_motor.absolute_angle_set+pi;
	   if(gimbal_ctrl.gimbal_yaw_motor.absolute_angle_set>pi)
		 {
			 gimbal_ctrl.gimbal_yaw_motor.absolute_angle_set -=2*pi;
		 
		 }
		 
//	 gimbal_ctrl.gimbal_yaw_motor.absolute_angle_set= gimbal_ctrl.gimbal_yaw_motor.absolute_angle_set % pi;
	   diversion_key_count = !diversion_key_count;
	}


	if(diversion_key_count==0)
	{
		yaw_offset_ecd  =  yaw_ecd;
		
		//麦轮,这里的set_v为遥控器比例化后的数据（v/s）所以算出来的也是v/s（轮子的线速度）
	  chassis_wheel_speed->wheel_speed[0] =  chassis_wheel_speed->set_vx  - chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;
    chassis_wheel_speed->wheel_speed[1] =  chassis_wheel_speed->set_vx  + chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;
    chassis_wheel_speed->wheel_speed[2] =  -chassis_wheel_speed->set_vx + chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;
    chassis_wheel_speed->wheel_speed[3] =  -chassis_wheel_speed->set_vx - chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;

	
	}
	else
	{
		
 		yaw_offset_ecd  =  yaw_back_ecd;
		chassis_wheel_speed->wheel_speed[0] = chassis_wheel_speed->set_vx  + chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;
    chassis_wheel_speed->wheel_speed[1] = -chassis_wheel_speed->set_vx + chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;
    chassis_wheel_speed->wheel_speed[2] = -chassis_wheel_speed->set_vx - chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;
    chassis_wheel_speed->wheel_speed[3] = chassis_wheel_speed->set_vx  - chassis_wheel_speed->set_vy + ( - 1.0f) * MOTOR_DISTANCE_TO_CENTER * chassis_wheel_speed->set_wz;
		
	}
	diversion_key = 0;
}

static void chassis_feedback_update(chassis *chassis_date_update)
{
	if (chassis_date_update == NULL)
    {
        return;
    }
	
	uint8_t i;
	for(i=0;i<4;i++)
	{
		//计算获得的电机速度并存入（这里输入的是轮子电机转子的rpm（转/分钟），需要转化为点击的线速度）
		chassis_date_update->chassis_motor[i].speed = CHASSIS_MOTOR_RPM_TO_VECTOR_SEN*chassis_date_update->chassis_motor[i].chassis_motor_measure->speed_rpm;
		
	}
	
	//空转下，由轮子速度计算车世界坐标系下的速度
	chassis_date_update->vx = ( chassis_date_update->chassis_motor[0].speed - chassis_date_update->chassis_motor[3].speed)/2;
	chassis_date_update->vy = ( -chassis_date_update->chassis_motor[0].speed + chassis_date_update->chassis_motor[1].speed)/2;
	chassis_date_update->wz = -( chassis_date_update->chassis_motor[0].speed + chassis_date_update->chassis_motor[1].speed + chassis_date_update->chassis_motor[2].speed + chassis_date_update->chassis_motor[3].speed)/4;
	
	//计算车世界坐标系下的yaw轴角度
	chassis_date_update->chassis_yaw = rad_format(*(chassis_date_update->chassis_INS_angle+0)-chassis_date_update->chassis_yaw_motor->relative_angle);
	
	
}

static void chassic_mode_set(chassis *mode)
{

  if(switch_is_down(mode->chassis_RC->rc.s[s_right]))//右下则全停机
	{
		mode->chassis_mode = chassic_zero;
		gimbal_ctrl.gimbal_yaw_motor.gimbal_motor_mode = GIMBAL_ZERO;
	}

  else if(switch_is_mid(mode->chassis_RC->rc.s[s_right]) ||(mode->chassis_RC->key.v& KEY_PRESSED_OFFSET_CTRL))//右中只开云台
	{     
		 gimbal_ctrl.gimbal_yaw_motor.gimbal_motor_mode = GIMBAL_MOTOR_GYRO;
	}
	
	else if(switch_is_up(mode->chassis_RC->rc.s[s_right])) //右上全开
	{		
		 gimbal_ctrl.gimbal_yaw_motor.gimbal_motor_mode = GIMBAL_MOTOR_GYRO;     
		       		
	}
//	else  if(mode->chassis_RC->key.v& KEY_PRESSED_OFFSET_CTRL)  //
//	{
//		mode->chassis_mode = chassic_zero;
//    gimbal_ctrl.gimbal_yaw_motor.gimbal_motor_mode = GIMBAL_MOTOR_ENCONDE;  
//	}

  if(/*switch_is_mid(mode->chassis_RC->rc.s[s_left]) ||*/ gimbal_ctrl.gimbal_rc->mouse.press_r == 1)  //左上视觉模式
	{
	 	gimbal_ctrl.gimbal_yaw_motor.gimbal_motor_mode = GIMBAL_MOTOR_VISUAL;
        gimbal_ctrl.gimbal_pitch_motor.gimbal_motor_mode = GIMBAL_MOTOR_VISUAL;
	}
	if((mode->chassis_RC->key.v& KEY_PRESSED_OFFSET_SHIFT || switch_is_up(mode->chassis_RC->rc.s[s_left])))  //shift键为小陀螺
	{
	  mode->chassis_mode = chassic_follow_gim_yaw;
	}

	
	if(KEY_IS_DOWN(rc_ctrl,B) && HAL_GetTick() - color_tick > 500)   //判断按键按下
	{
			color_tick = HAL_GetTick();
		  
		  robot_color_hero = !robot_color_hero;
	}
	
	
	
}

pid_type_def spin_pid;

static void chassic_mode_ctrl(chassis *mode_ctrl)
{

		 if(mode_ctrl->chassis_mode == chassic_follow_chassic_yaw)   //底盘跟随云台
		{
						
			chassic_follow_gimbal(mode_ctrl); //底盘跟随云台计算角速度函数
			
			chassis_control_vector(mode_ctrl);  //遥控器（处理后）值生成底盘速度
					
		}
		else if(mode_ctrl->chassis_mode == chassic_follow_gim_yaw)   //小陀螺
		{
			const static fp32 wz_pid[3] = {4, 0, 0.2};  //云台移动底盘不变速

        PID_init(&spin_pid,PID_POSITION,wz_pid,100,50);

        static fp32 delta = 0.0f;
        static fp32 angle_detla = 0.0f;
        static fp32 absolute_wz =0.0f;
				float vx,vy,wz;
        chassis_control_vector(mode_ctrl);//生成底盘速度
        delta = - mode_ctrl->chassis_yaw_motor->relative_angle * PITCH_DIRECTION ;
				//new
				float sin_delta = arm_sin_f32(delta);
				float cos_delta = arm_cos_f32(delta);
				vx = mode_ctrl->set_vx;
				vy = mode_ctrl->set_vy;
				mode_ctrl->set_vx = vx*cos_delta + vy*sin_delta;
        mode_ctrl->set_vy = -vx*sin_delta + vy*cos_delta;
        mode_ctrl->set_wz = 4.0;

		}
		else if(mode_ctrl->chassis_mode == chassic_follow_gim_yaw_fanxiang)   //小陀螺
		{
			const static fp32 wz_pid[3] = {4, 0, 0.2};  //云台移动底盘不变速

        PID_init(&spin_pid,PID_POSITION,wz_pid,100,50);

        static fp32 delta = 0.0f;
        static fp32 angle_detla = 0.0f;
        static fp32 absolute_wz =0.0f;
				float vx,vy,wz;
        chassis_control_vector(mode_ctrl);//生成底盘速度
        delta = - mode_ctrl->chassis_yaw_motor->relative_angle * PITCH_DIRECTION ;
				//new
				float sin_delta = arm_sin_f32(delta);
				float cos_delta = arm_cos_f32(delta);
				vx = mode_ctrl->set_vx;
				vy = mode_ctrl->set_vy;
				mode_ctrl->set_vx = vx*cos_delta + vy*sin_delta;
        mode_ctrl->set_vy = -vx*sin_delta + vy*cos_delta;
        mode_ctrl->set_wz = -4.0;

		}
		else if(mode_ctrl->chassis_mode == chassic_zero)
		{
			mode_ctrl->set_vx = 0;
			mode_ctrl->set_vy = 0;
			mode_ctrl->set_wz = 0;

		}
	
}

fp32 Chassis_pidout;
fp32 Scaling1,Scaling2,Scaling3,Scaling4;
fp32 Klimit=1,Plimit=1;
fp32 out1,out2,out3,out4;

static fp32 limit_Data(fp32 val, fp32 low, fp32 high)
{
		if(val<low)  val = low;
			else     val = val;
		if(val>high) val = high;
	        else     val = val;
        return val;
}

fp32 afa,kjjahlgjhjlahg;

void Chassis_Power_Limit(chassis *prowlimit)
{	

		Chassis_pidout=(
						fabs(prowlimit->wheel_speed[0]-prowlimit->chassis_motor[0].speed)+
						fabs(prowlimit->wheel_speed[1]-prowlimit->chassis_motor[1].speed)+
						fabs(prowlimit->wheel_speed[2]-prowlimit->chassis_motor[2].speed)+
						fabs(prowlimit->wheel_speed[3]-prowlimit->chassis_motor[3].speed));

		Scaling1=(prowlimit->wheel_speed[0]-prowlimit->chassis_motor[0].speed)/Chassis_pidout;	
		Scaling2=(prowlimit->wheel_speed[1]-prowlimit->chassis_motor[1].speed)/Chassis_pidout;
		Scaling3=(prowlimit->wheel_speed[2]-prowlimit->chassis_motor[2].speed)/Chassis_pidout;	
		Scaling4=(prowlimit->wheel_speed[3]-prowlimit->chassis_motor[3].speed)/Chassis_pidout;
		Klimit=Chassis_pidout;
		Klimit=limit_Data(Klimit,-10,10);

fp32 Chassis_pidout_max=2.5;//32768，40，960

kjjahlgjhjlahg=(fabs(prowlimit->chassis_motor[0].speed)+fabs(prowlimit->chassis_motor[1].speed)
              +fabs(prowlimit->chassis_motor[2].speed)+fabs(prowlimit->chassis_motor[3].speed))/8.0;
//      afa=1/limit_Data(kjjahlgjhjlahg,-1,10);
//    afa=1/((prowlimit->set_vx+prowlimit->set_vy+prowlimit->set_wz)/3.0);
    if(kjjahlgjhjlahg>0&&kjjahlgjhjlahg<=0.22)
			afa=1;
		else if(kjjahlgjhjlahg>0.22&&kjjahlgjhjlahg<=0.6)
//			afa=0.2;
    afa=-(2.0*(kjjahlgjhjlahg))*(2.0*(kjjahlgjhjlahg))+1.0;
		afa=limit_Data(afa,0,1);

		
		

}



