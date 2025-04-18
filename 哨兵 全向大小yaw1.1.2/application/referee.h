#ifndef __REFEREE_H
#define __REFEREE_H

#pragma pack(1)

#include "main.h"
#include <stdint.h>
#include <string.h>

/**
 * @brief 获取机器人ID
 * 
 */
#define GET_ROBOT_ID() \
    (ext_game_robot_status.robot_id)

/**
 * @brief 获取机器人等级
 * 
 */
#define GET_ROBOT_LEVEL()   \
    (ext_game_robot_status.robot_level)

/**
 * @brief 获取1号17毫米枪管热量冷却速率
 * 
 */
#define GET_17MM1_COOLING_RATE()    \
    (ext_game_robot_status.shooter_id1_17mm_cooling_rate)

/**
 * @brief 获取1号17毫米枪管热量上限
 * 
 */
#define GET_17MM1_COOLING_LIMIT()    \
    (ext_game_robot_status.shooter_id1_17mm_cooling_limit)

/**
 * @brief 获取1号17毫米枪管射速上限
 * 
 */
#define GET_17MM1_SPEED_LIMIT() \
    (ext_game_robot_status.shooter_id1_17mm_speed_limit)

/**
 * @brief 获取2号17毫米枪管热量冷却速率
 * 
 */
#define GET_17MM2_COOLING_RATE()    \
    (ext_game_robot_status.shooter_id2_17mm_cooling_rate)

/**
 * @brief 获取2号17毫米枪管热量上限
 * 
 */
#define GET_17MM2_COOLING_LIMIT()    \
    (ext_game_robot_status.shooter_id2_17mm_cooling_limit)

/**
 * @brief 获取2号17毫米枪管射速上限
 * 
 */
#define GET_17MM2_SPEED_LIMIT() \
    (ext_game_robot_status.shooter_id2_17mm_speed_limit)

/**
 * @brief 获取42毫米枪管热量冷却速率
 * 
 */
#define GET_42MM_COOLING_RATE()    \
    (ext_game_robot_status.shooter_id1_42mm_cooling_rate)

/**
 * @brief 获取42毫米枪管热量上限
 * 
 */
#define GET_42MM_COOLING_LIMIT()    \
    (ext_game_robot_status.shooter_id1_42mm_cooling_limit)

/**
 * @brief 获取42毫米枪管射速上限
 * 
 */
#define GET_42MM_SPEED_LIMIT() \
    (ext_game_robot_status.shooter_id1_42mm_speed_limit)

/**
 * @brief 获取1号17毫米枪管热量
 * 
 */
#define GET_ROBOT_17MM1_COOLING() \
    (ext_power_heat_data.shooter_id1_17mm_cooling_heat)

/**
 * @brief 获取2号17毫米枪管热量
 * 
 */
#define GET_ROBOT_17MM2_COOLING() \
    (ext_power_heat_data.shooter_id2_17mm_cooling_heat)

/**
 * @brief 获取42毫米枪管热量
 * 
 */
#define GET_ROBOT_42MM_COOLING() \
    (ext_power_heat_data.shooter_id1_42mm_cooling_heat)

/**
 * @brief 获取机器人底盘功率
 * 
 */
#define GET_ROBOT_POWER()   \
    (ext_power_heat_data.chassis_power)

typedef struct
{
    uint8_t SOF;                        //SOF码0xA5
    uint16_t Data_Length;               //数据包长度 / 字节
    uint8_t Seq;                        //包序号
    uint8_t CRC8;                   //CRC8校验码
    uint16_t CMD_ID;                    //cmd命令ID
} Data_Head;

typedef struct                  //0x0001     11     比赛状态数据，1Hz 周期发送
{
    uint8_t game_type : 4;
    uint8_t game_progress : 4;
    uint16_t stage_remain_time;
    uint64_t SyncTimeStamp;
} ext_game_status_t;

typedef struct                  //0x0002     1    比赛结果数据，比赛结束后发送
{
    uint8_t winner;
} ext_game_result_t;

typedef struct //0x0003        28        比赛机器人血量数据，1Hz 周期发送
{
    uint16_t red_1_robot_HP;    //红 1 英雄机器人血量，未上场以及罚下血量为 0
    uint16_t red_2_robot_HP;    //红 2 工程机器人血量
    uint16_t red_3_robot_HP;    // 红 3 步兵机器人血量
    uint16_t red_4_robot_HP;//红 4 步兵机器人血量
    uint16_t red_5_robot_HP;//红 5 步兵机器人血量
    uint16_t red_7_robot_HP;//红 7 哨兵机器人血量
    uint16_t red_outpost_HP;//红方前哨战血量
    uint16_t red_base_HP;//红方基地血量
    uint16_t blue_1_robot_HP;//蓝 1 英雄机器人血量
    uint16_t blue_2_robot_HP;//蓝 2 工程机器人血量
    uint16_t blue_3_robot_HP;//蓝 3 步兵机器人血量
    uint16_t blue_4_robot_HP;//蓝 4 步兵机器人血量
    uint16_t blue_5_robot_HP;//蓝 5 步兵机器人血量
    uint16_t blue_7_robot_HP;//蓝 7 哨兵机器人血量
    uint16_t blue_outpost_HP;//蓝方前哨站血量
    uint16_t blue_base_HP;//蓝方基地血量
} ext_game_robot_HP_t;

typedef struct  //0x0005        11      人工智能挑战赛加成与惩罚状态，1Hz 周期发送
{
    uint8_t F1_zone_status: 1;
    uint8_t F1_zone_buff_debuff_status: 3;
    uint8_t F2_zone_status: 1;
    uint8_t F2_zone_buff_debuff_status: 3;
    uint8_t F3_zone_status: 1;
    uint8_t F3_zone_buff_debuff_status: 3;
    uint8_t F4_zone_status: 1;
    uint8_t F4_zone_buff_debuff_status: 3;
    uint8_t F5_zone_status: 1;
    uint8_t F5_zone_buff_debuff_status: 3;
    uint8_t F6_zone_status: 1;
    uint8_t F6_zone_buff_debuff_status: 3;
    uint16_t red1_bullet_left;//红方 1 号剩余弹量
    uint16_t red2_bullet_left;//红方 2 号剩余弹量
    uint16_t blue1_bullet_left;//蓝方 1 号剩余弹量
    uint16_t blue2_bullet_left;//蓝方 2 号剩余弹量
} ext_ICRA_buff_debuff_zone_status_t;

typedef  struct  //0x0101   4     场地事件数据，1Hz 周期发送
{
    uint32_t event_type;
} ext_event_data_t;

typedef struct  //0x0102    3    场地补给站动作标识数据，动作改变后发送
{
    uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
    uint8_t supply_projectile_step;//出弹口开闭状态：0 为关闭，1 为子弹准备中，2 为子弹下落
    uint8_t supply_projectile_num;//补弹数量：
																	//50：50 颗子弹；
																	//100：100 颗子弹；
																	//150：150 颗子弹；
																	//200：200 颗子弹
} ext_supply_projectile_action_t;

typedef struct  //0x0104       2    裁判警告数据，警告发生后发送
{
    uint8_t level;
    uint8_t foul_robot_id;
} ext_referee_warning_t;

typedef struct  //0x0105     1  飞镖发射口倒计时，1Hz 周期发送
{
    uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;

typedef struct  //0x0201   数据长度15     机器人状态数据，10Hz 周期发送
{
    uint8_t robot_id;       // 本机器人 ID： 1：红方英雄机器人； 
														//2：红方工程机器人；
														//3/4/5：红方步兵机器人；
														//6：红方空中机器人；
														//7：红方哨兵机器人；
														//8：红方飞镖机器人；
														//9：红方雷达站；
														//101：蓝方英雄机器人；
														//102：蓝方工程机器人；
														//103/104/105：蓝方步兵机器人；
														//106：蓝方空中机器人；
														//107：蓝方哨兵机器人；
														//108：蓝方飞镖机器人；
														//109：蓝方雷达站。
    uint8_t  robot_level;   //等级  1：一级；2：二级；3：三级。
    uint16_t remain_HP;  // 机器人剩余血量
    uint16_t max_HP;//机器人上限血量
    uint16_t shooter_id1_17mm_cooling_rate;//机器人 1 号 17mm 枪口每秒冷却值
    uint16_t shooter_id1_17mm_cooling_limit;//机器人 1 号 17mm 枪口热量上限
    uint16_t shooter_id1_17mm_speed_limit;//机器人 1 号 17mm 枪口上限速度 单位 m/s
    uint16_t shooter_id2_17mm_cooling_rate;//机器人 2 号 17mm 枪口每秒冷却值
    uint16_t shooter_id2_17mm_cooling_limit;//机器人 2 号 17mm 枪口热量上限
    uint16_t shooter_id2_17mm_speed_limit;//机器人 2 号 17mm 枪口上限速度 单位 m/s
    uint16_t shooter_id1_42mm_cooling_rate;//机器人 42mm 枪口每秒冷却值
    uint16_t shooter_id1_42mm_cooling_limit;//机器人 42mm 枪口热量上限
    uint16_t shooter_id1_42mm_speed_limit;//机器人 42mm 枪口上限速度 单位 m/s
    uint16_t chassis_power_limit;         //机器人底盘功率限制上限
    uint8_t mains_power_gimbal_output : 1;    //主控电源输出情况：
																							//0 bit：gimbal 口输出： 1 为有 24V 输出，0 为无 24v 输出；
																							//1 bit：chassis 口输出：1 为有 24V 输出，0 为无 24v 输出；
																							//2 bit：shooter 口输出：1 为有 24V 输出，0 为无 24v 输出；
    uint8_t mains_power_chassis_output : 1;//
    uint8_t mains_power_shooter_output : 1;//
} ext_game_robot_status_t;

typedef struct  //0x0202     数据长度14    实时功率热量数据，50Hz 周期发送
{
    uint16_t chassis_volt;//底盘输出电压 单位 毫伏
    uint16_t chassis_current;//底盘输出电流 单位 毫安
    float chassis_power;//底盘输出功率 单位 W 瓦
    uint16_t chassis_power_buffer;//底盘功率缓冲 单位 J 焦耳 备注：飞坡根据规则增加至 250J
    uint16_t shooter_id1_17mm_cooling_heat;//1 号 17mm 枪口热量
    uint16_t shooter_id2_17mm_cooling_heat;//2 号 17mm 枪口热量
    uint16_t shooter_id1_42mm_cooling_heat;//42mm 枪口热量
} ext_power_heat_data_t;

typedef struct  //0x0203      16      机器人位置数据，10Hz 发送
{
    float x;//位置 x 坐标，单位 m
    float y;//位置 y 坐标，单位 m
    float z;//位置 z 坐标，单位 m
    float yaw;//位置枪口，单位度
} ext_game_robot_pos_t;

typedef struct  //0x0204    1     机器人增益数据，增益状态改变后发送
{ 
    uint8_t power_rune_buff;   //bit 0：机器人血量补血状态
                               //bit 1：枪口热量冷却加速
															 //bit 2：机器人防御加成
															 //bit 3：机器人攻击加成
} ext_buff_t;

typedef struct  //0x0205      3       空中机器人能量状态数据，10Hz 周期发送，只有空中机器人主控发送
{
    uint8_t attack_time;  //可攻击时间 单位 s。30s 递减至 0
} aerial_robot_energy_t;

typedef struct  //0x0206    1     伤害状态数据，伤害发生后发送
{
    uint8_t armor_id : 4;       
														//bit 0-3：当血量变化类型为装甲伤害，代表装甲 ID，其中数值为 0-4 号代表机器人的
														//五个装甲片，其他血量变化类型，该变量数值为 0。
														//bit 4-7：血量变化类型
														//0x0 装甲伤害扣血；
														//0x1 模块掉线扣血；
														//0x2 超射速扣血；
														//0x3 超枪口热量扣血；
														//0x4 超底盘功率扣血；
														//0x5 装甲撞击扣血
    uint8_t hurt_type : 4;
} ext_robot_hurt_t;

typedef struct  //0x0207   6     实时射击数据，子弹发射后发送
{ 
    uint8_t bullet_type;//子弹类型: 1：17mm 弹丸 2：42mm 弹丸
    uint8_t shooter_id;
												//发射机构 ID： 1：1 号 17mm 发射机构
												//2：2 号 17mm 发射机构
												//3：42mm 发射机构
    uint8_t bullet_freq;//子弹射频 单位 Hz
    float bullet_speed; //子弹射速 单位 m/s
} ext_shoot_data_t;

typedef struct  //0x0208     2    子弹剩余发送数，空中机器人以及哨兵机器人发送，1Hz 周期发送
{
    uint16_t bullet_remaining_num_17mm;//17mm 子弹剩余发射数目
    uint16_t bullet_remaining_num_42mm;//42mm 子弹剩余发射数目
    uint16_t coin_remaining_num;//剩余金币数量
} ext_bullet_remaining_t;

typedef struct  //0x0209    4   机器人 RFID 状态，1Hz 周期发送
{
    uint32_t rfid_status;
													//bit 0：基地增益点 RFID 状态；
													//bit 1：高地增益点 RFID 状态；
													//bit 2：能量机关激活点 RFID 状态；
													//bit 3：飞坡增益点 RFID 状态；
													//bit 4：前哨岗增益点 RFID 状态；
													//bit 6：补血点增益点 RFID 状态；
													//bit 7：工程机器人复活卡 RFID 状态；
} ext_rfid_status_t;

typedef struct  //0x020A     12     飞镖机器人客户端指令书，10Hz 周期发送
{
    uint8_t dart_launch_opening_status;	
																				//当前飞镖发射口的状态
																				//1：关闭；
																				//2：正在开启或者关闭中
																				//0：已经开启
    uint8_t dart_attack_target;
																//	飞镖的打击目标，默认为前哨站；
																//0：前哨站；
																//1：基地。
    uint16_t target_change_time;//切换打击目标时的比赛剩余时间，单位秒，从未切换默认为 0。
    uint16_t operate_launch_cmd_time;//最近一次操作手确定发射指令时的比赛剩余时间，单位秒, 初始值为 0
} ext_dart_client_cmd_t;

#pragma pack()

extern uint8_t referee_buff[256];

extern ext_game_status_t ext_game_status;
extern ext_game_result_t ext_game_result;
extern ext_game_robot_HP_t ext_game_robot_HP;
extern ext_ICRA_buff_debuff_zone_status_t ext_ICRA_buff_debuff_zone_status;
extern ext_event_data_t ext_event_data;
extern ext_supply_projectile_action_t ext_supply_projectile_action;
extern ext_referee_warning_t ext_referee_warning;
extern ext_dart_remaining_time_t ext_dart_remaining_time;
extern ext_game_robot_status_t ext_game_robot_status;
extern ext_power_heat_data_t ext_power_heat_data;
extern ext_game_robot_pos_t ext_game_robot_pos;
extern ext_buff_t ext_buff;
extern aerial_robot_energy_t aerial_robot_energy;
extern ext_robot_hurt_t ext_robot_hurt;
extern ext_shoot_data_t ext_shoot_data;
extern ext_bullet_remaining_t ext_bullet_remaining;
extern ext_rfid_status_t ext_rfid_status;
extern ext_dart_client_cmd_t ext_dart_client_cmd;

/**
 * @brief           根据cmd_id获取该cmd_id的数据长度
 * @author          张辉阳
 * @param cmd_id    具体要查询的cmd_id
 * @return          数据长度(字节)
 **/
uint16_t getDataLengthByCmdId(uint16_t cmd_id);

/**
 * @brief           根据cmd_id获取相应的数据存放地址指针
 * @author          张辉阳
 * @param cmd_id    具体要查询的cmd_id
 * @return          该cmd_id对应的数据首地址
 **/
uint8_t *getDataPointerById(uint16_t cmd_id);


uint8_t Get_CRC8_Check_Sum_UI(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);
uint16_t Get_CRC16_Check_Sum_UI(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);

/**
 * @brief       解析裁判系统数据
 * @author      张辉阳
 * @param head  接收数据缓冲的头指针
 * @param size  接收到数据的大小
 * @return      无
 **/
void parse(uint8_t *head, int size);

#endif
