/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       usb_task.c/h
  * @brief      no action.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#ifndef USB_TASK_H
#define USB_TASK_H
#include "struct_typedef.h"

#include "cmd_ctr.h"

#include "cmsis_os.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdarg.h>
#include "string.h"

#include "gimbal_task.h"
#include "shoot_task.h"

#include "INS_task.h"

#include "stm32_flash.h"

#define visual_mode_key       KEY_PRESSED_OFFSET_Q    //视觉模式切换
 
#define visual_fu_key         KEY_PRESSED_OFFSET_E    //视觉模式切换

typedef struct{
	uint32_t v;
	float a;
	uint8_t jkjkj;

}flash_save_date;

extern char visual_leave;
extern void usb_task(void const * argument);
extern unsigned  char send_buffer[9];
#endif
