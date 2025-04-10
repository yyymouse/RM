/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       usb_task.c/h
  * @brief      usb outputs the error message.usb输出错误信息
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Nov-11-2019     RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#include "usb_task.h"


static const char status[2][7] = {"OK", "ERROR!"};

int jjj;
int fff;

fp32 INS_Quat[4];

char visual_mode[]={'B','Y','X'};

char shoot_leave[]={'F','S','T'};

flash_save_date date_cmd={600,3.333,8};
flash_save_date jjjjjj;

char visual_leave;

unsigned   char  yaw_buffer[4];
unsigned   char  pitch_buffer[4]; 

unsigned   char  send_buffer[9];

void usb_task(void const * argument)
{
	
    MX_USB_DEVICE_Init();
	
//	cJSON_Hooks hooks;
//	hooks.malloc_fn = pvPortMalloc;
//	hooks.free_fn = vPortFree;
//	cJSON_InitHooks(&hooks);
  	while(1)
    {
      
        	if(gimbal_ctrl.gimbal_rc->key.v & visual_mode_key)   //判断按键按下
	{
	  jjj++;                  //消抖计数
		if(jjj>10)             //如果计数大于100 则射速等级升级1
		{
			visual_leave++;
			if(visual_leave>=(sizeof(visual_mode)-1) )
			{
				visual_leave=0;
		    jjj=0;       
			  
			}
		  jjj=0;  
		}

	}
	else
	{
		jjj=0;       
	
	}
	
	if(gimbal_ctrl.gimbal_rc->key.v & visual_fu_key)   //判断按键按下
	{
	  fff++;                  //消抖计数
		if(fff>10)             //如果计数大于100 则射速等级升级1
		{
      visual_leave=2;
		  fff=0;  
		}

	}
	else
	{
		fff=0;       
	
	}
    		
	  uint8_t speed_level_date;
		speed_level_date = shoot.speed_level+1;
		switch(shoot.speed_level)
		{
			case 0:
				{
					speed_level_date = shoot_leave[0];
					break;
			
			  } 
						case 1:
				{
					speed_level_date = shoot_leave[1];
			    break;
			  } 
			   case 2:
				{
					speed_level_date = shoot_leave[2];
			    break;
			  } 				
		
		}
	  send_buffer[0] = visual_mode[visual_leave];
//		memcpy(&send_buffer[1] ,(char)shoot.speed_level,1);
	  send_buffer[1] = speed_level_date;
		memcpy(&send_buffer[2],&gimbal_ctrl.gimbal_yaw_motor.absolute_angle,4);
		memcpy(&send_buffer[6],&gimbal_ctrl.gimbal_pitch_motor.absolute_angle,4);	  
	
		wavedisplay();
		if(strlen(json_str11) != 0)//判断是不是输出数据了
		{
		   usb_printf("%s\n", json_str11);
			 memset(json_str11,0,1024);
//			 free(json_str);
		}
    
    
		imu_cail_behaviour();//陀螺仪判断是否初始化
		osDelay(10);
    }

}
