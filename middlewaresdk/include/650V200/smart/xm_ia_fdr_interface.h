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

typedef struct
{
    unsigned char ucEyeClosedCnt;
    unsigned char ucHeadLowerCnt;
    unsigned char ucHeadTurnedCnt;
    unsigned char ucMouthWideOpenCnt;
   
    unsigned char ucReserved[24];
}FDR_IA_ATTENTION_STRATEGY_S;

/* 人脸检测识别算法工作模式 */
typedef enum
{
    XM_IA_FDR_MODE_FDR = 0,                     /* 指示算法正常运行人脸检测识别 */
    XM_IA_FDR_MODE_EXTRACT_FEATURE = 1,         /* 指示算法当前图像需要录入底库.(离线、在线都可以用) */
    XM_IA_FDR_MODE_DIRECT_EXTRACT_FEATURE = 2,  /* 指示算法跳过人脸检测步骤,直接提取人脸角度和人脸特征 */
    XM_IA_FDR_MODE_DIRECT_GET_FACE_ANGLE = 3,   /* 指示算法跳过人脸检测步骤,直接获取人脸角度 */
    XM_IA_FDR_MODE_UNSTABLE_FDR = 4,            /* 指示算法不用等人脸位置稳定即可人脸识别 */
    XM_IA_FDR_MODE_FD = 5,                      /* 指示算法人脸只检测不识别 */
    XM_IA_FDR_MODE_FDR_TWO_FEATURES = 6,        /* 指示算法人脸检测识别,提取双特征 */
    XM_IA_FDR_MODE_UNSTABLE_FDR_TWO_FEATURES = 7,/* 指示算法不用等人脸位置稳定即可人脸识别,并提取双特征 */
    XM_IA_FDR_MODE_EXTRACT_TWO_FEATURES = 8,    /* 指示算法提取视频流双特征(门锁使用,为兼容老版本,需与外部同步修改，暂时与1反过来设计) */
    XM_IA_FDR_MODE_NUM                          /* 人脸检测识别算法模式类型数 */

}XM_IA_FDR_MODE_E;

/* 活体开关 */
typedef enum
{
    XM_IA_ALIVE_MODE_INFRARED_INACTIVE = 0,                 /* 关闭活体模式 */
    XM_IA_ALIVE_MODE_INFRARED_ACTIVE = 1,                   /* 彩色 + 红外判断活体模式 */
    XM_IA_ALIVE_MODE_INFRARED_DUAL = 2,                     /* 双红外判断活体模式 */
    XM_IA_ALIVE_MODE_COLOR_INFRARED_PLUS_INFRARED = 3,      /* 彩色红外双通 + 红外判断活体模式 */
    XM_IA_ALIVE_MODE_COLOR_PLUS_INFRARED = 4,               /* 门锁RGB + 红外判断活体模式 */
    XM_IA_ALIVE_MODE_INFRARED_DUAL_ASYNC = 5,               /* 双红外异步判断活体模式 */
    XM_IA_ALIVE_MODE_COLOR_PLUS_INFRARED_DUAL_ASYNC = 6,    /* RGB + 双红外异步判断活体模式 */
    XM_IA_ALIVE_MODE_MONOCULAR_INFRARED = 7                 /* 单目红外活体模式 */
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
    XM_IA_ATTENTION_EYE_CLOSED = 15,            /* 闭眼 */
    XM_IA_ATTENTION_DOZE = 20,                  /* 打盹 */
    XM_IA_ATTENTION_CLOSE_EDGE = 30,            /* 靠近画面边缘 */

}XM_IA_ATTENTION_STATUS_E;

typedef struct
{
    unsigned char ucFaceAgeFlag;                /* 支持人脸年龄 */
    unsigned char ucFaceMaskFlag;               /* 支持口罩检测 */
    unsigned char ucFaceGlassFlag;              /* 支持眼镜检测 */
    unsigned char ucFaceGenderFlag;             /* 支持性别检测 */
    unsigned char ucFaceExpressionFlag;         /* 支持表情检测 */
    unsigned char ucFaceAttentionFlag;          /* 支持注意力 */
    unsigned char ucSafetyHelmetFlag;           /* 支持安全帽检测 */

    unsigned char ucReserved[31];               /* 保留字节 */
}XM_IA_ATTRIBUTES_SET_S;

typedef struct
{
    unsigned char ucFaceAgeEnable;              /* 人脸年龄开关 */
    unsigned char ucFaceMaskEnable;             /* 口罩开关 */
    unsigned char ucFaceGlassEnable;            /* 眼镜开关 */
    unsigned char ucFaceGenderEnable;           /* 性别开关 */
    unsigned char ucFaceExpressionEnable;       /* 表情开关 */
    unsigned char ucFaceAttentionEnable;        /* 注意力开关 */
    unsigned char ucSafetyHelmetEnable;         /* 安全帽检测开关 */

    unsigned char ucReserved[31];               /* 保留字节 */
}XM_IA_ATTRIBUTES_MODE_S;

typedef struct
{
    int iFaceMinWid;                            /* 图像中最小人脸宽度：与图像宽高窄边相对坐标(0~8192) */
    int iFaceMaxWid;                            /* 图像中最大人脸宽度：与图像宽高窄边相对坐标(0~8192) */

    unsigned char ucReserved[128];              /* 保留字节 */
}XM_IA_FD_CONFIG_S;

/* 人脸活体防御等级配置：越高越安全但通过率降低、越低通过率越高但相对不安全 */
typedef enum
{
    XM_IA_FR_ALIVE_LEVEL_LOWEST = -2,           /* 最低活体防御等级 */
    XM_IA_FR_ALIVE_LEVEL_LOW = -1,              /* 较低活体防御等级 */
    XM_IA_FR_ALIVE_LEVEL_MID = 0,               /* 中活体防御等级 */
    XM_IA_FR_ALIVE_LEVEL_HIGH = 1,              /* 较高活体防御等级 */
    XM_IA_FR_ALIVE_LEVEL_HIGHEST = 2,           /* 最高活体防御等级 */
    XM_IA_FR_ALIVE_LEVEL_NUM = 5,               /* 活体防御等级级数 */
}XM_IA_FR_ALIVE_LEVEL_E;

/* 人脸识别配置结构体 */
typedef struct
{
    int iClearThr;                              /* 人脸清晰度识别阈值 */
    XM_IA_ANGLE_S stFaceAngle;                  /* 配置人脸识别最大角度(0~90)：偏航角、横滚角、俯仰角 */
    XM_IA_RECT_S stFaceRecRegion;               /* 人脸识别区域：相对坐标(0~8191) */
    XM_IA_ATTRIBUTES_MODE_S stFaceAttrMode;     /* 人脸属性工作状态 */

    XM_IA_FR_ALIVE_LEVEL_E eAliveLevel;         /* 人脸活体防御等级 */
    XM_IA_FR_MATCH_LEVEL_E eMatchLevel;         /* 人脸识别等级 */

    unsigned char ucReserved[120];              /* 保留字节 */
}XM_IA_FR_CONFIG_S;

/* 人脸角度结构体 */
typedef struct
{
    short sYaw;                                 /* 偏航角(-90, 90)正左转；负右转 */
    short sRoll;                                /* 横滚角(-90, 90)正：左低右高；负：左高右低 */
    short sPitch;                               /* 俯仰角(-90, 90)正：仰视；负：俯视 */
    short sReserved;                            /* 保留字节 */
}XM_IA_FACE_ANGLE_S;

/* 人脸角度结构体 */
typedef struct
{
    short sRotateYaw;                           /* 转头[35, 90],左右转角度相同阈值,典型值45 */
    short asReserved[3];                        /* 保留字节 */

}XM_IA_FACE_ROTATION_ANGLE_THR_S;

/* 人脸检测识别配置参数 */
typedef struct
{
    XM_IA_SENSE_E eAlgSense;                    /* 算法灵敏度 */

    XM_IA_FD_CONFIG_S stFaceDetConfig;          /* 人脸检测配置 */
    XM_IA_FR_CONFIG_S stFaceRecConfig;          /* 人脸识别配置 */

    XM_IA_ROTATE_E eRotate;                     /* 旋转方向选择 */
    XM_IA_FACE_ANGLE_S stCalibrateFaceAngle;    /* 标定的正常人脸角度(DMS使用) */
    XM_IA_FACE_ROTATION_ANGLE_THR_S stRotationAngleThr;  /* 偏离正常驾驶位角度报警阈值(DMS使用,当前只有Yaw有效,并使用正数配置) */
    FDR_IA_ATTENTION_STRATEGY_S stAttentionStrategy;  /* 人脸注意力策略 */

    unsigned char ucReserved[212];              /* 保留字节 */
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

    unsigned char ucAutoCalibrateFlag;          /* DMS算法启用智能自动标定正常驾驶角度，使用车速信息实现. */
    unsigned char ucAutoCalibrateTotalCnt;      /* DMS智能自动标定,总统计数据量，建议设置100 */
    unsigned char ucAutoCalibrateStartSpeed;    /* DMS智能自动标定,起作用速度km/h，建议设置20～40 */
    unsigned char ucAutoCalibrateCntInterval;   /* DMS智能自动标定,统计帧间隔，建议设置5  */

    char *pcCalibrateFilePath;                  /* 双目活体算法双目标定:文件路径/加载到的内存地址 */
    char *pcModelFilePtr;                       /* 模型文件地址.(模型加载到内存中直接读取) */
    
    unsigned char ucReserved[244];              /* 保留字节 */
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
    char cInImgMode;                            /* 输入图像模式.0:彩色+IR输入,1:双IR输入.(双通识别使用),2:彩色+IR逆光场景,4:非夜间强转IR */
    unsigned char ucReserved1;

    XM_IA_IMAGE_S *pstImgMultiple[2];           /* 多目时的图像输入 */

    unsigned char ucReserved[116];              /* 保留字节 */
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
    unsigned char ucSafetyHelmet;                   /* 有无佩戴安全帽:0未佩戴/1佩戴.默认值255 */

    unsigned char ucReserved[126];                  /* 保留字节 */
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
    FACE_STATUS_LEFT_EDGE = 8,      /* 人脸位于画面左边缘 */
    FACE_STATUS_RIGHT_EDGE = 9,     /* 人脸位于画面右边缘 */
    FACE_STATUS_TOP_EDGE = 10,      /* 人脸位于画面上边缘 */
    FACE_STATUS_BOTTOM_EDGE = 11,   /* 人脸位于画面下边缘 */
    FACE_STATUS_NEAR_EDGE = 12,     /* 人脸位于画面近边缘 */
    FACE_STATUS_FAR_EDGE = 13,      /* 人脸位于画面远边缘 */
    FACE_STATUS_OVER_EXPOSURE = 14, /* 人脸过曝 */
    FACE_STATUS_UNDER_EXPOSURE = 15,/* 人脸欠曝 */
    FACE_STATUS_UNCLEAR = 16,       /* 人脸不清晰 */
    FACE_STATUS_BLOCK = 17,         /* 人脸遮挡 */
    FACE_STATUS_NUM                 /* 人脸状态数 */

}APPROPRIATE_FACE_STATUS_E;

/* 人脸检测识别指引 */
typedef enum
{
    FACE_GUIDE_APPROPRIATE = 0,     /* 恰当人脸 */
    FACE_GUIDE_LOOK_STRAIT = 1,     /* 人脸要平视,不要歪头 */
    FACE_GUIDE_MULTI_FACE = 2,      /* 多人脸,请保持一张人脸 */
    FACE_GUIDE_MOVE_LEFT = 3,       /* 人脸位于右侧边缘,请朝左移动 */
    FACE_GUIDE_MOVE_RIGHT = 4,      /* 人脸位于左侧边缘,请朝右移动 */
    FACE_GUIDE_MOVE_FORWARD = 5,    /* 人脸过远/补光不到,请朝前移动 */
    FACE_GUIDE_MOVE_BACKWARD = 6,   /* 人脸过近/补光过强,请朝后移动 */
    FACE_GUIDE_TAKE_OFF_MASK = 7,   /* 人脸带口罩无法识别 */
    FACE_GUIDE_MOVE_UP = 8,         /* 人脸位于下边缘,人脸请朝上移动 */
    FACE_GUIDE_MOVE_DOWN = 9,       /* 人脸位于上边缘,人脸请朝下移动 */
    FACE_GUIDE_SHOW_COMPLETE_FACE = 10,     /* 人脸遮挡,出示完整人脸 */
    FACE_GUIDE_NUM                  /* 人脸指引状态数 */

}XM_IA_FACE_GUIDE_E;

/* 人脸角度结构体 方向均以画面中人的方向*/
typedef struct
{
    int iYaw;                                       /* 偏航角(-90, 90)正：左转；负：右转 */
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

/* DMS状态结果 */
typedef struct
{
    int iCalibrateCnt;                          /* 智能标定已统计人脸角度帧数 */
    int iCalibrateTotal;                        /* 智能标定需要统计帧数 */

    unsigned char ucAutoCalibrateStatus;        /* 智能标定状态：0未完成；1已完成,ucDriveYaw、ucDriveRoll、ucDrivePitch生效 */
    char cDriveYaw;                             /* 驾驶姿态偏航角：一般为-30～30 */
    char cDriveRoll;                            /* 驾驶姿态横滚角：暂未有作用,固定为0 */
    char cDrivePitch;                           /* 驾驶姿态俯仰角：一般为-15～15 */

    unsigned char ucReserved[244];              /* 保留字节 */
}XM_IA_DMS_STATUS_S;

/*************************************************
Author: WangZelang
Date: 2022-10-06
Description: 提供通过人脸识别指引
INPUT:  pstFaceAngleInfo    人脸角度信息
        pstFdrRslt          人脸检测识别算法结果
OUTPUT: peFaceAppropriate   比对结果，最相似人脸索引，相似度
*************************************************/
int XM_IA_GetFaceRecGuide(XM_IA_FDR_ANGLE_S *pstFaceAngleInfo, XM_IA_FDR_RESULTS_S *pstFdrRslt, XM_IA_FACE_GUIDE_E *peFaceAppropriate);

/*************************************************
Author: WangZelang
Date: 2023-05-09
Description: 人脸降噪
INPUT:  pFdrHandle          人脸工作句柄
OUTPUT: pstFaceImg          人脸图
*************************************************/
int XM_IA_GetDenoisedFace(void *pFdrHandle, XM_IA_IMAGE_S *pstFaceImg);

/*************************************************
Author: WangZelang
Date: 2023-09-14
Description: 人脸超分辨率
INPUT:  pFdrWork            人脸工作句柄
        pstFaceInImg        人脸输入图
OUTPUT: pstFaceOutImg       人脸超分辨率图
*************************************************/
int XM_IA_GetSRFace(void *pFdrHandle, XM_IA_IMAGE_S *pstFaceInImg, XM_IA_IMAGE_S *pstFaceOutImg);

/*************************************************
Author: WangZelang
Date: 2023-05-17
Description: 获取DMS状态信息
INPUT:  pFdrHandle          人脸工作句柄
OUTPUT: pstDmsStatus        DMS状态
*************************************************/
int XM_IA_GetDmsStatus(void *pFdrHandle, XM_IA_DMS_STATUS_S *pstDmsStatus);


#ifdef __cplusplus
}
#endif

#endif