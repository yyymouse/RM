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

/*��������bug, ����
//#pragma pack(1)                           //?1????
*/

#define NULL 0
#define __FALSE 100

/****************************????*********************/
#define UI_SOF 0xA5
/****************************CMD_ID********************/
#define UI_CMD_Robo_Exchange 0x0301
/****************************��������ID********************/
#define UI_Data_ID_Del 0x100
#define UI_Data_ID_Draw1 0x101
#define UI_Data_ID_Draw2 0x102
#define UI_Data_ID_Draw5 0x103
#define UI_Data_ID_Draw7 0x104
#define UI_Data_ID_DrawChar 0x110
/****************************�췽������ID********************/
#define UI_Data_RobotID_RHero 1
#define UI_Data_RobotID_REngineer 2
#define UI_Data_RobotID_RStandard1 3
#define UI_Data_RobotID_RStandard2 4
#define UI_Data_RobotID_RStandard3 5
#define UI_Data_RobotID_RAerial 6
#define UI_Data_RobotID_RSentry 7
#define UI_Data_RobotID_RRadar 9
/****************************����������ID********************/
#define UI_Data_RobotID_BHero 101
#define UI_Data_RobotID_BEngineer 102
#define UI_Data_RobotID_BStandard1 103
#define UI_Data_RobotID_BStandard2 104
#define UI_Data_RobotID_BStandard3 105
#define UI_Data_RobotID_BAerial 106
#define UI_Data_RobotID_BSentry 107
#define UI_Data_RobotID_BRadar 109
/**************************�췽�ͻ���ID************************/
#define UI_Data_ClientID_RHero 0x0101
#define UI_Data_ClientID_REngineer 0x0102
#define UI_Data_ClientID_RStandard1 0x0103
#define UI_Data_ClientID_RStandard2 0x0104
#define UI_Data_ClientID_RStandard3 0x0105
#define UI_Data_ClientID_RAerial 0x0106
/***************************�����ͻ���ID***********************/
#define UI_Data_ClientID_BHero 0x0165
#define UI_Data_ClientID_BEngineer 0x0166
#define UI_Data_ClientID_BStandard1 0x0167
#define UI_Data_ClientID_BStandard2 0x0168
#define UI_Data_ClientID_BStandard3 0x0169
#define UI_Data_ClientID_BAerial 0x016A
/***************************ɾ������***************************/
#define UI_Data_Del_NoOperate 0
#define UI_Data_Del_Layer 1
#define UI_Data_Del_ALL 2
/***************************ͼ�����ò���********************/
#define UI_Graph_NONE 0
#define UI_Graph_ADD 1
#define UI_Graph_Change 2
#define UI_Graph_Del 3
/***************************ͼ������********************/
#define UI_Graph_Line 0      //ֱ��
#define UI_Graph_Rectangle 1 //����
#define UI_Graph_Circle 2    //Բ
#define UI_Graph_Ellipse 3   //��Բ
#define UI_Graph_Arc 4       //��
#define UI_Graph_Float 5     //������
#define UI_Graph_Int 6       //����
#define UI_Graph_Char 7      //�ַ�
/***************************��ɫ********************/
#define UI_Color_Main 0         //������ɫ
#define UI_Color_Yellow 1       //��ɫ
#define UI_Color_Green 2        //��ɫ
#define UI_Color_Orange 3       //��ɫ
#define UI_Color_Purplish_red 4 //�Ϻ�
#define UI_Color_Pink 5         //��ɫ
#define UI_Color_Cyan 6         //��ɫ
#define UI_Color_Black 7        //��ɫ
#define UI_Color_White 8        //��ɫ

#define Robot_ID UI_Data_RobotID_RStandard2   //��������ID
#define Client_ID UI_Data_ClientID_RStandard2 //��Ӧ�ͻ���ID

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
   u8 SOF;          // SOF��0xA5
   u16 Data_Length; //���ݰ����� / �ֽ�
   u8 Seq;          //�����
   u8 CRC8;         // CRC8У����
   u16 CMD_ID;      // cmd����ID
} UI_Packhead;      //���ݰ�ͷ

typedef __packed struct
{
   u16 Data_ID;     //����ID
   u16 Sender_ID;   //������ID
   u16 Receiver_ID; //������ID
} UI_Data_Operate;

typedef __packed struct
{
   u8 Delete_Operate;
   u8 Layer;
} UI_Data_Delete;

// 15�ֽڸ�����
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

// 15���ֽ�
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
   uint8_t cnt;               //������
   uint8_t checkSum;          //У���
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
 * @brief ���ݻ�����ID
 * 
 * @param id 
 * @return uint16_t 
 */
uint16_t getClientIDByRobotID(uint16_t id);

/**
 * @brief ɾ��UI
 *
 * @param Del_Operate ɾ������
 * @param Del_Layer   ͼ��
 */
void UI_Delete(u8 Del_Operate, u8 Del_Layer);

/**
 * @brief ��һ����
 *
 * @param image         ͼ������
 * @param imagename     ͼ������
 * @param Graph_Operate ���еĲ���
 * @param Graph_Layer   ͼ��
 * @param Graph_Color   ��ɫ
 * @param Graph_Width   ���
 * @param Start_x       ����ʼ��x����
 * @param Start_y       ����ʼ��y����
 * @param End_x         �߽�����x����
 * @param End_y         �߽�����y����
 */
void Line_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 End_x, u32 End_y);

/**
 * @brief д��ͼ�����ݵ���������
 *
 * @param cnt  ͼ������ 1/3/5
 * @param ...  ͼ��
 * @return int
 */
int UI_ReFresh(int cnt, ...);

/**
 * @brief ��Բ
 * 
 * @param image         ͼ������
 * @param imagename     ͼ������
 * @param Graph_Operate ���еĲ���
 * @param Graph_Layer   ͼ��
 * @param Graph_Color   ��ɫ
 * @param Graph_Width   �������
 * @param Start_x       Բ��x����
 * @param Start_y       Բ��y����
 * @param Graph_Radius  �뾶
 */
void Circle_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 Graph_Radius);

/**
 * @brief ��������
 * 
 * @param image         ͼ������
 * @param imagename     ͼ������
 * @param Graph_Operate ���еĲ���
 * @param Graph_Layer   ͼ��
 * @param Graph_Color   ��ɫ
 * @param Graph_Width   �������
 * @param Start_x       ��ʼx����
 * @param Start_y       ��ʼy����
 * @param End_x         �Խ�x����
 * @param End_y         �Խ�y����
 */
void Rectangle_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 End_x, u32 End_y);

/**
 * @brief ���Ƹ�����
 * 
 * @param image         ͼ������
 * @param imagename     ͼ������
 * @param Graph_Operate ���еĲ���
 * @param Graph_Layer   ͼ��
 * @param Graph_Color   ��ɫ
 * @param Font_Size     �����С
 * @param Graph_Digit   ����ת���ַ����ĳ���
 * @param Graph_Width   �������
 * @param Start_x       ��ʼx����
 * @param Start_y       ��ʼy����
 * @param Graph_Float   ������
 */
void Float_Draw(String_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Font_Size, u32 Graph_Digit, u32 Graph_Width, u32 Start_x, u32 Start_y, float Graph_Float);

/**
 * @brief �����ַ���
 * 
 * @param image         ͼ������
 * @param imagename     ͼ������
 * @param Graph_Operate ���еĲ���
 * @param Graph_Layer   ͼ��
 * @param Graph_Color   ��ɫ
 * @param Font_Size     �����С
 * @param Graph_Digit   �ַ�������
 * @param Graph_Width   �������
 * @param Start_x       ��ʼx����
 * @param Start_y       ��ʼy����
 * @param Char_Data     �ַ�������
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
 * @brief ����
 * 
 * @param image            ͼ������
 * @param imagename        ͼ������
 * @param Graph_Operate    ���еĲ���
 * @param Graph_Layer      ͼ��
 * @param Graph_Color      ��ɫ
 * @param Graph_StartAngle ��ʼ�Ƕ�
 * @param Graph_EndAngle   ��ֹ�Ƕ�
 * @param Graph_Width      �������
 * @param Start_x          Բ��x����
 * @param Start_y          Բ��y����
 * @param x_Length         x�볤�᳤��
 * @param y_Length         y�볤�᳤��
 */
void Arc_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_StartAngle, u32 Graph_EndAngle, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 x_Length, u32 y_Length);

void StaticUI_Init(void);
void DynamicUI_Init(void);
void ReFresh_SuperCap_Power(void);
void Float2String(float f, char *buff, uint8_t buffsize);

#endif
