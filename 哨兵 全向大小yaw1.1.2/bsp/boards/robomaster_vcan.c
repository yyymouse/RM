/*
 * @Author: your name
 * @Date: 2020-10-03 12:49:13
 * @LastEditTime: 2020-11-28 15:25:54
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \MDK-ARMf:\STM32\test program\STM32f103-HAL\Power_Control\Power_Control\App\robomaster_vcan\robomaster_vcan.c
 */
#include "robomaster_vcan.h"
#include "usart.h"
#include "bsp_usart.h"


//该.c文件主要是配合山外调试助手发送波形
short  wave_form_data[8] = {0};
void send_data(uint8_t date)
{
//	HAL_UART_Transmit(&huart1 ,&date,1,10);
//	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET); 
	//usart1_tx_dma_enable(&date,10);	//串口一是通过图传控制，这里改为山外使用
	
}
void shanwai_send_wave_form(void)
{
	uint8_t i;
	
	send_data(0x03);
	send_data(0xfc);
	for(i = 0;i<8;i++)
	{
	  send_data((wave_form_data[i]&0xff)); //现发送低位在发送高位
	  send_data((wave_form_data[i]>>8));
		
	  
	}
	send_data(0xfc);
	send_data(0x03);
}	
