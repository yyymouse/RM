#ifndef __UI_TASK_H
#define __UI_TASK_H

#include "main.h"
#include "cmsis_os.h"
#include "data_queue.h"
#include "usart.h"
#include "CAN_receive.h"

#define STR_BASE_X_POS      300     //�ַ���x�������
#define STR_BASE_Y_POS      700     //�ַ���y�������
#define STR_Y_OFFSET        30      //ÿ���ַ���y��ƫ��
#define UI_FONT_SIZE        15      //UI�����С
#define UI_LINE_WIDTH       3       //UI�߿�
#define UI_SEND_PERIOD      100     //UI��������

#define TRACE_LINE_WIDTH    4       //�г�����߿�
#define TRACE_S_LINE_WIDTH    1       //�г����ϸ�߿�
#define TRACE_C_LINE_WIDTH    2       //�г�������߿�

#define START_X_OFFSET      300      //�����С��ƫ��X���Ĵ�С
#define START_Y_POS         267      //�����С��Y��߶�

#define END_X_OFFSET        520     //����ߴ��ƫ��X���Ĵ�С
#define END_Y_POS           0       //����ߴ��Y��߶�

#define ANGLE_TO_PIXEL      100     //Ԥ��Ƕ�ת������ʾ���ص�ϵ��

void ui_task(void const * argument);

#define UI_SWITCH KEY_PRESSED_OFFSET_V

uint8_t usart6_transmit(uint8_t *data, uint16_t len);
queue_state getState(void);

void ui_init(void);

/**
 * @brief ˢ�³��������������ַ�����
 * 
 */
void refresh_supercap(super_cap_measure_t* supercap);

/**
 * @brief ˢ��UI
 * 
 */
void refresh_ui(void);

#endif //__UI_TASK_H
