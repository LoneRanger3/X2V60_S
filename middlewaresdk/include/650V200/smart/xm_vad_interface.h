/*****************************************************************************

------------------------------------------------------------------------------
                            xm_sc_interface.h
产品名:    人声检测算法
模块名:    mrcg
生成日期:  2019-10-14
作者:      马登攀
文件描述:  人声检测算法对外头文件
版本号：   V0.1.1.0
*****************************************************************************/
#ifndef _XM_VAD_INTERFACE_H_
#define _XM_VAD_INTERFACE_H_

#include "xm_ia_comm.h"


#ifdef __cplusplus
extern "C" {
#endif

#define VAD_MAJOR_VERSION               (0)             /* 哭声检测算法主版本号 */
#define VAD_MINOR_VERSION               (1)             /* 哭声检测算法次版本号 */
#define VAD_REVERSION                   (1)             /* 哭声检测算法修改版本号 */
#define VAD_BUILD                       (0)             /* 哭声检测算法编译版本号(暂不用) */

#define  RECORD                     (0)
#define  XM_VOICE_GATHER_NUM		(16)

typedef struct{
	unsigned int  u32PhyAddr;
	unsigned char *pu8VirAddr;
	unsigned int  u32Size;
}XM_IA_VOICE_MEM_S;

typedef struct{
	int iChannel;	//多路音频路数编号
	XM_IA_VOICE_MEM_S stVoiceMem;	//读入音频
}XM_IA_VOICE_S;

typedef struct{
	int iMaxChannel;	//多路总路数
	float fSampleRate;	//8000	
}XM_VAD_CFG_S;

typedef  struct{
	int iChannel;	//多路音频路数编号
	unsigned char ucResult[XM_VOICE_GATHER_NUM];	//返回结果
}XM_IA_VAD_RSLT_S;

typedef struct
{
	int iRoiLen;				/* 16,每次取得窗口数，每个窗口10ms */
	int iNfft;					/* fft长度，支持为1024或512 */
	int iBufferSize;			/* 数据总量iRoiLen*160；10ms一个窗口，160为一个窗口的数据总量 */
	int gL;						/* fftfilter参数，设定为512 */
	int iFeatureDim;			/* mrcg输出通道数 */
	int W;						/* 19 */
	int u;						/* 9 */
	int winLen1;				/* 耳蜗图1窗口大小，0.025*fFs (fFs取自VAD_CONFIG_S）*/
	int winLen2;				/* 耳蜗图2窗口大小，0.2*fFs（fFs取自VAD_CONFIG_S）*/
	int winShift;				/* 0.01* fFs（fFs取自VAD_CONFIG_S）*/
	int previous1;				/* winLen1 - winShift;耳蜗图1帧移位后得重叠区 */
	int previous2;				/* winLen2 - winShift;耳蜗图2帧移位后得重叠区 */
	int iRoiSize;				/* 计算区域的位置偏移行数（窗口数）*/
	int iCochleaOffset;			/* 似乎同iRoiLen */
	int iBitPadCocLen1;			/* 耳蜗图1滑动窗口（前）补边长度，首帧11*XM_MRCG_CHAN_NUM，第二帧 43*XM_MRCG_CHAN_NUM */
	int iBitPadCocLen2;			/* 耳蜗图2滑动窗口（前）补边长度，首帧0，第二帧 34*XM_MRCG_CHAN_NUM */
	int iCochOffLen;			/* 滑动窗口计算存储空间数据长度 */


	int fps;					/* 音频采样率 */
	int iNumChan;				/* fft通道数 */
	XM_IA_IMG_TYPE_E eImgType;  /* 输入图像格式 */

}XM_IA_VAD_INIT_S;

typedef struct
{
	unsigned char ucIsFirstFrm;     /* 是否是第一帧：1是第一帧，0是后续帧 */
	int size;
	int fps;						/* 采样率 */
	short *pstImage[XM_VOICE_GATHER_NUM];				/* 输入数据结构体 */
}XM_IA_VAD_IN_S;

typedef struct
{
	int pstResult[XM_VOICE_GATHER_NUM];		/* 检测结果：0无人声，1有人声 */
}XM_IA_VAD_RESULTS_S;







#ifdef __cplusplus
}
#endif

#endif