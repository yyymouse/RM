#ifndef __REFEREE_H
#define __REFEREE_H

#pragma pack(1)

#include "main.h"
#include <stdint.h>
#include <string.h>

/**
 * @brief ��ȡ������ID
 * 
 */
#define GET_ROBOT_ID() \
    (ext_game_robot_status.robot_id)

/**
 * @brief ��ȡ�����˵ȼ�
 * 
 */
#define GET_ROBOT_LEVEL()   \
    (ext_game_robot_status.robot_level)

/**
 * @brief ��ȡ1��17����ǹ��������ȴ����
 * 
 */
#define GET_17MM1_COOLING_RATE()    \
    (ext_game_robot_status.shooter_id1_17mm_cooling_rate)

/**
 * @brief ��ȡ1��17����ǹ����������
 * 
 */
#define GET_17MM1_COOLING_LIMIT()    \
    (ext_game_robot_status.shooter_id1_17mm_cooling_limit)

/**
 * @brief ��ȡ1��17����ǹ����������
 * 
 */
#define GET_17MM1_SPEED_LIMIT() \
    (ext_game_robot_status.shooter_id1_17mm_speed_limit)

/**
 * @brief ��ȡ2��17����ǹ��������ȴ����
 * 
 */
#define GET_17MM2_COOLING_RATE()    \
    (ext_game_robot_status.shooter_id2_17mm_cooling_rate)

/**
 * @brief ��ȡ2��17����ǹ����������
 * 
 */
#define GET_17MM2_COOLING_LIMIT()    \
    (ext_game_robot_status.shooter_id2_17mm_cooling_limit)

/**
 * @brief ��ȡ2��17����ǹ����������
 * 
 */
#define GET_17MM2_SPEED_LIMIT() \
    (ext_game_robot_status.shooter_id2_17mm_speed_limit)

/**
 * @brief ��ȡ42����ǹ��������ȴ����
 * 
 */
#define GET_42MM_COOLING_RATE()    \
    (ext_game_robot_status.shooter_id1_42mm_cooling_rate)

/**
 * @brief ��ȡ42����ǹ����������
 * 
 */
#define GET_42MM_COOLING_LIMIT()    \
    (ext_game_robot_status.shooter_id1_42mm_cooling_limit)

/**
 * @brief ��ȡ42����ǹ����������
 * 
 */
#define GET_42MM_SPEED_LIMIT() \
    (ext_game_robot_status.shooter_id1_42mm_speed_limit)

/**
 * @brief ��ȡ1��17����ǹ������
 * 
 */
#define GET_ROBOT_17MM1_COOLING() \
    (ext_power_heat_data.shooter_id1_17mm_cooling_heat)

/**
 * @brief ��ȡ2��17����ǹ������
 * 
 */
#define GET_ROBOT_17MM2_COOLING() \
    (ext_power_heat_data.shooter_id2_17mm_cooling_heat)

/**
 * @brief ��ȡ42����ǹ������
 * 
 */
#define GET_ROBOT_42MM_COOLING() \
    (ext_power_heat_data.shooter_id1_42mm_cooling_heat)

/**
 * @brief ��ȡ�����˵��̹���
 * 
 */
#define GET_ROBOT_POWER()   \
    (ext_power_heat_data.chassis_power)

typedef struct
{
    uint8_t SOF;                        //SOF��0xA5
    uint16_t Data_Length;               //���ݰ����� / �ֽ�
    uint8_t Seq;                        //�����
    uint8_t CRC8;                   //CRC8У����
    uint16_t CMD_ID;                    //cmd����ID
} Data_Head;

typedef struct                  //0x0001     11     ����״̬���ݣ�1Hz ���ڷ���
{
    uint8_t game_type : 4;
    uint8_t game_progress : 4;
    uint16_t stage_remain_time;
    uint64_t SyncTimeStamp;
} ext_game_status_t;

typedef struct                  //0x0002     1    ����������ݣ�������������
{
    uint8_t winner;
} ext_game_result_t;

typedef struct //0x0003        28        ����������Ѫ�����ݣ�1Hz ���ڷ���
{
    uint16_t red_1_robot_HP;    //�� 1 Ӣ�ۻ�����Ѫ����δ�ϳ��Լ�����Ѫ��Ϊ 0
    uint16_t red_2_robot_HP;    //�� 2 ���̻�����Ѫ��
    uint16_t red_3_robot_HP;    // �� 3 ����������Ѫ��
    uint16_t red_4_robot_HP;//�� 4 ����������Ѫ��
    uint16_t red_5_robot_HP;//�� 5 ����������Ѫ��
    uint16_t red_7_robot_HP;//�� 7 �ڱ�������Ѫ��
    uint16_t red_outpost_HP;//�췽ǰ��սѪ��
    uint16_t red_base_HP;//�췽����Ѫ��
    uint16_t blue_1_robot_HP;//�� 1 Ӣ�ۻ�����Ѫ��
    uint16_t blue_2_robot_HP;//�� 2 ���̻�����Ѫ��
    uint16_t blue_3_robot_HP;//�� 3 ����������Ѫ��
    uint16_t blue_4_robot_HP;//�� 4 ����������Ѫ��
    uint16_t blue_5_robot_HP;//�� 5 ����������Ѫ��
    uint16_t blue_7_robot_HP;//�� 7 �ڱ�������Ѫ��
    uint16_t blue_outpost_HP;//����ǰ��վѪ��
    uint16_t blue_base_HP;//��������Ѫ��
} ext_game_robot_HP_t;

typedef struct  //0x0005        11      �˹�������ս���ӳ���ͷ�״̬��1Hz ���ڷ���
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
    uint16_t red1_bullet_left;//�췽 1 ��ʣ�൯��
    uint16_t red2_bullet_left;//�췽 2 ��ʣ�൯��
    uint16_t blue1_bullet_left;//���� 1 ��ʣ�൯��
    uint16_t blue2_bullet_left;//���� 2 ��ʣ�൯��
} ext_ICRA_buff_debuff_zone_status_t;

typedef  struct  //0x0101   4     �����¼����ݣ�1Hz ���ڷ���
{
    uint32_t event_type;
} ext_event_data_t;

typedef struct  //0x0102    3    ���ز���վ������ʶ���ݣ������ı����
{
    uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
    uint8_t supply_projectile_step;//�����ڿ���״̬��0 Ϊ�رգ�1 Ϊ�ӵ�׼���У�2 Ϊ�ӵ�����
    uint8_t supply_projectile_num;//����������
																	//50��50 ���ӵ���
																	//100��100 ���ӵ���
																	//150��150 ���ӵ���
																	//200��200 ���ӵ�
} ext_supply_projectile_action_t;

typedef struct  //0x0104       2    ���о������ݣ����淢������
{
    uint8_t level;
    uint8_t foul_robot_id;
} ext_referee_warning_t;

typedef struct  //0x0105     1  ���ڷ���ڵ���ʱ��1Hz ���ڷ���
{
    uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;

typedef struct  //0x0201   ���ݳ���15     ������״̬���ݣ�10Hz ���ڷ���
{
    uint8_t robot_id;       // �������� ID�� 1���췽Ӣ�ۻ����ˣ� 
														//2���췽���̻����ˣ�
														//3/4/5���췽���������ˣ�
														//6���췽���л����ˣ�
														//7���췽�ڱ������ˣ�
														//8���췽���ڻ����ˣ�
														//9���췽�״�վ��
														//101������Ӣ�ۻ����ˣ�
														//102���������̻����ˣ�
														//103/104/105���������������ˣ�
														//106���������л����ˣ�
														//107�������ڱ������ˣ�
														//108���������ڻ����ˣ�
														//109�������״�վ��
    uint8_t  robot_level;   //�ȼ�  1��һ����2��������3��������
    uint16_t remain_HP;  // ������ʣ��Ѫ��
    uint16_t max_HP;//����������Ѫ��
    uint16_t shooter_id1_17mm_cooling_rate;//������ 1 �� 17mm ǹ��ÿ����ȴֵ
    uint16_t shooter_id1_17mm_cooling_limit;//������ 1 �� 17mm ǹ����������
    uint16_t shooter_id1_17mm_speed_limit;//������ 1 �� 17mm ǹ�������ٶ� ��λ m/s
    uint16_t shooter_id2_17mm_cooling_rate;//������ 2 �� 17mm ǹ��ÿ����ȴֵ
    uint16_t shooter_id2_17mm_cooling_limit;//������ 2 �� 17mm ǹ����������
    uint16_t shooter_id2_17mm_speed_limit;//������ 2 �� 17mm ǹ�������ٶ� ��λ m/s
    uint16_t shooter_id1_42mm_cooling_rate;//������ 42mm ǹ��ÿ����ȴֵ
    uint16_t shooter_id1_42mm_cooling_limit;//������ 42mm ǹ����������
    uint16_t shooter_id1_42mm_speed_limit;//������ 42mm ǹ�������ٶ� ��λ m/s
    uint16_t chassis_power_limit;         //�����˵��̹�����������
    uint8_t mains_power_gimbal_output : 1;    //���ص�Դ��������
																							//0 bit��gimbal ������� 1 Ϊ�� 24V �����0 Ϊ�� 24v �����
																							//1 bit��chassis �������1 Ϊ�� 24V �����0 Ϊ�� 24v �����
																							//2 bit��shooter �������1 Ϊ�� 24V �����0 Ϊ�� 24v �����
    uint8_t mains_power_chassis_output : 1;//
    uint8_t mains_power_shooter_output : 1;//
} ext_game_robot_status_t;

typedef struct  //0x0202     ���ݳ���14    ʵʱ�����������ݣ�50Hz ���ڷ���
{
    uint16_t chassis_volt;//���������ѹ ��λ ����
    uint16_t chassis_current;//����������� ��λ ����
    float chassis_power;//����������� ��λ W ��
    uint16_t chassis_power_buffer;//���̹��ʻ��� ��λ J ���� ��ע�����¸��ݹ��������� 250J
    uint16_t shooter_id1_17mm_cooling_heat;//1 �� 17mm ǹ������
    uint16_t shooter_id2_17mm_cooling_heat;//2 �� 17mm ǹ������
    uint16_t shooter_id1_42mm_cooling_heat;//42mm ǹ������
} ext_power_heat_data_t;

typedef struct  //0x0203      16      ������λ�����ݣ�10Hz ����
{
    float x;//λ�� x ���꣬��λ m
    float y;//λ�� y ���꣬��λ m
    float z;//λ�� z ���꣬��λ m
    float yaw;//λ��ǹ�ڣ���λ��
} ext_game_robot_pos_t;

typedef struct  //0x0204    1     �������������ݣ�����״̬�ı����
{ 
    uint8_t power_rune_buff;   //bit 0��������Ѫ����Ѫ״̬
                               //bit 1��ǹ��������ȴ����
															 //bit 2�������˷����ӳ�
															 //bit 3�������˹����ӳ�
} ext_buff_t;

typedef struct  //0x0205      3       ���л���������״̬���ݣ�10Hz ���ڷ��ͣ�ֻ�п��л��������ط���
{
    uint8_t attack_time;  //�ɹ���ʱ�� ��λ s��30s �ݼ��� 0
} aerial_robot_energy_t;

typedef struct  //0x0206    1     �˺�״̬���ݣ��˺���������
{
    uint8_t armor_id : 4;       
														//bit 0-3����Ѫ���仯����Ϊװ���˺�������װ�� ID��������ֵΪ 0-4 �Ŵ�������˵�
														//���װ��Ƭ������Ѫ���仯���ͣ��ñ�����ֵΪ 0��
														//bit 4-7��Ѫ���仯����
														//0x0 װ���˺���Ѫ��
														//0x1 ģ����߿�Ѫ��
														//0x2 �����ٿ�Ѫ��
														//0x3 ��ǹ��������Ѫ��
														//0x4 �����̹��ʿ�Ѫ��
														//0x5 װ��ײ����Ѫ
    uint8_t hurt_type : 4;
} ext_robot_hurt_t;

typedef struct  //0x0207   6     ʵʱ������ݣ��ӵ��������
{ 
    uint8_t bullet_type;//�ӵ�����: 1��17mm ���� 2��42mm ����
    uint8_t shooter_id;
												//������� ID�� 1��1 �� 17mm �������
												//2��2 �� 17mm �������
												//3��42mm �������
    uint8_t bullet_freq;//�ӵ���Ƶ ��λ Hz
    float bullet_speed; //�ӵ����� ��λ m/s
} ext_shoot_data_t;

typedef struct  //0x0208     2    �ӵ�ʣ�෢���������л������Լ��ڱ������˷��ͣ�1Hz ���ڷ���
{
    uint16_t bullet_remaining_num_17mm;//17mm �ӵ�ʣ�෢����Ŀ
    uint16_t bullet_remaining_num_42mm;//42mm �ӵ�ʣ�෢����Ŀ
    uint16_t coin_remaining_num;//ʣ��������
} ext_bullet_remaining_t;

typedef struct  //0x0209    4   ������ RFID ״̬��1Hz ���ڷ���
{
    uint32_t rfid_status;
													//bit 0����������� RFID ״̬��
													//bit 1���ߵ������ RFID ״̬��
													//bit 2���������ؼ���� RFID ״̬��
													//bit 3����������� RFID ״̬��
													//bit 4��ǰ�ڸ������ RFID ״̬��
													//bit 6����Ѫ������� RFID ״̬��
													//bit 7�����̻����˸�� RFID ״̬��
} ext_rfid_status_t;

typedef struct  //0x020A     12     ���ڻ����˿ͻ���ָ���飬10Hz ���ڷ���
{
    uint8_t dart_launch_opening_status;	
																				//��ǰ���ڷ���ڵ�״̬
																				//1���رգ�
																				//2�����ڿ������߹ر���
																				//0���Ѿ�����
    uint8_t dart_attack_target;
																//	���ڵĴ��Ŀ�꣬Ĭ��Ϊǰ��վ��
																//0��ǰ��վ��
																//1�����ء�
    uint16_t target_change_time;//�л����Ŀ��ʱ�ı���ʣ��ʱ�䣬��λ�룬��δ�л�Ĭ��Ϊ 0��
    uint16_t operate_launch_cmd_time;//���һ�β�����ȷ������ָ��ʱ�ı���ʣ��ʱ�䣬��λ��, ��ʼֵΪ 0
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
 * @brief           ����cmd_id��ȡ��cmd_id�����ݳ���
 * @author          �Ż���
 * @param cmd_id    ����Ҫ��ѯ��cmd_id
 * @return          ���ݳ���(�ֽ�)
 **/
uint16_t getDataLengthByCmdId(uint16_t cmd_id);

/**
 * @brief           ����cmd_id��ȡ��Ӧ�����ݴ�ŵ�ַָ��
 * @author          �Ż���
 * @param cmd_id    ����Ҫ��ѯ��cmd_id
 * @return          ��cmd_id��Ӧ�������׵�ַ
 **/
uint8_t *getDataPointerById(uint16_t cmd_id);


uint8_t Get_CRC8_Check_Sum_UI(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);
uint16_t Get_CRC16_Check_Sum_UI(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);

/**
 * @brief       ��������ϵͳ����
 * @author      �Ż���
 * @param head  �������ݻ����ͷָ��
 * @param size  ���յ����ݵĴ�С
 * @return      ��
 **/
void parse(uint8_t *head, int size);

#endif
