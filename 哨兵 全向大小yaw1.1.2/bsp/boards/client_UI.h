#ifndef __RM_Client_UI__
#define __RM_Client_UI__

#include <string.h>
#include <stdio.h>

#include "stm32f4xx.h"
#include "stdarg.h"
#include "usart.h"
#include "stm32f4xx_hal.h"
#include "referee.h"
#include "data_queue.h"

/*引起严重bug, 禁用
//#pragma pack(1)                           //?1????
*/

#define NULL 0
#define __FALSE 100

/****************************????*********************/
#define UI_SOF 0xA5
/****************************CMD_ID********************/
#define UI_CMD_Robo_Exchange 0x0301
/****************************数据内容ID********************/
#define UI_Data_ID_Del 0x100
#define UI_Data_ID_Draw1 0x101
#define UI_Data_ID_Draw2 0x102
#define UI_Data_ID_Draw5 0x103
#define UI_Data_ID_Draw7 0x104
#define UI_Data_ID_DrawChar 0x110
/****************************红方机器人ID********************/
#define UI_Data_RobotID_RHero 1
#define UI_Data_RobotID_REngineer 2
#define UI_Data_RobotID_RStandard1 3
#define UI_Data_RobotID_RStandard2 4
#define UI_Data_RobotID_RStandard3 5
#define UI_Data_RobotID_RAerial 6
#define UI_Data_RobotID_RSentry 7
#define UI_Data_RobotID_RRadar 9
/****************************蓝方机器人ID********************/
#define UI_Data_RobotID_BHero 101
#define UI_Data_RobotID_BEngineer 102
#define UI_Data_RobotID_BStandard1 103
#define UI_Data_RobotID_BStandard2 104
#define UI_Data_RobotID_BStandard3 105
#define UI_Data_RobotID_BAerial 106
#define UI_Data_RobotID_BSentry 107
#define UI_Data_RobotID_BRadar 109
/**************************红方客户端ID************************/
#define UI_Data_ClientID_RHero 0x0101
#define UI_Data_ClientID_REngineer 0x0102
#define UI_Data_ClientID_RStandard1 0x0103
#define UI_Data_ClientID_RStandard2 0x0104
#define UI_Data_ClientID_RStandard3 0x0105
#define UI_Data_ClientID_RAerial 0x0106
/***************************蓝方客户端ID***********************/
#define UI_Data_ClientID_BHero 0x0165
#define UI_Data_ClientID_BEngineer 0x0166
#define UI_Data_ClientID_BStandard1 0x0167
#define UI_Data_ClientID_BStandard2 0x0168
#define UI_Data_ClientID_BStandard3 0x0169
#define UI_Data_ClientID_BAerial 0x016A
/***************************删除操作***************************/
#define UI_Data_Del_NoOperate 0
#define UI_Data_Del_Layer 1
#define UI_Data_Del_ALL 2
/***************************图形配置参数********************/
#define UI_Graph_NONE 0
#define UI_Graph_ADD 1
#define UI_Graph_Change 2
#define UI_Graph_Del 3
/***************************图像类型********************/
#define UI_Graph_Line 0      //直线
#define UI_Graph_Rectangle 1 //矩形
#define UI_Graph_Circle 2    //圆
#define UI_Graph_Ellipse 3   //椭圆
#define UI_Graph_Arc 4       //弧
#define UI_Graph_Float 5     //浮点数
#define UI_Graph_Int 6       //整数
#define UI_Graph_Char 7      //字符
/***************************颜色********************/
#define UI_Color_Main 0         //红蓝主色
#define UI_Color_Yellow 1       //黄色
#define UI_Color_Green 2        //绿色
#define UI_Color_Orange 3       //橙色
#define UI_Color_Purplish_red 4 //紫红
#define UI_Color_Pink 5         //粉色
#define UI_Color_Cyan 6         //青色
#define UI_Color_Black 7        //黑色
#define UI_Color_White 8        //白色

#define Robot_ID UI_Data_RobotID_RStandard2   //本机器人ID
#define Client_ID UI_Data_ClientID_RStandard2 //对应客户端ID

#define X_CENTER        960
#define Y_CENTER        540

typedef unsigned char Uint8_t;
typedef unsigned char U8;

typedef unsigned char uint8_t;
typedef unsigned char u8;
typedef unsigned short uint16_t;
typedef unsigned short u16;
typedef unsigned int uint32_t;
typedef unsigned int u32;

typedef __packed struct
{
   u8 SOF;          // SOF码0xA5
   u16 Data_Length; //数据包长度 / 字节
   u8 Seq;          //包序号
   u8 CRC8;         // CRC8校验码
   u16 CMD_ID;      // cmd命令ID
} UI_Packhead;      //数据包头

typedef __packed struct
{
   u16 Data_ID;     //数据ID
   u16 Sender_ID;   //发送者ID
   u16 Receiver_ID; //接收者ID
} UI_Data_Operate;

typedef __packed struct
{
   u8 Delete_Operate;
   u8 Layer;
} UI_Data_Delete;

// 15字节浮点数
typedef __packed struct
{
   uint8_t graphic_name[3];
   uint32_t operate_type : 3;
   uint32_t graphic_type : 3;
   uint32_t layer : 4;
   uint32_t color : 4;
   uint32_t start_angle : 9;
   uint32_t end_angle : 9;
   uint32_t width : 10;
   uint32_t start_x : 11;
   uint32_t start_y : 11;
   float graph_Float;
} Float_Data;

// 15个字节
typedef __packed struct
{
   uint8_t graphic_name[3];
   uint32_t operate_type : 3;
   uint32_t graphic_type : 3;
   uint32_t layer : 4;
   uint32_t color : 4;
   uint32_t start_angle : 9;
   uint32_t end_angle : 9;
   uint32_t width : 10;
   uint32_t start_x : 11;
   uint32_t start_y : 11;
   uint32_t radius : 10;
   uint32_t end_x : 11;
   uint32_t end_y : 11;
} Graph_Data;

typedef __packed struct
{
   Graph_Data Graph_Control;
   uint8_t show_Data[30];
   uint8_t cnt;               //计数器
   uint8_t checkSum;          //校验和
} String_Data;

#define ADD_GRAPH(__name__, __type__)  \
   __type__ __name__

extern Graph_Data Line1, Line2;
extern String_Data upraise_mode, chassis_mode;
extern data_queue queue;

extern const uint8_t upraise_free[30];
extern const uint8_t upraise_init_[30];
extern const uint8_t upraise_get_craft[30];
extern const uint8_t upraise_exchange[30];
extern const uint8_t chassis_take_craft[30];
extern const uint8_t chassis_spin[30];

extern Graph_Data empty_frame;

/**
 * @brief 根据机器人ID
 * 
 * @param id 
 * @return uint16_t 
 */
uint16_t getClientIDByRobotID(uint16_t id);

/**
 * @brief 删除UI
 *
 * @param Del_Operate 删除操作
 * @param Del_Layer   图层
 */
void UI_Delete(u8 Del_Operate, u8 Del_Layer);

/**
 * @brief 画一条线
 *
 * @param image         图像数据
 * @param imagename     图像名称
 * @param Graph_Operate 进行的操作
 * @param Graph_Layer   图层
 * @param Graph_Color   颜色
 * @param Graph_Width   宽度
 * @param Start_x       线起始的x坐标
 * @param Start_y       线起始的y坐标
 * @param End_x         线结束的x坐标
 * @param End_y         线结束的y坐标
 */
void Line_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 End_x, u32 End_y);

/**
 * @brief 写入图像数据到缓冲区中
 *
 * @param cnt  图像数量 1/3/5
 * @param ...  图像
 * @return int
 */
int UI_ReFresh(int cnt, ...);

/**
 * @brief 画圆
 * 
 * @param image         图像数据
 * @param imagename     图像名称
 * @param Graph_Operate 进行的操作
 * @param Graph_Layer   图层
 * @param Graph_Color   颜色
 * @param Graph_Width   线条宽度
 * @param Start_x       圆心x坐标
 * @param Start_y       圆心y坐标
 * @param Graph_Radius  半径
 */
void Circle_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 Graph_Radius);

/**
 * @brief 画正方形
 * 
 * @param image         图像数据
 * @param imagename     图像名称
 * @param Graph_Operate 进行的操作
 * @param Graph_Layer   图层
 * @param Graph_Color   颜色
 * @param Graph_Width   线条宽度
 * @param Start_x       起始x坐标
 * @param Start_y       起始y坐标
 * @param End_x         对角x坐标
 * @param End_y         对角y坐标
 */
void Rectangle_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 End_x, u32 End_y);

/**
 * @brief 绘制浮点数
 * 
 * @param image         图像数据
 * @param imagename     图像名称
 * @param Graph_Operate 进行的操作
 * @param Graph_Layer   图层
 * @param Graph_Color   颜色
 * @param Font_Size     字体大小
 * @param Graph_Digit   数字转成字符串的长度
 * @param Graph_Width   线条宽度
 * @param Start_x       起始x坐标
 * @param Start_y       起始y坐标
 * @param Graph_Float   浮点数
 */
void Float_Draw(String_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Font_Size, u32 Graph_Digit, u32 Graph_Width, u32 Start_x, u32 Start_y, float Graph_Float);

/**
 * @brief 绘制字符串
 * 
 * @param image         图像数据
 * @param imagename     图像名称
 * @param Graph_Operate 进行的操作
 * @param Graph_Layer   图层
 * @param Graph_Color   颜色
 * @param Font_Size     字体大小
 * @param Graph_Digit   字符串长度
 * @param Graph_Width   线条宽度
 * @param Start_x       起始x坐标
 * @param Start_y       起始y坐标
 * @param Char_Data     字符串数据
 */
void Char_Draw(String_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Font_Size, u32 Graph_Digit, u32 Graph_Width, u32 Start_x, u32 Start_y, const char *Char_Data);
//void Char_Draw(String_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Font_Size, u32 Graph_Digit, u32 Graph_Width, u32 Start_x, u32 Start_y, char *Char_Data);
/**
 * @brief 
 * 
 * @param string_Data 
 * @return int 
 */
int Char_ReFresh(String_Data* string_Data);

/**
 * @brief 画弧
 * 
 * @param image            图像数据
 * @param imagename        图像名称
 * @param Graph_Operate    进行的操作
 * @param Graph_Layer      图层
 * @param Graph_Color      颜色
 * @param Graph_StartAngle 起始角度
 * @param Graph_EndAngle   终止角度
 * @param Graph_Width      线条宽度
 * @param Start_x          圆心x坐标
 * @param Start_y          圆心y坐标
 * @param x_Length         x半长轴长度
 * @param y_Length         y半长轴长度
 */
void Arc_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_StartAngle, u32 Graph_EndAngle, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 x_Length, u32 y_Length);

void StaticUI_Init(void);
void DynamicUI_Init(void);
void ReFresh_SuperCap_Power(void);
void Float2String(float f, char *buff, uint8_t buffsize);

#endif
