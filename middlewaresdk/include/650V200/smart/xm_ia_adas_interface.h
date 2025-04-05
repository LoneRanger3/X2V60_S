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
#ifndef __XM_ADAS_INTERFACE_H__
#define __XM_ADAS_INTERFACE_H__

#include "xm_ia_comm.h"

#ifdef  __cplusplus
extern "C"{
#endif

#define ADAS_MAJOR_VERSION               (1)             /* 车辆检测算法主版本号 */
#define ADAS_MINOR_VERSION               (3)             /* 车辆检测算法次版本号 */
#define ADAS_REVERSION                   (6)             /* 车辆检测算法修改版本号 */
#define ADAS_BUILD                       (0)             /* 车辆检测算法编译版本号(暂不用) */

#define XM_IA_ADAS_MAX_INPUT_IMAGE_NUM (4)               /* 最多支持图像分辨率个数 */
#define XM_IA_ADAS_MAX_OBJ_NUM (32)      /* 目标数量(输出) */
#define XM_IA_ADAS_MAX_LANE_POINTS (100) /* 车道线最大关键点数量 */
#define XM_IA_ADAS_MAX_LANE_NUM    (4)   /* 车道线条数 */

/* 车道线的颜色 */
typedef enum
{
    /* 0-8号类别弃用 */
    XM_ADAS_LANE_NONE = 0,                /* 未知类型 */
    XM_ADAS_LANE_WHITE_DOTTED = 1,        /* 单白色虚线 */
    XM_ADAS_LANE_WHITE_SOLID = 2,         /* 单白色实线 */
    XM_ADAS_LANE_YELLOW_DOTTED = 3,       /* 单黄色虚线 */
    XM_ADAS_LANE_YELLOW_SOLID = 4,        /* 单黄色实线 */
    XM_ADAS_DLANE_WHITE_DOTTED = 5,       /* 双白色虚线 */
    XM_ADAS_DLANE_WHITE_SOLID = 6,        /* 双白色实线 */
    XM_ADAS_DLANE_YELLOW_DOTTED = 7,      /* 双黄色虚线 */
    XM_ADAS_DLANE_YELLOW_SOLID = 8,       /* 双黄色实线 */

    XM_ADAS_DASHED_LANE = 10,             /* 虚线 */
    XM_ADAS_SOLID_LANE = 11,              /* 实线 */
    XM_ADAS_DOUBLE_DASHED_LANE = 12,      /* 双虚线 */
    XM_ADAS_DOUBLE_SOLID_LANE = 13,       /* 双实线 */
    XM_ADAS_SOLID_DASHED_LANE = 14,       /* 左实右虚线 */
    XM_ADAS_DASHED_SOLID_LANE = 15        /* 左虚右实线 */
}XM_ADAS_LANE_COLOR_E;

typedef enum
{
    XM_ADAS_NORMAL = 0,                       /* 正常 */
    XM_ADAS_CRIMPING_L = 2,                   /* 压左线预警 */
    XM_ADAS_CRIMPING_R = 3,                   /* 压右线预警 */
    XM_ADAS_COLLISIONWARNING = 4,             /* 碰撞预警 */
    XM_ADAS_CHANGING_LANE = 5,                /* 违法变道 */
    XM_ADAS_PUSH_IN = 6,                      /* 加塞 */
}XM_ADAS_ALARM_TYPE_E;

typedef enum
{
    XM_ADAS_LIGHT_GREEN_GO = 5,                     /* 绿灯*/
    XM_ADAS_LIGHT_GREEN_LEFT_GO = 6,                /* 左转绿灯 */
    XM_ADAS_LIGHT_GREEN_RIGHT_GO = 7,               /* 右转绿灯 */
    XM_ADAS_LIGHT_GREEN_STRAIGHT_GO = 8,            /* 直行绿灯 */
    XM_ADAS_LIGHT_GREEN_TURN_GO = 9,                /* 调头绿灯 */
}XM_ADAS_ALARM_LIGHT_TYPE_E;

typedef enum
{
    XM_ADAS_MODE_FCS = 0x1,                            /* 前车起步报警 */
    XM_ADAS_MODE_LANE = 0x2,                           /* 车道线偏离报警 */
    XM_ADAS_MODE_SPEEDLIMITSIGN= 0x4,                  /* 交通限速牌报警 */
    XM_ADAS_MODE_COLLISION_P= 0x8,                     /* 人形碰撞预警 */ 
    XM_ADAS_MODE_COLLISION_V= 0x10,                    /* 车辆碰撞预警 */ 
    XM_ADAS_MODE_VIRTUAL_BUMPER = 0x20,                /* 虚拟保险杠 */
    XM_ADAS_MODE_LCA = 0x40,                           /* 变道辅助系统:LCA(LaneChangeAssist) */
    XM_ADAS_MODE_COLLISION_REAR = 0x80,                /* 后车碰撞预警 */
    XM_ADAS_MODE_TRAFFICLIGHT = 0x100,                 /* 交通灯识别 */
    XM_ADAS_MODE_BACKUP_RANGING = 0x200,               /* 倒车测距 */
    XM_ADAS_MODE_CMS_RANGING = 0x400,                  /* CMS测距 */
    XM_ADAS_MODE_AUX_RANGING = 0x800,                  /* 辅助测距 */
}XM_ADAS_MODE_E;

/* 算法work输入参数 */
typedef struct
{
    XM_IA_SENSE_E eStopGoSense;                       /* 前车起步灵敏度 */
    XM_IA_SENSE_E eLaneDepartSense;                   /* 车道偏离灵敏度 */
    XM_IA_SENSE_E eCollisonVSense;                    /* 车辆碰撞灵敏度 */
    XM_IA_SENSE_E eCollisonPSense;                    /* 行人碰撞灵敏度 */
    XM_IA_SENSE_E eVBSense;                           /* 虚拟保险杠灵敏度 */
} XM_ADAS_SENSE_S;

typedef struct
{
    XM_ADAS_SENSE_S stVeSense;                /* 算法灵敏度 */
    float flLaneThred;                        /* 压线距离阈值 */
    int iLaneDepartureType;                   /* 0:灵敏度高，车辆靠近车道线开始报警 */
                                              /* 1:灵敏度低，车辆压过车道线开始报警 */
    float fDistanceGround;                    /* 相机与地面的距离(单位mm) */
    float fDistanceLeft;                      /* 相机与左侧车轮的距离(单位mm) */
    float fDistanceRight;                     /* 相机与右侧车轮的距离(单位mm) */
    float fDistanceFront;                     /* 相机与车辆前保险杠的距离(单位mm) */
    float fVanishPointX;                      /* 图像消失点X坐标 */
    float fVanishPointY;                      /* 图像消失点Y坐标 */
    float fLanePointY;                        /* 图像引擎盖处Y坐标/2米处Y坐标 */
    float fVeCollisionSpeed;                  /* 车辆碰撞预警功能开始速度，大于这个速度碰撞预警生效 */
    float fLaneDepartSpeed;                   /* 车道偏离预警开始速度，大于这个速度车道偏离预警生效 */
    float fCollisionTime;                     /* 车辆碰撞预警时间，小于这个时间报警 */
    char acResver[32];
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
    XM_IA_ADAS_CONFIG_S stCamCfg;              /* 辅助驾驶检测配置项 */
    XM_IA_IMG_SIZE_S stDevImgSize;              /* 设备输入分辨率 */
    XM_ADAS_MODE_E eMode;

    /* OUTPUT */
    XM_IA_IMG_SIZE_S stAlgImgSize;              /* Adas算法分辨率 */

    char acResver[64];
} XM_IA_ADAS_INIT_S;

/* 算法work输入参数 */
typedef struct
{
    int iStopFlag;                                                                 /* 车辆是否停止，1表示停止，0表示行驶 */
    int iVeSpeed;                                                                  /* 车辆的行驶速度，GPS没有信号时传-1 */
    unsigned long long liTimeStamp[XM_IA_ADAS_MAX_INPUT_IMAGE_NUM];                /* 设备图像时间戳，单位ms */
    XM_IA_IMAGE_S *pstDevImage[XM_IA_ADAS_MAX_INPUT_IMAGE_NUM];                    /* 设备图像结构体 */
    XM_IA_RECT_S stRoiRect;                                                        /* 检测区域(百分比表示0~8192对应0～100%)  */
    char acResver[52];
} XM_IA_ADAS_IN_S;

/* 车辆结果结构体 */
typedef struct 
{
    int iID;
    float flConfidence;         /* 目标置信度 */
    float flDistX;              /* 目标相对横向距离(mm),负数表示目标在左侧，正数表示目标在右侧 */
    float flDistY;              /* 目标相对纵向距离(mm) */
    int iSpeed;                 /* 交通限速/车辆的相对速度,正数表示相对速度在变大，反之表示相对速度在减少, 单位km/h */
    XM_ADAS_ALARM_TYPE_E eAlarm;/* 预警标识 */
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
    int iLaneNum;
    XM_IA_ADAS_LANE_INFO_S stLaneInfo[XM_IA_ADAS_MAX_LANE_NUM];
} XM_IA_ADAS_LANE_RSLT_S;

typedef struct
{
    int iGoFlag;                   /* 1表示前车起步了，需要报警，0表示不需要报警 */
    int iVirtualBumperFlag;        /* 1触发虚拟保险杠，需要报警，0表示不需要报警 */
    XM_IA_ADAS_OBJ_RSLT_S stObjResult;
    XM_IA_ADAS_LANE_RSLT_S stLaneResult;
    XM_ADAS_ALARM_LIGHT_TYPE_E iGreenFlag;       /* 交通灯报警信号 */
    char acResver[60];
} XM_IA_ADAS_RESULT_S;

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif