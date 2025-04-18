/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       remote_control.c/h
  * @brief      遥控器处理，遥控器是通过类似SBUS的协议传输，利用DMA传输方式节约CPU
  *             资源，利用串口空闲中断来拉起处理函数，同时提供一些掉线重启DMA，串口
  *             的方式保证热插拔的稳定性。
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.0.0     Nov-11-2019     RM              1. support development board tpye c
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */
#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H
#include "struct_typedef.h"
#include "bsp_rc.h"

#define SBUS_RX_BUF_NUM 36u

#define RC_FRAME_LENGTH 18u

#define RC_CH_VALUE_MIN         ((uint16_t)364)
#define RC_CH_VALUE_OFFSET      ((uint16_t)1024)
#define RC_CH_VALUE_MAX         ((uint16_t)1684)

/* ----------------------- RC Switch Definition----------------------------- */




#define RC_SW_UP                ((uint16_t)1)
#define RC_SW_MID               ((uint16_t)3)
#define RC_SW_DOWN              ((uint16_t)2)
#define switch_is_down(s)       (s == RC_SW_DOWN)
#define switch_is_mid(s)        (s == RC_SW_MID)
#define switch_is_up(s)         (s == RC_SW_UP)
/* ----------------------- PC Key Definition-------------------------------- */


#define KEY_IS_DOWN(pctrl, Key)  \
	(pctrl.key.v & KEY_PRESSED_OFFSET_##Key)
	
#define KEY_IS_EDGE_DOWN(plast, pcur, Key)	\
	((pcur.key.v & KEY_PRESSED_OFFSET_##Key) && !(plast.key.v & KEY_PRESSED_OFFSET_##Key))

#define KEY_IS_DOWN_WITHOUT_B(__key__) \
        ((!KEY_IS_DOWN(rc_ctrl, B)) && (KEY_IS_DOWN(rc_ctrl, __key__)))

#define KEY_IS_DOWN_WITH_B(__key__) \
        (KEY_IS_DOWN(rc_ctrl, B) && (KEY_IS_DOWN(rc_ctrl, __key__)))




#define KEY_PRESSED_OFFSET_W            ((uint16_t)1 << 0)//前进
#define KEY_PRESSED_OFFSET_S            ((uint16_t)1 << 1)//后退
#define KEY_PRESSED_OFFSET_A            ((uint16_t)1 << 2)//左
#define KEY_PRESSED_OFFSET_D            ((uint16_t)1 << 3)//右
#define KEY_PRESSED_OFFSET_SHIFT        ((uint16_t)1 << 4)//长按陀螺
#define KEY_PRESSED_OFFSET_CTRL         ((uint16_t)1 << 5)//
#define KEY_PRESSED_OFFSET_Q            ((uint16_t)1 << 6)//视觉模式切换（默认为预测，再按为不预测）
#define KEY_PRESSED_OFFSET_E            ((uint16_t)1 << 7)//打符模式
#define KEY_PRESSED_OFFSET_R            ((uint16_t)1 << 8)//开弹舱盖
#define KEY_PRESSED_OFFSET_F            ((uint16_t)1 << 9)//
#define KEY_PRESSED_OFFSET_G            ((uint16_t)1 << 10)//
#define KEY_PRESSED_OFFSET_Z            ((uint16_t)1 << 11)//掉头
#define KEY_PRESSED_OFFSET_X            ((uint16_t)1 << 12)//哨兵
#define KEY_PRESSED_OFFSET_C            ((uint16_t)1 << 13)//开超级电容
#define KEY_PRESSED_OFFSET_V            ((uint16_t)1 << 14)//开ui
#define KEY_PRESSED_OFFSET_B            ((uint16_t)1 << 15)//调射速
/* ----------------------- Data Struct ------------------------------------- */
typedef __packed struct
{
        __packed struct
        {
                int16_t ch[5];
                char s[2];
        } rc;
        __packed struct
        {
                int16_t x;
                int16_t y;
                int16_t z;
                uint8_t press_l;
                uint8_t press_r;
        } mouse;
        __packed struct
        {
                uint16_t v;
        } key;

} RC_ctrl_t;

/* ----------------------- Internal Data ----------------------------------- */

extern RC_ctrl_t rc_ctrl;
extern void remote_control_init(void);
extern const RC_ctrl_t *get_remote_control_point(void);
extern uint8_t RC_data_is_error(void);
extern void slove_RC_lost(void);
extern void slove_data_error(void);
extern void sbus_to_usart1(uint8_t *sbus);
#endif
