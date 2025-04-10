#ifndef __UI_TASK_H
#define __UI_TASK_H

#include "main.h"
#include "cmsis_os.h"
#include "data_queue.h"
#include "usart.h"
#include "CAN_receive.h"

#define STR_BASE_X_POS      300     //字符串x坐标基点
#define STR_BASE_Y_POS      700     //字符串y坐标基点
#define STR_Y_OFFSET        30      //每行字符串y轴偏移
#define UI_FONT_SIZE        15      //UI字体大小
#define UI_LINE_WIDTH       3       //UI线宽
#define UI_SEND_PERIOD      100     //UI发送周期

#define TRACE_LINE_WIDTH    4       //行车轨道线宽
#define TRACE_S_LINE_WIDTH    1       //行车轨道细线宽
#define TRACE_C_LINE_WIDTH    2       //行车轨道粗线宽

#define START_X_OFFSET      300      //轨道线小端偏离X中心大小
#define START_Y_POS         267      //轨道线小端Y轴高度

#define END_X_OFFSET        520     //轨道线大端偏离X中心大小
#define END_Y_POS           0       //轨道线大端Y轴高度

#define ANGLE_TO_PIXEL      100     //预测角度转换到显示像素的系数

void ui_task(void const * argument);

#define UI_SWITCH KEY_PRESSED_OFFSET_V

uint8_t usart6_transmit(uint8_t *data, uint16_t len);
queue_state getState(void);

void ui_init(void);

/**
 * @brief 刷新超级电容数据至字符串中
 * 
 */
void refresh_supercap(super_cap_measure_t* supercap);

/**
 * @brief 刷新UI
 * 
 */
void refresh_ui(void);

#endif //__UI_TASK_H
