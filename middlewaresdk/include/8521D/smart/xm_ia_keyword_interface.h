/***********************************************************************************

------------------------------------------------------------------------------------

                            IA_Keyword_Interface.h
    产品名: Keyword Recognize
    模块名:
  生成日期:
      作者:
  文件描述: 路侧停车头文件

------------------------------------------------------------------------------
   修改历史
   日期        姓名             描述
  --------------------------------------------------------------------------
  版本号：   V0.1.1.0
*****************************************************************************/
#ifndef _XM_IA_KEYWORD_INTERFACE_H_
#define _XM_IA_KEYWORD_INTERFACE_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include "xm_ia_comm.h"

#define KEYWORD_MAJOR_VERSION               (0)             /* 关键词算法主版本号 */
#define KEYWORD_MINOR_VERSION               (1)             /* 关键词算法次版本号 */
#define KEYWORD_REVERSION                   (1)             /* 关键词算法修改版本号 */
#define KEYWORD_BUILD                       (0)             /* 关键词算法编译版本号(暂不用) */

#define KEYWORD_MAX_ROI_NUM (1600)          /* 待检测的区域高度(输入) */
#define KEYWORD_MAX_RESULT_NUM (200)        /* 输出关键词数量(输出) = (检测高度 / 8) */
#define KEYWORD_MAX_CHANNEL_NUM (1)         /* 最大支持1通道(输入) */


/* 采样率 */
typedef enum
{
    XM_8K_FPS = 8000,              /* 8K采样率 */
    XM_16K_FPS = 16000              /* 16K采样率 */
}XM_IA_VOICE_FPS;


/* 目标信息 */
typedef struct
{
    unsigned int uiId;      /* 目标ID号 */
    float flConfidence;
    int tableNum;
}XM_IA_KEYWORD_INFO_S;


/* 检测结果结构体 */
typedef struct
{
    int iCount;      /* 结果个数 */
    XM_IA_KEYWORD_INFO_S astKeywordInfo[KEYWORD_MAX_RESULT_NUM];
    int aiResver[4];
} XM_IA_KEYWORD_RESULT_S;


/* 目标检测算法work输入参数 */
typedef struct
{
    int iPortNums;                  /* 待检测数量*/
    XM_IA_IMAGE_S *pstImage;        /* 图像结构体 */
    XM_IA_POINT_S astKeywordRect;   /* 待检测区域 */
} XM_IA_KEYWORD_IN_S;

typedef struct
{
    XM_IA_SENSE_E eAlgSense;        /* 算法灵敏度 */
}XM_IA_KEYWORD_CONFIG_S;

/* 目标检测算法初始化结构体 */
typedef struct
{
    /* 输入信息 */
    unsigned int uiInStructSize;                /* Run入参参数结构体检查Size */
    unsigned int uiInitStructSize;              /* 初始化参数结构体检查Size  */
    unsigned int uiConfigStructSize;            /* 配置参数结构体检查Size    */
    unsigned int uiRsltStructSize;              /* 算法结果参数结构体检查Size  */

    unsigned int uiKeywordMaxNum;               /* 最大目标检测数*/
    char acClassBinPath[XM_IA_PATH_MAX_LEN];    /* 分类器Bin文件路径 */
    XM_IA_PLATFORM_E ePlatForm;                 /* 芯片平台选择 */
    XM_IA_IMG_TYPE_E eDevImgType;               /* 设备图像类型 */
    XM_IA_IMG_SIZE_S stDevImgSize;              /* 设备图像尺寸 */
    XM_IA_KEYWORD_CONFIG_S stKeywordCfg;        /* 关键词识别配置项 */
    XM_IA_VOICE_FPS fps;                        /* 采样率 */
    int iTime;                                  /* 窗口时长/ms */
    int iStrTime;                               /* 窗口时移/ms */
    int iNum;                                   /* 窗口数 */
    int iNfft;                                  /* fft长度 */

    /* OUTPUT */
    XM_IA_IMG_SIZE_S stKeywordAlgImgSize;           /* 算法图像尺寸 */

    int iResver[3];

} XM_IA_KEYWORD_INIT_S;


#ifdef  __cplusplus
}

#endif /* end of __cplusplus */

#endif