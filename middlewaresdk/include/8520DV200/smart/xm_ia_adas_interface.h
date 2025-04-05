/***********************************************************************************

------------------------------------------------------------------------------------

                            xm_ADAS_interface.h
产品名:
模块名:
生成日期:
作者:
文件描述: adas头文件

------------------------------------------------------------------------------
   修改历史
   日期        姓名             描述
--------------------------------------------------------------------------

*****************************************************************************/
#ifndef __XM_ADAS_MEASURE_H__
#define __XM_ADAS_MEASURE_H__

#include "xm_ia_comm.h"

#ifdef  __cplusplus
extern "C"{
#endif

#define ADAS_MAJOR_VERSION               (1)             /* 车辆检测算法主版本号 */
#define ADAS_MINOR_VERSION               (3)             /* 车辆检测算法次版本号 */
#define ADAS_REVERSION                   (1)             /* 车辆检测算法修改版本号 */
#define ADAS_BUILD                       (0)             /* 车辆检测算法编译版本号(暂不用) */

#define XM_IA_ADAS_MAX_INPUT_IMAGE_NUM (4)               /* 最多支持图像分辨率个数 */
#define XM_IA_ADAS_MAX_OBJ_NUM (32)      /* 目标数量(输出) */
#define XM_IA_ADAS_MAX_LANE_POINTS (100) /* 车道线最大关键点数量 */
#define XM_IA_ADAS_MAX_LANE_NUM    (4)   /* 车道线条数 */

/* 车道线的颜色 */
typedef enum
{
    XM_ADAS_LANE_NONE = 0,                /* 未知类型 */
    XM_ADAS_LANE_WHITE_DOTTED = 1,        /* 单白色虚线 */
    XM_ADAS_LANE_WHITE_SOLID = 2,         /* 单白色实线 */
    XM_ADAS_LANE_YELLOW_DOTTED = 3,       /* 单黄色虚线 */
    XM_ADAS_LANE_YELLOW_SOLID = 4,        /* 单黄色实线 */
    XM_ADAS_DLANE_WHITE_DOTTED = 5,       /* 双白色虚线 */
    XM_ADAS_DLANE_WHITE_SOLID = 6,        /* 双白色实线 */
    XM_ADAS_DLANE_YELLOW_DOTTED = 7,      /* 双黄色虚线 */
    XM_ADAS_DLANE_YELLOW_SOLID = 8        /* 双黄色实线 */
}XM_ADAS_LANE_COLOR_E;

typedef enum
{
    XM_ADAS_LANE_LEFT = 0,              /* 左车道线 */
    XM_ADAS_LANE_RIGHT = 1,             /* 右车道线 */
}XM_ADAS_LANE_POSITION_E;

typedef enum
{
    XM_ADAS_NORMAL = 0,                     /* 正常 */
    XM_ADAS_CRIMPING = 2,                   /* 压线预警 */
}XM_ADAS_ALARM_TYPE_E;

typedef struct
{
    /* 相机外参 */
    float fDistanceGround;  /* 相机与地面的距离(单位mm) */
    float fDistanceLeft;    /* 相机与左侧车轮的距离(单位mm) */
    float fDistanceRight;   /* 相机与右侧车轮的距离(单位mm) */
    float fDistanceFront;   /* 相机与车辆前保险杠的距离(单位mm) */
    float fVanishPointX;	/* 图像消失点X坐标 */
    float fVanishPointY;	/* 图像消失点Y坐标 */
    float fLanePointY;      /* 图像引擎盖处Y坐标 */

    char acResver[64];
} XM_ADAS_CAM_CAL_S;

typedef struct
{
    int iEnable;                              /* 算法功能开关 */
    XM_IA_SENSE_E eVeSense;                   /* 算法灵敏度 */
    float flLaneThred;                        /* 压线距离阈值 */
    int iLaneDepartureType;                   /* 0:灵敏度高，车辆靠近车道线开始报警 */
    char acResver[60];                        /* 1:灵敏度低，车辆压过车道线开始报警 */
}XM_IA_ADAS_CONFIG_S;

/* 算法初始化结构体 */
typedef struct
{
    /* 输入信息 */
    unsigned int uiInStructSize;                /* Run入参参数结构体检查Size */
    unsigned int uiInitStructSize;              /* 初始化参数结构体检查Size  */
    unsigned int uiConfigStructSize;            /* 配置参数结构体检查Size    */
    unsigned int uiRsltStructSize;              /* 算法结果参数结构体检查Size  */

    unsigned int uiAdasMaxNum;                   /* 最大目标检测数*/
    char acClassBinPath[XM_IA_PATH_MAX_LEN];    /* 分类器Bin文件路径 */
    char acCamParamPath[XM_IA_PATH_MAX_LEN];    /* 相机参数文件路径 */
    XM_IA_PLATFORM_E ePlatForm;                 /* 芯片平台选择 */
    XM_IA_IMG_TYPE_E eDevImgType;               /* 设备图像类型 */
    XM_IA_ADAS_CONFIG_S stAdasCfg;              /* 辅助驾驶检测配置项 */
    XM_ADAS_CAM_CAL_S stCamCfg;                 /* 相机内参、外参配置项 */
    XM_IA_IMG_SIZE_S stDevImgSize;              /* 设备输入分辨率 */

    /* OUTPUT */
    XM_IA_IMG_SIZE_S stAlgImgSize;              /* Adas算法分辨率 */

    char acResver[64];
} XM_IA_ADAS_INIT_S;

/* 算法work输入参数 */
typedef struct
{
    XM_IA_IMAGE_S *pstDevImage[XM_IA_ADAS_MAX_INPUT_IMAGE_NUM];                    /* 设备图像结构体 */
    char acResver[64];
} XM_IA_ADAS_IN_S;

/* 车辆结果结构体 */
typedef struct 
{
    int iID;
    float flConfidence;         /* 目标置信度 */
    float flDistX;              /* 目标相对横向距离(mm),负数表示目标在左侧，正数表示目标在右侧 */
    float flDistY;              /* 目标相对纵向距离(mm) */
    XM_IA_TARGET_TYPE_E eClass; /* 目标类型 */
    XM_IA_RECT_S stRect;        /* 目标位置整型(百分比表示0~8192对应0～100%)  */
    char acResver[64];
} XM_IA_ADAS_OBJ_INFO_S;

/* 车辆结果结构体 */
typedef struct 
{
    int iCount;                  /* 车辆信息个数 */
    XM_IA_ADAS_OBJ_INFO_S stVeInfo[XM_IA_ADAS_MAX_OBJ_NUM];
} XM_IA_ADAS_OBJ_RSLT_S;

/* 车道线结果结构体 */
typedef struct 
{
    float flCoefA;          /* 车道线系数 */
    float flCoefB;          /* 车道线系数 */
    XM_IA_POINT_S stEndPoint;       /* 截止点坐标 */
    XM_IA_POINT_S stStartPoint;     /* 起始点坐标 */
    XM_ADAS_LANE_COLOR_E eLaneType; /* 判断车道线类别 */
    XM_ADAS_ALARM_TYPE_E eAlarm;    /* 压线预警标识 */
    int iconfidence;                /* 车道线质量指标:0表示质量差，1表示质量好 */
    int iCount;						/* 为0表示该车道线不存在 */
    char acResver[64];
} XM_IA_ADAS_LANE_INFO_S;

/* 车道线结果结构体 */
typedef struct 
{
    XM_IA_ADAS_LANE_INFO_S stLaneInfo[XM_IA_ADAS_MAX_LANE_NUM];     /* 0,1,2,3四条车道线、其中1,2为本车道左右车道线,包含报警信息 */
} XM_IA_ADAS_LANE_RSLT_S;                                           /* 0,3两条车道线仅做显示 */

typedef struct
{
    XM_IA_ADAS_OBJ_RSLT_S stObjResult;
    XM_IA_ADAS_LANE_RSLT_S stLaneResult;
    
    char acResver[64];
} XM_IA_ADAS_RESULT_S;

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif