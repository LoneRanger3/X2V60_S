/*****************************************************************************

------------------------------------------------------------------------------
        xm_ia_fdr_interface.h
产品名:    人脸检测识别
模块名:    人脸检测识别
生成日期:  2018-09-10
作者:      王则浪
文件描述:  人脸检测识别对外头文件
版本号：   V1.4.15.0
*****************************************************************************/
#ifndef _XM_IA_FDR_INTERFACE_H_
#define _XM_IA_FDR_INTERFACE_H_

#include "xm_ia_comm.h"
#include "xm_fr_interface.h"

#ifdef __cplusplus
extern "C" {
#endif
#define FDR_MAJOR_VERSION               (1)             /* 人脸检测识别算法主版本号 */
#define FDR_MINOR_VERSION               (4)             /* 人脸检测识别算法次版本号 */
#define FDR_REVERSION                   (15)             /* 人脸检测识别算法修改版本号 */
#define FDR_BUILD                       (0)             /* 人脸检测识别算法编译版本号(暂不用) */
#define FACE_POINT_NUM                  (68)            /* 人脸定位点数 */
#define FACE_POINT_MAX_NUM              (136)           /* 人脸定位最大点数 */
#define FACE_REC_VERSION_LEN            (10)            /* 人脸识别版本号长度 */

/* 人脸检测识别算法工作模式 */
typedef enum
{
    XM_IA_FDR_MODE_FDR = 0,                     /* 指示算法正常运行人脸检测识别 */
    XM_IA_FDR_MODE_EXTRACT_FEATURE = 1,         /* 指示算法当前帧人脸需要录入底库 */
    XM_IA_FDR_MODE_DIRECT_EXTRACT_FEATURE = 2,  /* 指示算法跳过人脸检测步骤,直接提取人脸角度和人脸特征 */
    XM_IA_FDR_MODE_DIRECT_GET_FACE_ANGLE = 3,   /* 指示算法跳过人脸检测步骤,直接获取人脸角度 */
    XM_IA_FDR_MODE_UNSTABLE_FDR = 4,            /* 指示算法不用等人脸位置稳定即可人脸识别 */
    XM_IA_FDR_MODE_FD = 5,                      /* 指示算法人脸只检测不识别 */
    XM_IA_FDR_MODE_NUM                          /* 人脸检测识别算法模式类型数 */

}XM_IA_FDR_MODE_E;

/* 活体开关 */
typedef enum
{
    XM_IA_ALIVE_MODE_INFRARED_INACTIVE = 0,     /* 关闭红外模式 */
    XM_IA_ALIVE_MODE_INFRARED_ACTIVE = 1        /* 启动红外模式 */
}XM_IA_ALIVE_MODE_E;

/* 人脸定位开关 */
typedef enum
{
    XM_IA_FP_MODE_INACTIVE = 0,                 /* 关闭特征点模式 */
    XM_IA_FP_MODE_ACTIVE = 1                    /* 启动特征点模式 */
}XM_IA_FP_MODE_E;

/* 活体结果 */
typedef enum
{
    XM_IA_ALIVE_INACTIVE = 0,                   /* 非活体 */
    XM_IA_ALIVE_ACTIVE = 1                      /* 活体 */
}XM_IA_ALIVE_S;

/* 注意力状态 */
typedef enum
{
    XM_IA_ATTENTION_NO = 0,                     /* 无困倦、分心迹象 */
    XM_IA_ATTENTION_YAWN = 10,                  /* 打哈欠 */
    XM_IA_ATTENTION_HEAD_TURNED = 13,           /* 转头 */
    XM_IA_ATTENTION_HEAD_LOWER = 14,            /* 低头抬头 */
    XM_IA_ATTENTION_EYE_CLOSED = 15,            /* 长时间闭眼 */
    XM_IA_ATTENTION_DOZE = 20,                  /* 打盹 */
    XM_IA_ATTENTION_CLOSE_EDGE = 30,            /* 靠近画面边缘 */

}XM_IA_ATTENTION_STATUS_E;

typedef struct
{
    unsigned char ucFaceAgeFlag;                /* 人脸年龄启用 */
    unsigned char ucFaceMaskFlag;               /* 口罩检测启用 */
    unsigned char ucFaceGlassFlag;              /* 眼镜检测启用 */
    unsigned char ucFaceGenderFlag;             /* 性别检测启用 */
    unsigned char ucFaceExpressionFlag;         /* 表情检测启用 */
    unsigned char ucFaceAttentionFlag;          /* 注意力启用 */

    unsigned char ucReserved[32];               /* 保留字节 */
}XM_IA_ATTRIBUTES_SET_S;

typedef struct
{
    unsigned char ucFaceAgeEnable;              /* 人脸年龄开关 */
    unsigned char ucFaceMaskEnable;             /* 口罩开关 */
    unsigned char ucFaceGlassEnable;            /* 眼镜开关 */
    unsigned char ucFaceGenderEnable;           /* 性别开关 */
    unsigned char ucFaceExpressionEnable;       /* 表情开关 */
    unsigned char ucFaceAttentionEnable;        /* 注意力开关 */

    unsigned char ucReserved[32];               /* 保留字节 */
}XM_IA_ATTRIBUTES_MODE_S;

typedef struct
{
    int iFaceMinWid;                            /* 图像中最小人脸宽度：与图像宽高窄边相对坐标(0~8192) */
    int iFaceMaxWid;                            /* 图像中最大人脸宽度：与图像宽高窄边相对坐标(0~8192) */

    unsigned char ucReserved[128];              /* 保留字节 */
}XM_IA_FD_CONFIG_S;

/* 人脸识别配置结构体 */
typedef struct
{
    int iClearThr;                              /* 人脸清晰度识别阈值 */
    XM_IA_ANGLE_S stFaceAngle;                  /* 配置人脸识别最大角度(0~90)：偏航角、横滚角、俯仰角 */
    XM_IA_RECT_S stFaceRecRegion;               /* 人脸识别区域：相对坐标(0~8191) */
    XM_IA_ATTRIBUTES_MODE_S stFaceAttrMode;     /* 人脸属性工作状态 */

    unsigned char ucReserved[128];              /* 保留字节 */
}XM_IA_FR_CONFIG_S;

/* 人脸角度结构体 */
typedef struct
{
    short sYaw;                                 /* 偏航角(-90, 90)正左转；负右转 */
    short sRoll;                                /* 横滚角(-90, 90)正：左低右高；负：左高右低 */
    short sPitch;                               /* 俯仰角(-90, 90)正：仰视；负：俯视 */
    short sReserved;                            /* 保留字节 */
}XM_IA_FACE_ANGLE_S;

/* 人脸检测识别配置参数 */
typedef struct
{
    XM_IA_SENSE_E eAlgSense;                    /* 算法灵敏度 */

    XM_IA_FD_CONFIG_S stFaceDetConfig;          /* 人脸检测配置 */
    XM_IA_FR_CONFIG_S stFaceRecConfig;          /* 人脸识别配置 */

    XM_IA_ROTATE_E eRotate;                     /* 旋转方向选择 */
    XM_IA_FACE_ANGLE_S stCalibrateFaceAngle;    /* 标定的正常人脸角度(DMS使用) */

    unsigned char ucReserved[248];              /* 保留字节 */
}XM_IA_FDR_CONFIG_S;

/* 人脸检测识别初始化参数 */
typedef struct
{
    char acClassBinPath[XM_IA_PATH_MAX_LEN];    /* 分类器Bin文件路径 */
    unsigned int uiInStructSize;                /* Run入参参数结构体检查Size */
    unsigned int uiInitStructSize;              /* 初始化参数结构体检查Size */
    unsigned int uiConfigStructSize;            /* 配置参数结构体检查Size */
    unsigned int uiRsltStructSize;              /* 算法结果参数结构体检查Size */

    int iFDMaxNum;                              /* 最大人脸检测数 */
    int iFRMaxNum;                              /* 最大人脸识别数 */
    int iRotateMode;                            /* 是否启用旋转模式：1启用；0不启用。启用旋转模式后支持0度不旋转、90度旋转和270度旋转 */
    XM_IA_IMG_TYPE_E eDevImgType;               /* 设备图像类型 */
    XM_IA_IMG_SIZE_S stDevImgSize;              /* 设备图像尺寸/XM510为算法确认过的图像尺寸 */
    XM_IA_FDR_CONFIG_S stFdrConfig;             /* 人脸检测识别配置 */
    XM_IA_ALIVE_MODE_E eInfraredMode;           /* 红外模式 */
    XM_IA_FP_MODE_E eFaceAngleMode;             /* 人脸角度检测开关，人脸识别默认开启 */
    XM_IA_PLATFORM_E ePlatForm;                 /* 芯片平台选择 */
    XM_IA_ATTRIBUTES_SET_S stFaceAttrSet;       /* 人脸属性:年龄 */

    char acFrAlgVersion[FACE_REC_VERSION_LEN];  /* OUTPUT: 人脸识别算法版本号 */
    XM_IA_IMG_SIZE_S stAlgImgSize;              /* OUTPUT: 算法图像尺寸 */

    unsigned char ucAutoCalibrateFlag;          /* 启用智能自动标定，使用车速信息实现(DMS使用). */

    unsigned char ucReserved[255];              /* 保留字节 */
}XM_IA_FDR_INIT_S;

typedef struct
{
    int iRegionNum;                             /* 待识别人脸区域个数,最大支持6个区域 */
    XM_IA_RECT_S astFaceRect[6];                /* 待识别人脸区域位置,相对坐标(0~8191) */
    
    unsigned char ucReserved[64];               /* 保留字节 */
}XM_IA_FDR_REGION_S;

/* 人脸检测识别工作入参 */
typedef struct
{
    unsigned char ucIsFirstFrm;                 /* 首帧非首帧标志：1首帧/0非首帧 */
    XM_IA_FDR_MODE_E eFdrMode;                  /* 人脸检测识别算法模式 */
    XM_IA_FDR_REGION_S stFaceRecRegion;         /* 人脸识别区域,仅XM_IA_FDR_MODE_DIRECT_EXTRACT_FEATURE、XM_IA_FDR_MODE_DIRECT_GET_FACE_ANGLE模式下生效 */
    XM_IA_IMAGE_S *pstImg[2];                   /* 图像结构体,0:彩图,1:副图像(双摄活体为红外图像,也可组成双通道图像加速算法XM580支持) */

    short sSpeed;                               /* 车辆当前行驶速度(常见值-20~140km/h).(DMS智能标定使用) */ 

    unsigned char ucReserved[126];              /* 保留字节 */
}XM_IA_FDR_IN_S;

/* 人脸检测面部信息 */
typedef struct
{
    int iFaceId;                                    /* 人脸ID */

    float fScore;                                   /* 人脸置信概率 */
    float fClearness;                               /* 人脸清晰评估值,值越高越清晰 */

    /* 以相对坐标方式提供(0~8191) */
    XM_IA_RECT_S stFace;                            /* 人脸位置 */
    XM_IA_POINT_S stNose;                           /* 鼻子位置 */
    XM_IA_POINT_S stLftEye;                         /* 左眼位置 */
    XM_IA_POINT_S stRgtEye;                         /* 右眼位置 */
    XM_IA_POINT_S stMouthLft;                       /* 左嘴角位置 */
    XM_IA_POINT_S stMouthRgt;                       /* 右嘴角位置 */
    XM_IA_POINT_S astFacePoint[FACE_POINT_MAX_NUM]; /* 人脸特征位置点 */
    XM_IA_ALIVE_S iAliveFace;                       /* 人脸活体检测 */
    int iAliveScore;                                /* 人脸活体分数[0～100],分数越高活体概率越高 */
    unsigned char ucAge;                            /* 人脸年龄[0～100].默认值255 */
    unsigned char ucGlass;                          /* 人脸有无佩戴眼镜:0未佩戴/1佩戴眼镜.默认值255 */
    unsigned char ucGender;                         /* 人脸性别:0男人/1女人.默认值255 */
    unsigned char ucFaceMask;                       /* 人脸有无佩戴口罩:0未佩戴/1佩戴口罩.默认值255 */
    unsigned char ucFaceExpression;                 /* 人脸表情:0平静/1开心/2悲痛/3愤怒/4恐惧/5惊奇.默认值255 */
    XM_IA_ATTENTION_STATUS_E eAttentionStatus;      /* 人脸注意力状态.默认值0 */
    unsigned char ucFaceQuanlity;                   /* 人脸质量[0～100] */

    unsigned char ucReserved[127];                  /* 保留字节 */
}XM_IA_FACE_DET_INFO_S;

/* 待识别人脸状态 */
typedef enum
{
    FACE_STATUS_APPROPRIATE = 0,    /* 恰当人脸 */
    FACE_STATUS_LEAN = 1,           /* 倾斜人脸 */
    FACE_STATUS_OUTDROP = 2,        /* 界外人脸 */
    FACE_STATUS_ANALYSISING = 3,    /* 尚在分析中人脸 */
    FACE_STATUS_LOW_RELIABLE = 4,   /* 低置信度人脸 */
    FACE_STATUS_INVALID = 5,        /* 人脸角度无法计算 */
    FACE_STATUS_SUBMARGINAL = 6,    /* 靠近边缘人脸 */
    FACE_STATUS_SMALL = 7,          /* 小人脸 */
    FACE_STATUS_NUM                 /* 人脸状态数 */
}APPROPRIATE_FACE_STATUS_E;

/* 人脸角度结构体 方向均以本人方向*/
typedef struct
{
    int iYaw;                                       /* 偏航角(-90, 90)正左转；负右转 */
    int iRoll;                                      /* 横滚角(-90, 90)正：左低右高；负：左高右低 */
    int iPitch;                                     /* 俯仰角(-90, 90)正：仰视；负：俯视 */

    APPROPRIATE_FACE_STATUS_E eFaceAngleStatus;     /* 人脸角度状态 */

    unsigned char ucReserved[32];                   /* 保留字节 */
}XM_IA_FDR_ANGLE_S;

/* 人脸检测结果 */
typedef struct
{
    int iFaceNum;                               /* 人脸检测数 */
    XM_IA_FACE_DET_INFO_S* pstFaceRect;         /* 人脸信息 */
    XM_IA_FDR_ANGLE_S* pstFaceAngle;            /* 人脸角度信息 */

    unsigned char ucReserved[128];              /* 保留字节 */
}XM_IA_FD_RESULTS_S;

/* 人脸识别信息 */
typedef struct
{
    int iFaceId;                                /* 识别人脸ID号 */
    XM_IA_RECT_S stFaceRect;                    /* 识别位置:相对坐标(0~8191) */
    XM_IA_FR_FEATRUES stFrFeatures;             /* 人脸特征 */

    unsigned char ucReserved[128];              /* 保留字节 */
}XM_IA_FACE_REC_INFO_S;

/* 人脸识别结果 */
typedef struct
{
    int iFaceNum;                               /* 人脸识别数 */
    XM_IA_FACE_REC_INFO_S* pstFaceRecInfo;      /* 人脸识别信息 */

    unsigned char ucReserved[128];              /* 保留字节 */
}XM_IA_FR_RESULTS_S;

/* 人脸检测区域信息 */
typedef struct
{
    int iActiveFlag;                    /* 区域激活标志 */
    int iAveLightness;                  /* 区域平均亮度 */
    int iLightnessLmtMin;               /* 人脸识别亮度下限 */
    int iLightnessLmtMax;               /* 人脸识别亮度上限 */
    XM_IA_RECT_S stRegion;              /* 提取人脸亮度区域位置(在白光图中，相对坐标0~8192) */

    unsigned char ucIrActiveFlag;       /* IR通道人脸激活标志 */
    unsigned char ucIrAveLightness;     /* IR通道人脸平均亮度 */
    unsigned char ucIrLightnessLmtMin;  /* IR通道人脸亮度下限要求 */
    unsigned char ucIrLightnessLmtMax;  /* IR通道人脸亮度上限要求 */
    unsigned char ucReserved[28];       /* 保留字节 */

}XM_IA_FD_REGION_INFO_S;

/* 人脸检测识别结果 */
typedef struct
{
    XM_IA_FD_RESULTS_S stFdResult;              /* 人脸检测结果 */
    XM_IA_FR_RESULTS_S stFrResult;              /* 人脸识别结果 */

    XM_IA_FD_REGION_INFO_S stFdRegionInfo;      /* 人脸区域信息 */

    unsigned char ucReserved[256];              /* 保留字节 */
}XM_IA_FDR_RESULTS_S;

#ifdef __cplusplus
}
#endif

#endif