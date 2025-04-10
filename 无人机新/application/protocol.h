#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>

typedef __packed struct{
	
	/* ֡ͷ 1byte */
	uint8_t frameHead;

	
	/*��̨����   20byte  */
		__packed struct{
 

            float relative_angle_communicate;
						float chassis_yaw_communicate;   //�����Ǻ���̨������ӵ�yaw�Ƕ�
//					  float set_wz_communicate;
						float pc_distance;               //  �Ӿ�
	         	float absolute_angle_set_communicate;
            float action_chassic ;
	}hero_gimbal_data;
	
	/* ң�������� 22byte*/
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

	/* CRC16У�� 2byte */
	uint16_t CRC16;
	
	/* ֡β 1byte */
	uint8_t frameTail;
	
	
}hero_protocol_t;
/*42byte*/

typedef __packed struct{
	
	/* ֡ͷ 1byte */
	uint8_t frameHead;
	
	/*����ϵͳ  4byte  */
		__packed struct{
 
            uint16_t cooling_heat_42mm;//42mm ǹ������
            uint16_t cooling_limit_42mm;//������ 42mm ǹ����������

         	}hero_referee_data;
	/*���̱�־λ  2byte  */
  __packed struct{

            uint16_t  logo_shoot ;
	
         }hero_dial_logo_data;
	/* CRC16У�� 2byte */
	uint16_t CRC16;
	
	/* ֡β 1byte */
	uint8_t frameTail;
	//10
}hero_A_protocol_t;

extern hero_A_protocol_t hero_A_protocol;
extern hero_protocol_t hero_protocol;

#endif
