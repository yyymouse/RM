#include "shoot_task.h"
#include "FreeRTOS.h"
#include "MOTOR_ANGLE_PID.h"
#include "CAN_receive.h"
#include "referee.h"
#include "tim.h"
#include "user_lib.h"

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
  * @brief         ×´Ì¬»úÄ£Ê½ÉèÖÃ          
  * @param[in]     null
	* @author        Áõ¸ù 
  * @retval        null 
  */
void shoot_mode_set(void);

/**
  * @brief         ×´Ì¬»úÐÐÎªÖ´ÐÐ                
  * @param[in]     null 
	* @author        Áõ¸ù 
  * @retval        null 
  */
void shoot_mode_behavior(void);

/**
  * @brief         °´¼üÏû¶¶º¯Êý          
  * @param[in]     GPIOx
  * @param[in]     GPIO_Pin£º¶ÔÓ¦Òý½Å	
	* @author        Áõ¸ù 
  * @retval        Ïû¶¶ºóÖµ£¬1Îª°´¼ü°´ÏÂ£¬0ÎªÃ»ÓÐ°´ÏÂ 
  */
int key_detect(shoot_all *key_det);


/**
  * @brief         shootÈÎÎñ³õÊ¼»¯º¯Êý          
  * @param[in]     null 
	* @author        Áõ¸ù 
  * @retval        null 
  */
void shoot_init(void);


/**
  * @brief         shootÈÎÎñµ÷¶Èº¯Êý          
  * @param[in]     pvParameters 
	* @author        Áõ¸ù 
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
    
    const static fp32 SHOOT2_filter[1] = {0.0005};    //ËÙ¶ÈÉèÖÃµÄÒ»½×ÂË²¨²ÎÊý
    const static fp32 SHOOT1_filter[1] = {0.0005};  
	  
		ramp_init(&shoot.shoot1_filter, 0.001f , 4700,2000);
		
		
//	  first_order_filter_init(&shoot.shoot2_filter, 0.001, SHOOT2_filter);  
//	  first_order_filter_init(&shoot.shoot1_filter, 0.001, SHOOT1_filter);  
	  shoot.shoot_speed_limit=&ext_game_robot_status.shooter_id1_17mm_speed_limit;         //·¢ÉäËÙ¶ÈÉÏÏÞ
	  shoot.shoot_cooling_rate=&ext_game_robot_status.shooter_id1_17mm_cooling_rate;    //Ç¹¿ÚÃ¿ÃëÀäÈ´Öµ
	  shoot.shoot_cooling_limit=&ext_game_robot_status.shooter_id1_17mm_cooling_limit;   //Ç¹¿ÚÈÈÁ¿ÉÏÏÞ
	  shoot.cooling_heat=&ext_power_heat_data.shooter_id1_17mm_cooling_heat;       //Ç¹¿ÚÈÈÁ¿
	
	  shoot.motor_data1=get_trigger2_motor_measure_point();       //²¦µ¯µç»ú
	  shoot.motor1_data=get_shoot_motor_measure_point(7);         //Ä¦²ÁÂÖ1
	  shoot.motor2_data=get_shoot_motor_measure_point(8);         //Ä¦²ÁÂÖ2
	
	  shoot.shoot_RC=get_remote_control_point(); //»ñÈ¡Ò£¿ØÆ÷Ö¸Õë
	  const fp32 PID_angle[3]={Trigger_angle_PID_P,Trigger_angle_PID_I,Trigger_angle_PID_D};	//P,I,D    ½Ç¶È»·
    const fp32 PID_speed[3]={Trigger_speed_PID_P,Trigger_speed_PID_I,Trigger_speed_PID_D};	//P,I,D    ËÙ¶È»·
		
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
    shoot.shoot_mode = shoot_down;           //Í£Ö¹²¦µ¯
    shoot.speed_level = 0 ;                 //³õÊ¼»¯ÎªµÚÒ»µµËÙ¶ÈµÈ¼¶
}	


/**
  * @brief         °´¼üÏû¶¶º¯Êý          
  * @param[in]     GPIOx
  * @param[in]     GPIO_Pin£º¶ÔÓ¦Òý½Å	
	* @author        Áõ¸ù 
  * @retval        Ïû¶¶ºóÖµ£¬1Îª°´¼ü°´ÏÂ£¬0ÎªÃ»ÓÐ°´ÏÂ 
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
//	if(shoot.shoot_RC->rc.s[s_left]== 2|| (shoot.shoot_RC->rc.ch[4]<660 && shoot.shoot_RC->mouse.press_l == 0)||(*shoot.cooling_heat>=(*shoot.shoot_cooling_limit-cooling_heat_spend)))   //µ±²¦¸ËÎª×îÏÂ»òÕß°´¼üÃ»ÓÐ°´ÏÂÊ±£¬Í£Ö¹²¦µ¯

	if(shoot.shoot_RC->rc.s[s_left]== 2|| (shoot.shoot_RC->rc.ch[4]<660 && shoot.shoot_RC->mouse.press_l == 0))   //µ±²¦¸ËÎª×îÏÂ»òÕß°´¼üÃ»ÓÐ°´ÏÂÊ±£¬Í£Ö¹²¦µ¯
	{
		
	  shoot.shoot_mode =shoot_down;
		
	}
	else   //Èç¹û×ó²¦¸Ëµ½ÖÐ¼äÔò´ò¿ªÄ¦²ÁÂÖºÍ²¦µ¯ÅÌ
	{
		
		shoot.shoot_mode =shoot_start;
	
	}
	if(fabs(shoot.set_angle-ABS_ANGLE)>(division_angle*3) && (shoot.shoot_RC->rc.ch[4]<660|| shoot.shoot_RC->mouse.press_l == 0) )  
	{
		
		shoot.tigger_blocked_count++;
		
	}
	
	if(shoot.tigger_blocked_count>tigger_blocked_count_judgment)  //µ±´óÓÚ½Ç¶È´óÓÚÒ»¶¨Ê±¼ä£¬ÔòÅÐ¶ÏÎª¶Â×ª
	{
		
		shoot.tigger_mode = tigger_blocked;
	
	}
	else 
	{
		shoot.tigger_mode = tigger_playing;
	
	}
	
	if(KEY_IS_DOWN(rc_ctrl,B) && HAL_GetTick() - speed_tick > 500)   //ÅÐ¶Ï°´¼ü°´ÏÂ
	{
		speed_tick = HAL_GetTick();
	  /*iii++;                  //Ïû¶¶¼ÆÊý
		if(iii>10)             //Èç¹û¼ÆÊý´óÓÚ100 ÔòÉäËÙµÈ¼¶Éý¼¶1
		{
			shoot.speed_level++;
			if(shoot.speed_level>=level_max)
			{
				shoot.speed_level=0;
		    iii=0;       
			  
			}
		  iii=0;  
		}*/
		
		shoot.speed_level++;
		if(shoot.speed_level>=level_max)
		{
			shoot.speed_level=0;       
		}

	}
	else
	{
		iii=0;       
	
	}

}


void shoot_mode_behavior(void)
{

	
  if(shoot.shoot_mode == shoot_down)
	{
		shoot.set_angle = ABS_ANGLE;

		shoot.MOTOR_SPEED1.out = 0;		
	}
	else if(shoot.shoot_mode == shoot_start)
	{
		

		
		//ÔÚ¶Â×ªÖ®ºó£¬ÓÉÓÚ¶Â×ªÅÐ¶ÏÊ±¼ä¹ý¶Ì£¬µ¼ÖÂ²¦¸Ë¼ÌÐø²¦¶¯ºó»á¼ÌÐøÔö¼Ó²¦ÅÌ½Ç¶ÈÄ¿±êÖµ
		
	if(gimbal_ctrl.gimbal_yaw_motor.gimbal_motor_mode == GIMBAL_MOTOR_VISUAL)
	{
   if(shoot.KEY_outcome==1&&shoot.tigger_mode == tigger_playing && gimbal_ctrl.flag_shoot ==1)
		{				
			shoot.set_angle=shoot.set_angle + tigger_direction*division_angle;       //Ã¿´Î²¦ÅÌÐý×ªµÄ½Ç¶È¡¢·½Ïò
		}
	}
	else
	{
			if(shoot.KEY_outcome==1&&shoot.tigger_mode == tigger_playing)
		{				
			shoot.set_angle=shoot.set_angle + tigger_direction*division_angle;       //Ã¿´Î²¦ÅÌÐý×ªµÄ½Ç¶È¡¢·½Ïò
		}
	}
		
		PID_calc(&shoot.MOTOR_ANGLE1,  ABS_ANGLE, shoot.set_angle);//½Ç¶È»·
		
		PID_calc(&shoot.MOTOR_SPEED1, shoot.motor_data1->speed_rpm, shoot.MOTOR_ANGLE1.out);//ËÙ¶È»·

  	}
	

		
		if(KEY_IS_DOWN(rc_ctrl,R)&& HAL_GetTick() - mag_tick > 500 )
		{
			mag_tick = HAL_GetTick();
			mag_flag = !mag_flag ;
		}

	  if(shoot.shoot_RC->rc.s[s_right]==1 || shoot.shoot_RC->rc.s[s_right]==3)
		{
			
			 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,CLOSE_SERVO); 
		   __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);  

			if(mag_flag)
			{
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,OPEN_SERVO);  
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,5000); 
			}
			else
      {
			 			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,CLOSE_SERVO); 
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);  
			}
			
			 PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm, -shoot_speed[shoot.speed_level][0]);//ËÙ¶È»·
	     PID_calc(&shoot.motor2, shoot.motor2_data->speed_rpm,  shoot_speed[shoot.speed_level][1]);//ËÙ¶È»·	
		}
		else 
		{__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,OPEN_SERVO); 
		   __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
		}
		if(shoot.shoot_RC->rc.s[s_left]==1 || shoot.shoot_RC->rc.s[s_left]==3)
		{ 
		   PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm, -shoot_speed[shoot.speed_level][0]);//ËÙ¶È»·
	     PID_calc(&shoot.motor2, shoot.motor2_data->speed_rpm,  shoot_speed[shoot.speed_level][1]);//ËÙ¶È»
		}
		else 
		{
	     shoot.motor1.out = 0 ;
	     shoot.motor2.out = 0 ;
		   shoot.MOTOR_SPEED1.out = 0;
		}

   CAN_cmd_shoot(0,shoot.MOTOR_SPEED1.out , shoot.motor1.out , shoot.motor2.out);	//·¢ËÍ¿ØÖÆµçÁ÷
		// CAN_cmd_shoot(0,shoot.MOTOR_SPEED1.out ,  shoot.shoot1_filter.out, 0);	//·¢ËÍ¿ØÖÆµçÁ÷
}

