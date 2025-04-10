#include "decoder.h"
#include "bsp_crc.h"
#include <string.h>
#include "remote_control.h"
#include "status.h"
#define BW_TFLidar_DATA_HEAD            0x59
#define BW_TFLidar_DATA_Len             9
		uint32_t tof_distance_1 ;
    uint32_t strength_1 ;
uint8_t data_buffer[128];
uint8_t data_buffer_A[128];
uint8_t tof_rx_buffer_1[18];

void decode_board_C(uint8_t* src, uint32_t len)
{
	//收到数据小于42字节退出
	if (len < 42) {
		return;
	}
	
	int pos = 0;
	while(*(src + pos) != 0x5A && (len - pos) >= 42) {
		pos ++;
	}
	
	if ((len - pos) < 42) {
		return;
	}
	
	uint16_t crc16 = *(uint16_t *)(src + pos + 39);
	
	if (crc16 != CRC16_calculate(src + pos, 39, 0xFFFF) || *(src + pos + 41) != 0xA5) 
	{
		return;
	}
	
	memcpy(&hero_protocol, (src + pos), 42);
	memcpy(&rc_ctrl, &hero_protocol.rm_control, 22);
	
}

void decode_board_A(uint8_t* src, uint32_t len)
{
	//收到数据小于10字节退出
	if (len < 10) {
		return;
	}
	
	int pos = 0;
	while(*(src + pos) != 0x5A && (len - pos) >= 10) {
		pos ++;
	}
	
	if ((len - pos) < 10) {
		return;
	}
	
	uint16_t crc16 = *(uint16_t *)(src + pos + 7);
	
	if (crc16 != CRC16_calculate(src + pos, 7, 0xFFFF) || *(src + pos + 9) != 0xA5) 
	{
		return;
	}
	
	memcpy(&hero_A_protocol, (src + pos), 10);	
}


