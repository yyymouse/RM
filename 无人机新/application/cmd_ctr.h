#ifndef __CMD_CRE_H
#define __CMD_CRE_H

#include "main.h"

#include "cJSON.h"
#include "usb_task.h"
#include "bsp_usb.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "chassic_task.h"
#include "stm32_flash.h"
#include "gimbal_task.h"
#include "INS_task.h"

#define BUF_MAX 1024   //2023.4.6增加数据缓冲区

#define MAX_CMD_NUM		20    //最大支持的指令数量		
#define CMD_NAME_LENGTH  50  //最长支持的字符长度

#define version_len 40//版本信息最大支持字符数

#define USE_DEFINE_DATE 0xFFFFFFFF
#define USE_FLASH_DATE  0x11111111



typedef struct{
	char cmd_name[CMD_NAME_LENGTH];
	cJSON* (*do_cmd)(cJSON *dat);
} cmd;

typedef struct
{
    fp32 offset[3]; //x,y,z
    fp32 scale[3];  //x,y,z
} imu_cali_t;

//所有存入flash的结构体
//每当新增或者删减这个结构体，需要将芯片flah整体擦除一边，防止数据受到污染
typedef struct{
	uint32_t flash_or_define;//读取flash里的这一位，如果为FFFF则使用define的参数，否则使用flash里的参数
	uint8_t flash_flag;     //判断flash里是否有数据，没有数据则自动覆盖为define的参数
	/////////////////////////////////////////////////在这个后面添加数据
  char version[version_len];//储存版本信息
	char wave_num_flag[10];//记录对应的字符串指针中的标志位,最大十个数据
	fp32 motor_speed_pid[3];//轮子电机的速度环
	fp32 chassis_angle_pid1[3];//底盘跟随云台的角度环
  fp32 chassis_speed_pid1[3];//底盘跟随云台的速度环
	
	fp32 PITCH_GYRO_ABSOLUTE_PID[3];//pitch的角度环
	fp32 Pitch_speed_pid[3];//pitch速度环
	
	fp32 YAW_GYRO_ABSOLUTE_PID[3];//yaw的角度环
    fp32 Yaw_speed_pid[3];//yaw速度环
	
//    fp32 IMAGE_GYRO_ABSOLUTE_PID[3];//pitch的角度环
//	fp32 Image_speed_pid[3];//pitch速度环
    
	imu_cali_t local_cali_t;//校准记录零飘的数据
	
}flash_date;

extern flash_date save_date;
extern flash_date mid_date;

extern char json_str11[1024];


extern float auto_set_v[3];//三个数据为x，y，wz的数据，flota类型

extern  cmd cmd_lsts[MAX_CMD_NUM];

void cmd_test(cJSON *cmd, cJSON *dat);

extern uint8_t cJson_buffer[BUF_MAX];

extern void flash_date_init();

extern char wavedisplay();

extern void parse_loop(const char *JSON);//ָjson命令分拣函数

extern uint8_t imu_cail_behaviour();

#endif

