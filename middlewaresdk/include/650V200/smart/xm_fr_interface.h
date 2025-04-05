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
#define FACE_REC_FEATURE_NUM            (512)       /* 人脸识别的特征数量 */
#define FACE_REC_FEATURE_ENCODE_SIZE    (528)       /* 压缩的人脸识别特征大小 */

/*--------------------------------------------------------------------------------
    3. data struct
--------------------------------------------------------------------------------*/
/* 人脸特征,固定结构体 */
typedef struct
{
    float afFaceFeat[FACE_REC_FEATURE_NUM];     /* 512维人脸特征 */

}XM_IA_FR_FEATRUES;

/* 人脸编码特征 */
typedef struct
{
    char acFaceFeat[FACE_REC_FEATURE_ENCODE_SIZE];                       /* 编码后的人脸特征 */

}XM_IA_FR_ENCODE_FEATRUES;

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

/* 人脸识别等级配置：越高越安全但通过率降低、越低通过率越高但相对不安全 */
typedef enum
{
    XM_IA_FR_MATCH_LEVEL_LOWEST = -2,           /* 最低识别阈值等级 */
    XM_IA_FR_MATCH_LEVEL_LOW = -1,              /* 较低识别阈值等级 */
    XM_IA_FR_MATCH_LEVEL_MID = 0,               /* 中识别阈值等级 */
    XM_IA_FR_MATCH_LEVEL_HIGH = 1,              /* 较高识别阈值等级 */
    XM_IA_FR_MATCH_LEVEL_HIGHEST = 2,           /* 最高识别阈值等级 */
    XM_IA_FR_MATCH_LEVEL_NUM = 5,               /* 识别阈值等级级数 */
}XM_IA_FR_MATCH_LEVEL_E;

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

/*************************************************
Author: WangZelang
Date: 2023-12-25
Description: 恢复人脸特征数据
INPUT:  pstEncFeats     经编码的人脸特征528字节
OUTPUT: pstDecFeats     解码后的人脸特征512浮点
*************************************************/
int XM_IA_FR_DecodeFaceFeatures(XM_IA_FR_ENCODE_FEATRUES *pstEncFeats, XM_IA_FR_FEATRUES *pstDecFeats);

/*************************************************
Author: WangZelang
Date: 2024-03-19
Description: 用编码方式压缩人脸特征数据.只能在初始化过人脸算法后使用
INPUT:  pstFeats        原始人脸特征 512浮点
OUTPUT: pstEncFeats     编码后人脸特征 528字节
*************************************************/
int XM_IA_FR_EncodeFaceFeatures(XM_IA_FR_FEATRUES *pstFeats, XM_IA_FR_ENCODE_FEATRUES *pstEncFeats);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif