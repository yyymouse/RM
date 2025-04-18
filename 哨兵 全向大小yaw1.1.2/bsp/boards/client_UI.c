#include "client_UI.h"

Graph_Data empty_frame = {
	.color = UI_Color_White,
	.graphic_name = "nul",
	.graphic_type = UI_Graph_ADD,
	.width = 1,
	.layer = 1,
	.start_x = 0,
	.start_y = 0,
	.end_x = 0,
	.end_y = 0};

unsigned char UI_Seq;

data_queue queue;

void UI_Delete(u8 Del_Operate, u8 Del_Layer)
{

    unsigned char *framepoint;
    u16 frametail = 0xFFFF;

    UI_Packhead framehead;
    UI_Data_Operate datahead;
    UI_Data_Delete del;

    framepoint = (unsigned char *)&framehead;

    framehead.SOF = UI_SOF;
    framehead.Data_Length = 8;
    framehead.Seq = UI_Seq;
    framehead.CRC8 = Get_CRC8_Check_Sum_UI(framepoint, 4, 0xFF);
    framehead.CMD_ID = UI_CMD_Robo_Exchange;

    datahead.Data_ID = UI_Data_ID_Del;
    datahead.Sender_ID = GET_ROBOT_ID();
    datahead.Receiver_ID = getClientIDByRobotID(GET_ROBOT_ID());

    del.Delete_Operate = Del_Operate;
    del.Layer = UI_Data_Del_ALL;

    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(framehead), frametail);
    framepoint = (unsigned char *)&datahead;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(datahead), frametail);
    framepoint = (unsigned char *)&del;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(del), frametail); // CRC16?????

    framepoint = (unsigned char *)&framehead;
    queue_write(&queue, framepoint, sizeof(framehead));

    framepoint = (unsigned char *)&datahead;
    queue_write(&queue, framepoint, sizeof(datahead));

    framepoint = (unsigned char *)&del;
    queue_write(&queue, framepoint, sizeof(del));

    framepoint = (unsigned char *)&frametail;
    queue_write(&queue, framepoint, sizeof(frametail));

    UI_Seq++;
}

void Line_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width,
               u32 Start_x, u32 Start_y, u32 End_x, u32 End_y)
{
    int i;
    for (i = 0; i < 3 && imagename[i] != '\0'; i++)
        image->graphic_name[i] = imagename[i];
    image->operate_type = Graph_Operate;
    image->layer = Graph_Layer;
    image->color = Graph_Color;
    image->width = Graph_Width;
    image->start_x = Start_x;
    image->start_y = Start_y;
    image->end_x = End_x;
    image->end_y = End_y;
}

void Rectangle_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color,
                    u32 Graph_Width, u32 Start_x, u32 Start_y, u32 End_x, u32 End_y)
{
    int i;
    for (i = 0; i < 3 && imagename[i] != '\0'; i++)
        image->graphic_name[i] = imagename[i];
    image->graphic_type = UI_Graph_Rectangle;
    image->operate_type = Graph_Operate;
    image->layer = Graph_Layer;
    image->color = Graph_Color;
    image->width = Graph_Width;
    image->start_x = Start_x;
    image->start_y = Start_y;
    image->end_x = End_x;
    image->end_y = End_y;
}

void Circle_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width,
                 u32 Start_x, u32 Start_y, u32 Graph_Radius)
{
    int i;
    for (i = 0; i < 3 && imagename[i] != '\0'; i++)
        image->graphic_name[i] = imagename[i];
    image->graphic_type = UI_Graph_Circle;
    image->operate_type = Graph_Operate;
    image->layer = Graph_Layer;
    image->color = Graph_Color;
    image->width = Graph_Width;
    image->start_x = Start_x;
    image->start_y = Start_y;
    image->radius = Graph_Radius;
}

void Arc_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color,
              u32 Graph_StartAngle, u32 Graph_EndAngle, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 x_Length,
              u32 y_Length)
{
    int i;

    for (i = 0; i < 3 && imagename[i] != '\0'; i++)
        image->graphic_name[i] = imagename[i];
    image->graphic_type = UI_Graph_Arc;
    image->operate_type = Graph_Operate;
    image->layer = Graph_Layer;
    image->color = Graph_Color;
    image->width = Graph_Width;
    image->start_x = Start_x;
    image->start_y = Start_y;
    image->start_angle = Graph_StartAngle;
    image->end_angle = Graph_EndAngle;
    image->end_x = x_Length;
    image->end_y = y_Length;
}

void Float_Draw(String_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Font_Size,
                u32 Graph_Digit, u32 Graph_Width, u32 Start_x, u32 Start_y, float Graph_Float)
{
    int i;

    for (i = 0; i < 3 && imagename[i] != '\0'; i++)
        image->Graph_Control.graphic_name[i] = imagename[i];
    image->Graph_Control.graphic_type = UI_Graph_Char;
    image->Graph_Control.operate_type = Graph_Operate;
    image->Graph_Control.layer = Graph_Layer;
    image->Graph_Control.color = Graph_Color;
    image->Graph_Control.width = Graph_Width;
    image->Graph_Control.start_x = Start_x;
    image->Graph_Control.start_y = Start_y;
    image->Graph_Control.start_angle = Font_Size;
    image->Graph_Control.end_angle = Graph_Digit;

    Float2String(Graph_Float, (char *)image->show_Data, 30);
}

void Char_Draw(String_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Font_Size,
               u32 Graph_Digit, u32 Graph_Width, u32 Start_x, u32 Start_y, const char *Char_Data)
{
    int i;

    for (i = 0; i < 3 && imagename[i] != '\0'; i++)
        image->Graph_Control.graphic_name[i] = imagename[i];
    image->Graph_Control.graphic_type = UI_Graph_Char;
    image->Graph_Control.operate_type = Graph_Operate;
    image->Graph_Control.layer = Graph_Layer;
    image->Graph_Control.color = Graph_Color;
    image->Graph_Control.width = Graph_Width;
    image->Graph_Control.start_x = Start_x;
    image->Graph_Control.start_y = Start_y;
    image->Graph_Control.start_angle = Font_Size;
    image->Graph_Control.end_angle = Graph_Digit;

    for (i = 0; i < Graph_Digit; i++)
    {
        image->show_Data[i] = *Char_Data;
        Char_Data++;
    }

    //计算校验和, 假如上次校验和与这次相等, 则认为数据未发生修改, 不做任何事情, 否则重置发送次数
    uint8_t checksum = 0;
    for(int i = 0 ; i < sizeof(String_Data) - 2 ; i++) {
        checksum += *(((uint8_t*)image) + i);
    }

    if (checksum != image->checkSum) {
        image->cnt = 5;
    }

    image->checkSum = checksum;
}


/*
int UI_ReFresh(int cnt, ...)
{
    int i;
    Graph_Data imageData;
    unsigned char *framepoint; //数据指针
    u16 frametail = 0xFFFF;

    UI_Packhead framehead;
    UI_Data_Operate datahead;

    va_list ap;
    va_start(ap, cnt);

    framepoint = (unsigned char *)&framehead;
    framehead.SOF = UI_SOF;
    framehead.Data_Length = 6 + cnt * 15;
    framehead.Seq = UI_Seq;
    framehead.CRC8 = Get_CRC8_Check_Sum_UI(framepoint, 4, 0xFF);
    framehead.CMD_ID = UI_CMD_Robo_Exchange;

    switch (cnt)
    {
    case 1:
        datahead.Data_ID = UI_Data_ID_Draw1;
        break;
    case 2:
        datahead.Data_ID = UI_Data_ID_Draw2;
        break;
    case 5:
        datahead.Data_ID = UI_Data_ID_Draw5;
        break;
    case 7:
        datahead.Data_ID = UI_Data_ID_Draw7;
        break;
    default:
        return (-1);
    }
    datahead.Sender_ID = Robot_ID;
    datahead.Receiver_ID = Client_ID;

    framepoint = (unsigned char *)&framehead;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(framehead), frametail);
    framepoint = (unsigned char *)&datahead;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(datahead), frametail);

    framepoint = (unsigned char *)&framehead;
    queue_write(&queue, framepoint, sizeof(framehead));

    framepoint = (unsigned char *)&datahead;
    queue_write(&queue, framepoint, sizeof(datahead));

    for (i = 0; i < cnt; i++)
    {
        imageData = va_arg(ap, Graph_Data);
        framepoint = (unsigned char *)&imageData;
        frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(imageData), frametail);
        queue_write(&queue, framepoint, sizeof(imageData));
    }

    framepoint = (unsigned char *)&frametail;
    queue_write(&queue, framepoint, sizeof(frametail));

    va_end(ap);

    UI_Seq++;
    return 0;
}
*/
int UI_ReFresh(int cnt, ...)
{
    int i;
    Graph_Data* imageData;
    unsigned char *framepoint; //数据指针
    u16 frametail = 0xFFFF;

    UI_Packhead framehead;
    UI_Data_Operate datahead;

    va_list ap;
    va_start(ap, cnt);

    framepoint = (unsigned char *)&framehead;
    framehead.SOF = UI_SOF;
    framehead.Data_Length = 6 + cnt * 15;
    framehead.Seq = UI_Seq;
    framehead.CRC8 = Get_CRC8_Check_Sum_UI(framepoint, 4, 0xFF);
    framehead.CMD_ID = UI_CMD_Robo_Exchange;

    switch (cnt)
    {
    case 1:
        datahead.Data_ID = UI_Data_ID_Draw1;
        break;
    case 2:
        datahead.Data_ID = UI_Data_ID_Draw2;
        break;
    case 5:
        datahead.Data_ID = UI_Data_ID_Draw5;
        break;
    case 7:
        datahead.Data_ID = UI_Data_ID_Draw7;
        break;
    default:
        return (-1);
    }
    datahead.Sender_ID = GET_ROBOT_ID();
    datahead.Receiver_ID = getClientIDByRobotID(GET_ROBOT_ID());

    framepoint = (unsigned char *)&framehead;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(framehead), frametail);
    framepoint = (unsigned char *)&datahead;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(datahead), frametail);

    framepoint = (unsigned char *)&framehead;
    queue_write(&queue, framepoint, sizeof(framehead));

    framepoint = (unsigned char *)&datahead;
    queue_write(&queue, framepoint, sizeof(datahead));

    for (i = 0; i < cnt; i++)
    {
        imageData = va_arg(ap, Graph_Data*);
        framepoint = (unsigned char *)imageData;
        frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(*imageData), frametail);
        queue_write(&queue, framepoint, sizeof(*imageData));
    }

    framepoint = (unsigned char *)&frametail;
    queue_write(&queue, framepoint, sizeof(frametail));

    va_end(ap);

    UI_Seq++;
    return 0;
}

int Char_ReFresh(String_Data* string_Data)
{
    //假如发送次数计数器降为0就停止发送, 否则发送一帧, 计数器减一
    if (!string_Data->cnt) {
        return -1;
    } else {
        string_Data->cnt--;
    }
    //    int i;
    String_Data* imageData;
    unsigned char *framepoint;
    u16 frametail = 0xFFFF;

    UI_Packhead framehead;
    UI_Data_Operate datahead;
    imageData = string_Data;

    framepoint = (unsigned char *)&framehead;
    framehead.SOF = UI_SOF;
    framehead.Data_Length = 6 + 45;
    framehead.Seq = UI_Seq;
    framehead.CRC8 = Get_CRC8_Check_Sum_UI(framepoint, 4, 0xFF);
    framehead.CMD_ID = UI_CMD_Robo_Exchange;

    datahead.Data_ID = UI_Data_ID_DrawChar;

    datahead.Sender_ID = GET_ROBOT_ID();
    datahead.Receiver_ID = getClientIDByRobotID(GET_ROBOT_ID());

    framepoint = (unsigned char *)&framehead;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(framehead), frametail);
    framepoint = (unsigned char *)&datahead;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(datahead), frametail);
    framepoint = (unsigned char *)imageData;
    frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(*imageData) - 2, frametail);

    framepoint = (unsigned char *)&framehead;
    queue_write(&queue, framepoint, sizeof(framehead));

    framepoint = (unsigned char *)&datahead;
    queue_write(&queue, framepoint, sizeof(datahead));

    framepoint = (unsigned char *)imageData;
    queue_write(&queue, framepoint, sizeof(*imageData) - 2);

    framepoint = (unsigned char *)&frametail;
    queue_write(&queue, framepoint, sizeof(frametail));

    UI_Seq++;
    return 0;
}

//刷新UI的超级电容电压数据
void ReFresh_SuperCap_Power(void)
{
    // Float2String((float)(cap->CapVot),(char *)(super_Cap.show_Data), 30);
}

//静态UI初始化
void StaticUI_Init(void)
{
}

//动态UI初始化
void DynamicUI_Init(void)
{
}

//浮点数 char数组 数组大小
void Float2String(float f, char *buff, uint8_t buffsize)
{
    snprintf(buff, buffsize, "%.3f V", f);
}

uint16_t getClientIDByRobotID(uint16_t id)
{
    switch (id) {
#define XX(__id__, __ret__) \
    case __id__: return __ret__

        XX(1, 0x0101);
        XX(2, 0x0102);
        XX(3, 0x0103);
        XX(4, 0x0104);
        XX(5, 0x0105);
        XX(6, 0x0106);

        XX(101, 0x0165);
        XX(102, 0x0166);
        XX(103, 0x0167);
        XX(104, 0x0168);
        XX(105, 0x0169);
        XX(106, 0x016A);

        default:
            return 0;
#undef XX
    }
}

