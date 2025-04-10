#ifndef __MYSTATUS_H
#define __MYSTATUS_H

#include "main.h"

typedef enum{
	OK = 0x00,
	Err = 0x01
}FLAG;

typedef enum{
	ACTIVATE = 0x00,	//����
	OFFLINE,			//����
}STATE;

typedef enum{
	MANUAL_CONTROL = 0x00,	//�ֶ�����
	AUTO_CONTROL,			//�Զ�����
	
}CONTROL_MODE;

typedef struct{
	uint32_t 	device_timestamp;
	STATE 		device_state;	
}Device_STATE;

typedef struct{
	STATE robot_state;			//����������״̬
	
	Device_STATE BroadA_state;			//��C״̬

	Device_STATE rm_control_state;		//ң����״̬
	Device_STATE referee_state;			//����ϵͳ״̬
	
	enum _move_mode{
		TAKE_CRAFT = 0x00,				//ȡ��ģʽ
		TURNING_MODE					//תͷģʽ, ͼ����ת180��
	}move_mode;
	
	
	
}ROBOT_STATUS;

extern ROBOT_STATUS robot_state;

#endif	//__MYSTATUS_H
