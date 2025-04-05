#ifndef __MPI_CAMERA_H__
#define __MPI_CAMERA_H__ 

#ifdef __cplusplus
extern "C" {
#endif
#include "isp_type.h"

typedef ISP_S32 CAM_DEV;

typedef enum stCAM_SCENE_TYPE_E
{
	SCENE_TYPE_AUTO,
	SCENE_TYPE_INDOOR,
	SCENE_TYPE_OUTDOOR,
	SCENE_TYPE_BUTT,
}CAM_SCENE_TYPE_E;


typedef enum stCAM_DNC_MODE_E
{
	DNC_MODE_AUTO,			// 自动切换
	DNC_MODE_MULTICOLOR,		// 彩色
	DNC_MODE_BLACKWHITE,		// 强制为黑白模式
	DNC_MODE_INTE_WHITE_INF, //智能警戒
	DNC_MODE_WHITELAMP_AUTO, //智能暖光
	DNC_MODE_IRLAMP_AUTO,	//智能红外
	DNC_MODE_LP_MODE,		//车牌模式
	DNC_MODE_BUTT
}CAM_DNC_MODE_E;


typedef enum stCAM_DN_STATUS_E
{
	DN_STATUS_COLOR,
	DN_STATUS_BW,
	DN_STATUS_BUTT,
}CAM_DN_STATUS_E;


typedef enum stCAM_IMG_STYLE_E
{
	IMG_STYLE_0,
	IMG_STYLE_1,
	IMG_STYLE_2,
	IMG_STYLE_BUTT,
}CAM_IMG_STYLE_E;

typedef enum stCAM_ESHUTTER_TYPE_E
{
	ESHUTTER_DISABLE=0,
	ESHUTTER_LOW=2,
	ESHUTTER_MIDDLE=4,
	ESHUTTER_HIGH=6,
	ESHUTTER_BUTT,
}CAM_ESHUTTER_TYPE_E;

typedef enum stCAM_LED_TYPE_E
{
	LED_TYPE_IR=0,
	LED_TYPE_WL=1,
	LED_TYPE_DOUBLE=2,
	LED_TYPE_MOOD=3,
	LED_TYPE_MUSIC=4,
	LED_TYPE_BUTT,
}CAM_LED_TYPE_E;

typedef enum stCAM_LED_OP_MODE_E
{
	LED_OP_MODE_AUTO=0,
	LED_OP_MODE_MANUAL=1,
	LED_OP_MODE_INTELL=2,
	LED_OP_MODE_DETER=3,
	LED_OP_MODE_BUTT,
}CAM_LED_OP_MODE_E;

typedef struct stCAM_COLOR_ATTR{
	ISP_U8 u8Brightness;
	ISP_U8 u8Contrast;
	ISP_U8 u8Saturation;
	ISP_U8 u8Hue;
	ISP_U8 u8Rsv0;
	ISP_U8 u8Rsv1;
	ISP_U16 u16Acutance;			//< 锐度，取值0-15, 低8位表示水平锐度，高8为表示垂直锐度。   	/
}CAM_COLOR_ATTR;

typedef struct stCAM_SOFTIR_THR
{
	int s32ColorExp;
	int s32ColorDlt;
	int s32ColorDlt2;
	int s32BwExp;
}CAM_SOFTIR_THR;

typedef struct stCAM_ATTR_PARA
{
	ISP_U32 u32EshutterLvEn;		// E Shutter enable level
	ISP_U32 u32EshutterLvDis;	// E Shutter Disbale level	
	CAM_SOFTIR_THR astSoftIr[6];	// 默认使用astSoftIr[3]	
	ISP_U32 au32Rsv[16];
}CAM_ATTR_PARA;



typedef struct stCAM_INNER_INFO
{
	ISP_U8 u8AiStrategyStr;	// AI相关策略强度
	ISP_U8 u8InfraredStatus;
	ISP_U8 u8LedStab;		// 灯光状态稳定
	ISP_U8 u8DnStatus;	
	ISP_U8 au8Rsv[4];
	ISP_U16 au16Rsv[4];
	ISP_U32 u32ExpLines;
	ISP_U32 u32ExpAllGain;
	ISP_U32 au32Rsv[2];
}CAM_INNER_INFO;

/* 人脸检测区域信息 */
typedef struct stCAM_IA_FD_REGION_INFO_S
{
    ISP_U8 u8ActiFlag;                    /* 人脸区域激活标志 */
    ISP_U16 u16AveLuma;                  /* 人脸区域平均亮度 */
    ISP_U16 u16LumaMin;               /* 人脸可识别亮度下限*/
    ISP_U16 u16LumaMax;               /* 人脸识别亮度上限*/
    ISP_U8 ucIrActiveFlag;       /* IR通道人脸激活标志 */
    ISP_U16 ucIrAveLightness;     /* IR通道人脸平均亮度 */
    ISP_U16 ucIrLightnessLmtMin;  /* IR通道人脸亮度下限要求 */
    ISP_U16 ucIrLightnessLmtMax;  /* IR通道人脸亮度上限要求 */
    ISP_U16 u16stRegion[4];  /* 提取人脸亮度区域位置(在白光图中，相对坐标0~8192) */
}CAM_IA_FD_REGION_INFO_S;

// 编码参数
typedef struct stCAM_VENC_INFO
{
    ISP_U8    u8Compression;        /*!< 压缩模式 */
    ISP_U8    u8BitRateControl;     /*!< 码流控制 */
    ISP_U8    u8ImageSize;          /*!< 图像分辨率 */
    ISP_U8    u8ImageQuality;       /*!< 图像画质 */
    ISP_U8    u8FramesPerSecond;    /*!< 帧率 */
    ISP_U8    u8AVOption;           /*!< 音视频选项 */
    ISP_U16   u16BitRate;            ///< 参考码流值，Kbps为单位
    ISP_U8    u8GOP;                /*< 帧间隔的数值，建议值49～99*/
    ISP_U8    u8Reserved[3];        /*< 保留字节*/
} CAM_VENC_INFO;

typedef struct stCAM_AI_RESULT_S
{	
	/***
	u32Cmd: 
	 0: 人形检测
	 	0x01: 坐标(全身)
	 	0x02: 坐标(半身)
	 	0x03: 坐标(人头)
	 0x10: 车牌检测
	 0x20: 移动侦测（灯泡智能警戒使用）
	 0x40: 人脸检测
	***/
	 unsigned int u32Cmd;  /*命令标记*/

	 unsigned int u32AlarmFlag; /*触发报警(人形/车牌)1：报警(检测到目标)  0：不报警(未检测到目标)*/

	 /***
	 u32TargetNum
	 	bit0~bit7: 目标个数    
	 	bit16~bit31: 框个数
	 ***/
	 unsigned int u32TargetNum; /*目标数(人形/车牌) */
	 
	/***
	u32Cmd = 0x10时:
	 u32Reserved[0]:框的起始点水平坐标
	 u32Reserved[1]:框的起始点垂直坐标
	 u32Reserved[2]:框的结束点水平坐标
	 u32Reserved[3]:框的结束点垂直坐标
	***/
	 unsigned int u32Reserved[64];
}CAM_AI_RESULT_S;

typedef struct stCAM_GET_AI_FUNC_S
{
	ISP_S32(*pfn_get_ai_result)(CAM_AI_RESULT_S *pstAiResult);	// 返回值：0:成功      	-1:失败
} CAM_GET_AI_FUNC_S;



/*********************************** 移动侦测相关参数 ***********************************/
typedef struct stCAM_MD_PARAM
{
	// 动态检测的敏感度档次
	ISP_S32 s32Level;

	/**************************
	a.按位有效(每位对应一个矩形块)
	b.位含义:   	1:需要检测    	0:无需检测
	c.最左边块对应最低位
	d.从上到下的行对应的下标从0到17
	**************************/
	ISP_U32 au32Win[18];

	// 灵敏度的具体值
	ISP_U8 u8Sensitiveness;

	// 帧间隔
	ISP_U8 u8Fieldinterval;

	// 使能开关   	1:表示使能，0:表示禁用(其他参数被忽略)。
	ISP_S32 s32Enable;
}CAM_MD_PARAM;

typedef struct stCAM_MD_CAPS
{
	ISP_S32 s32Support;		// 1.支持检测    	0:不支持
	ISP_U32 u32GridLines;	// 动态检测的区域需要划分成的列数。
	ISP_U32 u32GridRows;		// 动态检测的区域需要划分成的行数。
	ISP_U8 u8Result;			// 是否能得到每块区域的检测结果。
	ISP_U8 u8Hint;			// 是否能在检测成功时给予提示。
}CAM_MD_CAPS;

typedef struct stCAM_MD_RESULT
{
	/**************************
	a.按位有效(每位对应一个矩形块)
	b.位含义:   	1:触发报警    	0:未触发报警
	c.最左边块对应最低位
	d.从上到下的行对应的下标从0到17
	**************************/
	ISP_U32 au32Win[18];
}CAM_MD_RESULT;


/*********************************** 视频遮挡相关参数 ***********************************/
typedef struct stCAM_BD_CAPS
{
	ISP_U32 u32Surport;		// 1: 支持   0:不支持
}CAM_BD_CAPS;

typedef struct stCAM_BD_PARAM
{
	ISP_U8	u8Level;        // 等级 1~6
	
	// 使能开关   	1:表示使能，0:表示禁用(其他参数被忽略)。
	ISP_S32 s32Enable;
}CAM_BD_PARAM;





ISP_S32 XM_MPI_CAM_Init(ISP_S32 s32Rsv, ISP_VOID* pstAsConfig);
ISP_S32 XM_MPI_CAM_Exit(ISP_VOID);

ISP_S32 XM_MPI_CAM_SetPara(CAM_DEV CamDev, CAM_ATTR_PARA *pstConfig);
ISP_S32 XM_MPI_CAM_GetPara(CAM_DEV CamDev, CAM_ATTR_PARA *pstConfig);

ISP_S32 XM_MPI_CAM_SetScene(CAM_DEV CamDev, CAM_SCENE_TYPE_E enType);
ISP_S32 XM_MPI_CAM_GetScene(CAM_DEV CamDev, CAM_SCENE_TYPE_E *penType);

ISP_S32 XM_MPI_CAM_SetDncMode(CAM_DEV CamDev, CAM_DNC_MODE_E enMode);
ISP_S32 XM_MPI_CAM_GetDncMode(CAM_DEV CamDev, CAM_DNC_MODE_E *penMode);

ISP_S32 XM_MPI_CAM_SetQrcodeMode(CAM_DEV CamDev, ISP_S32 enMode);
ISP_S32 XM_MPI_CAM_GetQrcodeMode(CAM_DEV CamDev, ISP_S32 *penMode);


ISP_S32 XM_MPI_CAM_SetDncThr(CAM_DEV CamDev, ISP_S32 s32Thr);
ISP_S32 XM_MPI_CAM_GetDncThr(CAM_DEV CamDev, ISP_S32 *ps32Thr);

ISP_S32 XM_MPI_CAM_SetDncThrSoftIR(CAM_DEV CamDev, ISP_S32 s32Thr);
ISP_S32 XM_MPI_CAM_GetDncThrSoftIR(CAM_DEV CamDev, ISP_S32 *ps32Thr);

ISP_S32 XM_MPI_CAM_SetDncSpeed(CAM_DEV CamDev, ISP_S32 s32Val);
ISP_S32 XM_MPI_CAM_GetDncSpeed(CAM_DEV CamDev, ISP_S32 *ps32Val);

ISP_S32 XM_MPI_CAM_GetDnStatus(CAM_DEV CamDev, CAM_DN_STATUS_E *penStatus);

ISP_S32 XM_MPI_CAM_SetLumTarget(CAM_DEV CamDev, ISP_S32 s32Tgt);
ISP_S32 XM_MPI_CAM_GetLumTarget(CAM_DEV CamDev, ISP_S32 *ps32Tgt);

ISP_S32 XM_MPI_CAM_SetBlc(CAM_DEV CamDev, ISP_U32 u32En, ISP_U32 u32Level);
ISP_S32 XM_MPI_CAM_GetBlc(CAM_DEV CamDev, ISP_U32 *pu32En, ISP_U32 *pu32Level);

ISP_S32 XM_MPI_CAM_SetHlc(CAM_DEV CamDev, ISP_S32 s32En, ISP_S32 s32Level);
ISP_S32 XM_MPI_CAM_GetHlc(CAM_DEV CamDev, ISP_S32 *ps32En, ISP_S32 *ps32Level);

ISP_S32 XM_MPI_CAM_SetExposureLvl(CAM_DEV CamDev, ISP_S32 s32Lvl, ISP_U32 u32MinUs, ISP_U32 u32MaxUs);
ISP_S32 XM_MPI_CAM_GetExposureLvl(CAM_DEV CamDev, ISP_S32 *ps32Lvl, ISP_U32 *pu32MinUs, ISP_U32 *pu32MaxUs);

ISP_S32 XM_MPI_CAM_SetGainLvl(CAM_DEV CamDev, ISP_S32 s32En, ISP_S32 s32Level);
ISP_S32 XM_MPI_CAM_GetGainLvl(CAM_DEV CamDev, ISP_S32 *ps32En, ISP_S32 *ps32Level);

ISP_S32 XM_MPI_CAM_SetMirror(CAM_DEV CamDev, ISP_S32 s32En);
ISP_S32 XM_MPI_CAM_GetMirror(CAM_DEV CamDev, ISP_S32 *ps32En);

ISP_S32 XM_MPI_CAM_SetFlip(CAM_DEV CamDev, ISP_S32 s32En);
ISP_S32 XM_MPI_CAM_GetFlip(CAM_DEV CamDev, ISP_S32 *ps32En);

ISP_S32 XM_MPI_CAM_SetAntiFlicker(CAM_DEV CamDev, ISP_S32 s32En, ISP_S32 s32Rsv);
ISP_S32 XM_MPI_CAM_GetAntiFlicker(CAM_DEV CamDev, ISP_S32 *ps32En, ISP_S32 *ps32Rsv);

ISP_S32 XM_MPI_CAM_SetEShutter(CAM_DEV CamDev, CAM_ESHUTTER_TYPE_E enType);
ISP_S32 XM_MPI_CAM_GetEShutter(CAM_DEV CamDev, CAM_ESHUTTER_TYPE_E *penType);

ISP_S32 XM_MPI_CAM_SetNrLevel(CAM_DEV CamDev, CAM_DN_STATUS_E enDnMode, ISP_S32 s32Val);
ISP_S32 XM_MPI_CAM_GetNrLevel(CAM_DEV CamDev, CAM_DN_STATUS_E enDnMode, ISP_S32 *ps32Val);

ISP_S32 XM_MPI_CAM_GetWdr(CAM_DEV CamDev, ISP_S32 *ps32En, ISP_S32 *ps32Val);
ISP_S32 XM_MPI_CAM_SetWdr(CAM_DEV CamDev, ISP_S32 s32En, ISP_S32 s32Val);

ISP_S32 XM_MPI_CAM_GetDefog(CAM_DEV CamDev, ISP_S32 *ps32En, ISP_S32 *ps32Val);
ISP_S32 XM_MPI_CAM_SetDefog(CAM_DEV CamDev, ISP_S32 s32En, ISP_S32 s32Val);

ISP_S32 XM_MPI_CAM_GetImgStyle(CAM_DEV CamDev, CAM_IMG_STYLE_E *penImgStyle);
ISP_S32 XM_MPI_CAM_SetImgStyle(CAM_DEV CamDev, CAM_IMG_STYLE_E enImgStyle);

ISP_S32 XM_MPI_CAM_SetIcrSwap(CAM_DEV CamDev, ISP_S32 s32En);
ISP_S32 XM_MPI_CAM_GetIcrSwap(CAM_DEV CamDev, ISP_S32 *ps32En);

ISP_S32 XM_MPI_CAM_SetInfraredSwap(CAM_DEV CamDev, ISP_S32 s32En);
ISP_S32 XM_MPI_CAM_GetInfraredSwap(CAM_DEV CamDev, ISP_S32 *ps32En);

ISP_S32 XM_MPI_CAM_SetColor(CAM_DEV CamDev, CAM_COLOR_ATTR stColorAttr);
ISP_S32 XM_MPI_CAM_GetColor(CAM_DEV CamDev, CAM_COLOR_ATTR *pstColorAttr);

ISP_S32 XM_MPI_CAM_GetAiFdRegion(CAM_DEV CamDev, CAM_IA_FD_REGION_INFO_S *pstAiFdRegionInfo);

ISP_S32 XM_MPI_CAM_SetLedMode(CAM_DEV CamDev, CAM_LED_TYPE_E enType, CAM_LED_OP_MODE_E enMode, ISP_S32 s32Status);
ISP_S32 XM_MPI_CAM_GetLedMode(CAM_DEV CamDev, CAM_LED_TYPE_E *penType, CAM_LED_OP_MODE_E *penMode, ISP_S32 *ps32Status);

ISP_S32 XM_MPI_CAM_SetLedLevel(CAM_DEV CamDev, ISP_S32 s32Rsv, ISP_S32 s32Val);
ISP_S32 XM_MPI_CAM_GetLedLevel(CAM_DEV CamDev, ISP_S32 s32Rsv, ISP_S32 *ps32Val);

ISP_S32 XM_MPI_CAM_SetLedType(CAM_DEV CamDev, ISP_S32 s32Type);
ISP_S32 XM_MPI_CAM_GetLedType(CAM_DEV CamDev, ISP_S32 *ps32Type);

ISP_S32 XM_MPI_CAM_SetLedDrvSwap(CAM_DEV CamDev, ISP_S32 s32SWapEn);
ISP_S32 XM_MPI_CAM_GetLedDrvSwap(CAM_DEV CamDev, ISP_S32 *ps32SWapEn);

ISP_S32 XM_MPI_CAM_SetLedHold(CAM_DEV CamDev, ISP_U32 u32Tms);
ISP_S32 XM_MPI_CAM_GetLedHold(CAM_DEV CamDev, ISP_U32 *pu32Tms);

ISP_S32 XM_MPI_CAM_SetAperture(CAM_DEV CamDev, ISP_U32 u32Mode);
ISP_S32 XM_MPI_CAM_GetAperture(CAM_DEV CamDev, ISP_U32 *pu32Mode);

ISP_S32 XM_MPI_CAM_SetAeSensitivity(CAM_DEV CamDev, ISP_S32 s32Level);
ISP_S32 XM_MPI_CAM_GetAeSensitivity(CAM_DEV CamDev, ISP_S32 *ps32Level);

ISP_S32 XM_MPI_CAM_SetDnSensitivity(CAM_DEV CamDev, ISP_S32 s32Level);
ISP_S32 XM_MPI_CAM_GetDnSensitivity(CAM_DEV CamDev, ISP_S32 *ps32Level);

ISP_S32 XM_MPI_CAM_SetIrcutMode(CAM_DEV CamDev, ISP_S32 s32Mode);
ISP_S32 XM_MPI_CAM_GetIrcutMode(CAM_DEV CamDev, ISP_S32 *ps32Mode);

ISP_S32 XM_MPI_CAM_SetVStd(CAM_DEV CamDev, ISP_U32 u32Std);
ISP_S32 XM_MPI_CAM_GetVStd(CAM_DEV CamDev, ISP_U32 *pu32Std);

ISP_S32 XM_MPI_CAM_SetInfraredSwap(CAM_DEV CamDev, ISP_S32 s32En);
ISP_S32 XM_MPI_CAM_GetInfraredSwap(CAM_DEV CamDev, ISP_S32 *ps32En);


ISP_S32 XM_MPI_CAM_GetInfo(CAM_DEV CamDev, CAM_INNER_INFO *pstInfo);
ISP_S32 XM_MPI_CAM_AiFunRegCallBack(CAM_DEV CamDev, CAM_GET_AI_FUNC_S *pstGetAiFunc);

/*********************************** 移动侦测相关接口 ***********************************/
ISP_S32 XM_MPI_CAM_MD_Create(CAM_DEV CamDev);
ISP_S32 XM_MPI_CAM_MD_Destory(CAM_DEV CamDev);

ISP_S32 XM_MPI_CAM_MD_GetCaps(CAM_DEV CamDev, CAM_MD_CAPS *pstCaps);
ISP_S32 XM_MPI_CAM_MD_SetPara(CAM_DEV CamDev, CAM_MD_PARAM stPara);

ISP_S32 XM_MPI_CAM_MD_GetStatus(CAM_DEV CamDev, ISP_U32 *pu32Val);
ISP_S32 XM_MPI_CAM_MD_GetResult(CAM_DEV CamDev, CAM_MD_RESULT *pstResult);


/*********************************** 视频遮挡相关接口 ***********************************/
ISP_S32 XM_MPI_CAM_BD_GetCaps(CAM_DEV CamDev, CAM_BD_CAPS *pstCaps);
ISP_S32 XM_MPI_CAM_BD_SetPara(CAM_DEV CamDev, CAM_BD_PARAM stPara);
ISP_S32 XM_MPI_CAM_BD_GetStatus(CAM_DEV CamDev, ISP_U32 *pu32Val);

#ifdef __cplusplus
}
#endif

#endif

