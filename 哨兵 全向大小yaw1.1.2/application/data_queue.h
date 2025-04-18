#ifndef _DATA_QUEUE_H
#define _DATA_QUEUE_H

#include <math.h>
#include <string.h>
#include <stdint.h>

#define BUFFSIZE 1024	//缓冲区

/*
	数据队列的状态, 空, 满, 以及其他
*/
typedef enum {
	EMPTY = 0,
	FULL,
	OTHER
}data_state;

/*
	队列状态, 是否上锁
*/
typedef enum {
	UNLOCKED = 0,
	LOCKED
}queue_state;

typedef struct {
	uint8_t buff[BUFFSIZE];					/* 数据缓冲区 */
	uint16_t head;							/* 数据头指针 */
	uint16_t tail;							/* 数据尾指针 */
	data_state dataState;					/* 数据状态 */
	queue_state queueState;					/* 队列状态 */
	uint16_t cap;							/* 队列容量 */
	uint8_t (*send)(uint8_t* , uint16_t);	/* 发送数据函数指针 */
	queue_state (*getState)(void);			/* 获取队列状态函数指针 */
}data_queue;

/**
 * @brief 队列初始化
 * 
 * @param queue 	队列指针
 */
void queue_init(data_queue *queue);

/**
 * @brief 设置发送函数
 * 
 * @param queue 	队列指针
 * @param func 		发送函数的函数指针
 */
void set_send_func(data_queue *queue, uint8_t (*func)(uint8_t* , uint16_t));

/**
 * @brief 设置获取状态指针
 * 
 * @param queue 	队列指针
 * @param getState 	状态获取函数的函数指针
 */
void set_state_func(data_queue *queue, queue_state (*getState)(void));

/**
 * @brief 向队列中写数据
 * 
 * @param queue 	队列指针
 * @param data 		数据源指针
 * @param lenth 	数据长度
 */
void queue_write(data_queue *queue, uint8_t * data, uint16_t lenth);

/**
 * @brief 发送数据
 * 
 * @param queue 	队列指针
 */
void send(data_queue *queue);

#endif //_DATA_QUEUE_H
