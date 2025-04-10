#include "shoot_task.h"
#include "FreeRTOS.h"
#include "MOTOR_ANGLE_PID.h"
#include "CAN_receive.h"
#include "referee.h"
#include "tim.h"
#include "user_lib.h"
#include "usb_task.h"
#include "bsp_usb.h"

uint32_t speed_tick = 0;
uint32_t mag_tick = 0;    //?
uint8_t  mag_flag = 0 ;
shoot_all shoot;
uint32_t time_3508;
uint32_t time_2006;   
uint8_t dart_num =0;
uint8_t dart_num1 = 0;
uint8_t dart_num2 = 0;
uint8_t dart_num3 = 0;
uint8_t dart_num4 = 0;
uint8_t dart_num5 = 0;
uint8_t cnt_shoot =0;
int16_t rc;
float motor_speed_left = 0; 							//链轮转速
float motor_speed_right = 0;
int shoot_flag=0;
int shoot_behavior =0;
int position_flag =0;
int shoot_num =0;
int shoot_finish =0;

void shoot_mode();
/**
  * @brief         状态机模式设置          
  * @param[in]     null
	* @author        刘根 
  * @retval        null 
  */
void shoot_mode_set(void);

/**
  * @brief         状态机行为执行                
  * @param[in]     null 
	* @author        刘根 
  * @retval        null 
  */
void shoot_mode_behavior(void);

/**
  * @brief         按键消抖函数          
  * @param[in]     GPIOx
  * @param[in]     GPIO_Pin：对应引脚	
	* @author        刘根 
  * @retval        消抖后值，1为按键按下，0为没有按下 
  */
int key_detect(shoot_all *key_det);


/**
  * @brief         shoot任务初始化函数          
  * @param[in]     null 
	* @author        刘根 
  * @retval        null 
  */
void shoot_init(void);
void shoot_zero1();
void shoot_1();
void shoot_behave();
void shoot_2();
void shoot_loop();
double total_angle =105000.0;
void pid_math_speed();
void pid_math_double();

/**
  * @brief          斜波函数初始化
  * @author         RM
  * @param[in]      斜波函数结构体
  * @param[in]      间隔的时间，单位 s
  * @param[in]      最大值
  * @param[in]      最小值
  * @retval         返回空
  */
void ramp_init(ramp_function_source_t *ramp_source_type, fp32 frame_period, fp32 max, fp32 min)
{
    ramp_source_type->frame_period = frame_period;
    ramp_source_type->max_value = max;
    ramp_source_type->min_value = min;
    ramp_source_type->input = 0.0f;
    ramp_source_type->out = 0.0f;
}

/**
  * @brief          斜波函数计算，根据输入的值进行叠加， 输入单位为 /s 即一秒后增加输入的值
  * @author         RM
  * @param[in]      斜波函数结构体
  * @param[in]      输入值
  * @param[in]      滤波参数
  * @retval         返回空
  */
void ramp_calc(ramp_function_source_t *ramp_source_type, fp32 input)
{
    ramp_source_type->input = input;
    ramp_source_type->out += ramp_source_type->input * ramp_source_type->frame_period;
    if (ramp_source_type->out > ramp_source_type->max_value)
    {
        ramp_source_type->out = ramp_source_type->max_value;
    }
    else if (ramp_source_type->out < ramp_source_type->min_value)
    {
        ramp_source_type->out = ramp_source_type->min_value;
    }
}


/**
  * @brief          一阶低通滤波初始化
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @param[in]      滤波参数
  * @retval         返回空
  */
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period, const fp32 num[1])
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num[0] = num[0];
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
}

/**
  * @brief          一阶低通滤波计算
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @retval         返回空
  */
void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->out =
        first_order_filter_type->num[0] / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->out + first_order_filter_type->frame_period / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->input;
}



void push()//推出牵引
{ 
    motor_speed_left = motor_speed;
    motor_speed_right=-motor_speed;
	  pid_math_speed();
}

void pull()//拉回牵引
{
		motor_speed_left =-motor_speed;
    motor_speed_right= motor_speed;
	  pid_math_speed();
}

void motor_stay()//待机模式
{
		motor_speed_left = 0;
    motor_speed_right= 0;
	  pid_math_speed();
}
void gimbal_deadarea()//云台死区限制
{
	if(rc_ctrl.rc.ch[0]>deadarea||rc_ctrl.rc.ch[0]<-deadarea)
	rc =rc_ctrl.rc.ch[0];//遥控器死区
	else
	rc =0;
}

/**
  * @brief         shoot任务调度函数          
  * @param[in]     pvParameters 
	* @author        刘根 
  * @retval        null 
  */
uint8_t status =0;

/****发射任务函数****/
void shoot_task(void const * pvParameters)
{		int time1 =0 ;
	  time1 =HAL_GetTick();
	  shoot_init();
		
		//shoot.shoot_motor1_angle = 0;
		//shoot.shoot_motor2_angle = 0;
		for(;;)
		{
			shoot.shoot_motor1_angle == shoot.motor1_data->ecd+length;
//			if((HAL_GetTick()-time1)>5000)
//			{
//			motor_speed_left  = 0;
//      motor_speed_right = 0;
//			}
//			else
//			{
//			motor_speed_left  = 0;
//      motor_speed_right = 0;
//      motor_speed_left = motor_speed;
//      motor_speed_right=-motor_speed;	
//      pid_math_double();				
//			}
//CAN_cmd_shoot(0,0,shoot.motor1.out ,shoot.motor2.out);
//			shoot_mode();
//			shoot_loop();
			osDelay(1);
		}	

}
			
			
			

/****发射使能函数****/
void shoot_init()
{
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    
    const static fp32 SHOOT2_filter[1] = {0.0005};    //速度设置的一阶滤波参数
    const static fp32 SHOOT1_filter[1] = {0.0005};  
	  
		ramp_init(&shoot.shoot1_filter, 0.001f , 4700,2000);
	  shoot.motor_data1=get_trigger2_motor_measure_point();       //拨弹电机      //?
	  shoot.motor1_data=get_shoot_motor_measure_point(7);         //摩擦轮1
	  shoot.motor2_data=get_shoot_motor_measure_point(8);         //摩擦轮2
	
	  shoot.shoot_RC=get_remote_control_point(); //获取遥控器指针
	  const fp32 PID_angle[3]={Trigger_angle_PID_P,Trigger_angle_PID_I,Trigger_angle_PID_D};	//P,I,D    角度环
    const fp32 PID_speed[3]={Trigger_speed_PID_P,Trigger_speed_PID_I,Trigger_speed_PID_D};	//P,I,D    速度环
		
		
		const fp32 PID_motor1_angle[3]={motor1_angle_P,motor1_angle_I,motor1_angle_D};	//P,I,D	
		const fp32 PID_motor1[3]={motor1_PID_P,motor1_PID_I,motor1_PID_D};	//P,I,D
		const fp32 PID_motor2_angle[3]={motor2_angle_P,motor2_angle_I,motor2_angle_D};
		const fp32 PID_motor2[3]={motor2_PID_P,motor2_PID_I,motor2_PID_D};	//P,I,D
		
		PID_init(&shoot.MOTOR_ANGLE1,PID_POSITION,PID_angle,16000,11000);
		PID_init(&shoot.MOTOR_SPEED1,PID_POSITION,PID_speed,16000,11000);
		
		PID_init(&shoot.motor1_angle,PID_POSITION,PID_motor1_angle,7000,700);
		PID_init(&shoot.motor1,PID_POSITION,PID_motor1,7000,700);
		PID_init(&shoot.motor2_angle,PID_POSITION,PID_motor2_angle,7000,700);
		PID_init(&shoot.motor2,PID_POSITION,PID_motor2,7000,700);
		shoot.shoot_motor1_angle = shoot.motor1_data->ecd;
		shoot.shoot_motor2_angle = shoot.motor2_data->ecd;
		
}	

void shoot_loop()
{
	if(shoot.shoot_mode == shoot_start)
				{
						if(shoot.shoot_mode == shoot_one&&shoot_finish==0)
						{

									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 0&&position_flag == 0)//扳机微动触发
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,100);//舵机锁定
										pull();
										if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 1)
										{
											
											position_flag = 1;
										}
											vTaskDelay(1);
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 0&&position_flag == 1)
									{
										push();
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 1&&position_flag == 1)//推送板到达指定位置
									{
										motor_stay();
										shoot_num=1;
									}
									if(shoot_num ==1)
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,10000);//解锁
										shoot_finish=1;//发射结束状态置1
										position_flag=0;
									}
							}
/****双发模式****/							
							if(shoot.shoot_mode == shoot_double&&shoot_finish==0)
							{
										if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 0&&position_flag == 0)//扳机微动触发
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,100);//舵机锁定
										pull();
										if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 1)
										{
											
											position_flag = 1;
										}
											vTaskDelay(1);
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 0&&position_flag == 1)
									{
										push();
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 1&&position_flag == 1)//推送板到达指定位置
									{
										motor_stay();
										shoot_num=1;
									}
									if(shoot_num ==1)
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,10000);//解锁
										shoot_num =0;//发射结束状态置1
									}
								
							}
					}
				else
				{
				shoot.motor1.out =0 ; 
				shoot.motor2.out =0 ;
				}

/****can发送函数****/
	CAN_cmd_shoot(0,0,shoot.motor1.out,shoot.motor2.out);
}

/****发射状态函数****/
void shoot_mode()
{
	if(rc_ctrl.rc.s[0]==(2||1))//左下停止
	{
	shoot.shoot_mode=shoot_zero;
	shoot_finish=0;
	}
  else if(rc_ctrl.rc.s[0]==3)//左中开发射
	{
		shoot.shoot_mode=shoot_start;
	if(shoot.shoot_mode==shoot_start&&rc_ctrl.rc.s[1]==3)//单发模式
	{
	 shoot.shoot_mode=shoot_one;
	}
	else if(shoot.shoot_mode==shoot_start&&rc_ctrl.rc.s[1]==1)//双发模式
	{
		shoot.shoot_mode=shoot_double;
	}
	}
}

void shoot_math()
{
	
}
void pid_math_double()//双环解算 
{
PID_calc(&shoot.motor1_angle, shoot.motor1_data->ecd,shoot.motor1_angle_out);
PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm, shoot.motor1_angle.out);
	
PID_calc(&shoot.motor2_angle, shoot.motor2_data->ecd,shoot.motor2_angle_out);
PID_calc(&shoot.motor2, shoot.motor1_data->speed_rpm,shoot.motor2_angle.out);

}
void pid_math_speed()//单环解算
{
//PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm, shoot.motor1_angle.out);
//PID_calc(&shoot.motor2, shoot.motor1_data->speed_rpm,shoot.motor2_angle.out);
PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm,motor_speed_left);
PID_calc(&shoot.motor2, shoot.motor1_data->speed_rpm,motor_speed_right);
}


