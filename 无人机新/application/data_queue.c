#include "data_queue.h"

void queue_init(data_queue *queue)
{
	memset(&(queue->buff),0,BUFFSIZE);
	queue->head = 0;
	queue->tail = 0;
	queue->dataState = EMPTY;
	queue->queueState = UNLOCKED;
	queue->cap = BUFFSIZE;
}

void set_send_func(data_queue *queue, uint8_t (*func)(uint8_t* , uint16_t))
{
	queue->send = func;
}

void set_state_func(data_queue *queue, queue_state (*getState)(void))
{
	queue->getState = getState;
}

void queue_write(data_queue *queue, uint8_t * data, uint16_t length)
{
	if(queue->cap < length){
		return;
	} else {
		if(queue->head <= queue->tail){
			if(length <= BUFFSIZE - queue->tail){
				memcpy(&(queue->buff[queue->tail]), data, length);
				queue->tail += length;
				if(queue->tail == BUFFSIZE) queue->tail = 0;
			} else {
				memcpy(&(queue->buff[queue->tail]), data, BUFFSIZE - queue->tail);
				memcpy(&(queue->buff[0]), data + BUFFSIZE - queue->tail , length + queue->tail - BUFFSIZE);
				queue->tail = length + queue->tail - BUFFSIZE;
			}
		} else if(queue->head > queue->tail){
			memcpy(&(queue->buff[queue->tail]), data, length);
			queue->tail += length;
		}
	}
	
	queue->cap -= length;
	if(queue->cap == 0) queue->dataState = FULL;
	else if(queue->cap == BUFFSIZE) queue->dataState = EMPTY;
	else queue->dataState = OTHER;
}

void send(data_queue *queue)
{
	if(queue->getState == NULL || queue->send == NULL) return;
	queue->queueState = queue->getState();
	
	if(queue->queueState == LOCKED){
		return;
	}
	
	if(queue->dataState == FULL){
		queue->send(&(queue->buff[queue->head]), BUFFSIZE - queue->head);
		queue->cap += BUFFSIZE - queue->head;
		queue->head = 0;
	} else if(queue->dataState == EMPTY);
	else {
		if(queue->head < queue->tail){
			int32_t len = queue->send(&(queue->buff[queue->head]), queue->tail - queue->head);
			if (len == 0) {
				return;
			}
			queue->cap += queue->tail - queue->head;
			queue->head += queue->tail - queue->head;
		} else if(queue->head > queue->tail){
			int32_t len = queue->send(&(queue->buff[queue->head]), BUFFSIZE - queue->head);
			if (len == 0) {
				return;
			}
			queue->cap += BUFFSIZE - queue->head;
			queue->head = 0;
		}
	}
	
	if(queue->cap == 0) queue->dataState = FULL;
	else if(queue->cap == BUFFSIZE) queue->dataState = EMPTY;
	else queue->dataState = OTHER;
	
}
