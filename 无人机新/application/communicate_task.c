#include "communicate_task.h"
#include "remote_control.h"
#include "INS_task.h"
#include "bsp_crc.h"
#include "gimbal_task.h"
#include "chassic_task.h"
#include "protocol.h"
#include <string.h>
#include "decoder.h"
#include "nlink_tofsense_frame0.h"
#include "nlink_utils.h"

int size_proto;
int time_delay;


void communicate_task(void const * argument)
{
	size_proto = sizeof(hero_protocol_t);

	hero_protocol.frameHead = 0x5A;
	hero_protocol.frameTail = 0xA5;

	for(;;){
		memcpy(&hero_protocol.rm_control,get_remote_control_point(), sizeof(rc_ctrl));
		
    hero_protocol.hero_gimbal_data.action_chassic = diversion_key_count ;
		hero_protocol.hero_gimbal_data.pc_distance = TOF[0].dis;
    hero_protocol.hero_gimbal_data.relative_angle_communicate = chassic_ctrl.chassis_yaw_motor->relative_angle;
		hero_protocol.hero_gimbal_data.chassis_yaw_communicate = chassic_ctrl.chassis_yaw;
    hero_protocol.hero_gimbal_data.absolute_angle_set_communicate = chassic_ctrl.chassis_yaw_motor->absolute_angle_set;
		hero_protocol.CRC16 = CRC16_calculate((uint8_t *)&hero_protocol, 43, 0xFFFF);
		
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&hero_protocol, size_proto);
//    time_delay ++ ;
		osDelay(10);
	}
}