/*     
@author    刘根 
所有的cmd指令都在这个文件里，总体使用cjson函数进行json数据的解析、打包工作
解析相应的指令之后通过指针函数跳转到对应的函数执行对应的命令
所有指令函数的初始化写在这个文件的最后处
新增函数以及指令后直接在最后处加入结构体初始化即可，无需添加其他部分
*/

#include "cmd_ctr.h"
#include "chassic_task.h"

flash_date save_date;//flash的数据
flash_date mid_date;//传输给init的中间数据

uint8_t cJson_buffer[BUF_MAX];

uint8_t send_flag=0;//发送wave数据的flag
char json_str11[1024];


void flash_date_init()
{
	 if(save_date.flash_or_define==USE_FLASH_DATE)//如果读取到标志位为flash使用flash中的数据
		{
			memcpy(&mid_date,&save_date,sizeof(save_date));
		}
		else//否则使用define中的数据
		{
		  mid_date.motor_speed_pid[0]=M3505_v_P;
			mid_date.motor_speed_pid[1]=M3505_v_I;
			mid_date.motor_speed_pid[2]=M3505_v_D;	
			      
			mid_date.chassis_angle_pid1[0]=M3505_angle_P;
			mid_date.chassis_angle_pid1[1]=M3505_angle_I;
			mid_date.chassis_angle_pid1[2]=M3505_angle_D;
			
			mid_date.chassis_speed_pid1[0]=M3505_speed_P;
			mid_date.chassis_speed_pid1[1]=M3505_speed_I;
			mid_date.chassis_speed_pid1[2]=M3505_speed_D;			
      			
			mid_date.Pitch_speed_pid[0]=PITCH_SPEED_PID_KP;
		  mid_date.Pitch_speed_pid[1]=PITCH_SPEED_PID_KI;
		  mid_date.Pitch_speed_pid[2]=PITCH_SPEED_PID_KD;			

      mid_date.PITCH_GYRO_ABSOLUTE_PID[0]=PITCH_GYRO_ABSOLUTE_PID_KP;
			mid_date.PITCH_GYRO_ABSOLUTE_PID[1]=PITCH_GYRO_ABSOLUTE_PID_KI;
			mid_date.PITCH_GYRO_ABSOLUTE_PID[2]=PITCH_GYRO_ABSOLUTE_PID_KD;
			
      mid_date.YAW_GYRO_ABSOLUTE_PID[0]=YAW_GYRO_ABSOLUTE_PID_KP;
			mid_date.YAW_GYRO_ABSOLUTE_PID[1]=YAW_GYRO_ABSOLUTE_PID_KI;
      mid_date.YAW_GYRO_ABSOLUTE_PID[2]=YAW_GYRO_ABSOLUTE_PID_KD;

			mid_date.Yaw_speed_pid[0]=YAW_SPEED_PID_KP;
			mid_date.Yaw_speed_pid[1]=YAW_SPEED_PID_KI;			
			mid_date.Yaw_speed_pid[2]=YAW_SPEED_PID_KD;

      mid_date.local_cali_t.offset[0]=0;
			mid_date.local_cali_t.offset[1]=0;
			mid_date.local_cali_t.offset[2]=0;
			mid_date.local_cali_t.scale[0]=1;
			mid_date.local_cali_t.scale[1]=1;
			mid_date.local_cali_t.scale[2]=1;
		}
		if(save_date.flash_flag==0XFF)//这里又加了一个flag是为了区分如果flash中有数据就不重新写入了
		{			
			memcpy(&save_date,&mid_date,sizeof(save_date));//将define的参数复制给save数据并保存到flashsave_date
			save_date.flash_flag=0;
			STMFLASH_Write(DATE_START_ADD, (uint32_t *)&save_date, sizeof(save_date));//写入define的数据到flash		
		}
}

void parse_loop(const char *JSON)//
{

	cJSON *json,*cmd , *dat;
	json = cJSON_Parse(JSON);//
	if(json==NULL)
	{
		usb_printf("Json format error");
		
	}
	else                                                
	{
    
		cmd = cJSON_GetObjectItem(json,"cmd");   
		dat = cJSON_GetObjectItem(json,"dat");   
		cmd_test(cmd,dat);//

	}

	cJSON_Delete(json); // 这里只需要释放一次内存，在子函数里不需要再次释放（会导致卡死）
	
}

void cmd_test(cJSON *cmd, cJSON *dat)
{
		for (uint8_t i = 0; i < MAX_CMD_NUM; i++)//遍历是哪一个字符串指令
	{
		if(strcmp(cmd->valuestring,cmd_lsts[i].cmd_name)==0)
		{
            cmd_lsts[i].do_cmd(dat);  //执行对应的函数
			break;
		}
	}
}

/*
遥控器控制
（自启动默认为遥控器控制）
例：
{
"cmd":"ctr_mode",
"dat":
{"mode":"remote",
 "date":[0]
}
}

//上位机控制
（date三个数据为x，y，wz的数据，flota类型）
例：
{
	"cmd": "ctr_mode",
	"dat": {
		"mode": "auto",
		"date": [0.1, 0.1, 0.1]
	}
}

//date两个数据为yaw轴数据和pitch轴数据
{
	"cmd": "ctr_mode",
	"dat": {
		"mode": "visual",
		"date": [0.1, 0.1]
	}
}
*/

float auto_set_v[3];//三个数据为x，y，wz的数据，flota类型

static cJSON *ctr_mode(cJSON *dat)
{
	static char remote[]="remote";
	static char auto_m[]="auto";
	static char visual_m[]="visual";
	
//	if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,remote)==0)	//解析对象中的mode字符串
//	{
//		  chassic_ctrl.Sport_mode=remote_mode;//设置为遥控器控制
//		  char * mode_str=cJSON_GetObjectItem(dat,"mode")->valuestring;
//			usb_printf("%s\n",mode_str);
//		  memset(auto_set_v,0,4*3);//运动用数据清零
//			free(mode_str);//释放指针内存
//			mode_str = NULL;
//	}
//	else if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,auto_m)==0)
//	{
//		  chassic_ctrl.Sport_mode=auto_mod;//设置为自动控制
//		  char * mode_str=cJSON_GetObjectItem(dat,"mode")->valuestring;
//			usb_printf("%s\n",mode_str);
//		  cJSON* date = cJSON_GetObjectItem(dat,"date");//解析数据
//			int date_i_size = cJSON_GetArraySize(date);	//获取数组成员个数 
//			for(int i=0;i<date_i_size;i++)
//			{
//				auto_set_v[i]=cJSON_GetArrayItem(date,i)->valuedouble;//传入接收到的数据
//			}	
//		
//			free(mode_str);//释放指针内存
//			mode_str = NULL;
//	}
	 if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,visual_m)==0)
	{
		  char * mode_str=cJSON_GetObjectItem(dat,"mode")->valuestring;
			usb_printf("%s\n",mode_str);
		  cJSON* date = cJSON_GetObjectItem(dat,"date");//解析数据
			gimbal_ctrl.add_yaw=cJSON_GetArrayItem(date,0)->valuedouble;//传入接收到的数据到yaw视觉参数
		  gimbal_ctrl.add_pitch=cJSON_GetArrayItem(date,1)->valuedouble;//传入接收到的数据到pitch视觉参数
		  gimbal_ctrl.flag_shoot = cJSON_GetArrayItem(date,2)->valuedouble;//传入接收到的数据到shoot参数	
	  
			free(mode_str);//释放指针内存
			mode_str = NULL;
		
	}
	else
	{
	   usb_printf("ctr_mode format error");//控制指令错误
	}
	
//	

}

//设置参数
/*

例：date数据依次为 p i d
{
"cmd":"set_par",
"dat":
{"mode":"motor_speed_pid",
 "date":[0.1,0.1,0.1]  
}
}
*/

//	fp32 motor_speed_pid[3];//轮子电机的速度环
//	fp32 chassis_angle_pid1[3];//底盘跟随云台的角度环
//  fp32 chassis_speed_pid1[3];//底盘跟随云台的速度环
//	
//	fp32 PITCH_GYRO_ABSOLUTE_PID[3];//pitch的角度环
//	fp32 Pitch_speed_pid[3];//pitch速度环
//	
//	fp32 YAW_GYRO_ABSOLUTE_PID[3];//yaw的角度环
//	fp32 Yaw_speed_pid[3];//yaw速度环

static cJSON *set_par(cJSON *dat)
{
	static char motor_speed_pid[]="motor_speed_pid";
	static char chassis_angle_pid[]="chassis_angle_pid1";	
	static char chassis_speed_pid[]="chassis_angle_pid1";
	
	static char pitch_angle_pid[]="pitch_angle_pid";	
	static char Pitch_speed_pid[]="chassis_angle_pid1";	
	static char yaw_angle_pid[]="yaw_angle_pid";	
	static char Yaw_speed_pid[]="Yaw_speed_pid";	
	
	if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,motor_speed_pid)==0)	//解析对象中的mode字符串
	{
		  char * mode_str=cJSON_GetObjectItem(dat,"mode")->valuestring;
		  cJSON* date = cJSON_GetObjectItem(dat,"date");//解析数据
			int date_i_size = cJSON_GetArraySize(date);	//获取数组成员个数 
			for(int i=0;i<date_i_size;i++)
			{
				save_date.motor_speed_pid[i]=cJSON_GetArrayItem(date,i)->valuedouble;//传入接收到的数据
			}	
			gimbal_init(&gimbal_ctrl); //初始化
			usb_printf("motor_speed_pid");
			free(mode_str);//释放指针内存
			mode_str = NULL;
			
	}
//	else if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,auto_m)==0)
//	{
//		  chassic_ctrl.Sport_mode=auto_mod;//设置为自动控制
//		  char * mode_str=cJSON_GetObjectItem(dat,"mode")->valuestring;
//			usb_printf("%s\n",mode_str);
//		  cJSON* date = cJSON_GetObjectItem(dat,"date");//解析数据
//			int date_i_size = cJSON_GetArraySize(date);	//获取数组成员个数 
//			for(int i=0;i<date_i_size;i++)
//			{
//				auto_set_v[i]=cJSON_GetArrayItem(date,i)->valuedouble;//传入接收到的数据
//			}	
//		
//			free(mode_str);//释放指针内存
//			mode_str = NULL;
//	
//	}
	else
	{
	   usb_printf("set_par format error");//控制指令错误
	}

}

//获取结构体中的数据并打印
/*例：
{
"cmd":"get_par",
"dat":0
}
*/
//这里usb_printf只能调用一次，调用多次会导致打印不完整
static cJSON *get_par(cJSON *dat)
{
//	usb_printf("%s\n",save_date.version);
	
	usb_printf(
	"\r\n\
	%s\r\n\
	motor_speed_pid:[%f,%f,%f]\r\n\
	\r\n", 
	save_date.version,
  save_date.motor_speed_pid[0],
	save_date.motor_speed_pid[1],
	save_date.motor_speed_pid[2]);
}

/*例：将结构体数据保存到flash中
{
"cmd":"save",
"dat":0
}
*/
static cJSON *save(cJSON *dat)
{
	STMFLASH_Write(DATE_START_ADD, (uint32_t *)&save_date, sizeof(save_date));
	usb_printf("Save successfully\n");
}


/*例：使用代码原始数据中的define
{
"cmd":"use_def",
"dat":0
}
*/
static cJSON *use_def(cJSON *dat)
{
	save_date.flash_or_define=USE_DEFINE_DATE;
	gimbal_init(&gimbal_ctrl);
	//chassis_init(&chassic_ctrl); //初始化
	usb_printf("use_def successfully\n");
}

/*例：使用flash里的数据(程序里会在开机时读取flash数据，如果读取到有flash标志位的数据，则优先使用flash里的数据，如果没有写入过flash数据则默认使用defin的数据)
{
"cmd":"use_flash",
"dat":0
}
*/
static cJSON *use_flash(cJSON *dat)
{
	save_date.flash_or_define=USE_FLASH_DATE;
	gimbal_init(&gimbal_ctrl);//初始化
	usb_printf("use flash date\n");
}

/*例：
{
"cmd":"def2flash",
"dat":0
}
*/
static cJSON *def2flash(cJSON *dat)
{
	STMFLASH_Write(DATE_START_ADD, (uint32_t *)&mid_date, sizeof(mid_date));//写入define的数据到flash		
}

//获取所有支持的指令，发送给上位机
//接收到的
/*例：
{
"cmd":"get_cmd_name",
"dat":0
}

//发送出去的
{
	"cmd": "get_cmd_name",
	"dat": {
		"name":["name1", "name2", "name3"]
	}
}
*/
int num_cmds;
static cJSON *get_cmd_name(cJSON *dat)
{
	  cJSON *cmd_date_str = cJSON_CreateObject();

    // 添加cmd字段
    cJSON_AddStringToObject(cmd_date_str, "cmd", "get_cmd_name");

    // 创建dat子对象
    cJSON *date = cJSON_CreateObject();

    // 创建name数组
    cJSON *name_array = cJSON_CreateArray();
	
	 num_cmds = sizeof(cmd_lsts) / sizeof(cmd);
	
	 int valid_count = 0;

    for (int i = 0; i < num_cmds; i++) {
        if (cmd_lsts[i].do_cmd != NULL) {
            valid_count++;
        }
    }

		for(int i=0;i<valid_count;i++)
		{
			 cJSON_AddStringToObject(name_array,"",cmd_lsts[i].cmd_name); 		//添加字符串到数组
		}

    // 将name数组添加到date子对象中
    cJSON_AddItemToObject(date, "name", name_array);

    // 将dat子对象添加到cmd_date_str对象中
    cJSON_AddItemToObject(cmd_date_str, "dat", date);

    // 将JSON对象转换为字符串
    char *json_str = cJSON_Print(cmd_date_str);

    // 输出JSON字符串
    usb_printf("%s\n", json_str);

    // 释放内存
    cJSON_Delete(cmd_date_str);
    free(json_str);
	
}

/*例：软件复位
{
"cmd":"reset",
"dat":0
}
*/
static cJSON *reset(cJSON *dat)
{
  SCB->AIRCR =((0X5FA << SCB_AIRCR_VECTKEY_Pos)     |
	            (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
	              SCB_AIRCR_SYSRESETREQ_Msk);
}

//定义一个字符串指针，记录每个支持的数据类型
char *wave_name[]={
     "imu_yaw",
	   "imu_pitch",
	   "imu_roll",
};
//定义一个字符串指针，记录每个支持的数据类型
enum wave_date
{
	imu_yaw,
	imu_pitch,
	imu_roll
};

#define WAVE_NUM (sizeof(wave_name) / sizeof(wave_name[0]))

uint8_t wave_start_flag=0;

uint8_t check_wave_start_flag() 
{
     wave_start_flag = 0; // 初始化为0

    for (int i = 0; i < WAVE_NUM; i++) 
	  {
        if (save_date.wave_num_flag[i] == 1) 
				{
            wave_start_flag = 1; // 只要有一个元素为1，则标志位置为1
            break; // 提前结束循环
        }
    }

    return wave_start_flag;
}



//接收数据
/*例:使能波形数据
date_name1为要输出的数据名称，后面跟的1为使能，0为不使能
{
"cmd":"wave_set",
"dat":{
       "imu_roll":0,
       "imu_yaw":1,
       "imu_pitch":1
      }
}        
*/
static cJSON *wave_set(cJSON *dat)
{
//	int num_strings = sizeof(wave_name) / sizeof(wave_name[0]);
	//获取子节点
    cJSON* cur_item = dat->child;
	  while (cur_item) //判断是不是空指针
		{
        if (cur_item->type == cJSON_Number) 
				{
					for(uint8_t i=0;i<WAVE_NUM;i++)//遍历数组指针，和子节点的string做对比
					{
						if(strcmp(cur_item->string,wave_name[i])==0)//判断是哪一个波形，赋值对应的数组指针
						{
							 save_date.wave_num_flag[i]=cur_item->valueint;
						}
					}
        }
        cur_item = cur_item->next;//转移到下一个链表节点
    }
			
			for(int i=0;i<WAVE_NUM;i++)
			{
				if(save_date.wave_num_flag[i]==1)//当有一个数据为1，则置为1，并标志位开始输出数据
				 {
				    wave_start_flag=1;
				 }
				 
			}
}
/*打开或者关闭wave数据
例：
{
	"cmd": "switch_wave",
	"dat": {
		"mode": "open_send"
	}
}
*/
uint8_t switch_wave_flag=1;

static cJSON *switch_wave(cJSON *dat)
{
	if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,"open_send")==0)
	{
	   switch_wave_flag=1;
	}
	else 
	{
	   switch_wave_flag=0;
	}
}


//用于生成虚拟示波器的数据流
/*例:发送波形数据
date_name1为要输出的数据名称，后面跟的为对应的数据
{
"cmd":"wave_dis",
"dat":{
       "imu_roll":0.5,
       "imu_yaw":0.6
      }
}
*/




char wavedisplay()
{
    // 构建JSON字符串
    sprintf(json_str11, "{\"cmd\":\"wave_dis\",\"dat\":{");

    if(save_date.wave_num_flag[imu_roll]==1)
    {
      char roll_str[32];
      sprintf(roll_str, "\"imu_roll\":%f,", 0.5);
    strcat(json_str11, roll_str);
    }

    if(save_date.wave_num_flag[imu_yaw]==1)
    {
      char yaw_str[32];
      sprintf(yaw_str, "\"imu_yaw\":%f,", gimbal_ctrl.gimbal_yaw_motor.absolute_angle);
      strcat(json_str11, yaw_str);
    }

    if(save_date.wave_num_flag[imu_pitch]==1)
    {
      char pitch_str[32];
      sprintf(pitch_str, "\"imu_pitch\":%f,", gimbal_ctrl.gimbal_pitch_motor.absolute_angle);
      strcat(json_str11, pitch_str);
    }
////   if(save_date.wave_num_flag[robot_color]==1)
////    {
      //char color_str[32];
      //sprintf(color_str, "\"robot_color\":%d,",robot_color );
      //strcat(json_str11, color_str);
////    }
////		   if(save_date.wave_num_flag[imu_pitch]==1)
////    {
      char speed_level_str[32];
      sprintf(speed_level_str, "\"speed_level\":%d,", shoot.speed_level);
      strcat(json_str11, speed_level_str);
////    }
    if(json_str11[strlen(json_str11)-1] == ',') // 判断是否需要删除最后一个逗号
      json_str11[strlen(json_str11)-1] = '\0';

    strcat(json_str11, "}}");
	
}

//陀螺仪零飘初始化
/*例:1: 代表需要校准,
   date后面跟的为校准5s
{
"cmd":"imu_cali",
"dat":{
       "mode": "0",
       "date":[5]
      }
}
*/

uint8_t imu_cail_flag=0;
uint16_t imu_cali_time;

static cJSON *imu_cali(cJSON *dat)
{
	if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,"1")==0)
	imu_cail_flag=1;
	else if(strcmp(cJSON_GetObjectItem(dat,"mode")->valuestring,"0")==0)
	imu_cail_flag=0;	
	cJSON *date = cJSON_GetObjectItem(dat, "date");
	imu_cali_time = cJSON_GetArrayItem(date, 0)->valuedouble*100;	
}

// 0: 代表用校准数据初始化原始数据
// 1: 代表需要校准
uint8_t imu_cail_behaviour()
{	 
	  if(imu_cail_flag==0)//默认为0，表示不用校准，直接设置参数为flash内部数据
    {
        INS_set_cali_gyro(save_date.local_cali_t.scale, save_date.local_cali_t.offset);
        
        return 0;
    }
    else if (imu_cail_flag==1)//如果接收到校准指令
    {
        static uint16_t count_time = 0;
        INS_cali_gyro(save_date.local_cali_t.scale, save_date.local_cali_t.offset, &count_time);
        if (count_time > imu_cali_time)
        {
            count_time = 0;
					  RC_restart(SBUS_RX_BUF_NUM);//打开遥控器使能
					  imu_cail_flag=0;//置0
					  usb_printf("imu cali is ok\n");
					  return 1;
        }
        else
        {
            RC_unable();//关闭遥控器
            return 0;
        }
    }

    return 0;
}

/*设置版本信息
{
"cmd":"imu_cali",
"dat":{
"mode": "version:v1.0 ",
      }
}
*/
static cJSON *set_version(cJSON *dat)
{
	  memset(save_date.version,0xff,version_len);//清除这片内存里的所有数据
	  char *str=cJSON_GetObjectItem(dat,"mode")->valuestring;
		strcpy(save_date.version,str);
		usb_printf("%s\n",save_date.version);
	  free(str);
}

cmd cmd_lsts[MAX_CMD_NUM] = 
{
    {"ctr_mode", ctr_mode},
		{"set_par",set_par},
    {"get_par",get_par},
		{"save",save},
		{"use_def",use_def},
		{"use_flash",use_flash},
		{"reset",reset},
	  {"def2flash",def2flash},
		{"get_cmd_name",get_cmd_name},
		{"wave_set",wave_set},
		{"imu_cali",imu_cali},
		{"set_version",set_version},
		{"switch_wave",switch_wave}
};
