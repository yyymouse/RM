#ifndef __MYSTATUS_H
#define __MYSTATUS_H

#include "main.h"

typedef enum{
	OK = 0x00,
	Err = 0x01
}FLAG;

typedef enum{
	ACTIVATE = 0x00,	//在线
	OFFLINE,			//离线
}STATE;

typedef enum{
	MANUAL_CONTROL = 0x00,	//手动控制
	AUTO_CONTROL,			//自动控制
	
}CONTROL_MODE;

typedef struct{
	uint32_t 	device_timestamp;
	STATE 		device_state;	
}Device_STATE;

typedef struct{
	STATE robot_state;			//机器人总体状态
	
	Device_STATE BroadA_state;			//板C状态

	Device_STATE rm_control_state;		//遥控器状态
	Device_STATE referee_state;			//裁判系统状态
	
	enum _move_mode{
		TAKE_CRAFT = 0x00,				//取矿模式
		TURNING_MODE					//转头模式, 图传旋转180°
	}move_mode;
	
	
	
}ROBOT_STATUS;

extern ROBOT_STATUS robot_state;

#endif	//__MYSTATUS_H
