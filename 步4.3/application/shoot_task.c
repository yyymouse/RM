#include "shoot_task.h"
#include "FreeRTOS.h"
#include "MOTOR_ANGLE_PID.h"
#include "CAN_receive.h"
#include "referee.h"
#include "tim.h"
#include "user_lib.h"
#include "gimbal_task.h"

uint32_t speed_tick = 0;
uint32_t mag_tick = 0;
uint8_t  mag_flag = 0 ;
shoot_all shoot;

int shoot_speed[3][2] = {
                         {motor1_speed1,motor2_speed1},
												 {motor1_speed2,motor2_speed2},
												 {motor1_speed3,motor2_speed3}
                        };

/**
  * @brief         ״̬��ģʽ����          
  * @param[in]     null
	* @author        ���� 
  * @retval        null 
  */
void shoot_mode_set(void);

/**
  * @brief         ״̬����Ϊִ��                
  * @param[in]     null 
	* @author        ���� 
  * @retval        null 
  */
void shoot_mode_behavior(void);

/**
  * @brief         ������������          
  * @param[in]     GPIOx
  * @param[in]     GPIO_Pin����Ӧ����	
	* @author        ���� 
  * @retval        ������ֵ��1Ϊ�������£�0Ϊû�а��� 
  */
int key_detect(shoot_all *key_det);


/**
  * @brief         shoot�����ʼ������          
  * @param[in]     null 
	* @author        ���� 
  * @retval        null 
  */
void shoot_init(void);


/**
  * @brief         shoot������Ⱥ���          
  * @param[in]     pvParameters 
	* @author        ���� 
  * @retval        null 
  */
void shoot_task(void const * pvParameters)
{
	
	shoot_init();
	
	shoot.xLastWakeTime = xTaskGetTickCount();
	
 
  for(;;)
 {
	  shoot_mode_set();
	 
	  shoot_mode_behavior();
    osDelay(1);
		
  }

}

void shoot_init()
{
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    
    const static fp32 SHOOT2_filter[1] = {0.0005};    //�ٶ����õ�һ���˲�����
    const static fp32 SHOOT1_filter[1] = {0.0005};  
	  
		ramp_init(&shoot.shoot1_filter, 0.001f , 4700,2000);
		
		
//	  first_order_filter_init(&shoot.shoot2_filter, 0.001, SHOOT2_filter);  
//	  first_order_filter_init(&shoot.shoot1_filter, 0.001, SHOOT1_filter);  
	  shoot.shoot_speed_limit=&ext_game_robot_status.shooter_id1_17mm_speed_limit;         //�����ٶ�����
	  shoot.shoot_cooling_rate=&ext_game_robot_status.shooter_id1_17mm_cooling_rate;    //ǹ��ÿ����ȴֵ
	  shoot.shoot_cooling_limit=&ext_game_robot_status.shooter_id1_17mm_cooling_limit;   //ǹ����������
	  shoot.cooling_heat=&ext_power_heat_data.shooter_id1_17mm_cooling_heat;       //ǹ������
	
	  shoot.motor_data1=get_trigger2_motor_measure_point();       //�������
	  shoot.motor1_data=get_shoot_motor_measure_point(7);         //Ħ����1
	  shoot.motor2_data=get_shoot_motor_measure_point(8);         //Ħ����2
	
	  shoot.shoot_RC=get_remote_control_point(); //��ȡң����ָ��
	  const fp32 PID_angle[3]={Trigger_angle_PID_P,Trigger_angle_PID_I,Trigger_angle_PID_D};	//P,I,D    �ǶȻ�
    const fp32 PID_speed[3]={Trigger_speed_PID_P,Trigger_speed_PID_I,Trigger_speed_PID_D};	//P,I,D    �ٶȻ�
		
		const fp32 PID_motor1[3]={motor1_PID_P,motor1_PID_I,motor1_PID_D};	//P,I,D  
		const fp32 PID_motor2[3]={motor2_PID_P,motor2_PID_I,motor2_PID_D};	//P,I,D
		
		PID_init(&shoot.MOTOR_ANGLE1,PID_POSITION,PID_angle,16000,11000);
		PID_init(&shoot.MOTOR_SPEED1,PID_POSITION,PID_speed,16000,11000);
		
		PID_init(&shoot.motor1,PID_POSITION,PID_motor1,7000,700);
		PID_init(&shoot.motor2,PID_POSITION,PID_motor2,7000,700);
		
		shoot.set_angle = 0;
		shoot.KEY_outcome=0;
		shoot.key_run_num=0;
		shoot.Init_count =0;
    
		shoot.tigger_mode = tigger_over ;
    shoot.shoot_mode = shoot_down;           //ֹͣ����
    shoot.speed_level = 0 ;                 //��ʼ��Ϊ��һ���ٶȵȼ�
}	


/**
  * @brief         ������������          
  * @param[in]     GPIOx
  * @param[in]     GPIO_Pin����Ӧ����	
	* @author        ���� 
  * @retval        ������ֵ��1Ϊ�������£�0Ϊû�а��� 
  */
int key_detect(shoot_all *key_det)
{	
	if( key_det->shoot_RC->rc.ch[4] >= KEY_input || key_det->shoot_RC->mouse.press_l == 1)		
	{
		
		shoot.key_run_num ++;	
		
			if( shoot.key_run_num >= shooting_frequency)
		{

			shoot.key_run_num = 0;
			
			return 1;
			
		}
	
	} 
	else
	{
		
		shoot.key_run_num=0;
		
		return 0;

	}
	return 0;
			
}

int iii;

void shoot_mode_set(void)
{ 
  shoot.KEY_outcome = key_detect(&shoot);	
	if(shoot.shoot_RC->rc.s[s_left]== 2 ||(shoot.shoot_RC->rc.ch[4]<660 && shoot.shoot_RC->mouse.press_l == 0))   //������Ϊ���»��߰���û�а���ʱ��ֹͣ����
	{
		
	  shoot.shoot_mode =shoot_down;
		
	}
	else   //����󲦸˵��м����Ħ���ֺͲ�����
	{		
			shoot.shoot_mode =shoot_start;
	}
	if(fabs(shoot.set_angle-ABS_ANGLE)>(division_angle*3) && (shoot.shoot_RC->rc.ch[4]<660|| shoot.shoot_RC->mouse.press_l == 0) )  
	{
		
		shoot.tigger_blocked_count++;
		
	}
	
	if(shoot.tigger_blocked_count>tigger_blocked_count_judgment)  //�����ڽǶȴ���һ��ʱ�䣬���ж�Ϊ��ת
	{
		
		shoot.tigger_mode = tigger_blocked;
	
	}
	else 
	{
		shoot.tigger_mode = tigger_playing;
	
	}
}
void shoot_mode_behavior(void)
{	

	if(shoot.shoot_mode == shoot_down)
	{
		shoot.set_angle = ABS_ANGLE;
		shoot.MOTOR_SPEED1.out = 0;		
	}//���ģʽΪֹͣ�������ֵΪ0
	else if(shoot.shoot_mode == shoot_start)
	{		
//		�ڶ�ת֮�����ڶ�ת�ж�ʱ����̣����²��˼����������������Ӳ��̽Ƕ�Ŀ��ֵ
	if(gimbal_ctrl.gimbal_yaw_motor.gimbal_motor_mode == GIMBAL_MOTOR_VISUAL)
	{
		
   if(((shoot.KEY_outcome==1||(shoot.shoot_RC->rc.ch[4]<660))&&(shoot.tigger_mode == tigger_playing)) && gimbal_ctrl.flag_shoot ==1)
		{				
			shoot.set_angle=shoot.set_angle + tigger_direction*division_angle;       //ÿ�β�����ת�ĽǶȡ�����
			
		}
		else 
		{
		shoot.MOTOR_SPEED1.out = 0;
		shoot.set_angle = ABS_ANGLE;
		}

	}
	else
	{
			if(shoot.KEY_outcome==1&&shoot.tigger_mode == tigger_playing)
		{				
			shoot.set_angle=shoot.set_angle + tigger_direction*division_angle;       //ÿ�β�����ת�ĽǶȡ�����
		}
		if(shoot.tigger_mode == tigger_blocked)
			{
			 shoot.set_angle = ABS_ANGLE;
			
			}
	}

		
		PID_calc(&shoot.MOTOR_ANGLE1,ABS_ANGLE, shoot.set_angle);//�ǶȻ�
		
		PID_calc(&shoot.MOTOR_SPEED1, shoot.motor_data1->speed_rpm, shoot.MOTOR_ANGLE1.out);//�ٶȻ�

  	}
	
		if(shoot.shoot_RC->rc.s[s_left]==1 || shoot.shoot_RC->rc.s[s_left]==3)
		{ 
	     PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm, -shoot_speed[shoot.speed_level][0]);//�ٶȻ�
	     PID_calc(&shoot.motor2, shoot.motor2_data->speed_rpm,  shoot_speed[shoot.speed_level][1]);//�ٶȄ1�7
		}
		else 
		{
	     shoot.motor1.out = 0 ;
	     shoot.motor2.out = 0 ;
		   shoot.MOTOR_SPEED1.out = 0;
		}
		
		CAN_cmd_shoot(0,shoot.MOTOR_SPEED1.out , shoot.motor1.out , shoot.motor2.out);	//���Ϳ��Ƶ���

}


