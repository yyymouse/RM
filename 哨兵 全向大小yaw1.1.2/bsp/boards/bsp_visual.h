#ifndef __BSP_VISUAL_H
#define __BSP_VISUAL_H

#include "main.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/**
  * @brief         视觉处理结构体          
	* @author        刘根  
  */
typedef struct {
	uint8_t i ;                  //
	uint8_t flag ;               //标志位
	uint8_t whileflag ;               //标志位
  uint8_t RInit_data[100] ;     //接收的初始数据
	uint32_t Coordinates[2] ;    //坐标(x,y):Coordinates[0] --- x and Coordinates[1] --- y '
	int16_t RDecode_data[10] ;   //处理后数据
}Usb_Type_t;

extern Usb_Type_t Visual_Ture;//usb接收到的数据 

uint8_t Visual_Receive(uint8_t* Buf,uint32_t *Len,USBD_HandleTypeDef *pdev);

#endif

