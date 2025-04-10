#include "main.h"
#include "DMPower.h"

int float_to_uint(float x, float x_min, float x_max, int bits)
{ 
  float span = x_max - x_min;
  float offset = x_min;
  return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}

extern CAN_HandleTypeDef hcan1;

//void can_filter_init(void)
//{
//    CAN_FilterTypeDef can_filter_st;
//    can_filter_st.FilterActivation = ENABLE;
//    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
//    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
//    can_filter_st.FilterIdHigh = 0x0000;
//    can_filter_st.FilterIdLow = 0x0000;
//    can_filter_st.FilterMaskIdHigh = 0x0000;
//    can_filter_st.FilterMaskIdLow = 0x0000;
//    can_filter_st.FilterBank = 0;
//    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
//    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
//    HAL_CAN_Start(&hcan1);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
//}


int a;
void Enable_CtrlMotor(CAN_HandleTypeDef* hcan,uint8_t ID, uint8_t data0, uint8_t data1,uint8_t data2, uint8_t data3, uint8_t data4,uint8_t data5,uint8_t data6,uint8_t data7)
{
	CAN_TxPacketTypeDef packet;
	packet.hdr.StdId = ID;
	packet.hdr.IDE = CAN_ID_STD;
	packet.hdr.RTR = CAN_RTR_DATA;
	packet.hdr.DLC = 0x08;
	packet.payload[0] = (uint8_t)data0;
	packet.payload[1] = (uint8_t)data1;
	packet.payload[2] = (uint8_t)data2;
	packet.payload[3] = (uint8_t)data3;
	packet.payload[4] = (uint8_t)data4;
	packet.payload[5] = (uint8_t)data5;
	packet.payload[6] = (uint8_t)data6;
	packet.payload[7] = (uint8_t)data7;

	/*找到空的发送邮箱，把数据发送出去*/
	if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) //
	{
		if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
		{
			HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0);
			a=HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0);
		}
	}
}

void Enable_CtrlMotor1(CAN_HandleTypeDef* hcan,uint8_t ID, uint8_t data0, uint8_t data1,uint8_t data2, uint8_t data3)
{
	CAN_TxPacketTypeDef packet;
	packet.hdr.StdId = ID;
	packet.hdr.IDE = CAN_ID_STD;
	packet.hdr.RTR = CAN_RTR_DATA;
	packet.hdr.DLC = 0x04;
	packet.payload[0] = (uint8_t)data0;
	packet.payload[1] = (uint8_t)data1;
	packet.payload[2] = (uint8_t)data2;
	packet.payload[3] = (uint8_t)data3;


	/*找到空的发送邮箱，把数据发送出去*/
	if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) //
	{
		if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
		{
			HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0);
			a=HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0);
		}
	}
}

void MIT_CtrlMotor(CAN_HandleTypeDef* hcan,uint16_t ID, float _pos, float _vel,float _KP, float _KD, float _torq)
{
    static CAN_TxPacketTypeDef packet;
    uint16_t pos_tmp,vel_tmp,kp_tmp,kd_tmp,tor_tmp;
    
    packet.hdr.StdId = ID;
    packet.hdr.IDE = CAN_ID_STD;
    packet.hdr.RTR = CAN_RTR_DATA;
    packet.hdr.DLC = 0x08;

    pos_tmp = float_to_uint(_pos, P_MIN, P_MAX, 16);
    vel_tmp = float_to_uint(_vel, V_MIN, V_MAX, 12);
    kp_tmp = float_to_uint(_KP, KP_MIN, KP_MAX, 12);
    kd_tmp = float_to_uint(_KD, KD_MIN, KD_MAX, 12);
    tor_tmp = float_to_uint(_torq, T_MIN, T_MAX, 12);

    packet.payload[0] = (pos_tmp >> 8);
    packet.payload[1] = pos_tmp;
    packet.payload[2] = (vel_tmp >> 4);
    packet.payload[3] = ((vel_tmp&0xF)<<4)|(kp_tmp>>8);
    packet.payload[4] = kp_tmp;
    packet.payload[5] = (kd_tmp >> 4);
    packet.payload[6] = ((kd_tmp&0xF)<<4)|(tor_tmp>>8);
    packet.payload[7] = tor_tmp;

    //找到空的发送邮箱，把数据发送出去
	  if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) //
	  {
		if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
		{
			HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX2);
		}
	}
}
  uint8_t             rx_data_1[8];
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//    CAN_RxHeaderTypeDef rx_header_1;
//  if(hcan->Instance == CAN1)
//  {
//    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header_1, rx_data_1); //receive can data
//  }

//}

void Speed_CtrlMotor(CAN_HandleTypeDef* hcan, uint16_t ID, float _vel)
{
    static CAN_TxPacketTypeDef packet;
    uint8_t *vbuf;
    vbuf=(uint8_t*)&_vel;

    packet.hdr.StdId = 0x200+ID;
    packet.hdr.IDE = CAN_ID_STD;
    packet.hdr.RTR = CAN_RTR_DATA;
    packet.hdr.DLC = 0x04;

    packet.payload[0] = *vbuf;
    packet.payload[1] = *(vbuf+1);
    packet.payload[2] = *(vbuf+2);
    packet.payload[3] = *(vbuf+3);


      if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) //
      {
        if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
        {
            HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX2);
    }
    }
}

void PosSpeed_CtrlMotor(CAN_HandleTypeDef* hcan, uint16_t ID, float _pos, float _vel)
{
    static CAN_TxPacketTypeDef packet;
    uint8_t *pbuf,*vbuf;
    pbuf=(uint8_t*)&_pos;
    vbuf=(uint8_t*)&_vel;

    packet.hdr.StdId =0x100+ID;
    packet.hdr.IDE = CAN_ID_STD;
    packet.hdr.RTR = CAN_RTR_DATA;
    packet.hdr.DLC = 0x08;

    packet.payload[0] = *pbuf;;
    packet.payload[1] = *(pbuf+1);
    packet.payload[2] = *(pbuf+2);
    packet.payload[3] = *(pbuf+3);
    packet.payload[4] = *vbuf;
    packet.payload[5] = *(vbuf+1);
    packet.payload[6] = *(vbuf+2);
    packet.payload[7] = *(vbuf+3);


      if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) //
      {
        if(HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
        {
            HAL_CAN_AddTxMessage(hcan, &packet.hdr, packet.payload, (uint32_t*)CAN_TX_MAILBOX2);
    }
    }
}