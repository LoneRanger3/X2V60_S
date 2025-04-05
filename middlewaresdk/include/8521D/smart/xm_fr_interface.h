/*****************************************************************************

------------------------------------------------------------------------------
        xm_fr_interface.h
产品名:    人脸识别
模块名:    人脸识别
生成日期:  2018-09-10
作者:      王则浪
文件描述:  人脸识别对外头文件
版本号：   V0.1.1
*****************************************************************************/

#ifndef _XM_FR_INTERFACE_H_
#define _XM_FR_INTERFACE_H_
/*--------------------------------------------------------------------------------
    1. include headers
--------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
#include "xm_ia_comm.h"

/*--------------------------------------------------------------------------------
    2. constant defines
--------------------------------------------------------------------------------*/
#define FR_INPUT_WIDTH                  (112)       /* FR 网络Input层网络输入图像宽度 */
#define FR_INPUT_HEIGHT                 (112)       /* FR 网络Input层网络输入图像高度 */
#define FR_INPUT_WIDTH_V2               (80)        /* FR 网络类型2的Input层网络输入图像宽度 */
#define FR_INPUT_HEIGHT_V2              (80)        /* FR 网络类型2的Input层网络输入图像高度 */
#define FACE_REC_FEATURE_NUM            (512)       /* 人脸识别的特征数量 */

/*--------------------------------------------------------------------------------
    3. data struct
--------------------------------------------------------------------------------*/

/* 人脸识别算法模型 */
typedef enum
{
    CLASS_ANGELA = 0,                       /* 40MB人脸识别网络 */
    CLASS_BELLA = 1,                        /* 20MB人脸识别网络 */
    CLASS_CAROL = 2,                        /* 25MB人脸识别网络 */
    CLASS_DAISY = 3,                        /* 20MB人脸识别网络 */
    CLASS_EARTHA = 4,                       /* 23MB人脸识别网络 */
    CLASS_FAITHE = 5,                       /* 44MB人脸识别网络 */
    CLASS_GABRIELLE = 6,                    /* 44MB人脸识别网络 */
    CLASS_HANNAH = 7,                       /* 165MB人脸识别网络 */
    CLASS_IDA = 8,                          /* 41MB 80x80人脸识别网络 */
    CLASS_JACQUELINE = 9,                   /* 44MB 112x112人脸识别网络,人脸识别区域下移 */
    CLASS_KAREN = 10,                       /* 41MB 80x80人脸识别网络,人脸识别区域下移 */
    CLASS_LAURA = 11,                       /* 165MB 112x112人脸识别网络,人脸识别区域下移 */
    CLASS_MADELINE = 12,                    /* 165MB 112x112人脸识别网络,人脸识别区域下移,低位宽重训练 */
    CLASS_NANCY = 13,                       /* 165MB 112x112人脸识别网络,逆光识别优化 */
    CLASS_OCTAVIA = 14,                     /* 165MB 112x42人脸识别网络,戴口罩人脸识别(弃用) */
    CLASS_PAG = 15                          /* 165MB 112x112人脸识别网络,识别优化 */
    
}XM_IA_FR_CLASS_E;

/* 人脸识别初始化参数 */
typedef struct
{
    /* INPUT */
    char acClassBinPath[XM_IA_PATH_MAX_LEN];    /* 分类器Bin文件路径 */
    XM_IA_FR_CLASS_E eFrClass;                  /* 人脸识别分类器 */
    XM_IA_INFERENCE_ENGINE_E eInferenceEngine;  /* 推理引擎 */

    unsigned char ucReserved[64];               /* 保留字节 */

}XM_IA_FR_INIT_S;

/* 人脸特征,固定结构体 */
typedef struct
{
    float afFaceFeat[FACE_REC_FEATURE_NUM];     /* 512维人脸特征 */

}XM_IA_FR_FEATRUES;

/* 人脸特征列表 */
typedef struct
{
    int iFaceNumInList;                         /* 人脸特征库中人脸数 */
    XM_IA_FR_FEATRUES **ppstFeatureItem;        /* 人脸特征列表指针 */

    unsigned char ucReserved[128];              /* 保留字节 */
}XM_IA_FR_FEATRUES_LIST;

/* 人脸特征比对结果 */
typedef struct
{
    int iFaceRecIdx;                            /* 识别人在特征列表中相似人脸 */
    float fMatchingRate;                        /* 配对率(0~100%) */

    unsigned char ucReserved[64];               /* 保留字节 */
}XM_IA_FR_MATCH_RSLT_S;

/* 特征比对设置 */
typedef struct
{
    int iMaxMatchNum;                           /* 最大允许比对数 */
    float fThresh;                              /* 特征比对阈值 */

    unsigned char ucReserved[64];               /* 保留字节 */
}XM_IA_FR_MATCH_CFG_S;

/* 在特征列表里比对出所有大于设定阈值的比对结果 */
typedef struct
{
    int iMatchNum;                              /* 比对出的人脸数 */
    XM_IA_FR_MATCH_RSLT_S *pstMatchList;        /* 比对结果列表 */

    unsigned char ucReserved[64];               /* 保留字节 */
}XM_IA_MATCH_LISTS_RSLT_S;

/// 人脸特征比对,输出1-2个最相似人脸结果
/// \param [in] pfFaceFeat      人脸特征
/// \param [in] pstFeatureList  人脸特征列表
/// \param [in] fThresh         比对阈值(0~100%),大于阈值结果才匹配成功
/// \param [out] pstMatchRslt   人脸匹配结果
/// \return 0  成功
/// \return !0 失败
int XM_IA_FR_MatchFace(XM_IA_FR_FEATRUES *pstFaceFeat, XM_IA_FR_FEATRUES_LIST *pstFeatureList, float fThresh, XM_IA_FR_MATCH_RSLT_S *pstMatchRslt);

/// 人脸特征比对,输出大于阈值的人脸结果
/// \param [in] pfFaceFeat      人脸特征
/// \param [in] pstFeatureList  人脸特征列表
/// \param [in] stMatchCfg      人脸比对配置项
/// \param [out] pstMatchRslt   人脸匹配结果
/// \return 0  成功
/// \return !0 失败
int XM_IA_FR_ListOfMatchFace(XM_IA_FR_FEATRUES *pstFaceFeat, XM_IA_FR_FEATRUES_LIST *pstFeatureList, XM_IA_FR_MATCH_CFG_S stMatchCfg, XM_IA_MATCH_LISTS_RSLT_S *pstMatchRslt);

/*************************************************
Author: WangZelang
Date: 2021-04-20
Description: 提供标准差，人脸特征快速比对
INPUT:  pfFaceFeat      待识别人脸特征
        fNorm1          待识别人脸特征标准差
        pstFeatureList  人脸特征底库中的人脸特征
        pfNormList      底库人脸特征标准差
        fThresh         比对阈值(0~100%),大于阈值结果才匹配成功,默认识别阈值0.50
OUTPUT: pstMatchRslt    比对结果，最相似人脸索引，相似度
*************************************************/
int XM_IA_FR_FastMatchFace(XM_IA_FR_FEATRUES *pfFaceFeat, float fNorm1, XM_IA_FR_FEATRUES_LIST *pstFeatureList, float *pfNormList, float fThresh, XM_IA_FR_MATCH_RSLT_S *pstMatchRslt);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif