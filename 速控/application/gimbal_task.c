#include "gimbal_task.h"
//#include "MOTOR_ANGLE_PID.h"
#include "bsp_usb.h"
#include "kalman.h"
#include "robomaster_vcan.h"
#include "user_lib.h"
#include <math.h>
#include "bsp_usart.h"
#include "usart.h"
#include "usb_task.h"
#include "cmd_ctr.h"
#include "stm32_flash.h"

int yaw_offset_ecd  =  4;

float zero_go(float set, float get)
{
    float aset = (set +3.14)/3.14*2 * 360;
    float aget = (get +3.14)/3.14*2 * 360;

    float res = 0.0f;
    if(aset >= aget) 
    {
        if((aset - aget) > 180)
        {	//aset = 350 aget = 10
            res = aset - 360 - aget;
        }
        else 
        {//aset = 20 aget = 10
        res = aset - aget;
        }
    } 
    else 
    {
        if((aset - aget) < -180)
        {//aset = 10 aget = 350
            res = 360 + aset - aget;
        } 
        else 
        {					//aset = 10 aget = 20
            res = aset - aget;
        }
    }
    return res +aget;
}
#define rc_deadband_limit(input, output, dealine)        \
    {                                                    \
        if ((input) > (dealine) || (input) < -(dealine)) \
        {                                                \
            (output) = (input);                          \
        }                                                \
        else                                             \
        {                                                \
            (output) = 0;                                \
        }                                                \
    }

#define ecd_format(ecd)         \
    {                           \
        if ((ecd) > ECD_RANGE)  \
            (ecd) -= ECD_RANGE; \
        else if ((ecd) < 0)     \
            (ecd) += ECD_RANGE; \
    }

gimbal gimbal_ctrl;

void gimbal_init(gimbal *gimbal_init);
void gimbal_loop(gimbal *loop);
static void gimbal_feedback_update(gimbal *feedback_update);
static void gimbal_movement(gimbal *movement);
		uint8_t visual_use_date[2][20];
		uint8_t send_date[100];
    
float set_yaw_angle;
float new_yaw_ecd;
float yaw_set_ecd;
    
float pitch_set_angle;
float pitch_new_ecd;
float pitch_set_ecd;
    int16_t pitch_set;
    extern motor_measure_t motor_chassis[10];
//    pid_type_def motor_speed;  
    pid_type_def motor_angle;
    
    pid_type_def motor_speed_pitch;  
    pid_type_def motor_angle_pitch;
    fp32 PID_speed_pitch[3] = {10.0f, 0.0f, 0.0f};
//	fp32 PID_angle_pitch[3] = {12500.0f, 10.0f, 10.0f};pitch在动，靶前
    fp32 PID_angle_pitch[3] = {12500.0f, 0.0f, 15.0f};
    fp32 PID_speed_yaw[3] = {10.0f, 0.0f, 0.0f};
	fp32 PID_angle_yaw[3] = {10000.0f, 0.0f, 20.0f};
    int i=0;
    float set_ecd;
    extern uint8_t status;
    uint16_t pitch_ecd_change;
void gimbal_task(void const *pvParameters)
{
//    PID_init(&motor_angle, PID_POSITION, PID_angle_yaw, 16000.0f, 16000.0f);
//    PID_init(&motor_speed, PID_POSITION, PID_speed_yaw, 16000.0f, 16000.0f);
    
    PID_init(&motor_angle_pitch, PID_POSITION, PID_angle_pitch, 16000.0f, 16000.0f);
    PID_init(&motor_speed_pitch, PID_POSITION, PID_speed_pitch, 16000.0f, 16000.0f);
    yaw_set_ecd =4350;
    pitch_set_ecd =9194;//max8341   9194 -30度
	for(;;)
	{
        status =HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7);
        if(rc_ctrl.rc.s[1]==2&&rc_ctrl.rc.s[0]==2)
        {
            CAN_cmd_gimbal(0, 0, 0, 0);            
        }
        else if((rc_ctrl.rc.s[1]==2&&rc_ctrl.rc.s[0]==3)||(rc_ctrl.rc.s[1]==3&&rc_ctrl.rc.s[0]==3))
        {
            yaw_set_ecd = yaw_set_ecd +rc_ctrl.rc.ch[0]/100;
            if(yaw_set_ecd>5400)
                yaw_set_ecd =5400;
            if(yaw_set_ecd<3300)
                yaw_set_ecd =3300;            
            set_yaw_angle= yaw_set_ecd/8191.0*2*pi;

            pitch_set_ecd = pitch_set_ecd +rc_ctrl.rc.ch[1]/50;
            if(pitch_set_ecd>9350)
                pitch_set_ecd =9350;//gai yuabkai8325
            if(pitch_set_ecd<8200)
                pitch_set_ecd =8200;            
            pitch_set_angle= pitch_set_ecd/8191.0*2*pi;
            
            new_yaw_ecd=motor_chassis[4].ecd/8191.0*2*pi;
            
            if(motor_chassis[5].ecd<3000)
                pitch_ecd_change =motor_chassis[5].ecd +8191;
            else 
                pitch_ecd_change =motor_chassis[5].ecd;
            pitch_new_ecd =pitch_ecd_change/8191.0*2*pi;
        }

        vTaskDelay(5);
	}
}


/**
  * @brief          "gimbal_control" valiable initialization, include pid initialization, remote control data point initialization, gimbal motors
  *                 data point initialization, and gyro sensor angle point initialization.
  * @param[out]     gimbal_init: "gimbal_control" valiable point
  * @retval         none
  */
/**
  * @brief          初始化"gimbal_control"变量，包括pid初始化， 遥控器指针初始化，云台电机指针初始化，陀螺仪角度指针初始化
  * @param[out]     gimbal_init:"gimbal_control"变量指针.
  * @retval         none
  */
static void gimbal_PID_init(gimbal_PID_t *pid, fp32 maxout, fp32 max_iout, fp32 kp, fp32 ki, fp32 kd)
{
    if (pid == NULL)
    {
        return;
    }
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->err = 0.0f;
    pid->get = 0.0f;

    pid->max_iout = max_iout;
    pid->max_out = maxout;
}

static fp32 gimbal_PID_calc(gimbal_PID_t *pid, fp32 get, fp32 set, fp32 error_delta)
{
    fp32 err;
    if (pid == NULL)
    {
        return 0.0f;
    }
    pid->get = get;
    pid->set = set;

    err = set - get;
    pid->err = rad_format(err);
    pid->Pout = pid->kp * pid->err;
    pid->Iout += pid->ki * pid->err;
    pid->Dout = pid->kd * error_delta;
    abs_limit(&pid->Iout, pid->max_iout);
    pid->out = pid->Pout + pid->Iout + pid->Dout;
    abs_limit(&pid->out, pid->max_out);
    return pid->out;
}

/**
  * @brief          gimbal PID clear, clear pid.out, iout.
  * @param[out]     gimbal_pid_clear: "gimbal_control" valiable point
  * @retval         none
  */
/**
  * @brief          云台PID清除，清除pid的out,iout
  * @param[out]     gimbal_pid_clear:"gimbal_control"变量指针.
  * @retval         none
  */
static void gimbal_PID_clear(gimbal_PID_t *gimbal_pid_clear)
{
    if (gimbal_pid_clear == NULL)
    {
        return;
    }
    gimbal_pid_clear->err = gimbal_pid_clear->set = gimbal_pid_clear->get = 0.0f;
    gimbal_pid_clear->out = gimbal_pid_clear->Pout = gimbal_pid_clear->Iout = gimbal_pid_clear->Dout = 0.0f;
}

/**
  * @brief          计算ecd与offset_ecd之间的相对角度
  * @param[in]      ecd: 电机当前编码
  * @param[in]      offset_ecd: 电机中值编码
	* @author         RM
  * @retval         相对角度，单位rad
  */
static fp32 motor_ecd_to_angle_change(uint16_t ecd, uint16_t offset_ecd)
{
    int32_t relative_ecd = ecd - offset_ecd;
    if (relative_ecd > HALF_ECD_RANGE)
    {
        relative_ecd -= ECD_RANGE;
    }
    else if (relative_ecd < -HALF_ECD_RANGE)
    {
        relative_ecd += ECD_RANGE;
    }

    return relative_ecd * MOTOR_ECD_TO_RAD;
}

/**
  * @brief          底盘测量数据更新，包括电机速度，欧拉角度，机器人速度
  * @param[out]     gimbal_feedback_update:"gimbal"变量指针.
	* @author         刘根 
  * @retval         none
  */
static void gimbal_feedback_update(gimbal *feedback_update)
{
	    if (feedback_update == NULL)
    {
        return;
    }
			
    feedback_update->gimbal_pitch_motor.absolute_angle = *(feedback_update->gimbal_INT_angle_point + INS_PITCH_ADDRESS_OFFSET);
    feedback_update->gimbal_pitch_motor.relative_angle = motor_ecd_to_angle_change(feedback_update->gimbal_pitch_motor.gimbal_motor_measure->ecd,
                                                                                      feedback_update->gimbal_pitch_motor.offset_ecd);
		feedback_update->gimbal_pitch_motor.motor_gyro =  *(feedback_update->gimbal_INT_gyro_point + INS_GYRO_Y_ADDRESS_OFFSET);    //更新陀螺仪角度
	
		feedback_update->gimbal_yaw_motor.absolute_angle = *(feedback_update->gimbal_INT_angle_point + INS_YAW_ADDRESS_OFFSET);
    feedback_update->gimbal_yaw_motor.relative_angle = motor_ecd_to_angle_change(feedback_update->gimbal_yaw_motor.gimbal_motor_measure->ecd,
                                                                                        feedback_update->gimbal_yaw_motor.offset_ecd);

    feedback_update->gimbal_yaw_motor.motor_gyro = arm_cos_f32(feedback_update->gimbal_pitch_motor.relative_angle) * (*(feedback_update->gimbal_INT_gyro_point + INS_GYRO_Z_ADDRESS_OFFSET))
                                                        - arm_sin_f32(feedback_update->gimbal_pitch_motor.relative_angle) * (*(feedback_update->gimbal_INT_gyro_point + INS_GYRO_X_ADDRESS_OFFSET));

}

static void gimbal_movement(gimbal *movement)   //控制云台角度
{
	
  	movement->gimbal_pitch_motor.absolute_angle_set -= movement->gimbal_rc->rc.ch[gimbal_pitch_CHANNEL]*pitch_rc_multiple -movement->gimbal_rc->mouse.y * PITCH_MOUSE_SEN;	  
	  
  	movement->gimbal_yaw_motor.absolute_angle_set   -= movement->gimbal_rc->rc.ch[gimbal_yaw_CHANNEL]*yaw_rc_multiple +movement->gimbal_rc->mouse.x * YAW_MOUSE_SEN;  
	  if(movement->gimbal_yaw_motor.absolute_angle_set>pi)  //遥控器控制加减回零点
		{
			
			movement->gimbal_yaw_motor.absolute_angle_set=-pi;
		
		}
		else if(movement->gimbal_yaw_motor.absolute_angle_set<-pi)
		{
		  movement->gimbal_yaw_motor.absolute_angle_set=pi;
		}
		
		if(movement->gimbal_pitch_motor.absolute_angle_set>=MAX_ANGLE)  //限幅
		{
			movement->gimbal_pitch_motor.absolute_angle_set = MAX_ANGLE;
		
		}
		else if(movement->gimbal_pitch_motor.absolute_angle_set<=MIN_ANGLE)
		{
			movement->gimbal_pitch_motor.absolute_angle_set = MIN_ANGLE;
		
		}
		
		
		
}

/**
  * @brief         返回云台yaw轴电机数据指针           
  * @param[in]     void
	* @author        刘根 
  * @retval        gimbal_ctrl.gimbal_yaw_motor
  */
const gimbal_motor_t *get_yaw_motor_point(void)
{
    return &gimbal_ctrl.gimbal_yaw_motor;
}

/**
  * @brief         云台初始化函数          
  * @param[in]     gimbal整体云台参数结构体 
	* @author        刘根 
  * @retval        none 
  */
void gimbal_init(gimbal *gimbal_init)
{
	if (gimbal_init == NULL)
	{
			return;
	}
  flash_date_init();
//	gimbal_init->gimbal_yaw_motor.offset_ecd = yaw_offset_ecd;
//	gimbal_init->gimbal_pitch_motor.offset_ecd = pitch_offset_ecd;
	gimbal_init->first_angle_point = 0;
	gimbal_init->yaw_first_angle_flag = 0;
	gimbal_init->gimbal_yaw_motor.gimbal_motor_mode = GIMBAL_ZERO;
	gimbal_init->gimbal_pitch_motor.max_relative_angle=MAX_ANGLE;//机械角度限幅
	gimbal_init->gimbal_pitch_motor.min_relative_angle=MIN_ANGLE;
	
	gimbal_init->gimbal_yaw_motor.gimbal_motor_measure = get_yaw_gimbal_motor_measure_point();  //yaw           
	gimbal_init->gimbal_pitch_motor.gimbal_motor_measure = get_pitch_gimbal_motor_measure_point();  //pitch
  gimbal_init->gimbal_rc = get_remote_control_point(); //获取遥控器值
	gimbal_init->gimbal_INT_angle_point = get_INS_angle_point(); //获取欧拉角数据
	gimbal_init->gimbal_INT_gyro_point = get_gyro_data_point();//获取角速度数据
	 
	
	PID_init(&gimbal_init->gimbal_pitch_motor.gimbal_motor_gyro_pid, PID_POSITION, mid_date.Pitch_speed_pid, PITCH_SPEED_PID_MAX_OUT, PITCH_SPEED_PID_MAX_IOUT);
	PID_init(&gimbal_init->gimbal_yaw_motor.gimbal_motor_gyro_pid, PID_POSITION, mid_date.Yaw_speed_pid, YAW_SPEED_PID_MAX_OUT, YAW_SPEED_PID_MAX_IOUT);
		
		//初始化yaw电机pid
	gimbal_PID_init(&gimbal_init->gimbal_yaw_motor.gimbal_motor_absolute_angle_pid, YAW_GYRO_ABSOLUTE_PID_MAX_OUT, YAW_GYRO_ABSOLUTE_PID_MAX_IOUT, 
		              mid_date.YAW_GYRO_ABSOLUTE_PID[0], mid_date.YAW_GYRO_ABSOLUTE_PID[1], mid_date.YAW_GYRO_ABSOLUTE_PID[2]);
	gimbal_PID_init(&gimbal_init->gimbal_pitch_motor.gimbal_motor_absolute_angle_pid, PITCH_GYRO_ABSOLUTE_PID_MAX_OUT, PITCH_GYRO_ABSOLUTE_PID_MAX_IOUT, 
	                mid_date.PITCH_GYRO_ABSOLUTE_PID[0], mid_date.PITCH_GYRO_ABSOLUTE_PID[1], mid_date.PITCH_GYRO_ABSOLUTE_PID[2]);

	gimbal_init->gimbal_yaw_motor.motor_gyro_set = gimbal_init->gimbal_yaw_motor.motor_gyro_set ;//陀螺仪初始化

	kalmanCreate(&visual_x_fillter, 80, 300);  
	kalmanCreate(&visual_y_fillter, 80, 300); 
		
  
}


float gimbal_yaw_calc(void)//去零点
{
	
	float set;
	set = gimbal_ctrl.gimbal_yaw_motor.absolute_angle_set;
	float aset = (set +pi)/(2*pi) * 360;
	float aget = (gimbal_ctrl.gimbal_yaw_motor.absolute_angle+pi) / (2*pi) * 360;

	float res = 0.0f;
	
	
	if(aset >= aget) {
		if((aset - aget) > 180){	//aset = 350 aget = 10
			res = aset - 360 - aget;
		} else {					//aset = 20 aget = 10
			res = aset - aget;
		}
	} else {
		if((aset - aget) < -180){ 	//aset = 10 aget = 350
			res = 360 + aset - aget;
		} else {					//aset = 10 aget = 20
			res = aset - aget;
		}
	}
	
	return  (aget + res);  //角度环
}

fp32 ramp(fp32 input,fp32 frame_period)//斜波函数
{
	fp32 out;
	if(input>0)
	{
		out=input-frame_period;
	}
	else if(input<0)
	{
	  out=input+frame_period;
	}
  else if(input==0)
	{
	  out=0;
	}
	return out;
}

/**
  * @brief          云台控制模式:GIMBAL_MOTOR_GYRO，使用陀螺仪计算的欧拉角进行控制
  * @param[out]     gimbal_motor:yaw电机或者pitch电机
  * @retval         none
  */
static void gimbal_absolute_angle_limit(gimbal_motor_t *gimbal_motor, fp32 add)
{
    static fp32 bias_angle;
    static fp32 angle_set;
    if (gimbal_motor == NULL)
    {
        return;
    }
    //now angle error
    //当前控制误差角度
    bias_angle = rad_format(gimbal_motor->absolute_angle_set - gimbal_motor->absolute_angle);
    //relative angle + angle error + add_angle > max_relative angle
    //云台相对角度+ 误差角度 + 新增角度 如果大于 最大机械角度
    if (gimbal_motor->relative_angle + bias_angle + add > gimbal_motor->max_relative_angle)
    {
			
        //如果是往最大机械角度控制方向
        if (add > 0.0f)
        {
            //calculate max add_angle
            //计算出一个最大的添加角度，
            add = gimbal_motor->max_relative_angle - gimbal_motor->relative_angle - bias_angle;
        }
    }
    else if (gimbal_motor->relative_angle + bias_angle + add < gimbal_motor->min_relative_angle)
    {
        if (add < 0.0f)
        {
            add = gimbal_motor->min_relative_angle - gimbal_motor->relative_angle - bias_angle;
        }
    }
    angle_set = gimbal_motor->absolute_angle_set;
    gimbal_motor->absolute_angle_set = rad_format(angle_set + add);
}


/** 
  * @brief         云台电机参数传出循环函数		          
  * @param[in]     gimbal整体云台参数结构体  
	* @author        刘根 
  * @retval        none 
  */
		static fp32 yaw_motor_visual_pixel = 0;
		static fp32 pitch_motor_visual_pixel = 0;
		static fp32 pitch_motor_visual_date = 0;
   	 fp32 pitch_angle_add = 0;
     fp32 yaw_angle_add = 0;

static uint8_t Visual_Ture_RInit_data_first;
static uint8_t Visual_Ture_RInit_data_end;
char i_count;
char switch_flag = 0;

void gimbal_loop(gimbal *loop)
{
	
	loop->gimbal_yaw_motor.offset_ecd = yaw_offset_ecd;
	loop->gimbal_pitch_motor.offset_ecd = pitch_offset_ecd;
	 		i_count++;
		if(i_count>50)
		{
			
			yaw_motor_visual_pixel = 0;
			pitch_motor_visual_pixel = 0;
			loop->visual_pitch_motor_speed.out=0;
			loop->visual_pitch_motor_angle.out=0;
		
		}
	float yaw_set;
	if(loop->yaw_first_angle_flag == 0 )//记录开机第一次计算出来的角度值
		 {
			 loop->gimbal_pitch_motor.absolute_angle_set = 0;
		   loop->yaw_first_angle_flag =1;
			 
		 }


	if(loop->gimbal_yaw_motor.gimbal_motor_mode == GIMBAL_MOTOR_GYRO	) //自稳模式
	{
		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,SET);
		 gimbal_movement(&gimbal_ctrl);      //只计算云台遥感设置值

	}

	else if(loop->gimbal_yaw_motor.gimbal_motor_mode == GIMBAL_MOTOR_VISUAL)  //视觉模式
	{
   static int16_t yaw_channel = 0, pitch_channel = 0;
     uint8_t rate_yaw=1;
		 uint8_t rate_pitch=1;		
		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,RESET);

    //Visual_Ture.flag   判断是否收到数据
		if(1/*(Visual_Ture.flag==1)&&( Visual_Ture.RInit_data[0] == 0x73) && (Visual_Ture.RInit_data[9] == 0x65 )*/)
		{				
				rc_deadband_limit(loop->gimbal_rc->rc.ch[gimbal_yaw_CHANNEL], yaw_channel, RC_DEADBAND);
				rc_deadband_limit(loop->gimbal_rc->rc.ch[gimbal_pitch_CHANNEL], pitch_channel, RC_DEADBAND);

				loop->gimbal_yaw_motor.absolute_angle_set =-(loop->add_yaw);   //计算yaw轴目标角度值

			 loop->gimbal_pitch_motor.absolute_angle_set =  -loop->add_pitch; //计算yaw轴目标角度值
								
				if(fabs(loop->gimbal_pitch_motor.absolute_angle_set-loop->gimbal_pitch_motor.absolute_angle)>0.4)//自瞄消抖
				{
					loop->gimbal_pitch_motor.absolute_angle_set = loop->gimbal_pitch_motor.absolute_angle;
				
				}
				if(fabs(loop->gimbal_yaw_motor.absolute_angle_set-loop->gimbal_yaw_motor.absolute_angle)>0.4)
				{
					loop->gimbal_yaw_motor.absolute_angle_set = loop->gimbal_yaw_motor.absolute_angle;
				
				}
			
				
			}

			gimbal_movement(&gimbal_ctrl);   //计算absolute_angle_set值
		
		}
		yaw_set = gimbal_yaw_calc(); 
		
		//yaw
	  loop->gimbal_yaw_motor.motor_gyro_set = gimbal_PID_calc(&loop->gimbal_yaw_motor.gimbal_motor_absolute_angle_pid, 
																										((gimbal_ctrl.gimbal_yaw_motor.absolute_angle+pi) / (2*pi) * 360)*pi/180, 
																										yaw_set*pi/180, 
																										loop->gimbal_yaw_motor.motor_gyro);  
		
    loop->gimbal_yaw_motor.current_set = PID_calc(&loop->gimbal_yaw_motor.gimbal_motor_gyro_pid, 
																					loop->gimbal_yaw_motor.motor_gyro, 
																					loop->gimbal_yaw_motor.motor_gyro_set);
		
		//pitch
		loop->gimbal_pitch_motor.motor_gyro_set = gimbal_PID_calc(&loop->gimbal_pitch_motor.gimbal_motor_absolute_angle_pid, 
																										loop->gimbal_pitch_motor.absolute_angle, 
																										loop->gimbal_pitch_motor.absolute_angle_set, 
																										loop->gimbal_pitch_motor.motor_gyro);  
		
    loop->gimbal_pitch_motor.current_set = PID_calc(&loop->gimbal_pitch_motor.gimbal_motor_gyro_pid, 
																					loop->gimbal_pitch_motor.motor_gyro, 
																					loop->gimbal_pitch_motor.motor_gyro_set);
		
	  if(loop->gimbal_yaw_motor.gimbal_motor_mode == GIMBAL_ZERO)//如果云台关闭则清除所有输出值
			{
				
				loop->yaw_motor_speed.out = 0;
				loop->pitch_motor_speed.out = 0;
				loop->gimbal_pitch_motor.absolute_angle_set = 0;
				loop->gimbal_yaw_motor.current_set = 0;
				loop->gimbal_pitch_motor.current_set = 0;
								
			}

		//CAN_cmd_gimbal(loop->gimbal_yaw_motor.current_set, loop->gimbal_pitch_motor.current_set, 0, 0);
    
			
}

