/*******************************************************************************
Copyright © XmSilicon Tech Co., Ltd. 2022-2022. All rights reserved.
文件名: adr.h
作者@编号： 肖建飞
功能描述: demo头文件
******************************************************************************/
#ifndef __ADR__H__
#define __ADR__H__

#define MY_ERR_PRT(str, err_num) printf("\033[31m %s error ret=0x%x \033[0m \n", str, err_num)
#define MY_STATUS_PRT(str) printf("/033[33m %s /033[0m \n", str)
#define CHECK_RET(express, name) \
			do { \
				XM_S32 Ret; \
				Ret = express; \
				if (XM_SUCCESS != Ret) { \
					printf("\033[0;31m[%s:%d]%s failed with %#x!\033[0;39m\n", \
					__FILE__, __LINE__, name, Ret); \
					return Ret; \
				} \
			} while(0)

#define CHN_NUM 1

#define LCD_DISPLAY_GPIO 14

#define LUNINANCE_VALUE 50	

#if 0
//begin ---驱动部(宋留洋)提供接口
typedef unsigned int	uint32_t;
typedef struct lens_control_ops {
// 变倍，取值范围[0,40]
uint32_t (*zoom_get)(void);
void (*zoom_set)(uint32_t v);

// 光圈控制
// 自动模式(1:auto 0:manual)
uint32_t (*aperture_auto_get)(void);
void (*aperture_auto_set)(uint32_t v);
// 光圈大小，取值范围[0,17] (仅在手动模式有效)
uint32_t (*aperture_get)(void);
void (*aperture_set)(uint32_t v);

// IR_CUT(0:彩色	 1:黑白)
uint32_t (*ircut_get)(void);
void (*ircut_set)(uint32_t v);
} lens_control_ops_st;
#endif

typedef enum _NC_VIDEO_FMT
{
	FMT_NONE = 0,

	SD_960,
	AHD_720,
	AHD_1080,
	SD_720,
	FMT_MAX
} NC_VIDEO_FMT_E;

typedef enum _NC_VIDEO_FPS
{
	FPS_NONE = 0,

	FPS15,
	FPS25,
	FPS30,
	FPS50,
	FPS60,

	FPS_MAX
} NC_VIDEO_FPS_E;


#endif
