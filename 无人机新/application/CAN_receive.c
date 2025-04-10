/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       can_receive.c/h
  * @brief      there is CAN interrupt function  to receive motor data,
  *             and CAN send function to send motor current to control motor.
  *             这里是CAN中断接收函数，接收电机数据,CAN发送函数发送电机电流控制电机.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.1.0     Nov-11-2019     RM              1. support hal lib
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include "CAN_receive.h"

#include "cmsis_os.h"
#include "MOTOR_ANGLE_PID.h"

#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
//motor data read
#define get_motor_measure(ptr, data)                                    \
    {                                                                   \
        (ptr)->last_ecd = (ptr)->ecd;                                   \
        (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);            \
        (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);      \
        (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);  \
        (ptr)->temperate = (data)[6];                                   \
    }
/*
motor data,  0:chassis motor1 3508;1:chassis motor3 3508;2:chassis motor3 3508;3:chassis motor4 3508;
4:yaw gimbal motor 6020;5:pitch gimbal motor 6020;6:trigger motor 2006;
电机数据, 0:底盘电机1 3508电机,  1:底盘电机2 3508电机,2:底盘电机3 3508电机,3:底盘电机4 3508电机;
4:yaw云台电机 6020电机; 5:pitch云台电机 6020电机; 6:拨弹电机 2006电机；7摩擦轮1电机；8摩擦轮2电机*/
static motor_measure_t motor_chassis[15];
 super_cap_measure_t super_cap_measure;

static CAN_TxHeaderTypeDef  gimbal_tx_message;
static uint8_t              gimbal_can_send_data[8];
static CAN_TxHeaderTypeDef  chassis_tx_message;
static uint8_t              chassis_can_send_data[8];
static CAN_TxHeaderTypeDef  image_tx_message;
static uint8_t              image_can_send_data[8];
static CAN_TxHeaderTypeDef  gimbal_shoot_tx_message;                         //can2的0~3号id数据
static uint8_t              gimbal_shoot_can_send_data[8];
		   
/**
  * @brief          hal CAN fifo call back, receive motor data
  * @param[in]      hcan, the point to CAN handle
  * @retval         none
  */
/**
  * @brief          hal库CAN回调函数,接收电机数据
  * @param[in]      hcan:CAN句柄指针
  * @retval         none
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header_1;
  uint8_t             rx_data_1[8];
	
	CAN_RxHeaderTypeDef rx_header_2;
  uint8_t             rx_data_2[8];
    
	  
	  if(hcan->Instance == CAN1)
  {
    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header_1, rx_data_1); //receive can data
  }
	  if(hcan->Instance == CAN2)
  {
    HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &rx_header_2, rx_data_2); //receive can data
  }
	
    switch (rx_header_1.StdId)                                   //接收can1的数据存入
    {
        case CAN_3508_M1_ID:
				{
				  get_motor_measure(&motor_chassis[0], rx_data_1);
					 break;
				}
        case CAN_3508_M2_ID:
				{
				  get_motor_measure(&motor_chassis[1], rx_data_1);
					 break;
				}
        case CAN_3508_M3_ID:
				{
					get_motor_measure(&motor_chassis[2], rx_data_1);
					 break;
				}
        case CAN_3508_M4_ID:
				{
					get_motor_measure(&motor_chassis[3], rx_data_1);
					 break;
				}
         
            {
                
                    get_motor_measure(&motor_chassis[13], rx_data_1);
            }
//        case CAN_IMAGE_MOTOR_ID:
//				{
//					get_motor_measure(&motor_chassis[13], rx_data_1);
//				}
//        case CAN_YAW_MOTOR_ID:
//        case CAN_PIT_MOTOR_ID:
       // case CAN_TRIGGER_MOTOR_ID:
       
				//超级电容接收报文
				case CAN_SUPER_CAP_RE_ID:
				{
					
					uint16_t * pPowerdata = (uint16_t *)rx_data_1;
					super_cap_measure.InputVot =     (float)pPowerdata[0] / 100.0f;//输入电压
					super_cap_measure.CapVot =       (float)pPowerdata[1] / 100.0f;//电容电压
					super_cap_measure.Test_current = (float)pPowerdata[2] / 100.0f;//输入电流
					super_cap_measure.Target_Power = (float)pPowerdata[3] / 100.0f;//设定功率
				break;
				}
				
				

        default:
        {
            break;
        }
    }
		
		switch (rx_header_2.StdId)                                   //接收can2的数据存入
    {

        case CAN_TRIGGER_MOTOR_ID2:  //2
				{
					get_motor_measure(&motor_chassis[9], rx_data_2);
					
					 break;
				}
        case CAN_3508_shoot1://3
				{
					get_motor_measure(&motor_chassis[7], rx_data_2);
					
					 break;
				}
        case CAN_3508_shoot2://4
				{
					get_motor_measure(&motor_chassis[8], rx_data_2);
			
					 break;
				}
          case CAN_YAW_MOTOR_ID:
				{
					get_motor_measure(&motor_chassis[4], rx_data_2);
					 break;
				}

        default:
        {
            break;
        }
	
    }
					   Motor_Angle_Cal(&motor_pid);       //角度实际值

		
}



/**
  * @brief          send control current of motor (0x205, 0x206, 0x207, 0x208)
  * @param[in]      yaw: (0x205) 6020 motor control current, range [-30000,30000] 
  * @param[in]      pitch: (0x206) 6020 motor control current, range [-30000,30000]
  * @param[in]      shoot: (0x207) 2006 motor control current, range [-10000,10000]
  * @param[in]      rev: (0x208) reserve motor control current
  * @retval         none
  */
/**
  * @brief          发送电机控制电流(0x205,0x206,0x207,0x208)
  * @param[in]      yaw: (0x205) 6020电机控制电流, 范围 [-30000,30000]
  * @param[in]      pitch: (0x206) 6020电机控制电流, 范围 [-30000,30000]
  * @param[in]      shoot: (0x207) 2006电机控制电流, 范围 [-10000,10000]
  * @param[in]      rev: (0x208) 保留，电机控制电流
  * @retval         none
  */
void CAN_cmd_gimbal(int16_t yaw, int16_t pitch, int16_t shoot, int16_t rev)
{
    uint32_t send_mail_box;
    gimbal_tx_message.StdId = CAN_GIMBAL_ALL_ID;
    gimbal_tx_message.IDE = CAN_ID_STD;
    gimbal_tx_message.RTR = CAN_RTR_DATA;
    gimbal_tx_message.DLC = 0x08;
    gimbal_can_send_data[0] = (yaw >> 8);
    gimbal_can_send_data[1] = yaw;
    gimbal_can_send_data[2] = (pitch >> 8);
    gimbal_can_send_data[3] = pitch;
    gimbal_can_send_data[4] = (shoot >> 8);
    gimbal_can_send_data[5] = shoot;
    gimbal_can_send_data[6] = (rev >> 8);
    gimbal_can_send_data[7] = rev;
    HAL_CAN_AddTxMessage(&GIMBAL_CAN, &gimbal_tx_message, gimbal_can_send_data, &send_mail_box);
}

/**
  * @brief          send CAN packet of ID 0x700, it will set chassis motor 3508 to quick ID setting
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          发送ID为0x700的CAN包,它会设置3508电机进入快速设置ID
  * @param[in]      none
  * @retval         none
  */
void CAN_cmd_chassis_reset_ID(void)
{
    uint32_t send_mail_box;
    chassis_tx_message.StdId = 0x700;
    chassis_tx_message.IDE = CAN_ID_STD;
    chassis_tx_message.RTR = CAN_RTR_DATA;
    chassis_tx_message.DLC = 0x08;
    chassis_can_send_data[0] = 0;
    chassis_can_send_data[1] = 0;
    chassis_can_send_data[2] = 0;
    chassis_can_send_data[3] = 0;
    chassis_can_send_data[4] = 0;
    chassis_can_send_data[5] = 0;
    chassis_can_send_data[6] = 0;
    chassis_can_send_data[7] = 0;

    HAL_CAN_AddTxMessage(&CHASSIS_CAN, &chassis_tx_message, chassis_can_send_data, &send_mail_box);
}


/**
  * @brief          send control current of motor (0x201, 0x202, 0x203, 0x204)
  * @param[in]      motor1: (0x201) 3508 motor control current, range [-16384,16384] 
  * @param[in]      motor2: (0x202) 3508 motor control current, range [-16384,16384] 
  * @param[in]      motor3: (0x203) 3508 motor control current, range [-16384,16384] 
  * @param[in]      motor4: (0x204) 3508 motor control current, range [-16384,16384] 
  * @retval         none
  */
/**
  * @brief          发送电机控制电流(0x201,0x202,0x203,0x204)
  * @param[in]      motor1: (0x201) 3508电机控制电流, 范围 [-16384,16384]
  * @param[in]      motor2: (0x202) 3508电机控制电流, 范围 [-16384,16384]
  * @param[in]      motor3: (0x203) 3508电机控制电流, 范围 [-16384,16384]
  * @param[in]      motor4: (0x204) 3508电机控制电流, 范围 [-16384,16384]
  * @retval         none
  */
void CAN_cmd_chassis(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    uint32_t send_mail_box;
    chassis_tx_message.StdId = CAN_CHASSIS_ALL_ID;
    chassis_tx_message.IDE = CAN_ID_STD;
    chassis_tx_message.RTR = CAN_RTR_DATA;
    chassis_tx_message.DLC = 0x08;
    chassis_can_send_data[0] = motor1 >> 8;
    chassis_can_send_data[1] = motor1;
    chassis_can_send_data[2] = motor2 >> 8;
    chassis_can_send_data[3] = motor2;
    chassis_can_send_data[4] = motor3 >> 8;
    chassis_can_send_data[5] = motor3;
    chassis_can_send_data[6] = motor4 >> 8;
    chassis_can_send_data[7] = motor4;

    HAL_CAN_AddTxMessage(&CHASSIS_CAN, &chassis_tx_message, chassis_can_send_data, &send_mail_box);
}



/**
  * @brief          发送摩擦轮电机控制电流(0x201,0x202,0x203,0x204)
  * @param[in]      motor1: (0x201) 3508电机控制电流, 范围 [-16384,16384]
  * @param[in]      motor2: (0x202) 3508电机控制电流, 范围 [-16384,16384]
  * @param[in]      motor3: (0x203) 3508电机控制电流, 范围 [-16384,16384]
  * @param[in]      motor4: (0x204) 3508电机控制电流, 范围 [-16384,16384]
  * @retval         none
  */
void CAN_cmd_shoot(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    uint32_t send_mail_box;
    gimbal_shoot_tx_message.StdId = 0x200;
    gimbal_shoot_tx_message.IDE = CAN_ID_STD;
    gimbal_shoot_tx_message.RTR = CAN_RTR_DATA;
    gimbal_shoot_tx_message.DLC = 0x08;
    gimbal_shoot_can_send_data[0] = motor1 >> 8;
    gimbal_shoot_can_send_data[1] = motor1;
    gimbal_shoot_can_send_data[2] = motor2 >> 8;
    gimbal_shoot_can_send_data[3] = motor2;
    gimbal_shoot_can_send_data[4] = motor3 >> 8;
    gimbal_shoot_can_send_data[5] = motor3;
    gimbal_shoot_can_send_data[6] = motor4 >> 8;
    gimbal_shoot_can_send_data[7] = motor4;

    HAL_CAN_AddTxMessage(&GIMBAL_CAN, &gimbal_shoot_tx_message, gimbal_shoot_can_send_data, &send_mail_box);
}

/**
  * @brief          return the yaw 6020 motor data point
  * @param[in]      none
  * @retval         motor data point
  */
/**
  * @brief          返回yaw 6020电机数据指针
  * @param[in]      none
  * @retval         电机数据指针
  */
const motor_measure_t *get_yaw_gimbal_motor_measure_point(void)
{
    return &motor_chassis[4];
}

/**
  * @brief          return the pitch 6020 motor data point
  * @param[in]      none
  * @retval         motor data point
  */
/**
  * @brief          返回pitch 6020电机数据指针
  * @param[in]      none
  * @retval         电机数据指针
  */
const motor_measure_t *get_pitch_gimbal_motor_measure_point(void)
{
    return &motor_chassis[5];
}


/**
  * @brief          return the trigger 2006 motor data point
  * @param[in]      none
  * @retval         motor data point
  */
/**
  * @brief          返回拨弹电机 2006电机数据指针
  * @param[in]      none
  * @retval         电机数据指针
  */
const motor_measure_t *get_trigger_motor_measure_point(void)
{
    return &motor_chassis[6];
}


/**
  * @brief          return the chassis 3508 motor data point
  * @param[in]      i: motor number,range [0,3]
  * @retval         motor data point
  */
/**
  * @brief          返回底盘电机 3508电机数据指针
  * @param[in]      i: 电机编号,范围[0,3]
  * @retval         电机数据指针
  */
const motor_measure_t *get_chassis_motor_measure_point(uint8_t i)
{
    return &motor_chassis[(i & 0x03)];
}


/**
  * @brief          返回摩擦轮电机 3508电机数据指针
  * @param[in]      i: 电机编号,范围[7,8]
  * @retval         电机数据指针
  */
const motor_measure_t *get_shoot_motor_measure_point(uint8_t i)
{
    return &motor_chassis[i];
}

/**
  * @brief          返回摩擦轮电机 2006电机数据指针
  * @param[in]      i: 电机编号,范围[9]
  * @retval         电机数据指针
  */
const motor_measure_t *get_trigger2_motor_measure_point(void)
{
    return &motor_chassis[9];
}

/**
  * @brief        	设置超级电容功率值
* 用法：发送值从3000-13000对应30W到130W
  */

void CAN_cmd_supercap(int16_t temPower)
{
		uint32_t send_mail_box;
    chassis_tx_message.StdId = CAN_SUPER_CAP_SET_ID;
    chassis_tx_message.IDE = CAN_ID_STD;
    chassis_tx_message.RTR = CAN_RTR_DATA;
    chassis_tx_message.DLC = 0x08;
    chassis_can_send_data[0] = temPower >> 8;
    chassis_can_send_data[1] = temPower;

    HAL_CAN_AddTxMessage(&CHASSIS_CAN, &chassis_tx_message, chassis_can_send_data, &send_mail_box);

}

const super_cap_measure_t *get_super_cap_measure_point(uint8_t i)
{
	return &super_cap_measure;
}
