/*************************************************************************
 * Copyright (C) 2022 Xmsilicon Tech. Co., Ltd.
 * @File Name: fvideo.h
 * @Description: 
 * @Author: XM R&D
 * @Created Time: 2022.01.04
 * @Modification: 
 ************************************************************************/

#ifndef __FVIDEO_H__
#define __FVIDEO_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


typedef enum
{
	VIDEO_FORMAT_UNKNOW=0,
	VIDEO_FORMAT_CVBS,			//25/30		960H
	VIDEO_FORMAT_AHDM_25,		//25/30		720P
	VIDEO_FORMAT_AHDL,
	VIDEO_FORMAT_AHDM_50,
	VIDEO_FORMAT_AHDH,			//25/30		1080P
	VIDEO_FORMAT_AHD3M,			//25/30		3M
	VIDEO_FORMAT_AHD5M,			//20		5M
	VIDEO_FORMAT_AHD4K,			//15		8M
	VIDEO_FORMAT_AFHD_UNKONW=9,
	VIDEO_FORMAT_AHD_4M,  		//25/30		4M
	VIDEO_FORMAT_AHD3M_NRT,		//18		3M
	VIDEO_FORMAT_AHD4M_NRT,		//15		4M
	VIDEO_FORMAT_AHD5M_NRT,		//12.5		5M
	VIDEO_FORMAT_AHDH_NRT=14,	//15/12.5	1080p
	VIDEO_FORMAT_AHD8M_15P,
	VIDEO_FORMAT_AHD8M_12_5P,
	VIDEO_FORMAT_AHD8M_7_5P,	//7.5		8M
	
	VIDEO_FORMAT_TVI_CVBS=101,
	VIDEO_FORMAT_TVIM_25V2,		//25/30		720P 2.0
	VIDEO_FORMAT_TVIM_50,
	VIDEO_FORMAT_TVIH,			//25/30		1080p
	VIDEO_FORMAT_TVIM,			//25/30		720p 1.0
	VIDEO_FORMAT_TVI3M,
	VIDEO_FORMAT_TVI5M,
	VIDEO_FORMAT_TVI_4K,		//15		8M		3840x2160
	VIDEO_FORMAT_TVI_4M,		//25/30		4M		2688x1520
	VIDEO_FORMAT_TVI_QHD=110,
	VIDEO_FORMAT_TVI3M_18,		//18		3M		2048x1536->1920x1536
	VIDEO_FORMAT_TVI3M_20,
	VIDEO_FORMAT_TVI4M_12,
	VIDEO_FORMAT_TVI4M_15,		//15		4M
	VIDEO_FORMAT_TVI5M_12,		//12.5		5M		2592x1944->2560x1920
	VIDEO_FORMAT_TVI5M_20,		//20		5M
	VIDEO_FORMAT_TVI6M_10,
	VIDEO_FORMAT_TVI8M_15=118,
	VIDEO_FORMAT_TVI8M_12_5,
	
	VIDEO_FORMAT_CVI_CVBS=200,
	VIDEO_FORMAT_CVIM_25,		//25/30		720p
	VIDEO_FORMAT_CVIM_50,
	VIDEO_FORMAT_CVIH,			//25/30		1080p
	VIDEO_FORMAT_CVI3M,
	VIDEO_FORMAT_CVI5M,			//20		5M
	VIDEO_FORMAT_CVI_4K,		//12.5		8M
	VIDEO_FORMAT_CVI_4M,		//25/30		4M
	VIDEO_FORMAT_CVI8M_15P,
	VIDEO_FORMAT_CVI8M_12_5P
} VIDEO_FORMAT_E;


typedef struct ahd_type {
	unsigned char video_format; 	// 视频格式   AHD、CVBS
	unsigned char video_standard;	// 视频制式(NTSC、PAL)
} AHD_VIDEO_TYPE;


/******************************************************************************
 *	函数: XM_VIDEO_VideoCreate
 *	描述: 视频创建
 *	参数:	无
 *
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int XM_VIDEO_VideoCreate(void);

/******************************************************************************
 *	函数: XM_VIDEO_VideoDestory
 *	描述: 视频销毁
 *	参数:	无
 *
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int XM_VIDEO_VideoDestory(void);


/******************************************************************************
 *	函数: XM_VIDEO_GetVideoLossStat
 *	描述: 获取视频丢失状态
 *	参数:	pu32Stat		所有通道的视频丢失状态，第几位即几个通道
 *						1 表示视频丢失，0 表示视频未丢失
 *
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int XM_VIDEO_GetVideoLossStat(unsigned int *pu32Stat);


/******************************************************************************
 *	函数: XM_VIDEO_GetVideoType
 *	描述: 获取前端接入视频类型
 *	参数:		pstVideoType	所有通道的视频类型
 *
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int XM_VIDEO_GetVideoType(AHD_VIDEO_TYPE **pstVideoType);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
