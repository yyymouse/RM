#ifndef __COMMUNICATE_TASK_H
#define __COMMUNICATE_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"

extern void communicate_task(void const * argument);
extern int time_delay;
#endif //__COMMUNICATE_TASK_H
