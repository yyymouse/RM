#include "ui_task.h"
#include "usart.h"
#include "client_UI.h"
#include "remote_control.h"
#include "shoot_task.h"
#include "chassic_task.h"


#include "usb_task.h"
#include "gimbal_task.h"

#define SPEED_LEVEL1 "15"
#define SPEED_LEVEL2 "18"
#define SPEED_LEVEL3 "30"

#define POWER_LEVEL1 "40"
#define POWER_LEVEL2 "45"
#define POWER_LEVEL3 "50"
#define POWER_LEVEL4 "55"
#define POWER_LEVEL5 "60"
#define POWER_LEVEL6 "80"
#define POWER_LEVEL7 "100"


/**
 * @brief 超级电容电压
 * 
 */
volatile char super_cap_power[30] = "CAP POWER: 00 V";

/**
 * @brief 枪口速度上限
 * 
 */
const char speed_level_1[30] = "Speed Level: 1, " SPEED_LEVEL1 " m/s";
const char speed_level_2[30] = "Speed Level: 2, " SPEED_LEVEL2 " m/s";
const char speed_level_3[30] = "Speed Level: 3, " SPEED_LEVEL3 " m/s";

/**
 * @brief 底盘功率
 * 
 */
const char power_level_1[30] = "Power Level: 1, " POWER_LEVEL1 " W ";
const char power_level_2[30] = "Power Level: 2, " POWER_LEVEL2 " W ";
const char power_level_3[30] = "Power Level: 3, " POWER_LEVEL3 " W ";
const char power_level_4[30] = "Power Level: 4, " POWER_LEVEL4 " W ";
const char power_level_5[30] = "Power Level: 5, " POWER_LEVEL5 " W ";
const char power_level_6[30] = "Power Level: 6, " POWER_LEVEL6 " W ";
const char power_level_7[30] = "Power Level: 7, " POWER_LEVEL7 " W ";

/**
 * @brief 视觉模式
 * 
 */
const char visual_mode_1[30] = "Visual Mode: VISUAL OFF ";
const char visual_mode_2[30] = "Visual Mode: VISUAL ON  ";
const char visual_mode_3[30] = "Visual Mode: BIG BUFF  ";
const char visual_mode_4[30] = "Visual Mode: SMALL BUFF";


/**
 * @brief 弹仓开合
 * 
 */
const char magazine_close[30] = "MAGAZINE: CLOSE";
const char magazine_open[30] = "MAGAZINE: OPEN ";

/**
 * @brief 增益数据
 * 
 */
const char attack_buff_on[30] = "BUFF: ON";
const char attack_buff_off[30] = "BUFF: OFF";

/**
 * @brief 小陀螺开启
 * 
 */
volatile char spin_on[30] =  "SPIN: ON  MAG: ";
volatile char spin_off[30] = "SPIN: OFF MAG: ";

/**
 * @brief 超级电容, 速度等级, 底盘功率等级, 弹仓开合, 是否攻击大符, 小陀螺
 * 
 */
String_Data super_cap, speed_level, power_level, magazine, attack_buff, spin, visual_mode_lebel;

Graph_Data Line1, Line2, Line3, Circle1;

Graph_Data Line_shoot_1m,Line_shoot_2m, Line_shoot_3m, Line_shoot_4m, Line_shoot_5m,Line_shoot_6m, Line_shoot_8m, Line_shoot, Line_shoot1, Line_shoot2;
//基地图样
Graph_Data Line_home_1,Line_home_2,Line_home_3,Line_home_4,Line_home_5;
//能量机关
Graph_Data Line_buff_1,Line_buff_2,Line_buff_3,Line_buff_4,Line_buff_5;
//飞泼增益
Graph_Data Line_slope_1,Line_slope_2,Line_slope_3,Line_slope_4,Line_slope_5;

ADD_GRAPH(test, String_Data);



extern int date_middle;

void ui_task(void const * argument)
{
	/* 初始化队列 */
	int i = 0;
	bool_t ui_flag=0;
	queue_init(&queue);
	set_send_func(&queue, usart6_transmit);
	set_state_func(&queue, getState);
	
	UI_Delete(UI_Data_Del_ALL,9);
	send(&queue); //发送数据
	
	osDelay(100);
	for(;;){
		if(rc_ctrl.key.v & UI_SWITCH)
		ui_flag=1;
		if(ui_flag==1)
		{
			i = 0;
			ui_init();
			while(i < 3){
					UI_ReFresh(2, &Line1,&Line2);
					UI_ReFresh(1, &Line3);
//				
					send(&queue);
					osDelay(100);
				
					UI_ReFresh(5, &Line_shoot_1m, &Line_shoot_2m,&Line_shoot_3m,&Line_shoot_4m,&Line_shoot_5m);
					UI_ReFresh(2, &Line_shoot_6m, &Line_shoot_8m);
					UI_ReFresh(1, &Line_shoot);
				
					send(&queue);
					osDelay(100);

				
//					UI_ReFresh(5, &Line_slope_1, &Line_slope_2, &Line_slope_3, &Line_slope_4, &Line_slope_5);
					UI_ReFresh(1, &Circle1);
					send(&queue);
					osDelay(100);

					Char_ReFresh(&super_cap);
					Char_ReFresh(&speed_level);
					Char_ReFresh(&power_level);

					send(&queue);
					osDelay(100);

					Char_ReFresh(&attack_buff);
					Char_ReFresh(&spin);
					//Char_ReFresh(&magazine);
				
					send(&queue);
					osDelay(100);

					Char_ReFresh(&visual_mode_lebel);
					
					send(&queue);
					osDelay(UI_SEND_PERIOD);
					i++;
				}
			ui_flag=0;
		}
		/* 队列发送 */

		refresh_ui();

		Char_ReFresh(&super_cap);
		Char_ReFresh(&speed_level);
		Char_ReFresh(&power_level);

		send(&queue);
		osDelay(UI_SEND_PERIOD);

		Char_ReFresh(&attack_buff);
		Char_ReFresh(&spin);
		//Char_ReFresh(&magazine);	
		
		send(&queue);
		osDelay(UI_SEND_PERIOD);
		
		Char_ReFresh(&visual_mode_lebel);
		//UI_ReFresh(1, &Circle1);
		UI_ReFresh(5, &Line_shoot_1m, &Line_shoot_2m,&Line_shoot_3m,&Line_shoot_4m,&Line_shoot_5m);
		UI_ReFresh(2, &Line_shoot_6m, &Line_shoot_8m);
		UI_ReFresh(1, &Line_shoot);
				
//		UI_ReFresh(5, &Line_home_1, &Line_home_2, &Line_home_3, &Line_home_4, &Line_home_5);
//		UI_ReFresh(5, &Line_buff_1, &Line_buff_2, &Line_buff_3, &Line_buff_4, &Line_buff_5);
//		UI_ReFresh(5, &Line_slope_1, &Line_slope_2, &Line_slope_3, &Line_slope_5, &Line_slope_5);
		
		send(&queue);
		osDelay(UI_SEND_PERIOD);
	}
}

uint8_t usart6_transmit(uint8_t *data, uint16_t len)
{
	if (__HAL_DMA_GET_COUNTER(&hdma_usart6_tx) != 0) {
		return 0;
	}
	HAL_UART_Transmit_DMA(&huart6, data, len);
	return len;
}

queue_state getState(void)
{
	if(1) return UNLOCKED;
	else return UNLOCKED;
}

static void update_mag()
{
	if (mag_flag) {
		memcpy((void*)(spin_on + 15), "ON \0", 4);
		memcpy((void*)(spin_off + 15), "ON \0", 4);
	} else {
		memcpy((void*)(spin_on + 15), "OFF \0", 5);
		memcpy((void*)(spin_off + 15), "OFF \0", 5);
	}
}

void ui_init()
{
	UI_Delete(UI_Data_Del_ALL,9);
	send(&queue); //发送数据
	osDelay(100);

	//super_cap, speed_level, power_level, magazine, attack_buff, spin, visual
	memset(&super_cap, 0, sizeof(String_Data));
	memset(&speed_level, 0, sizeof(String_Data));
	memset(&power_level, 0, sizeof(String_Data));
	memset(&magazine, 0, sizeof(String_Data));
	memset(&attack_buff, 0, sizeof(String_Data));
	memset(&spin, 0, sizeof(String_Data));
	memset(&visual_mode_lebel, 0, sizeof(String_Data));
	

	refresh_supercap(&super_cap_measure);

	Char_Draw(&super_cap, "000", UI_Graph_ADD, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen((const char*)super_cap_power), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS, (const char*)super_cap_power);
	
	Char_Draw(&speed_level, "001", UI_Graph_ADD, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(speed_level_1), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 1 * STR_Y_OFFSET, speed_level_1);
	
	Char_Draw(&power_level, "002", UI_Graph_ADD, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(power_level_1), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 2 * STR_Y_OFFSET, power_level_1);
	
	//Char_Draw(&magazine, "pwd", UI_Graph_ADD, 7, UI_Color_Cyan, UI_FONT_SIZE,
	//	strlen(magazine_open), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 3 * STR_Y_OFFSET, magazine_open);
	
	Char_Draw(&attack_buff, "004", UI_Graph_ADD, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(attack_buff_off), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 4 * STR_Y_OFFSET, attack_buff_off);
	
	update_mag();
	Char_Draw(&spin, "005", UI_Graph_ADD, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen((const char*)spin_off), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 5 * STR_Y_OFFSET, (const char*)spin_off);
		
	Char_Draw(&visual_mode_lebel, "006", UI_Graph_ADD, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(visual_mode_1), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 6 * STR_Y_OFFSET, visual_mode_1);

	Line_Draw(&Line1, "007", UI_Graph_ADD, 9, UI_Color_White, TRACE_S_LINE_WIDTH,
		X_CENTER - START_X_OFFSET, START_Y_POS, X_CENTER - END_X_OFFSET, END_Y_POS);
		
  Line_Draw(&Line2, "008", UI_Graph_ADD, 9, UI_Color_White, TRACE_S_LINE_WIDTH,
		X_CENTER + START_X_OFFSET, START_Y_POS, X_CENTER + END_X_OFFSET, END_Y_POS);
		
	Line_Draw(&Line3, "009", UI_Graph_ADD, 9, UI_Color_White, TRACE_S_LINE_WIDTH,
		X_CENTER - START_X_OFFSET, START_Y_POS, X_CENTER + START_X_OFFSET, START_Y_POS);
		
	Circle_Draw(&Circle1, "010", UI_Graph_ADD , 6, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
                 X_CENTER, Y_CENTER, 10);
//	//基地图样，初始值为细的	
//	Line_Draw(&Line_home_1, "ho1", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1528, 678, 1528, 624);
//	Line_Draw(&Line_home_2, "ho2", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1493, 618, 1500, 682);
//	Line_Draw(&Line_home_3, "ho3", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1563, 618, 1556, 682);
//	Line_Draw(&Line_home_4, "ho4", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1500, 682, 1556, 682);
//	Line_Draw(&Line_home_5, "ho5", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1493, 618, 1563, 618);
//		
//	//能量机关图样
//	Line_Draw(&Line_buff_1, "bu1", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1672, 651, 1672, 707);
//	Line_Draw(&Line_buff_2, "bu2", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1672, 651, 1715, 676);
//	Line_Draw(&Line_buff_3, "bu3", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1672, 651, 1629, 676);
//	Line_Draw(&Line_buff_4, "bu4", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1672, 651, 1698, 628);
//	Line_Draw(&Line_buff_5, "bu5", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1672, 651, 1610, 628);
//	
//	//飞坡增益图样
//	Line_Draw(&Line_slope_1, "sl1", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1488, 510, 1575, 510);
//	Line_Draw(&Line_slope_2, "sl2", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1488, 510, 1557, 562);
//	Line_Draw(&Line_slope_3, "sl3", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1481, 534, 1531, 578);
//	Line_Draw(&Line_slope_4, "sl4", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1531, 578, 1513, 577);
//	Line_Draw(&Line_slope_5, "sl5", UI_Graph_ADD, 8, UI_Color_Green, TRACE_S_LINE_WIDTH,
//		1531, 578, 1529, 561);

//初始15m/s 弹速
	Line_Draw(&Line_shoot, "shm", UI_Graph_ADD, 9, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER, Y_CENTER, X_CENTER, 300);
		
	Line_Draw(&Line_shoot_1m, "s1m", UI_Graph_ADD, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER+40, 444, X_CENTER-40, 444);//1m
	
	Line_Draw(&Line_shoot_2m, "s2m", UI_Graph_ADD, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER+35, 440, X_CENTER-35, 440);//2m
		
	Line_Draw(&Line_shoot_3m, "s3m", UI_Graph_ADD, 4, UI_Color_Orange, TRACE_C_LINE_WIDTH,
		X_CENTER+30, 446, X_CENTER-30, 446);//3m

	Line_Draw(&Line_shoot_4m, "s4m", UI_Graph_ADD, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER+25, 448, X_CENTER-25, 448);//4m
		
	Line_Draw(&Line_shoot_5m, "s5m", UI_Graph_ADD, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER+20, 417, X_CENTER-20, 417);//5m

	Line_Draw(&Line_shoot_6m, "s6m", UI_Graph_ADD, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER+15, 382, X_CENTER-15, 382);//6m
		
	Line_Draw(&Line_shoot_8m, "s8m", UI_Graph_ADD, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER+10, 323, X_CENTER-10, 323);//8m
		
//	Line_Draw(&Line_shoot1, "sh1", UI_Graph_ADD, 9, UI_Color_Orange, TRACE_S_LINE_WIDTH,
//		X_CENTER+20, 410, X_CENTER, 267);
//	Line_Draw(&Line_shoot2, "sh2", UI_Graph_ADD, 9, UI_Color_Orange, TRACE_S_LINE_WIDTH,
//		X_CENTER-20, 410, X_CENTER, 267);
				
}

void refresh_supercap(super_cap_measure_t* supercap)
{
	super_cap_power[11] = (char)((supercap->CapVot / 10) + 48);
	super_cap_power[12] = (char)((supercap->CapVot % 10) + 48);
}

void refresh_hurt_data(ext_robot_hurt_t* robothurt)
{
	
}
	
void refresh_ui(void)
{
	refresh_supercap(&super_cap_measure);

	if(super_cap_measure.CapVot<=18)
		Char_Draw(&super_cap, "000", UI_Graph_Change, 7, UI_Color_Main, UI_FONT_SIZE,
			strlen((const char*)super_cap_power), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS, (const char*)super_cap_power);
	else
		Char_Draw(&super_cap, "000", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
			strlen((const char*)super_cap_power), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS, (const char*)super_cap_power);
	
	if (shoot.speed_level == 0) {
		Char_Draw(&speed_level, "001", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(speed_level_1), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 1 * STR_Y_OFFSET, speed_level_1);
		//15m/s的射表
			Line_Draw(&Line_shoot, "shm", UI_Graph_Change, 9, UI_Color_Orange, TRACE_S_LINE_WIDTH,
		X_CENTER, Y_CENTER, X_CENTER, 300);
		
		Line_Draw(&Line_shoot_1m, "s1m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
			Y_CENTER+50, 965, Y_CENTER-50, 965);//1m
		
		Line_Draw(&Line_shoot_1m, "s1m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
			X_CENTER+40, 415, X_CENTER-40, 415);//1m
		
		Line_Draw(&Line_shoot_2m, "s2m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
			X_CENTER+35, 418, X_CENTER-35, 418);//2m
			
		Line_Draw(&Line_shoot_3m, "s3m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_C_LINE_WIDTH,
			X_CENTER+30, 400, X_CENTER-30, 400);//3m

		Line_Draw(&Line_shoot_4m, "s4m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
			X_CENTER+25, 394, X_CENTER-25, 394);//4m
			
		Line_Draw(&Line_shoot_5m, "s5m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
			X_CENTER+20, 372, X_CENTER-20, 372);//5m

		Line_Draw(&Line_shoot_6m, "s6m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
			X_CENTER+15, 342, X_CENTER-15, 342);//6m
			
		Line_Draw(&Line_shoot_8m, "s7m", UI_Graph_Change, 4, UI_Color_Orange, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 315, X_CENTER-10, 315);//7m
		
	} else if (shoot.speed_level == 1) {
		Char_Draw(&speed_level, "001", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(speed_level_2), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 1 * STR_Y_OFFSET, speed_level_2);
		
		//18m/s的射表
			Line_Draw(&Line_shoot, "shm", UI_Graph_Change, 9, UI_Color_Purplish_red, TRACE_S_LINE_WIDTH,
		X_CENTER, Y_CENTER, X_CENTER, 300);
		
		Line_Draw(&Line_shoot_1m, "s1m", UI_Graph_Change, 4, UI_Color_Purplish_red, TRACE_S_LINE_WIDTH,
			X_CENTER+40, 429, X_CENTER-40, 429);//1m
		
		Line_Draw(&Line_shoot_2m, "s2m", UI_Graph_Change, 4, UI_Color_Purplish_red, TRACE_S_LINE_WIDTH,
			X_CENTER+35, 470, X_CENTER-35, 470);//2m
			
		Line_Draw(&Line_shoot_3m, "s3m", UI_Graph_Change, 4, UI_Color_Purplish_red, TRACE_C_LINE_WIDTH,
			X_CENTER+30, 459, X_CENTER-30, 459);//3m

		Line_Draw(&Line_shoot_4m, "s4m", UI_Graph_Change, 4, UI_Color_Purplish_red, TRACE_S_LINE_WIDTH,
			X_CENTER+25, 393, X_CENTER-25, 393);//4m
			
		Line_Draw(&Line_shoot_5m, "s5m", UI_Graph_Change, 4, UI_Color_Purplish_red, TRACE_S_LINE_WIDTH,
			X_CENTER+20, 436, X_CENTER-20, 436);//5m

		Line_Draw(&Line_shoot_6m, "s6m", UI_Graph_Change, 4, UI_Color_Purplish_red, TRACE_S_LINE_WIDTH,
			X_CENTER+15, 435, X_CENTER-15, 435);//6m
			
		Line_Draw(&Line_shoot_8m, "s8m", UI_Graph_Change, 4, UI_Color_Purplish_red, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 406, X_CENTER-10, 406);//8m
		
	} else if (shoot.speed_level == 2) {
		Char_Draw(&speed_level, "001", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(speed_level_3), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 1 * STR_Y_OFFSET, speed_level_3);
		
		//30m/s的射表
		Line_Draw(&Line_shoot, "shm", UI_Graph_Change, 9, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
		X_CENTER, Y_CENTER, X_CENTER, 300);
		
		Line_Draw(&Line_shoot_1m, "s1m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+40, 474, X_CENTER-40, 474);//1m
		
		Line_Draw(&Line_shoot_2m, "s2m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+35, 490, X_CENTER-35, 490);//2m
			
		Line_Draw(&Line_shoot_3m, "s3m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_C_LINE_WIDTH,
			X_CENTER+30, 491, X_CENTER-30, 491);//3m

		Line_Draw(&Line_shoot_4m, "s4m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+25, 495, X_CENTER-25, 495);//4m
			
		Line_Draw(&Line_shoot_5m, "s5m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+20, 494, X_CENTER-20, 494);//5m

		Line_Draw(&Line_shoot_6m, "s6m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+15, 493, X_CENTER-15, 493);//6m
			
		Line_Draw(&Line_shoot_8m, "s7m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 489, X_CENTER-10, 489);//7m
			
		Line_Draw(&Line_shoot_8m, "s8m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 484, X_CENTER-10, 484);//8m
			
			Line_Draw(&Line_shoot_8m, "s9m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 477, X_CENTER-10, 477);//9m
			
			Line_Draw(&Line_shoot_8m, "s10m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 475, X_CENTER-10, 475);//10m

			Line_Draw(&Line_shoot_8m, "s11m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 467, X_CENTER-10, 467);//11m
			
			Line_Draw(&Line_shoot_8m, "s11m", UI_Graph_Change, 4, UI_Color_Yellow, TRACE_S_LINE_WIDTH,
			X_CENTER+10, 457, X_CENTER-10, 457);//12m
	
	}
	
#define XX(_val, _level_)	\
	case _val:				\
	Char_Draw(&power_level, "002", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,	\
		strlen(power_level_##_level_), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 2 * STR_Y_OFFSET, power_level_##_level_);	\
	break

	switch (date_middle) {
		XX(40, 1);
		XX(45, 2);
		XX(50, 3);
		XX(55, 4);
		XX(60, 5);
		XX(80, 6);
		XX(100, 7);
		default:
			break;
	}

#undef XX

	/*if ((mag_flag)) {
		Char_Draw(&magazine, "pwd", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
			strlen(magazine_open), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 3 * STR_Y_OFFSET, magazine_open);
	} else {
		Char_Draw(&magazine, "pwd", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
			strlen(magazine_close), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 3 * STR_Y_OFFSET, magazine_close);
	}*/
	
//	if(ext_buff.power_rune_buff == 1)
		
	Char_Draw(&attack_buff, "004", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(attack_buff_off), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 4 * STR_Y_OFFSET, attack_buff_off);
	update_mag();
	if (!(rc_ctrl.key.v & KEY_PRESSED_OFFSET_SHIFT)) {
		Char_Draw(&spin, "005", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
			strlen((const char*)spin_off), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 5 * STR_Y_OFFSET, (const char*)spin_off);
	} else {
		Char_Draw(&spin, "005", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
			strlen((const char*)spin_on), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 5 * STR_Y_OFFSET, (const char*)spin_on);
	}
		//视觉mode更新
	if(visual_leave == 0)
	{
		Char_Draw(&visual_mode_lebel, "006", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(visual_mode_1), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 6 * STR_Y_OFFSET, visual_mode_1);
	}
	else if(visual_leave == 1)
	{
		Char_Draw(&visual_mode_lebel, "006", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(visual_mode_2), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 6 * STR_Y_OFFSET, visual_mode_2);
	}
	else if(visual_leave == 2)
	{
		Char_Draw(&visual_mode_lebel, "006", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(visual_mode_3), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 6 * STR_Y_OFFSET, visual_mode_3);
	}
	else if(visual_leave == 3)
	{
		Char_Draw(&visual_mode_lebel, "006", UI_Graph_Change, 7, UI_Color_Cyan, UI_FONT_SIZE,
		strlen(visual_mode_4), UI_LINE_WIDTH, STR_BASE_X_POS, STR_BASE_Y_POS + 6 * STR_Y_OFFSET, visual_mode_4);
	}
	//预测点更新
		Circle_Draw(&Circle1, "010", UI_Graph_Change , 6, UI_Color_Yellow, TRACE_LINE_WIDTH,
                 X_CENTER + (gimbal_ctrl.add_yaw-gimbal_ctrl.gimbal_yaw_motor.absolute_angle)*ANGLE_TO_PIXEL , Y_CENTER - gimbal_ctrl.add_pitch*ANGLE_TO_PIXEL , 10);
		
//	//表示条件可能有问题
//	if(ext_rfid_status.rfid_status & 0x01)
//	{
//		//基地图样，初始值为细的	
//		Line_Draw(&Line_home_1, "home_1", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1528, 678, 1528, 624);
//		Line_Draw(&Line_home_2, "home_2", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1493, 618, 1500, 682);
//		Line_Draw(&Line_home_3, "home_3", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1563, 618, 1556, 682);
//		Line_Draw(&Line_home_4, "home_4", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1500, 682, 1556, 682);
//		Line_Draw(&Line_home_5, "home_5", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1493, 618, 1563, 618);
//	}
//	else
//	{
//		//基地图样，初始值为细的	
//		Line_Draw(&Line_home_1, "home_1", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1528, 678, 1528, 624);
//		Line_Draw(&Line_home_2, "home_2", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1493, 618, 1500, 682);
//		Line_Draw(&Line_home_3, "home_3", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1563, 618, 1556, 682);
//		Line_Draw(&Line_home_4, "home_4", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1500, 682, 1556, 682);
//		Line_Draw(&Line_home_5, "home_5", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1493, 618, 1563, 618);
//	}
//	
//	if(ext_rfid_status.rfid_status & 0x02)
//	{
//					//能量机关图样
//		Line_Draw(&Line_buff_1, "buff_1", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1672, 651, 1672, 707);
//		Line_Draw(&Line_buff_2, "buff_2", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1672, 651, 1715, 676);
//		Line_Draw(&Line_buff_3, "buff_3", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1672, 651, 1629, 676);
//		Line_Draw(&Line_buff_4, "buff_4", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1672, 651, 1698, 628);
//		Line_Draw(&Line_buff_5, "buff_5", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1672, 651, 1610, 628);
//	}
//	else
//	{
//				//能量机关图样
//		Line_Draw(&Line_buff_1, "buff_1", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1672, 651, 1672, 707);
//		Line_Draw(&Line_buff_2, "buff_2", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1672, 651, 1715, 676);
//		Line_Draw(&Line_buff_3, "buff_3", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1672, 651, 1629, 676);
//		Line_Draw(&Line_buff_4, "buff_4", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1672, 651, 1698, 628);
//		Line_Draw(&Line_buff_5, "buff_5", UI_Graph_Change, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1672, 651, 1610, 628);
//	}
//	
//	if(ext_rfid_status.rfid_status & 0x03)
//	{
//				//飞坡增益图样
//		Line_Draw(&Line_slope_1, "slope_1", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1488, 510, 1575, 510);
//		Line_Draw(&Line_slope_2, "slope_2", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1488, 510, 1557, 562);
//		Line_Draw(&Line_slope_3, "slope_3", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1481, 534, 1531, 578);
//		Line_Draw(&Line_slope_4, "slope_4", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1531, 578, 1513, 577);
//		Line_Draw(&Line_slope_5, "slope_5", UI_Graph_Change, 9, UI_Color_Green, TRACE_C_LINE_WIDTH,
//			1531, 578, 1529, 561);
//	}
//	else
//	{
//				//飞坡增益图样
//		Line_Draw(&Line_slope_1, "slope_1", UI_Graph_ADD, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1488, 510, 1575, 510);
//		Line_Draw(&Line_slope_2, "slope_2", UI_Graph_ADD, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1488, 510, 1557, 562);
//		Line_Draw(&Line_slope_3, "slope_3", UI_Graph_ADD, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1481, 534, 1531, 578);
//		Line_Draw(&Line_slope_4, "slope_4", UI_Graph_ADD, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1531, 578, 1513, 577);
//		Line_Draw(&Line_slope_5, "slope_5", UI_Graph_ADD, 9, UI_Color_Green, TRACE_S_LINE_WIDTH,
//			1531, 578, 1529, 561);
//	}
}
