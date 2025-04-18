#include "bsp_visual.h"
#include "cJSON.h"
#include "malloc.h"
#include "cmd_ctr.h"

Visual_Type_t Visual_Ture;

uint8_t Visual_Receive(uint8_t* Buf,uint32_t *Len,USBD_HandleTypeDef *pdev)
{
	uint8_t result = USBD_OK;
	uint32_t i;
	//HAL_GPIO_TogglePin(GPIOG,LED7_Pin);
	result = USBD_CDC_ReceivePacket(pdev);
	for(i=0;i<Len[0];i++)
	{
		Visual_Ture.RInit_data[i] = * Buf++;
	}
	//Visual_Ture.RInit_data[i] = '\n';
  Visual_Ture.flag=1;
	Visual_Ture.whileflag=1;
	memcpy(cJson_buffer,Visual_Ture.RInit_data,i);//将数据复制到cjson处理的数组里
	parse_loop((const char*)cJson_buffer);	//处理cjson	
	memset(Visual_Ture.RInit_data , 0,i);
	memset(cJson_buffer , 0,i);
  return result;
}

