#ifndef __MPI_SDNC_H__
#define __MPI_SDNC_H__
#include "isp_common.h"
typedef struct stSDNC_DNC_ATTR_S
{
	int s32ColorExpMin;
	int s32ColorExp;
	int s32ColorDlt;
	int s32ColorDlt2;
	int s32BwExp;
}SDNC_DNC_ATTR_S;

typedef struct stSDNC_DNC_INFO_S
{
	int s32Data[16];
}SDNC_DNC_INFO_S;


typedef struct stSDNC_FUNC_S
{
/**************************************
函数功能: 控制灯板光源
输入参数:   u8Mode
			0: 关闭
			1: 打开
返回参数:	-1: 	出错
		0:		操作完成
		1:		操作无效
**************************************/
    int(*pfn_led_ctrl)(int chn, unsigned char u8Mode);

/**************************************
函数功能: 获取灯板状态
输入参数:  无
输出参数:	*ps32Mode: 0:Auto 1:Manual
		* ps32State: 0:Close 1:Open
		
返回参数:	-1: 	出错
		0:		成功
**************************************/
	int(*pfn_led_get)(int chn, int *ps32Mode, int* ps32State);

/**************************************
函数功能: 日夜状态获取
输入参数: *ps32State:  0: Day 1:Night
输出参数: 无
返回参数:	-1: 	出错
		0:		成功
**************************************/
	int(*pfn_dnStatusExt_get)(int chn, int* ps32State);

} SDNC_FUNC_S;

typedef struct stSDNC_CTRL_ATTR_S
{
	unsigned char u8Delay2Color;		// (s)
	unsigned char u8Delay2Bw;			// (s)
	unsigned char u8DelayLedReOpen;	// (s)
	unsigned char u8Mode;				// 2: No PhotoR_V2  3: Have PhotoR_V2
	unsigned short u16PeriodMs;		// 调用周期(ms)
} SDNC_CTRL_ATTR_S;


/*********************************************************************************
函数功能:	获取日夜状态
输入参数:		IspDev: ISP设备号
			u8DnNow: 当前日夜模式(实际生效)
					0: 彩色
					1: 黑白
			pstIrDncAttr
					s32ColorExp:  <=该值进入彩色模式
					s32ColorDlt:  	>=该值进入彩色模式
					s32ColorDlt2:	>=该值进入彩色模式(Example: 50)
					s32BwExp:	>=该值进入黑白模式
输出参数:	无
返回参数:	-1:	出错
				0:	彩色
				1:	黑白
*********************************************************************************/
ISP_S32 XM_MPI_SDNC_GetDNCStatus(ISP_DEV IspDev, ISP_U8 u8DnNow, SDNC_DNC_ATTR_S* pstIrDncAttr);

/*********************************************************************************
函数功能:	获取统计信息
输入参数:	无
输出参数:	pstDncInfo: 统计信息
返回参数:	-1:	出错
				0:	彩色
				1:	黑白
*********************************************************************************/
ISP_S32 XM_MPI_SDNC_GetInfo(ISP_DEV IspDev, SDNC_DNC_INFO_S *pstDncInfo);

/*********************************************************************************
函数功能:	注册灯板控制接口
输入参数:	pstRegister: 指向接口地址
输出参数:	无
返回参数:	-1:	出错
		0:     成功
*********************************************************************************/
ISP_S32 XM_MPI_SDNC_FunCallBack(ISP_DEV IspDev, SDNC_FUNC_S *pstRegister);


/*********************************************************************************
函数功能:	配置控制参数
输入参数:	pstCtrlAttr: 指向参数属性
输出参数:	无
返回参数:	-1:	出错
		0:     成功
*********************************************************************************/
ISP_S32 XM_MPI_SDNC_SetCtrlAttr(ISP_DEV IspDev, SDNC_CTRL_ATTR_S *pstCtrlAttr);


/*********************************************************************************
函数功能:	获取控制参数
输入参数:	无
输出参数:	pstCtrlAttr: 指向参数属性
返回参数:	-1:	出错
		0:     成功
*********************************************************************************/
ISP_S32 XM_MPI_SDNC_GetCtrlAttr(ISP_DEV IspDev, SDNC_CTRL_ATTR_S *pstCtrlAttr);


/*********************************************************************************
函数功能:	切换模式
输入参数:	u8Mode: 
					0: IR
					1: WL
输出参数:	无
返回参数:	-1:	出错
				0:	成功
*********************************************************************************/
ISP_S32 XM_MPI_SDNC_SwitchMode(ISP_DEV IspDev, ISP_U8 u8Mode);


#endif /* __MPI_SDNC_H__ */

