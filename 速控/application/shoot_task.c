#include "shoot_task.h"
#include "FreeRTOS.h"
#include "MOTOR_ANGLE_PID.h"
#include "CAN_receive.h"
#include "referee.h"
#include "tim.h"
#include "user_lib.h"
#include "usb_task.h"
#include "bsp_usb.h"

uint32_t speed_tick = 0;
uint32_t mag_tick = 0;    //?
uint8_t  mag_flag = 0 ;
shoot_all shoot;
uint32_t time_3508;
uint32_t time_2006;   
uint8_t dart_num =0;
uint8_t dart_num1 = 0;
uint8_t dart_num2 = 0;
uint8_t dart_num3 = 0;
uint8_t dart_num4 = 0;
uint8_t dart_num5 = 0;
uint8_t cnt_shoot =0;
int16_t rc;
float motor_speed_left = 0; 							//����ת��
float motor_speed_right = 0;
int shoot_flag=0;
int shoot_behavior =0;
int position_flag =0;
int shoot_num =0;
int shoot_finish =0;

void shoot_mode();
/**
  * @brief         ״̬��ģʽ����          
  * @param[in]     null
	* @author        ���� 
  * @retval        null 
  */
void shoot_mode_set(void);

/**
  * @brief         ״̬����Ϊִ��                
  * @param[in]     null 
	* @author        ���� 
  * @retval        null 
  */
void shoot_mode_behavior(void);

/**
  * @brief         ������������          
  * @param[in]     GPIOx
  * @param[in]     GPIO_Pin����Ӧ����	
	* @author        ���� 
  * @retval        ������ֵ��1Ϊ�������£�0Ϊû�а��� 
  */
int key_detect(shoot_all *key_det);


/**
  * @brief         shoot�����ʼ������          
  * @param[in]     null 
	* @author        ���� 
  * @retval        null 
  */
void shoot_init(void);
void shoot_zero1();
void shoot_1();
void shoot_behave();
void shoot_2();
void shoot_loop();
double total_angle =105000.0;
void pid_math_speed();
void pid_math_double();

/**
  * @brief          б��������ʼ��
  * @author         RM
  * @param[in]      б�������ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @param[in]      ���ֵ
  * @param[in]      ��Сֵ
  * @retval         ���ؿ�
  */
void ramp_init(ramp_function_source_t *ramp_source_type, fp32 frame_period, fp32 max, fp32 min)
{
    ramp_source_type->frame_period = frame_period;
    ramp_source_type->max_value = max;
    ramp_source_type->min_value = min;
    ramp_source_type->input = 0.0f;
    ramp_source_type->out = 0.0f;
}

/**
  * @brief          б���������㣬���������ֵ���е��ӣ� ���뵥λΪ /s ��һ������������ֵ
  * @author         RM
  * @param[in]      б�������ṹ��
  * @param[in]      ����ֵ
  * @param[in]      �˲�����
  * @retval         ���ؿ�
  */
void ramp_calc(ramp_function_source_t *ramp_source_type, fp32 input)
{
    ramp_source_type->input = input;
    ramp_source_type->out += ramp_source_type->input * ramp_source_type->frame_period;
    if (ramp_source_type->out > ramp_source_type->max_value)
    {
        ramp_source_type->out = ramp_source_type->max_value;
    }
    else if (ramp_source_type->out < ramp_source_type->min_value)
    {
        ramp_source_type->out = ramp_source_type->min_value;
    }
}


/**
  * @brief          һ�׵�ͨ�˲���ʼ��
  * @author         RM
  * @param[in]      һ�׵�ͨ�˲��ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @param[in]      �˲�����
  * @retval         ���ؿ�
  */
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period, const fp32 num[1])
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num[0] = num[0];
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
}

/**
  * @brief          һ�׵�ͨ�˲�����
  * @author         RM
  * @param[in]      һ�׵�ͨ�˲��ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @retval         ���ؿ�
  */
void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->out =
        first_order_filter_type->num[0] / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->out + first_order_filter_type->frame_period / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->input;
}



void push()//�Ƴ�ǣ��
{ 
    motor_speed_left = motor_speed;
    motor_speed_right=-motor_speed;
	  pid_math_speed();
}

void pull()//����ǣ��
{
		motor_speed_left =-motor_speed;
    motor_speed_right= motor_speed;
	  pid_math_speed();
}

void motor_stay()//����ģʽ
{
		motor_speed_left = 0;
    motor_speed_right= 0;
	  pid_math_speed();
}
void gimbal_deadarea()//��̨��������
{
	if(rc_ctrl.rc.ch[0]>deadarea||rc_ctrl.rc.ch[0]<-deadarea)
	rc =rc_ctrl.rc.ch[0];//ң��������
	else
	rc =0;
}

/**
  * @brief         shoot������Ⱥ���          
  * @param[in]     pvParameters 
	* @author        ���� 
  * @retval        null 
  */
uint8_t status =0;

/****����������****/
void shoot_task(void const * pvParameters)
{		int time1 =0 ;
	  time1 =HAL_GetTick();
	  shoot_init();
		
		//shoot.shoot_motor1_angle = 0;
		//shoot.shoot_motor2_angle = 0;
		for(;;)
		{
			shoot.shoot_motor1_angle == shoot.motor1_data->ecd+length;
//			if((HAL_GetTick()-time1)>5000)
//			{
//			motor_speed_left  = 0;
//      motor_speed_right = 0;
//			}
//			else
//			{
//			motor_speed_left  = 0;
//      motor_speed_right = 0;
//      motor_speed_left = motor_speed;
//      motor_speed_right=-motor_speed;	
//      pid_math_double();				
//			}
//CAN_cmd_shoot(0,0,shoot.motor1.out ,shoot.motor2.out);
//			shoot_mode();
//			shoot_loop();
			osDelay(1);
		}	

}
			
			
			

/****����ʹ�ܺ���****/
void shoot_init()
{
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    
    const static fp32 SHOOT2_filter[1] = {0.0005};    //�ٶ����õ�һ���˲�����
    const static fp32 SHOOT1_filter[1] = {0.0005};  
	  
		ramp_init(&shoot.shoot1_filter, 0.001f , 4700,2000);
	  shoot.motor_data1=get_trigger2_motor_measure_point();       //�������      //?
	  shoot.motor1_data=get_shoot_motor_measure_point(7);         //Ħ����1
	  shoot.motor2_data=get_shoot_motor_measure_point(8);         //Ħ����2
	
	  shoot.shoot_RC=get_remote_control_point(); //��ȡң����ָ��
	  const fp32 PID_angle[3]={Trigger_angle_PID_P,Trigger_angle_PID_I,Trigger_angle_PID_D};	//P,I,D    �ǶȻ�
    const fp32 PID_speed[3]={Trigger_speed_PID_P,Trigger_speed_PID_I,Trigger_speed_PID_D};	//P,I,D    �ٶȻ�
		
		
		const fp32 PID_motor1_angle[3]={motor1_angle_P,motor1_angle_I,motor1_angle_D};	//P,I,D	
		const fp32 PID_motor1[3]={motor1_PID_P,motor1_PID_I,motor1_PID_D};	//P,I,D
		const fp32 PID_motor2_angle[3]={motor2_angle_P,motor2_angle_I,motor2_angle_D};
		const fp32 PID_motor2[3]={motor2_PID_P,motor2_PID_I,motor2_PID_D};	//P,I,D
		
		PID_init(&shoot.MOTOR_ANGLE1,PID_POSITION,PID_angle,16000,11000);
		PID_init(&shoot.MOTOR_SPEED1,PID_POSITION,PID_speed,16000,11000);
		
		PID_init(&shoot.motor1_angle,PID_POSITION,PID_motor1_angle,7000,700);
		PID_init(&shoot.motor1,PID_POSITION,PID_motor1,7000,700);
		PID_init(&shoot.motor2_angle,PID_POSITION,PID_motor2_angle,7000,700);
		PID_init(&shoot.motor2,PID_POSITION,PID_motor2,7000,700);
		shoot.shoot_motor1_angle = shoot.motor1_data->ecd;
		shoot.shoot_motor2_angle = shoot.motor2_data->ecd;
		
}	

void shoot_loop()
{
	if(shoot.shoot_mode == shoot_start)
				{
						if(shoot.shoot_mode == shoot_one&&shoot_finish==0)
						{

									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 0&&position_flag == 0)//���΢������
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,100);//�������
										pull();
										if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 1)
										{
											
											position_flag = 1;
										}
											vTaskDelay(1);
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 0&&position_flag == 1)
									{
										push();
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 1&&position_flag == 1)//���Ͱ嵽��ָ��λ��
									{
										motor_stay();
										shoot_num=1;
									}
									if(shoot_num ==1)
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,10000);//����
										shoot_finish=1;//�������״̬��1
										position_flag=0;
									}
							}
/****˫��ģʽ****/							
							if(shoot.shoot_mode == shoot_double&&shoot_finish==0)
							{
										if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 0&&position_flag == 0)//���΢������
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,100);//�������
										pull();
										if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == 1)
										{
											
											position_flag = 1;
										}
											vTaskDelay(1);
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 0&&position_flag == 1)
									{
										push();
									}
									if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == 1&&position_flag == 1)//���Ͱ嵽��ָ��λ��
									{
										motor_stay();
										shoot_num=1;
									}
									if(shoot_num ==1)
									{
										__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,10000);//����
										shoot_num =0;//�������״̬��1
									}
								
							}
					}
				else
				{
				shoot.motor1.out =0 ; 
				shoot.motor2.out =0 ;
				}

/****can���ͺ���****/
	CAN_cmd_shoot(0,0,shoot.motor1.out,shoot.motor2.out);
}

/****����״̬����****/
void shoot_mode()
{
	if(rc_ctrl.rc.s[0]==(2||1))//����ֹͣ
	{
	shoot.shoot_mode=shoot_zero;
	shoot_finish=0;
	}
  else if(rc_ctrl.rc.s[0]==3)//���п�����
	{
		shoot.shoot_mode=shoot_start;
	if(shoot.shoot_mode==shoot_start&&rc_ctrl.rc.s[1]==3)//����ģʽ
	{
	 shoot.shoot_mode=shoot_one;
	}
	else if(shoot.shoot_mode==shoot_start&&rc_ctrl.rc.s[1]==1)//˫��ģʽ
	{
		shoot.shoot_mode=shoot_double;
	}
	}
}

void shoot_math()
{
	
}
void pid_math_double()//˫������ 
{
PID_calc(&shoot.motor1_angle, shoot.motor1_data->ecd,shoot.motor1_angle_out);
PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm, shoot.motor1_angle.out);
	
PID_calc(&shoot.motor2_angle, shoot.motor2_data->ecd,shoot.motor2_angle_out);
PID_calc(&shoot.motor2, shoot.motor1_data->speed_rpm,shoot.motor2_angle.out);

}
void pid_math_speed()//��������
{
//PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm, shoot.motor1_angle.out);
//PID_calc(&shoot.motor2, shoot.motor1_data->speed_rpm,shoot.motor2_angle.out);
PID_calc(&shoot.motor1, shoot.motor1_data->speed_rpm,motor_speed_left);
PID_calc(&shoot.motor2, shoot.motor1_data->speed_rpm,motor_speed_right);
}


