#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>

typedef __packed struct{
	
	/* 帧头 1byte */
	uint8_t frameHead;

	
	/*云台数据   20byte  */
		__packed struct{
 

            float relative_angle_communicate;
						float chassis_yaw_communicate;   //陀螺仪和云台电机叠加的yaw角度
//					  float set_wz_communicate;
						float pc_distance;               //  视觉
	         	float absolute_angle_set_communicate;
            float action_chassic ;
	}hero_gimbal_data;
	
	/* 遥控器数据 22byte*/
	__packed struct{
		__packed struct
        {
                int16_t ch[5];
                char s[2];
        } rc;
        __packed struct
        {
                int16_t x;
                int16_t y;
                int16_t z;
                uint8_t press_l;
                uint8_t press_r;
        } mouse;
        __packed struct
        {
                uint16_t v;
        } key;
	}rm_control;

	/* CRC16校验 2byte */
	uint16_t CRC16;
	
	/* 帧尾 1byte */
	uint8_t frameTail;
	
	
}hero_protocol_t;
/*42byte*/

typedef __packed struct{
	
	/* 帧头 1byte */
	uint8_t frameHead;
	
	/*裁判系统  4byte  */
		__packed struct{
 
            uint16_t cooling_heat_42mm;//42mm 枪口热量
            uint16_t cooling_limit_42mm;//机器人 42mm 枪口热量上限

         	}hero_referee_data;
	/*拨盘标志位  2byte  */
  __packed struct{

            uint16_t  logo_shoot ;
	
         }hero_dial_logo_data;
	/* CRC16校验 2byte */
	uint16_t CRC16;
	
	/* 帧尾 1byte */
	uint8_t frameTail;
	//10
}hero_A_protocol_t;

extern hero_A_protocol_t hero_A_protocol;
extern hero_protocol_t hero_protocol;

#endif
