/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "hrtim.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "pid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LDO_VOL 3.308f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float duty;
float buck_duty;
float boost_duty;
uint16_t adc1_buf[6];
uint16_t adc2_buf[6];
float cap_voltage;
float cap_current;
float bat_voltage;
float bat_current;
PID cap_vol_h;
PID cap_vol_l;
PID inp_power;
PID cap_cur;
float kf = 0.9;//前馈参数
float power_limit = 40;//初始功率限制
float power_ref = 5;//功率参考
uint8_t isPWD = 1;//停止供电标识符
uint8_t isCAP_START = 0;//电容是否高于最低电压
uint16_t PWUcnt = 0;//开启供电计数
uint8_t bsp_can_rxbuf[8];
float power_target;
//uint8_t cancnt=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_HRTIM1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_CAN_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	while(HAL_OK != (HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED)));
	while(HAL_OK != (HAL_ADCEx_Calibration_Start(&hadc1,ADC_DIFFERENTIAL_ENDED)));
	while(HAL_OK != (HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED)));
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adc1_buf,6);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)adc2_buf,6);
	HAL_CAN_Start(&hcan); 
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
	pid_init(&cap_vol_h,PID_DELTA, 0.5,0.05,0,0,10,-18);
	pid_init(&cap_vol_l,PID_DELTA, 0.5,0.05,0,0,10,-18);
	pid_init(&inp_power,PID_DELTA, 0.04,0.008,0,0,10,-18);
	pid_init(&cap_cur,PID_DELTA, 0.005,0.0005,0,0,1.7,0);
	cap_vol_h.ref = 26.0;
	cap_vol_l.ref = 8.0;
	inp_power.ref = 5.0;
	duty = 0.9;
	HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_MASTER);
	HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
	HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
	//HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2);
	//HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim7);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_HRTIM1;
  PeriphClkInit.Hrtim1ClockSelection = RCC_HRTIM1CLK_PLLCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void BSP_CAN_Send(uint8_t *data,uint32_t len)
{
    uint32_t tx_mailbox;
    CAN_TxHeaderTypeDef txconf;
    txconf.StdId = 0x211;
    txconf.IDE = CAN_ID_STD;
    txconf.RTR = CAN_RTR_DATA;
    txconf.DLC = 8;
    /*uint32_t can_tx_start_time = HAL_GetTick();
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) {
        // HAL_CAN_AbortTxRequest
        can_tx_now_time = HAL_GetTick();
        // 2ms还没有发送出去，放弃发送
        if (can_tx_now_time > can_tx_start_time + 1) {
            HAL_CAN_AbortTxRequest(&hcan, CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | CAN_TX_MAILBOX2);
            __NOP();
            break;
        }
    }*/
    HAL_CAN_AddTxMessage(&hcan, &txconf, data, &tx_mailbox);
}
void SendPowerManagementData() {
    uint16_t data[4];
    data[0] = (uint16_t)(bat_voltage * 100.f);
    data[1] = (uint16_t)(cap_voltage * 100.f);
    data[2] = (uint16_t)(bat_current * 100.f);
    data[3] = (uint16_t)(power_limit * 100.f);
    BSP_CAN_Send((uint8_t *)data, 8);  // 0x211
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}// 10hz

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim7)
	{
		if(bat_voltage < 18.f || bat_voltage > 30.f)//掉电及过压保护
		{
			//HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);//关闭电荷泵
			//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);//关闭输出
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2);//关闭半桥
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
			isPWD = 1;
			PWUcnt = 0;
		}
		else if(bat_voltage < 29.f && bat_voltage > 20.f)
		{
			//重置pid和缓启动
			if(isPWD)
			{
				PWUcnt++;
				if(PWUcnt > 1000)
				{
					power_ref = 5;
					pid_reset(&inp_power);
					pid_reset(&cap_vol_h);
					pid_reset(&cap_vol_l);
					pid_reset(&cap_cur);
					float output_preload = kf * cap_voltage/bat_voltage;
					//cap_vol_h.output = output_preload;
					//cap_vol_l.output = output_preload;
					cap_cur.output = output_preload;
					//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);//开启电荷泵
					//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//开启输出
					HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2);//开启半桥
					HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
					isPWD = 0;
					PWUcnt = 0;
					isCAP_START = 0;
				}
			}
			if(cap_voltage>10.0f)
				isCAP_START = 1;
		}
		if(power_ref < power_limit)
			power_ref += 0.01f;
		else if(power_ref > power_limit)
			power_ref = power_limit;//功率缓启动
		
		cap_voltage = (adc2_buf[0]+adc2_buf[2]+adc2_buf[4])*LDO_VOL*16.f/4095.f/3.f;//电容电压
		bat_voltage = (adc2_buf[1]+adc2_buf[3]+adc2_buf[5])*LDO_VOL*16.f/4095.f/3.f;//电源电压
		bat_current = (adc1_buf[1]+adc1_buf[3]+adc1_buf[5])*LDO_VOL/4095.f*10.f/3.f ;//输入电流
		cap_current = (adc1_buf[0]+adc1_buf[2]+adc1_buf[4]-2047*3)*LDO_VOL/4095.f*20.f/3.f;//电容电流
		
		pid_calculate(&cap_vol_h, cap_voltage);//计算电容高压电压环
		pid_calculate(&cap_vol_l, cap_voltage);//计算电容低压电压环
		inp_power.ref = power_ref;
		//inp_power.ref = 20;
		pid_calculate(&inp_power, bat_current * bat_voltage);//计算功率环pid
		float current_ref = inp_power.output;
		//三环并联
		if(current_ref > cap_vol_h.output)
		{
			current_ref = cap_vol_h.output;
			inp_power.output = current_ref;
		}
		else if(isCAP_START && (current_ref < cap_vol_l.output))
		{
			current_ref = cap_vol_l.output;
			inp_power.output = current_ref;
		}
		else
		{
			cap_vol_h.output = current_ref;
			cap_vol_l.output = current_ref;
		}
		//current_ref = 0.2;
		cap_cur.ref = current_ref;
		pid_calculate(&cap_cur, cap_current);//计算低侧电流环pid
		duty = cap_cur.output;//（广义）占空比为高侧电流环输出
		//广义占空比转换为buck和boost占空比
		if(duty>1.5f)
			duty = 1.5f;
		else if(duty<0.2f)
			duty = 0.2f;
		if(duty>0.9f)
		{
			buck_duty = 0.9f;
			boost_duty = 0.81f / duty;
		}
		else
		{
			buck_duty = duty;
			boost_duty = 0.9f;
		}
			
		//设定HRTIM占空比
		__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_1, 12800 - 12800 * buck_duty);
		__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2, 12800 + 12800 * buck_duty);
		__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_3, 12800 - 12800 * boost_duty);
		__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_4, 12800 + 12800 * boost_duty);
	}
	else if(htim == &htim6)
	{
		SendPowerManagementData();
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rxconf;
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxconf, bsp_can_rxbuf);
	if(rxconf.StdId == 0x210)
	{
    power_target = ((float)(bsp_can_rxbuf[1] + (((uint16_t)bsp_can_rxbuf[0]) << 8))) / 100;
    if (power_target >= 20 && power_target <= 200) {
        power_limit = power_target;
    } else if (power_target < 20) {
        power_limit = 20;
    } else if (power_target > 200) {
        power_limit = 200;
    }
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef rxconf;
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rxconf, bsp_can_rxbuf);
	if(rxconf.StdId == 0x210)
	{
    power_target = ((float)(bsp_can_rxbuf[1] + (((uint16_t)bsp_can_rxbuf[0]) << 8))) / 100;
    if (power_target >= 20 && power_target <= 200) {
        power_limit = power_target;
    } else if (power_target < 20) {
        power_limit = 20;
    } else if (power_target > 200) {
        power_limit = 200;
    }
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
