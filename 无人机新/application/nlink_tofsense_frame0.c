#include "nlink_tofsense_frame0.h"
#include "decoder.h"
#include "nlink_utils.h"

#pragma pack(1)
typedef struct
{
  uint8_t header[2];
  uint8_t reserved0;
  uint8_t id;
  uint32_t system_time;
  nint24_t dis;
  uint8_t dis_status;
  uint16_t signal_strength;
  uint8_t range_precision;
  uint8_t check_sum;
} nts_frame0_raw_t;
#pragma pack()

static nts_frame0_raw_t g_frame;

static uint8_t UnpackData(const uint8_t *data, size_t data_length)
{
  if (data_length < g_nts_frame0.fixed_part_size ||
      data[0] != g_nts_frame0.frame_header ||
      data[1] != g_nts_frame0.function_mark)
    return 0;
  if (!NLINK_VerifyCheckSum(data, g_nts_frame0.fixed_part_size))
    return 0;

  memcpy(&g_frame, data, g_nts_frame0.fixed_part_size);
  g_nts_frame0.result.id = g_frame.id;
  g_nts_frame0.result.system_time = g_frame.system_time;
  g_nts_frame0.result.dis_status = g_frame.dis_status;
  g_nts_frame0.result.signal_strength = g_frame.signal_strength;
  g_nts_frame0.result.range_precision = g_frame.range_precision;
  g_nts_frame0.result.dis = NLINK_ParseInt24(g_frame.dis) / 1000.0f;

  return 1;
}


Tofsense TOF[8];
nts_frame0_t g_nts_frame0 = {.fixed_part_size = 16,
                             .frame_header = 0x57,
                             .function_mark = 0x00,
                             .UnpackData = UnpackData};

uint32_t TOF_data_length=0;		//DMA接收数据长度														 
uint8_t TOF_ID = 0;			//仅用作级联时更矮查询命令
uint8_t u_tx_buf[8] = { 0x57,0x10,0xff,0xff,0x00,0xff,0xff,0x63};  //查询命令
uint8_t u_rx_buf[400];




/******************************************************************************
函数名称 ： Tof_Unpack_Data
函数功能 ： 将接收到模块的一帧数据传入解包函数，并将解包后的数据存入结构体变量中
参    数 ： 无
返 回 值 ： 无
调用方法：在空闲中断中调用Tof_Unpack_Data();进行赋值
支持模块：TOFSense/TOFSense-P/PS/F/FP
*******************************************************************************/
void Tof_Unpack_Data()
{
	uint8_t tof_flag,tof_id;

	tof_flag = g_nts_frame0.UnpackData(u_rx_buf,TOF_data_length);		//获取解包标志位
	tof_id = g_nts_frame0.result.id;									//获取该帧数据的模块ID
	if(tof_flag == 1)
		{													//解包成功
		TOF[tof_id].ID = tof_id;
		TOF[tof_id].dis = g_nts_frame0.result.dis;
		TOF[tof_id].status = g_nts_frame0.result.dis_status;
		TOF[tof_id].signal_strength = g_nts_frame0.result.signal_strength;

	}
}