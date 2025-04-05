/*****************************************************************************

------------------------------------------------------------------------------
xm_ia_tc_interface.h
产品名:    目标分类算法
模块名:    目标分类算法
生成日期:   2023-05-29
作者:      冯世益
文件描述:   目标分类算法对外头文件
版本号：    V0.0.1

算法日志开关及含义：IA_LOG:XXXXX+1->Flag
Flag = 00000001     保存分类检测送检图像->./pic

更新：

*****************************************************************************/
#ifndef _XM_IA_TC_INTERFACE_H_
#define _XM_IA_TC_INTERFACE_H_

#include "xm_ia_comm.h"

#define TC_MAJOR_VERSION                (0)             /* 目标检测算法主版本号 */
#define TC_MINOR_VERSION                (0)             /* 目标检测算法次版本号 */
#define TC_REVERSION                    (1)             /* 目标检测算法修改版本号 */
#define TC_BUILD                        (1)             /* 目标检测算法编译版本号 */

/* 目标检测类型 */
typedef enum
{
    XM_IA_TC_FIGHT = 10,                        /* 目标打架分类 */
    XM_IA_TC_FACECLS = 50,                      /* 人脸分类 */
    XM_IA_TC_OBJ_10 = 100,                      /* 10类目标分类 airplane、automobile、bird、cat、deer、dog、frog、horse、ship、truck*/
    XM_IA_TC_NUM                                /* 目标分类算法模型个数 */
}XM_IA_TC_TYPE_E;

/* 打架结果类型 */
typedef enum
{
    XM_IA_TC_NOFIGHT_E = 0,                     /* 目标未打架 */
    XM_IA_TC_FIGHT_E = 1,                       /* 目标打架 */
}XM_IA_TC_FIGHTRELT_E;

/* 笑脸结果类型 */
typedef enum
{
    XM_IA_TC_NORMALFACE_E = 0,                  /* 正常人脸 */
    XM_IA_TC_SMILINGFACE_E = 1,                 /* 笑脸 */
}XM_IA_TC_FACECLS_E;

/* 打架结果类型 */
typedef enum
{
    XM_IA_TC_airplane_E = 0,                    
    XM_IA_TC_automobile_E = 1,    
    XM_IA_TC_bird_E = 2,
    XM_IA_TC_cat_E = 3,
    XM_IA_TC_deer_E = 4,
    XM_IA_TC_dog_E = 5,
    XM_IA_TC_frog_E = 6,
    XM_IA_TC_horse_E = 7,
    XM_IA_TC_ship_E = 8,
    XM_IA_TC_truck_E = 9,
}XM_IA_TC_OBJRELT_E;

/* 目标分类算法配置项结构体 */
typedef struct
{
    XM_IA_TC_TYPE_E iDetectType;                /* 目标分类类型(不同类型对应不同分类器) */
    XM_IA_SENSE_E eSensitivity;                 /* 目标检测灵敏度 */
    XM_IA_RECT_S stDetRegion;                   /* 识别区域 */
    unsigned char ucReserved[112];              /* 预留位 */
}XM_IA_TC_CONFIG_S;
//TD
/* 目标分类算法初始化结构体 */
typedef struct
{
    /* INPUT */
    unsigned int uiInStructSize;                /* Run入参参数结构体（XM_IA_TC_IN_S）检查Size */
    unsigned int uiInitStructSize;              /* 初始化参数结构体(XM_IA_TC_INIT_S)检查Size */
    unsigned int uiConfigStructSize;            /* 配置参数结构体(XM_IA_TC_CONFIG_S)检查Size */
    unsigned int uiRsltStructSize;              /* 算法结果参数结构体(XM_IA_TC_RESULT_S)检查Size */

    char acClassBinPath[XM_IA_PATH_MAX_LEN];    /* 分类器文件(.xmnn, .wk,.bin等)路径 */
    XM_IA_PLATFORM_E ePlatForm;                 /* 芯片平台选择 */
    XM_IA_IMG_TYPE_E eDevImgType;               /* 设备图像类型 */
    XM_IA_IMG_SIZE_S stDevImgSize;              /* 设备图像尺寸 */
    XM_IA_TC_CONFIG_S stTcCfg;                  /* 目标检测配置项 */

    /* OUTPUT */
    XM_IA_IMG_SIZE_S stAlgImgSize;              /* 算法图像尺寸 */

    unsigned char ucReserved[128];              /* 预留位 */
}XM_IA_TC_INIT_S;

/* 目标分类算法work输入参数 */
typedef struct
{
    XM_IA_IMAGE_S *pstImage;                    /* 图像结构体 */
    unsigned char ucReserved[124];              /* 预留位 */
}XM_IA_TC_IN_S;

/* 规则目标检测算法结果 */
typedef struct
{
    union
    {
        XM_IA_TC_FIGHTRELT_E eFightStatus;      /* 打架状态 */
        XM_IA_TC_FACECLS_E eFaceClass;          /* 人脸类型 */
        XM_IA_TC_OBJRELT_E eObjClass;           /* 目标类型 */
    }unTcStatus;
    float fScore;                               /* 打架得分，大于0.5即为打架 */
    unsigned char ucReserved[124];              /* 预留位 */
}XM_IA_TC_RESULT_S;

#endif