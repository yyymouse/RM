#include "test_task.h"
#include "main.h"
#include "cmsis_os.h"
#include "stm32_flash.h"
#include "cmd_ctr.h"
#include "shoot_task.h"
/**
  * @brief          test task
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
/**
  * @brief          test»ŒŒÒ
  * @param[in]      pvParameters: NULL
  * @retval         none
  */

extern void positive_motion();
extern void negative_motion();
void test_task(void const * argument)
{  

	
    while(1)
    {



       osDelay(5);
    }
}



