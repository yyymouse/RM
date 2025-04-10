#ifndef _DATA_QUEUE_H
#define _DATA_QUEUE_H

#include <math.h>
#include <string.h>
#include <stdint.h>

#define BUFFSIZE 1024	//������

/*
	���ݶ��е�״̬, ��, ��, �Լ�����
*/
typedef enum {
	EMPTY = 0,
	FULL,
	OTHER
}data_state;

/*
	����״̬, �Ƿ�����
*/
typedef enum {
	UNLOCKED = 0,
	LOCKED
}queue_state;

typedef struct {
	uint8_t buff[BUFFSIZE];					/* ���ݻ����� */
	uint16_t head;							/* ����ͷָ�� */
	uint16_t tail;							/* ����βָ�� */
	data_state dataState;					/* ����״̬ */
	queue_state queueState;					/* ����״̬ */
	uint16_t cap;							/* �������� */
	uint8_t (*send)(uint8_t* , uint16_t);	/* �������ݺ���ָ�� */
	queue_state (*getState)(void);			/* ��ȡ����״̬����ָ�� */
}data_queue;

/**
 * @brief ���г�ʼ��
 * 
 * @param queue 	����ָ��
 */
void queue_init(data_queue *queue);

/**
 * @brief ���÷��ͺ���
 * 
 * @param queue 	����ָ��
 * @param func 		���ͺ����ĺ���ָ��
 */
void set_send_func(data_queue *queue, uint8_t (*func)(uint8_t* , uint16_t));

/**
 * @brief ���û�ȡ״ָ̬��
 * 
 * @param queue 	����ָ��
 * @param getState 	״̬��ȡ�����ĺ���ָ��
 */
void set_state_func(data_queue *queue, queue_state (*getState)(void));

/**
 * @brief �������д����
 * 
 * @param queue 	����ָ��
 * @param data 		����Դָ��
 * @param lenth 	���ݳ���
 */
void queue_write(data_queue *queue, uint8_t * data, uint16_t lenth);

/**
 * @brief ��������
 * 
 * @param queue 	����ָ��
 */
void send(data_queue *queue);

#endif //_DATA_QUEUE_H
