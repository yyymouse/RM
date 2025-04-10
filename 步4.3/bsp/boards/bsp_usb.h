#ifndef __BSP_USB_H
#define __BSP_USB_H

#include "main.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/**
  * @brief         �Ӿ�����ṹ��          
	* @author        ����  
  */
typedef struct {
	uint8_t i ;                  //
	uint8_t flag ;               //��־λ
	uint8_t whileflag ;               //��־λ
  uint8_t RInit_data[1000] ;     //���յĳ�ʼ����
	uint32_t Coordinates[2] ;    //����(x,y):Coordinates[0] --- x and Coordinates[1] --- y '
	int16_t RDecode_data[10] ;   //���������
}Usb_Type_t;

//extern Usb_Type_t Usb_Ture;//usb���յ������� 

uint8_t usb_Receive(uint8_t* Buf,uint32_t Len,USBD_HandleTypeDef *pdev);

extern void usb_printf(const char *fmt,...);

#endif
