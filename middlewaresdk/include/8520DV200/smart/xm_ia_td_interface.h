/*****************************************************************************

------------------------------------------------------------------------------
                            xm_ia_td_interface.h
产品名:    规则目标检测算法
模块名:    规则目标检测算法
生成日期:   2022-07-10
作者:      冯世益
文件描述:   规则目标检测算法对外头文件
版本号：    V1.1.0
*****************************************************************************/
#ifndef _XM_IA_TD_INTERFACE_H_
#define _XM_IA_TD_INTERFACE_H_

#include "xm_ia_comm.h"
#include "xm_ia_rule.h"

#define TD_MAJOR_VERSION                (1)             /* 目标检测算法主版本号 */
#define TD_MINOR_VERSION                (1)             /* 目标检测算法次版本号 */
#define TD_REVERSION                    (0)             /* 目标检测算法修改版本号 */
#define TD_BUILD                        (0)             /* 目标检测算法编译版本号(暂不用) */
#define XM_IA_TD_MAX_PIXEL_NUM          (4)             /* 最多支持图像分辨率个数 */
#define XM_IA_TD_FEATURE_NUM            (512)           /* 最大特征数目 */

#ifdef  __cplusplus
extern "C" {
#endif

/* 人形跌到状态 */
typedef enum
{
    XM_IA_STANDING,                 /* 站立 */
    XM_IA_FALLDOWN,                 /* 跌倒 */
    XM_IA_LYING,                    /* 平躺着 */
    XM_IA_GETUP,                    /* 站起来 */
}XM_IA_FALLDOWN_E;

/* 人形抽烟打电话行为 */
typedef enum
{
    XM_IA_TD_ACT_NORMAL = 0,                /* 不抽烟不打电话 */
    XM_IA_TD_ACT_SMOKE = 1,                 /* 抽烟 */
    XM_IA_TD_ACT_PHONE = 2,                 /* 打电话 */
}XM_IA_TD_ACTION_E;

/* 目标检测类型 */
typedef enum
{
    XM_IA_TD_PED_Near = 1,          /* 近距离人形检测版本 cif下300ms */
    XM_IA_TD_PED_N = 3,             /* 人形检测nano版本 */
    XM_IA_TD_PED_T = 4,             /* 人形检测tiny版本 */
    XM_IA_TD_PED_S = 5,             /* 人形检测small版本 */
    XM_IA_TD_PED_M = 6,             /* 人形检测middle版本 */
    XM_IA_TD_PED_L = 7,             /* 人形检测large版本 */
    XM_IA_TD_PED_X = 8,             /* 人形检测extra large版本 */
    XM_IA_TD_DRIVER = 9,             /* 驾驶室司机检测 矿机项目 */

    XM_IA_TD_PED_HEAD = 10,         /* 人头检测，人头计数 */
    XM_IA_TD_BIKE = 30,             /* 非机动车检测 */
    XM_IA_TD_PBC = 50,              /* 机非人检测：机动车、骑车的人和人形*/

    XM_IA_TD_InVehicle_Front = 160, /* 车载摄像头系列，前装 */
    XM_IA_TD_InVehicle_Side = 161,  /* 车载摄像头系列，侧装 */
    XM_IA_TD_InVehicle_After = 162, /* 车载摄像头系列，后装 */

    XM_IA_TD_DRIVER_BEHAVIOR = 200, /* DMS使用,驾驶员行为检测。检测抽烟、打电话、系安全带、喝水、红外遮挡镜 */
    XM_IA_TD_NUM                    /* 目标检测算法模型个数 */
}XM_IA_TD_TYPE_E;

/* 送检图像越小，检测距离越近，性能越省 */
typedef enum
{
    XM_IA_TD_IMG_HCIF = 0,          /* Half CIF图像送检 */
    XM_IA_TD_IMG_CIF = 1,           /* CIF图像送检 */
    XM_IA_TD_IMG_2CIF = 2,          /* 2CIF图像送检 */
    XM_IA_TD_IMG_D1 = 3,            /* D1图像送检 */
    XM_IA_TD_IMG_CIF_2W = 4,        /* CIF图像送检,两图横向拼接 */
}XM_IA_TD_PATTERN_E;

/* 跟踪方法 */
typedef enum
{
    XM_IA_TD_TRACK_FEATURE = 0,     /* 深度学习特征法 */
    XM_IA_TD_TRACK_IOU = 1,         /* IOU重叠法 */
    XM_IA_TD_TRACK_SC = 2,          /* SHIFT特征法 */
    XM_IA_TD_TRACK_SINGLE = 3,      /* 单目标SHIFT特征法，(带npu暂不支持)默认开启动检 */
}XM_IA_TD_TRACK_TYPE_E;

/* 人脸检测频率方法 */
typedef enum
{
    XM_IA_TD_FACE_NORMAL = 0,       /* 常规人脸模式，所有人脸都检测 */
    XM_IA_TD_FACE_SINGLE = 10,      /* 单个人脸模式，一张图只检测一次人脸 */
    XM_IA_TD_FACE_HIGHF = 20,       /* 高频模式人脸检测,每隔3帧/次 */
    XM_IA_TD_FACE_MIDF = 22,        /* 中频模式人脸检测,每隔6帧/次 */
    XM_IA_TD_FACE_LOWF = 24,        /* 低频模式人脸检测,每隔12帧/次 */
    XM_IA_TD_FACE_ONCE = 100,       /* 每个人脸只检测一次 未支持*/
}XM_IA_TD_FACE_FREQ_TYPE_E;

/* 跟踪配置 */
typedef struct
{
    int iTrackFlag;                                 /* 跟踪开关（0:关闭 1:开启）*/
    XM_IA_TD_TRACK_TYPE_E iTrackType;               /* 跟踪方法 */

    unsigned char ucReserved[128];                  /* 预留位 */
}XM_IA_TD_TRACK_S;

/* 额外功能配置 */
typedef struct
{
    unsigned char ucPedFallDownEnable;      /* 人形跌倒开关 */
    unsigned char ucTDFeatureEnable;        /* 目标特征开关 （0:关闭 1:开启）*/
    unsigned char ucSmokeEnable;            /* 抽烟打电话检测开关 */
    unsigned char ucFaceDetEnable;          /* 人脸检测开关 */
    unsigned char ucFaceRegEnable;          /* 人脸识别开关 */
    unsigned char ucPlateDetEnable;         /* 车牌检测开关 */
    unsigned char ucPlateRegEnable;         /* 车牌识别开关 */
    unsigned char acReservedEnable[32];     /* 能力集开关预留位 */

    XM_IA_TD_FACE_FREQ_TYPE_E efdfreq;      /* 人脸检测频率类型 */
    unsigned char acReserved[92];           /* 能力集配置预留位 */
}XM_IA_TD_EXTRAFUNC_S;

/* 特征信息 */
typedef struct
{
    float afTDFeature[XM_IA_TD_FEATURE_NUM];     /* 目标特征 */
}XM_IA_TD_FEATRUES_S;

/* 人脸信息 */
typedef struct
{
    unsigned char ucFaceDetFlag;                /* 是否存在人脸，1：存在，0 不存在 */
    unsigned char ucFaceRegFlag;                /* 是否识别人脸，1：识别，0 未识别 */
    float fScore;                               /* 人脸置信概率 */
    float fClearness;                           /* 人脸清晰评估值,值越高越清晰 */
    XM_IA_RECT_S stRect;                        /* 人脸坐标 (取值范围[0,8191]，对应0～100%) */
    XM_IA_TD_FEATRUES_S *pstFeature;            /* 人脸特征结果 */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TD_FACERELT_S;

/* 车辆信息 */
typedef struct
{
    unsigned char ucPlateDetFlag;               /* 是否存在车牌，1：存在，0 不存在 */
    unsigned char ucPlateRegFlag;               /* 是否识别车牌，1：识别，0 未识别 */
    float fScore;                               /* 车牌置信概率 */
    char acPlateColor[3];                       /* 车牌颜色，黄蓝黑白或"无"代表未知，字符串结束符"\0" */
    char acPlateCode[20];                       /* 车牌号码，尾部有字符串结束符"\0" */
    XM_IA_RECT_S stRect;                        /* 车牌坐标 (取值范围[0,8191]，对应0～100%) */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TD_PLATERELT_S;

/* 规则触发信息 */
typedef struct
{
    unsigned int uiAlarmRuleIdx;                            /* 触发规则号 */
    XM_IA_DIRECTION_E eAlarmDirection;                      /* 报警方向 */
    XM_IA_RULE_STATUS_S astRuleStatus[XM_IA_RULE_NUM_MAX];  /* 每个规则对应状态 */
    unsigned char ucAllAlarm[XM_IA_RULE_NUM_MAX];           /* 每个规则对应报警信号：0为未报警/1为报警 */

    unsigned char ucReserved[124];                          /* 预留位 */
}XM_IA_TD_RULE_INFO_S;

typedef struct
{
    XM_IA_FALLDOWN_E eStatus;               /* 人形跌倒状态 */
    XM_IA_TD_FEATRUES_S *pstFeature;        /* 目标特征 */
    XM_IA_TD_ACTION_E eAction;              /* 人形目标行为（抽烟、打电话等） */
    XM_IA_TD_FACERELT_S stFaceInfo;         /* 人脸结果信息 */
    XM_IA_TD_PLATERELT_S stPlateInfo;       /* 车牌结果信息 */

    unsigned char ucReserved[512];          /* 预留位 */
}XM_IA_TD_CHARACTER_S;

/* 目标信息 */
typedef struct
{
    unsigned int uiId;                          /* 目标ID号 */
    unsigned char ucAlarm;                      /* 报警信号(0:未报警 1:报警) */
    float fScore;                               /* 目标置信度0.0-1.0 */
    XM_IA_RECT_S stRect;                        /* 目标坐标 (取值范围[0,8191]，对应0～100%) */
    XM_IA_TARGET_TYPE_E eTargetType;            /* 目标类型 */
    XM_IA_TD_RULE_INFO_S stRuleinfo;            /* 规则触发相关信息 */
    XM_IA_TD_CHARACTER_S stCharacter;           /* 目标附加功能结果 */
    unsigned char ucSingleTrackFlag;            /* 单目标跟踪模式下，当前id是否被跟踪(1:被跟踪 0:未被跟踪) */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TD_INFO_S;

/* 规则目标检测算法结果 */
typedef struct
{
    unsigned int uiTargetNum;                   /* 目标数 */
    XM_IA_TD_INFO_S *pstInfo;                   /* 目标信息，接口调用时需要根据最大目标数来创建内存 */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TD_INFOS_S;

/* 规则目标检测算法配置项结构体 */
typedef struct
{
    int iMDFlag;                                                /* 动检开关 （0:关闭 1:开启）*/
    unsigned char ucDetectDisable;                              /* 目标检测功能关闭，关闭后只有移动侦测; 0：开启，1：关闭 */
    XM_IA_TD_TYPE_E iDetectType;                                /* 目标检测类型(不同类型对应不同分类器) */
    XM_IA_TD_PATTERN_E ePattern;                                /* 标目检测图像大小选择，图像越大，检测距离越远，消耗内存越大帧率越低 */
    XM_IA_SENSE_E eSensitivity;                                 /* 目标检测灵敏度 */
    XM_IA_RECT_S stDetRegion;                                   /* 检测区域 */
    XM_IA_PD_RULES_S stTdRules[XM_IA_RULE_NUM_MAX];             /* 目标检测规则 */

    unsigned char ucReserved[128];                              /* 预留位 */
}XM_IA_TD_CONFIG_S;

/* 目标检测算法初始化结构体 */
typedef struct
{
    /* INPUT */
    unsigned int uiInStructSize;                    /* Run入参参数结构体（XM_IA_TD_IN_S）检查Size */
    unsigned int uiInitStructSize;                  /* 初始化参数结构体(XM_IA_TD_INIT_S)检查Size */
    unsigned int uiConfigStructSize;                /* 配置参数结构体(XM_IA_TD_CONFIG_S)检查Size */
    unsigned int uiRsltStructSize;                  /* 算法结果参数结构体(XM_IA_TD_RESULT_S)检查Size */

    unsigned int uiTdMaxNum;                        /* 最大目标检测数，默认设置32 */
    char acClassBinPath[XM_IA_PATH_MAX_LEN];        /* 分类器文件(.xmnn, .wk,.bin等)路径 */
    XM_IA_PLATFORM_E ePlatForm;                     /* 芯片平台选择 */
    XM_IA_IMG_TYPE_E eDevImgType;                   /* 设备图像类型 */
    XM_IA_IMG_SIZE_S stDevImgSize;                  /* 设备图像尺寸 */
    XM_IA_TD_CONFIG_S stTdCfg;                      /* 目标检测配置项 */
    XM_IA_TD_TRACK_S stTrack;                       /* 跟踪配置 */
    XM_IA_TD_EXTRAFUNC_S stExtraFunc;               /* 目标检测功能开关控制体 */

    /* OUTPUT */
    XM_IA_IMG_SIZE_S stAlgImgSize;                  /* 算法图像尺寸 */
    unsigned int uiBufferSize;                      /* 需要申请的内存大小，work中需要用到 */

    unsigned char ucReserved[128];                  /* 预留位 */
}XM_IA_TD_INIT_S;

/* 目标检测算法work输入参数 */
typedef struct
{
    void *pcBuffer;                                         /* 根据初始化返回的uiBufferSize大小来申请内存 */
    XM_IA_IMAGE_S *pstImage[XM_IA_TD_MAX_PIXEL_NUM];        /* 图像结构体,当前只用pstImage[0] */

    unsigned char ucReserved[128];                          /* 预留位 */
}XM_IA_TD_IN_S;

/* 规则目标检测算法结果 */
typedef struct
{
    XM_IA_TD_INFOS_S stTargetInfo;              /* 目标信息，当前画面结果 */
    XM_IA_TD_INFOS_S stLostInfo;                /* 目标丢失信息，当前丢失的目标 */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TD_RESULT_S;

/* 人形特征列表 */
typedef struct
{
    int iFeatureListNum;                        /* 人形特征库中人形数 */
    XM_IA_TD_FEATRUES_S **ppstFeatureItem;      /* 人脸特征列表指针 */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TD_FEATRUES_LIST_S;

/* 人形特征比对结果 */
typedef struct
{
    int iTDMatchId;                             /* 匹配到的目标位于XM_IA_TD_FEATRUES_LIST_S的数组下标 */
    float fMatchingRate;                        /* 配对率(0~100%) */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TD_MATCH_RSLT_S;

/* 目标特征比对,输出1-2个最相似目标结果
param [in] pstFeature      目标特征
param [in] pstFeatureList  目标特征列表
param [in] fThresh         比对阈值(0~100%),大于阈值结果才匹配成功
param [out] pstMatchRslt   目标匹配结果
return 0  成功
return !0 失败 */
int XM_IA_TD_MatchFeature(XM_IA_TD_FEATRUES_S *pstFeature, XM_IA_TD_FEATRUES_LIST_S *pstFeatureList, float fThresh, XM_IA_TD_MATCH_RSLT_S *pstMatchRslt);

#ifdef __cplusplus
}
#endif

#endif