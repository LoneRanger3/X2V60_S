/*************************************************
Copyright ? xmsilicon. 2023-2024. All rights reserved. 
文件名: xm_tp2804.h
作者@编号： wuyunhui
版本: V1.00
完成日期: 2023.06.26
功能描述: AD的SDK提供
具体说明：
		 1.确认AD 地址
		 2.修改 XM_MPI_AD_SocInit，配置当前方案硬件对应的GPIO内容
		 3.修改 XM_MPI_AD_AdInit，配置AD使用的对应分辨率制式的寄存器
		 4.修改 XM_MPI_AD_LossDetection，配置对应寄存器视频丢失检测内容（可以根据方案是否需要，不需要的直接 return 0）
		 
修改历史: 无
杭州雄迈集成电路技术股份有限公司
*************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdbool.h>
#include "i2c.h"
#include "Log.h"
#include "PeripheryThread.h"
#define DEBUG_TEST(fmt,arg...)           printf("<<-AD-INFO->> " fmt"\n",##arg)
#define AD_WARN(fmt,arg...)           printf("<<-AD-WARN->> [%d]" fmt"\n",__LINE__, ##arg)
#define AD_ERROR(fmt,arg...)          printf("<<-AD-ERROR->> [%d]" fmt"\n",__LINE__, ##arg)
#define AD_DEBUG(fmt,arg...)          printf("<<-AD-DEBUG->> [%d]" fmt"\n",__LINE__, ##arg);
                                       

/***********************************************
功能: AD_GPIO_REST 为 AD reset GPIO 号
***********************************************/
#define AD_GPIO_REST 137

/***********************************************
使用软件IIC：#define USE_SIMULATE_IIC = 1
使用硬件IIC：#define USE_SIMULATE_IIC = 0
***********************************************/
#define USE_SIMULATE_IIC 1

/***********************************************
IIC_GPIO_SCL = scl的GPIO号,如scl = IO_GPIO_E135，即IIC_GPIO_SCL = 135;
IIC_GPIO_SDA = sda的GPIO号,如sda = IO_GPIO_E133，即IIC_GPIO_SDA = 133;
IIC_ID 为 硬件IIC 的 ID 号,使用软件IIC时不做修改
{
	IIC1: IIC_ID = 1,
	IIC2: IIC_ID = 2,
		......
		以此类推
}
***********************************************/
#if USE_SIMULATE_IIC
	#define IIC_GPIO_SCL 135
	#define IIC_GPIO_SDA 133
	#define IIC_GPIO_UDELAY 2
#else
	#define IIC_ID 1
#endif

/***********************************************
备注：27M时钟，如 GPIO_SEN1_MCLK = GPIO号，如SEN1_MCLK = H236，即GPIO_SEN1_MCLK = 236
***********************************************/
#define GPIO_SEN1_MCLK 236

/***********************************************
功能: AD I2C地址
备注:
IIC通信读写寄存器使用，目前为TP2804使用的AD地址
***********************************************/
#define XM_AD_ADDR 0x88

/***********************************************
枚举功能: I2C模式
备注:
IIC_MODE_HARDWARE 为硬件IIC
IIC_MODE_SIMULATE 为软件IIC
***********************************************/
typedef enum xm_IIC_MODE_E
{
	IIC_MODE_HARDWARE,
	IIC_MODE_SIMULATE,
	IIC_MODE_MAX,
}IIC_MODE_E;
	
/***********************************************
枚举功能: 使用AD出图的前端信息，选择AHD TVI CVI
***********************************************/
typedef enum xm_VIDEO_TYPE_E
{
	VIDEO_AHD = 0,
	VIDEO_TVI,
	VIDEO_CVI,
	VIDEO_TYPE_MAX
}VIDEO_TYPE_E;

/***********************************************
枚举功能: 使用AD出图的前端信息，选择分辨率及帧率
***********************************************/
typedef enum xm_VIDEO_RESOLUTION_E
{
	VIDEO_1M_25FPS = 0,
	VIDEO_1M_30FPS,
	VIDEO_2M_25FPS,
	VIDEO_2M_30FPS,
	VIDEO_RESOLUTION_MAX
}VIDEO_RESOLUTION_E;
	
/***********************************************
结构体功能:配置使用前端的信息
备注:
1.根据前端信息的枚举进行设置
2.video_type 配置制式（AHD\TVI\CVI）
3.video_resolution 配置对应分辨率帧率，比如2M 25帧设置 VIDEO_2M_25FPS
***********************************************/
typedef struct xm_VIDEO_INFO_S
{
	unsigned char video_type;
	unsigned char video_resolution;
}VIDEO_INFO_S;

/***********************************************
结构功能: I2C配置结构体
备注:
1.选择AD reset GPIO 号
2.根据硬件确认GPIO使用的是硬件IIC还是软件IIC，使用 xm_IIC_MODE_E 枚举值
3.如果使用的是软件IIC，data[0] = scl的GPIO号，如GPIO号=E135 ，即data[0] = 135;
  data[1] = sda对应的GPIO号,即data[0] = 133
4.gpio_senl_mclk 为 27M时钟
***********************************************/
typedef struct xm_SOC_CONFIG_S
{
	unsigned int reset_ad_gpio;
	int iic_mode;
	int data[2];
	VIDEO_INFO_S video_info;
	unsigned int gpio_senl_mclk;
}SOC_CONFIG_S;

/***********************************************
函数功能：对AD寄存器进行读操作，
输入参数：device_addr：AD IIC地址，reg_addr：AD内部寄存器地址
***********************************************/
extern "C" int xm_Gpio_IIc_Write(unsigned char device_addr, unsigned char reg_addr, unsigned char reg_val);

/***********************************************
函数功能：对AD寄存器进行写操作，
输入参数：device_addr：AD IIC地址，reg_addr：AD内部寄存器地址，reg_val：需要写入的寄存器值
***********************************************/
extern  "C" unsigned char xm_Gpio_IIc_Read(unsigned char device_addr, unsigned char reg_addr);
int rx_i2c_write(unsigned char device_addr, unsigned char reg_addr, unsigned char reg_val)
{	
	int err = 0;
	int cnt = 5;
	err = xm_Gpio_IIc_Write(device_addr, reg_addr, reg_val);
	while(err == -1){
		AD_WARN(">>>>>>>rx_i2c_write:%#X,%#X",reg_addr,reg_val);
		cnt--;
		if(cnt<=0){
			AD_ERROR(">>>>>>>rx_i2c_write err over:%#X,%#X",reg_addr,reg_val);
			break;
		}
		err = xm_Gpio_IIc_Write(device_addr, reg_addr, reg_val);
	}
	return err;
}


/* read ad ic register */
unsigned char rx_i2c_read(unsigned char device_addr, unsigned char reg_addr)
{
	unsigned char reg_val = 0x00;
	int err = 0;
	int cnt = 5;
	reg_val = xm_Gpio_IIc_Read(device_addr, reg_addr);
	while(err == -1){
		AD_WARN(">>>>>>>rx_i2c_read:%#X,%#X",reg_addr,reg_val);
		cnt--;
		if(cnt<=0){
			AD_ERROR(">>>>>>>rx_i2c_read err over:%#X,%#X",reg_addr,reg_val);
			break;
		}
		reg_val = xm_Gpio_IIc_Read(device_addr, reg_addr);
	}
	return reg_val;	
}


/***********************************************
只读数组功能:AD涉及的寄存器配置
备注:
1.选择所需要的分辨率制式配置存入数组
2.目前使用的是AHD 1080P 25FPS的配置
3.第一列：s_aau8InitList_AD[][0]为寄存器地址
  第二列：s_aau8InitList_AD[][1]为寄存器对应的值
***********************************************/
static const unsigned char s_aau8InitList_AD[][2] = {
///////////////XM 2804-1080P-AHD-P制///////////////
  #if 1  
	{0x40, 0x00},
	{0x42, 0x00},
	{0x4c, 0x43},
	{0x4e, 0x1d},
	{0x54, 0x04},
	{0xf3, 0x02},//原始不配置，相当于默认0
	{0xf6, 0x00},
	{0xf7, 0x44},
	{0xfa, 0x00},
	{0x1b, 0x01},
	{0x41, 0x00},
	{0x40, 0x08},
	{0x13, 0xef},
	{0x14, 0x41},
	{0x15, 0x02},
	{0x40, 0x00},
	{0x40, 0x08},
	{0x12, 0x54},
	{0x13, 0xef},
	{0x14, 0x41},
	{0x15, 0x02},
	{0x40, 0x00},
	{0x40, 0x00},
	{0x02, 0xc8},
	{0x07, 0xc0}, 
	{0x0b, 0xc0},		
	{0x0c, 0x03}, 
	{0x0d, 0x50},  
	{0x15, 0x03},
	{0x16, 0xd2}, 
	{0x17, 0x80}, 
	{0x18, 0x29},
	{0x19, 0x38},
	{0x1a, 0x47},				
	{0x1c, 0x0a},
	{0x1d, 0x50},
	{0x20, 0x30},
	{0x21, 0x84},
	{0x22, 0x36},
	{0x23, 0x3c},
	{0x2b, 0x60},  
	{0x2c, 0x2a}, 
	{0x2d, 0x30},
	{0x2e, 0x70},
	{0x30, 0x48},  
	{0x31, 0xbb}, 
	{0x32, 0x2e},
	{0x33, 0x90},
	{0x35, 0x05},
	{0x38, 0x00}, 
	{0x39, 0x1C},	
	{0x10, 0x08},
	{0x11, 0x48},
	{0x12, 0x50},
	{0x13, 0x08},
	{0x02, 0xcc},
	{0x0d, 0x73},
	{0x15, 0x01},
	{0x16, 0xf0},
	{0x18, 0x2a},
	{0x20, 0x3c},
	{0x21, 0x84},  //原始 0x46   修改0x4F
	{0x25, 0xfe},
	{0x26, 0x0d},
	{0x2c, 0x3a},
	{0x2d, 0x54},
	{0x2e, 0x40},
	{0x30, 0xa5},
	{0x31, 0x86},
	{0x32, 0xfb},
	{0x33, 0x60},	
	{0x34, 0x10},//关掉bt656 sav eav 异或校验位
	#endif
////////////////////////////////////////////////////

	
///////////////JL 2804-1080P-AHD-P制///////////////
#if 0
{0x40, 0x00}, //select decoder page
{0x42, 0x00},	//common setting for all format
{0x4c, 0x43},	//common setting for all format
{0x4e, 0x1d}, //common setting for dvp output
{0x54, 0x04}, //common setting for dvp output

{0xf6, 0x00},	//common setting for all format
{0xf7, 0x44}, //common setting for dvp output
{0xfa, 0x00}, //common setting for dvp output
{0x1b, 0x01}, //common setting for dvp output
{0x41, 0x00},		//video MUX select

{0x40, 0x08},	//common setting for all format
{0x13, 0xef}, //common setting for dvp output
{0x14, 0x41}, //common setting for dvp output
{0x15, 0x02}, //common setting for dvp output

{0x40, 0x00}, //select decoder page

{0x40, 0x08},
{0x12, 0x54},
{0x13, 0xef},
{0x14, 0x41},
{0x15, 0x02},

{0x40, 0x00},

{0x02, 0xc8},
{0x07, 0xc0},
{0x0b, 0xc0},
{0x0c, 0x03},
{0x0d, 0x50},

{0x0F,0x00},
{0x10,0x00},
{0x11,0x50},
{0x12,0x60},
{0x13,0x00},
{0x14,0x00},

{0x15, 0x03},
{0x16, 0xd2},
{0x17, 0x80},
{0x18, 0x29},
{0x19, 0x38},
{0x1a, 0x47},

{0x1c, 0x0a},  //1920*1080, 25fps
{0x1d, 0x50},  //

{0x20, 0x30},
{0x21, 0x84},
{0x22, 0x36},
{0x23, 0x3c},

{0x2b, 0x60},
{0x2c, 0x2a},
{0x2d, 0x30},
{0x2e, 0x70},

{0x30, 0x48},
{0x31, 0xbb},
{0x32, 0x2e},
{0x33, 0x90},

{0x35, 0x05},
{0x38, 0x00},
{0x39, 0x1C},


{0x02, 0xcc},

{0x0d, 0x73},

{0x15, 0x01},
{0x16, 0xf0},
{0x18, 0x2a},

{0x20, 0x3c},
{0x21, 0x46},

{0x25, 0xfe},
{0x26, 0x0d},

{0x2c, 0x3a},
{0x2d, 0x54},
{0x2e, 0x40},

{0x30, 0xa5},
{0x31, 0x86},
{0x32, 0xfb},
{0x33, 0x60},
{0x34, 0x10},//关掉bt656 sav eav 异或校验位
#endif
/////////////////////////////////////////////

///////////////2804-720P-AHD-P制///////////////
	#if 0  
    {0x40, 0x00}, //select decoder page
	{0x42, 0x00},	//common setting for all format
	{0x4c, 0x43},	//common setting for all format
	{0x4e, 0x1d}, //common setting for dvp output
	{0x54, 0x04}, //common setting for dvp output

	{0xf6, 0x00},	//common setting for all format
	{0xf7, 0x44}, //common setting for dvp output
	{0xfa, 0x00}, //common setting for dvp output
	{0x1b, 0x01}, //common setting for dvp output
	{0x41, 0x00},		//video MUX select

	{0x40, 0x08},	//common setting for all format
	{0x13, 0xef}, //common setting for dvp output
	{0x14, 0x41}, //common setting for dvp output
	{0x15, 0x02}, //common setting for dvp output
    {0x40, 0x00},
    {0x40, 0x08},

	
	{0x12, 0x54},
	{0x13, 0xef},
	{0x14, 0x41},
	{0x15, 0x12},
	{0x40, 0x00},
	{0x40, 0x00},
			
	
	{0x02, 0xca},
	{0x07, 0xc0},
	{0x0b, 0xc0},
	{0x0c, 0x13},
	{0x0d, 0x50},

	{0x0F,0x00},
    {0x10,0x00},
    {0x11,0x50},
    {0x12,0x60},
    {0x13,0x00},
    {0x14,0x00},

	{0x15, 0x13},
	{0x16, 0x15},
	{0x17, 0x00},
	{0x18, 0x19},
	{0x19, 0xd0},
	{0x1a, 0x25},
	{0x1c, 0x07},  //1280*720, 25fps
	{0x1d, 0xbc},  //1280*720, 25fps

	{0x20, 0x30},
	{0x21, 0x84},
	{0x22, 0x36},
	{0x23, 0x3c},

	{0x2b, 0x60},
	{0x2c, 0x2a},
	{0x2d, 0x30},
	{0x2e, 0x70},

	{0x30, 0x48},
	{0x31, 0xbb},
	{0x32, 0x2e},
	{0x33, 0x90},

	{0x35, 0x25},
	{0x38, 0x00},
	{0x39, 0x18},


	{0x02, 0xce},

	{0x0d, 0x71},

	{0x18, 0x1b},

	{0x20, 0x40},
	{0x21, 0x46},

	{0x25, 0xfe},
	{0x26, 0x01},

	{0x2c, 0x3a},
	{0x2d, 0x5a},
	{0x2e, 0x40},

	{0x30, 0x9e},
	{0x31, 0x20},
	{0x32, 0x10},
	{0x33, 0x90},
	{0x34, 0x10}, //关掉bt656 sav eav 异或校验位
	#endif
 //////////////////////////////////////

///////////////9950-1080P-AHD-P制///////////////
#if 0
{0x02,0xCC},
{0x05,0x00},
{0x06,0x32},
{0x07,0xC0},
{0x08,0x00},
{0x09,0x24},
{0x0A,0x48},
{0x0B,0xC0},
{0x0C,0x03},
{0x0D,0x73},
{0x0E,0x00},
{0x0F,0x00},
{0x10,0x00},
{0x11,0x40},
{0x12,0x60},
{0x13,0x00},
{0x14,0x00},
{0x15,0x01},
{0x16,0xF0},
{0x17,0x80},
{0x18,0x29},
{0x19,0x38},
{0x1A,0x47},
{0x1B,0x01},
{0x1C,0x0A},
{0x1D,0x50},
{0x1E,0x80},
{0x1F,0x80},
{0x20,0x3C},
{0x21,0x46},
{0x22,0x36},
{0x23,0x3C},
{0x24,0x04},
{0x25,0xFE},
{0x26,0x0D},
{0x27,0x2D},
{0x28,0x00},
{0x29,0x48},
{0x2A,0x30},
{0x2B,0x60},
{0x2C,0x3A},
{0x2D,0x54},
{0x2E,0x40},
{0x2F,0x00},
{0x30,0xA5},
{0x31,0x82},//0x86
{0x32,0xFB},
{0x33,0x60},
{0x34,0x00},
{0x35,0x05},
{0x36,0xDC},
{0x37,0x00},
{0x38,0x00},
{0x39,0x1C},
{0x3A,0x32},
{0x3B,0x26},
{0x3C,0x00},
{0x3D,0x60},
{0x3E,0x00},
{0x3F,0x00},
{0x40,0x00},
{0x41,0x00},
{0x42,0x00},
{0x43,0x00},
{0x44,0x00},
{0x45,0x00},
{0x46,0x00},
{0x47,0x00},
{0x48,0x00},
{0x49,0x00},
{0x4A,0x00},
{0x4B,0x00},
{0x4C,0x43},
{0x4D,0x00},
{0x4E,0x17},
{0x4F,0x00},
{0x50,0x00},
{0x51,0x00},
{0x52,0x00},
{0x53,0x00},
{0x54,0x00},

{0xB3,0xFA},
{0xB4,0x00},
{0xB5,0x00},
{0xB6,0x00},
{0xB7,0x00},
{0xB8,0x00},
{0xB9,0x00},
{0xBA,0x00},
{0xBB,0x00},
{0xBC,0x00},
{0xBD,0x00},
{0xBE,0x00},
{0xBF,0x00},
{0xC0,0x00},
{0xC1,0x00},
{0xC2,0x0B},
{0xC3,0x0C},
{0xC4,0x00},
{0xC5,0x00},
{0xC6,0x1F},
{0xC7,0x78},
{0xC8,0x27},
{0xC9,0x00},
{0xCA,0x00},
{0xCB,0x07},
{0xCC,0x08},
{0xCD,0x00},
{0xCE,0x00},
{0xCF,0x04},
{0xD0,0x00},
{0xD1,0x00},
{0xD2,0x60},
{0xD3,0x10},
{0xD4,0x06},
{0xD5,0xBE},
{0xD6,0x39},
{0xD7,0x27},
{0xD8,0x00},
{0xD9,0x00},
{0xDA,0x00},
{0xDB,0x00},
{0xDC,0x00},
{0xDD,0x00},
{0xDE,0x00},
{0xDF,0x00},
{0xE0,0x00},
{0xE1,0x00},
{0xE2,0x00},
{0xE3,0x00},
{0xE4,0x00},
{0xE5,0x00},
{0xE6,0x00},
{0xE7,0x13},
{0xE8,0x03},
{0xE9,0x00},
{0xEA,0x00},
{0xEB,0x00},
{0xEC,0x00},
{0xED,0x00},
{0xEE,0x00},
{0xEF,0x00},
{0xF0,0x00},
{0xF1,0x00},
{0xF2,0x00},
{0xF3,0x00},
{0xF4,0x20},
{0xF5,0x10},
{0xF6,0x00},
{0xF7,0x00},
{0xF8,0x00},
{0xF9,0x00},
{0xFA,0x88},
{0xFB,0x00},
{0xFC,0x00},

{0x40,0x08},
{0x00,0x00},
{0x01,0xf8},
{0x02,0x01},
{0x08,0xF0},
{0x13,0x04},
{0x14,0x73},
{0x15,0x08},
{0x20,0x12},
{0x34,0x1b},
{0x23,0x02},
{0x23,0x00},

{0x40,0x00},
{0x34, 0x10}, //关掉bt656 sav eav 异或校验位
#endif
/////////////////////////////////////////
};

/***********************************************
函数功能: AD	涉及主控一些内容的配置
输入参数: SOC_CONFIG_S 配置结构体
返回参数: 无
备注:
1.根据不同的方案设计，修改对应方案 AD reset 的对应GPIO
2.根据不同的方案设计，修改对应方案 IIC 的对应GPIO，以及使用的IIC模式
3.拷贝结构体以供库中调用进行对应配置的设置
4.目前此接口使用的GPIO 为X27方案中使用
***********************************************/

extern "C" void XM_MPI_AD_SocInit(SOC_CONFIG_S *pSocConfig)
{
	SOC_CONFIG_S stSocConfig;
	memset(&stSocConfig, 0 , sizeof(SOC_CONFIG_S));

	stSocConfig.reset_ad_gpio = AD_GPIO_REST;			// AD reset gpio 使用的是 E137 则赋值 GPIO号 137
	stSocConfig.gpio_senl_mclk = GPIO_SEN1_MCLK;
	
#if USE_SIMULATE_IIC
	stSocConfig.iic_mode = IIC_MODE_SIMULATE;
	stSocConfig.data[0] = IIC_GPIO_SCL;
	stSocConfig.data[1] = IIC_GPIO_SDA;
	DEBUG_TEST("IIC_GPIO_SCL = %d,IIC_GPIO_SDA = %d\n",stSocConfig.data[0],stSocConfig.data[1]);
#else
	stSocConfig.iic_mode = IIC_MODE_HARDWARE;
	stSocConfig.data[0] = IIC_ID;
	DEBUG_TEST("IIC_ID = %d\n",stSocConfig.data[0]);
#endif
	stSocConfig.video_info.video_type = VIDEO_AHD;				// 配置 AHD
	stSocConfig.video_info.video_resolution = VIDEO_2M_25FPS;// 配置 2M 25帧
	memcpy(pSocConfig , &stSocConfig, sizeof(SOC_CONFIG_S));
	return;
}

/***********************************************
函数功能: AD初始化及对应分辨率配置
输入参数: 无
返回参数: 无
备注:
1.从AD厂家提供的SDK中，整理初始化以及需要使用的分辨率制式配置写入此接口
2.目前此接口使用的是TP2804 AHD 1080P 25帧的出图配置
***********************************************/
extern "C" void XM_MPI_AD_AdInit(void)
{
	unsigned int i,num;
	int val1,val2;
	val1 = rx_i2c_read(XM_AD_ADDR, 0xFE);
	val2 = rx_i2c_read(XM_AD_ADDR, 0xFF);
	if(val1==0x28 && val2==0x60){       //TP2804
	//	if(val1==0x28 && val2==0x50){//TP9950
		DEBUG_TEST("\n2804 check id success\n");	
		num = sizeof(s_aau8InitList_AD)/sizeof(s_aau8InitList_AD[0]);
		for(i = 0; i < num; i++){
			rx_i2c_write(XM_AD_ADDR, s_aau8InitList_AD[i][0], s_aau8InitList_AD[i][1]);
		}
		DEBUG_TEST("AHD 1080P25fps\n");
	}else{
		DEBUG_TEST("\n2804 check id fail\n");
	}
	usleep(600*1000);
	return;
}

/***********************************************
函数功能: 通道视频丢失检测
输入参数: 无
返回参数: 视频丢失检测结果
备注:
1.据AD提供的视频丢失寄存器判断各个通道视频丢失状态。
2.返回值是各个预览通道视频丢失状态。每个bit代表一个通道，低通道在低位，高通道在高位。
3.on video 视频在线则对应 bit 位置 0。
4.video loss 视频丢失则对应 bit 位置 1。
5.比如如果两路视频，第一路有视频接入，第二路没有，则根据以上规则，二进制查看对应bit0=0;bit1=1。返回值为0x02--二进制 1 0
6.需要注意的AD的通道与实际预览通道的对应关系，比如一路出图的时候为预览1通道，但是硬件可能使用的是AD的第二通道，这样寄存器的读取配置应该使用AD的2通道内容
7.目前此接口内容对应的是TP2804 1通道视频丢失检测，对应使用的是AD VIN1 第1通道，操作对应寄存器
***********************************************/
extern "C" int XM_MPI_AD_LossDetection(void)
{
	unsigned char channel = 0; // TP2804一路出图，第一通道
	int val = 0;
	int val_los = 0;

#if 1//信号检测	
	rx_i2c_write(XM_AD_ADDR, 0x40, 0x00); // 对应使用的是 AD VIN1 读取对应VIN1 的视频丢失寄存器状态
	val = rx_i2c_read(XM_AD_ADDR, 0x01);

	val = (val & 0x80) >> 7; // bit7: =1:video loss ; =0:on video

	val_los |= (val << channel); // 根据低通道低位，高通道高位，左移操作，此时第一通道，左移 0
	val_los &= 0xffffffff;
 #else //硬件检测
  val_los=CarinDetect();
 #endif
	return val_los;
}


