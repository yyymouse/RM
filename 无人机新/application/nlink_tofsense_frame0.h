#ifndef NLINK_TOFSENSE_FRAME0_H
#define NLINK_TOFSENSE_FRAME0_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "nlink_typedef.h"

  typedef struct
  {
    uint8_t id;
    uint32_t system_time;
    float dis;
    uint8_t dis_status;
    uint16_t signal_strength;
    uint8_t range_precision;//cm, only valid in tofsense-f
  } nts_frame0_result_t;

  typedef struct
  {
    const size_t fixed_part_size;
    const uint8_t frame_header;
    const uint8_t function_mark;
    nts_frame0_result_t result;

    uint8_t (*const UnpackData)(const uint8_t *data, size_t data_length);
  } nts_frame0_t;
	
	/*TOFSense/TOFSense-P/PS/F/FP等型号的解包数据结构体*/
typedef struct {
	uint8_t ID;

	float dis;					//解包后的距离数据
	uint8_t status;				//解包后的信号状态指示
	uint16_t signal_strength;	//解包后的信号强度
}Tofsense;

// Tofsense TOF[8];

  extern nts_frame0_t g_nts_frame0;
  extern uint8_t u_rx_buf[400];
  extern uint8_t u_tx_buf[8];
	extern uint32_t TOF_data_length;
extern Tofsense TOF[8];
	
	
void Tof_Unpack_Data();
	
	
	
#ifdef __cplusplus
}
#endif
#endif // NLINK_TOFSENSE_FRAME0_H
