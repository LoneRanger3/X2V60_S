/*****************************************************************************

------------------------------------------------------------------------------
                            xm_ia_comm.h
产品名:    智能分析算法
模块名:    公共文件
生成日期:  2018-06-11
作者:     王则浪
文件描述:  智能分析对外公共头文件
*****************************************************************************/
#ifndef _XM_IA_COMM_H_
#define _XM_IA_COMM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* 雄迈智能分析模块错误返回码 */
#define XM_IA_SUCCESS          (0)
#define XM_IA_MALLOC_FAIL      (1)
#define XM_IA_FREE_FAIL        (2)
#define XM_IA_NULL_POINTER     (3)
#define XM_IA_HISI_IVE_FAIL    (4)
#define XM_IA_PARA_FAULT       (5)
#define XM_IA_EXCEPTION        (6)
#define XM_IA_HW_FAULT         (7)

/* 雄迈智能分析模块常量定义 */
#define XM_IA_RELATIVE_COORD_SCALE          (8192)          /* 相对坐标量程 */
#define XM_IA_PATH_MAX_LEN                  (256)           /* 路径最大字符长度(字节) */
#define XM_IA_MAX_VERSION_LEN               (64)

/* 芯片选择 */
typedef enum
{
    /* 雄迈平台 */
    XM_IA_PLATFORM_XM530 = 0,               /* XM530 */
    XM_IA_PLATFORM_XM550 = 1,               /* XM550 */
    XM_IA_PLATFORM_XM580 = 2,               /* XM580 */
    XM_IA_PLATFORM_XM8536 = 3,              /* XM580 NVR后端 */
    XM_IA_PLATFORM_XM8520 = 4,              /* XM580 DVR后端4路 */
    XM_IA_PLATFORM_XM650 = 5,               /* XM650 */
    XM_IA_PLATFORM_XM8521D = 6,             /* XM580 DVR后端8路 */
    XM_IA_PLATFORM_XM8516D = 7,             /* XM580 双摄考勤机 */
    XM_IA_PLATFORM_XM350AI = 8,             /* XM350 AI */
    XM_IA_PLATFORM_XM530V200 = 50,          /* XM530V200 */
    
    XM_IA_PLATFORM_XM580V200 = 70,          /* XM580V200 */
    XM_IA_PLATFORM_XM8520DV200 = 71,        /* XM580V200后端DVR */

    XM_IA_PLATFORM_XM530V300 = 90,          /* XM530V300 */

    /* 海思平台 */
    XM_IA_PLATFORM_HI3516EV200 = 1000,
    XM_IA_PLATFORM_HI3536 = 1001,  
    HI_IA_PLATFORM_31DV200 = 1100,        
    XM_IA_PLATFORM_HI3535A = 1101,        
    XM_IA_PLATFORM_HI3516DV300 = 1102,    
    XM_IA_PLATFORM_HI3516CV500 = 1103,    

    /* NVT平台 */
    XM_IA_PLATFORM_NVT98321 = 2000,         /* NOVA带NPU 32版本 */
    XM_IA_PLATFORM_NVT98336 = 2001,         /* NOVA带NPU 64版本 */
    XM_IA_PLATFORM_NVT98331 = 2002,         /* NOVA带NPU 32版本 */
    XM_IA_PLATFORM_NVT98312 = 2010,         /* NOVA平台 */
    XM_IA_PLATFORM_NVT98562 = 2020,         /* NOVA平台 */
    XM_IA_PLATFORM_NVT98566 = 2030,         /* NOVA带NPU版本 */

    /* MSTAR平台 */
    XM_IA_PLATFORM_SSC337DE = 3000,         /* MSTAR 前端IPC */

    XM_IA_PLATFORM_NUM                      /* 平台数 */

}XM_IA_PLATFORM_E;

/* 推理引擎选择, 再次声明，不能修改，切记切记*/
typedef enum
{
    XM_IA_ARM_INFERENCE_ENGINE = 0x00000000,         /* ARM推理引擎 */
    XM_IA_XMNPU_INFERENCE_ENGINE = 0x00000001,       /* XM580 XMNPU推理引擎 */
    XM_IA_NNIE_INFERENCE_ENGINE_1_2 = 0x00000002,    /* Hisi NNIE1.2版本推理引擎 */
    XM_IA_NNIE_INFERENCE_ENGINE_1_3 = 0x00000003,    /* Hisi NNIE1.3版本推理引擎 */
    XM_IA_NOVATEK_INFERENCE_ENGINE_32X = 0x00000004, /* Novatek 32x engine,加宽8bit */
    XM_IA_NOVATEK_INFERENCE_ENGINE_64X = 0x00000006, /* Novatek 64x engine */
    XM_IA_XMDPU_INFERENCE_ENGINE_1_0 = 0x00000005,   /* XM DPU v1.0推理引擎 V200 */
    XM_IA_XMDPU_INFERENCE_ENGINE_1_1 = 0x00000008,   /* XM DPU v1.1推理引擎 V300 */
    XM_IA_ARM_INFERENCE_ENGINE_1_0 = 0x0000007,      /* ARM推理引擎升级版 */
    XM_IA_INFERENCE_ENGINE_NUM = 16                  /* 当前设计最大支持16种推理引擎 */
}XM_IA_INFERENCE_ENGINE_E;

/* 训练框架 */
typedef enum
{
    XM_IA_CAFFE_FRAMEWORK = 0x00000000,             /* Caffe框架 */
    XM_IA_TENSORFLOW_FRAMEWORK = 0x00000001,        /* TF框架 */
    XM_IA_PYTORCH_FRAMEWORK = 0x00000002,           /* PyTorch框架 */
    XM_IA_TRAIN_FRAMEWORK = 16                      /* 当前设计最大支持16种训练框架 */
}XM_IA_TRAIN_FRAMEWORK_E;

/* 旋转角度 */
typedef enum
{
    XM_IA_ROTATE_NO = 0,                /* 不旋转 */
    XM_IA_ROTATE_CLOCKWISE = 1,         /* 顺时针旋转90度 */
    XM_IA_ROTATE_CLOCKWISE_INVERT = 2   /* 逆时针旋转90度/顺时针旋转270度 */

}XM_IA_ROTATE_E;

/* 目标类型,最低字节保留目标子类型 */
typedef enum
{
    XM_IA_PEDESTRAIN = 0x00,           /* 行人 */
    XM_IA_VEHICLE = 0x10,              /* 汽车,比如0x11可表示成小轿车、0x12可表示成SUV,暂时不用 */
    XM_IA_NONMOTOR = 0x20,             /* 非机动车 */
    
    XM_IA_THINGS = 0x100,              /* 物品:占据0x100～0x1FF */
    XM_IA_THINGS_SEAT_BELT = 0x101,         /* 安全带 */
    XM_IA_THINGS_CIGARETTE = 0x102,         /* 香烟 */
    XM_IA_THINGS_MOBILE_PHONE = 0x103,      /* 手机 */
    XM_IA_THINGS_CUP = 0x104,               /* 水杯 */
    XM_IA_THINGS_IR_BLOCK_GLASS = 0x105,    /* 阻绝红外眼镜 */
    
    XM_IA_UNKOWN_TYPE = -1             /* 未知类型 */

}XM_IA_TARGET_TYPE_E;

/* 图像类型结构体 */
typedef enum
{
    XM_IMG_TYPE_U8C1 = 0x0,         /* 单通道8bit型图像 */
    XM_IMG_TYPE_S8C1 = 0x1,

    XM_IMG_TYPE_U16C1 = 0x2,        /* 单通道16bit型图像 */
    XM_IMG_TYPE_S16C1 = 0x3,

    XM_IMG_TYPE_U32C1 = 0x4,        /* 单通道32bit型图像 */
    XM_IMG_TYPE_S32C1 = 0x5,

    XM_IMG_TYPE_U64C1 = 0x6,        /* 单通道64bit型图像 */
    XM_IMG_TYPE_S64C1 = 0x7,

    XM_IMG_TYPE_U8C3_PKG = 0x8,     /* 三通道8bit Package型图像,BGR序 BGRBGRBGR */
    XM_IMG_TYPE_U8C3_PLN = 0x9,     /* 三通道8bit Planar型图像,BGR序 BBBGGGRRR */

    XM_IMG_TYPE_U16C3_PKG = 0xa,    /* 三通道16bit Package型图像 */
    XM_IMG_TYPE_U16C3_PLN = 0xb,    /* 三通道16bit Planar型图像 */

    /* YUV420 */
    XM_IMG_TYPE_YUV420SP = 0x10,    /* YUV420SP型图像 */
    XM_IMG_TYPE_NV12 = 0x11,        /* NV12型图像 */
    XM_IMG_TYPE_NV21 = 0x12,        /* NV21型图像 */

    XM_IMG_TYPE_YUV422SP = 0x20,    /* YUV422SP型图像 */
    XM_IMG_TYPE_YUV444 = 0x30,      /* YUV444型图像 */

    XM_IMG_TYPE_FREQENCY_DOMAIN = 0x40, /* 单通道float型频域RI图像 */
    
    XM_IMG_TYPE_F32C1 = 0x50,           /* 单通道浮点 */

    XM_IMG_TYPE_UNKNOWN

}XM_IA_IMG_TYPE_E;

/** 点结构定义 */
typedef struct
{
    short s16X;         /**< 点坐标x */
    short s16Y;         /**< 点坐标y */
}XM_IA_POINT_S;

/** 矩形结构定义 */
typedef struct
{
    short s16X1;        /**< 左上角x坐标 */
    short s16Y1;        /**< 左上角y坐标 */
    short s16X2;        /**< 右下角x坐标 */
    short s16Y2;        /**< 右下角y坐标 */
}XM_IA_RECT_S;

/** 坐标系结构定义 */
typedef struct
{
    float fX;           /**< 坐标x */
    float fY;           /**< 坐标y */
    float fZ;           /**< 坐标z */
    char acReserved[16];   /* 保留字节 */

}XM_IA_COORDINATE_FL_S;

/* 图像信息结构体 */
/* 【XM_IA图像规则】 */
/* 规则1：关于不同图像类型所使用的图像数据指针和跨距问题 */
/*       1) 所有类型可适用单个数据指针和跨距,此时默认不同颜色分量接连且跨距相同 */
/*       2) 若需要使用多通道的数据指针和跨距,RGB_PLN 3通道、YUV420SP 3通道、NV12和NV21 2通道其中通道0指向Y数据通道1指向UV数据 */
/* 规则2：关于物理地址 */
/*       1) 无硬件加速的平台,可以使用不带物理地址的系统内存 */
/*       2) 带硬件加速的平台,除虚拟地址需要填写，还需要同时填写32位物理地址和64位物理地址,32位平台填写64位物理地址方法为(unsigned long long)((u32PhyAddr) | (unsigned long long)0)
 */
typedef struct
{
    unsigned short usWidth;        /* 图像宽度 */
    unsigned short usHeight;       /* 图像高度 */
    unsigned short usBitCnt;       /* 像素Bit数 */
    XM_IA_IMG_TYPE_E eImgType;     /* 图像格式 */

    void *pvImgData;               /* 图像数据通道0虚拟地址 */
    void *pvImgData1;              /* 图像数据通道1虚拟地址 */
    void *pvImgData2;              /* 图像数据通道2虚拟地址 */
    unsigned int u32PhyAddr;       /* 图像数据通道0 32位物理地址 */
    unsigned int u32PhyAddr1;      /* 图像数据通道1 32位物理地址 */
    unsigned int u32PhyAddr2;      /* 图像数据通道2 32位物理地址 */
    unsigned short usStride;       /* 图像通道0跨距 */
    unsigned short usStride1;      /* 图像通道1跨距 */
    unsigned short usStride2;      /* 图像通道2跨距 */
    unsigned long long u64PhyAddr;  /* 图像数据通道0 64位物理地址 */
    unsigned long long u64PhyAddr1; /* 图像数据通道1 64位物理地址 */
    unsigned long long u64PhyAddr2; /* 图像数据通道2 64位物理地址 */

    unsigned int uiReserved[16];   /* 保留字节 */
}XM_IA_IMAGE_S;

/* 目标跟踪状态 */
typedef enum
{
    XM_IA_TRACK_LAST,              /* 持续目标 */
    XM_IA_TRACK_LOSS               /* 丢失目标 */

}XM_IA_TRACK_STATUS_E;

/* 算法灵敏度 */
typedef enum
{
    IA_SENSE_LOW,                  /* 低灵敏度 */
    IA_SENSE_MID,                  /* 中灵敏度 */
    IA_SENSE_HIGH,                 /* 高灵敏度 */
    IA_SENSE_NUM,                  /* 灵敏度数量 */
}XM_IA_SENSE_E;

/* 算法图像尺寸握手结构体 */
typedef struct
{
    int iDevImgWid;                 /* 设备图像宽/XM510为算法确认过的图像宽 */
    int iDevImgHgt;                 /* 设备图像高/XM510为算法确认过的图像高 */
    int iAlgImgWid;                 /* 算法图像宽 */
    int iAlgImgHgt;                 /* 算法图像高 */
}XM_IA_IMAGESIZE_EXCHANGE_S;

/* 相对线或框的方向 */
typedef enum
{
    XM_IA_DIRECT_FORWARD,               /* 正向 */
    XM_IA_DIRECT_BACKWARD,              /* 逆向 */
    XM_IA_BIDIRECTION,                  /* 双向 */
    XM_IA_DIRECT_NUM
}XM_IA_DIRECTION_E;

/* 旋转角度 */
typedef struct
{
    int iYaw;                       /* 偏航角(-90, 90)正左转；负右转 */
    int iRoll;                      /* 横滚角(-90, 90)正：左低右高；负：左高右低 */
    int iPitch;                     /* 俯仰角(-90, 90)正：仰视；负：俯视*/

}XM_IA_ANGLE_S;

/* 限制算法资源 */
typedef struct
{
    unsigned int uiMemPoolSizeMax;
    int iDevIdx;                    /* NVT98336主从DevIdx配置 0:主； 1：从 */
    void** ppvHandle;               /* 当前算法句柄 */
    int iMSFlag;                    /* 板子是否含主从功能开关 0：关闭；1：打开*/

    unsigned int uiReserved[61];    /* 保留字节 */
}XM_IA_RESOURCE_LIMIT_S;

/* 算法版本号 */
typedef struct
{
    int iMajorVersion;              /* 主版本号.接口大小变化更新 */
    int iMinorVersion;              /* 子版本号.接口内容有变化更新 */
    int iReversion;                 /* 修改版本.每次版本有修改更新 */
    int iBuild;                     /* 编译版本.暂不用 */

}XM_IA_VERSION_S;

/* 智能IA算法类型 */
typedef enum
{
    /* 人脸算法占用 0～99 */
    XM_FACE_DET_IA = 1,                         /* 人脸检测算法 */
    XM_FACE_DET_REC_IA = 2,                     /* 人脸检测识别算法 */
    XM_FACE_DET_SSH_IA = 3,                     /* 人脸检测SSH算法 */
    XM_FACE_DET_REC_LOW_MEM_IA = 4,             /* 人脸检测识别算法(识别低存储低精度) */
    XM_FACE_REC_IA = 5,                         /* 人脸识别算法 */
    XM_FACE_ALIVE_IR_IA = 6,                    /* 红外人脸活体判断算法 */
    XM_FACE_POINT_IA = 7,                       /* 人脸关键点判断算法 */
    XM_FACE_DET_REC_MASK_IA = 8,                /* 戴口罩人脸检测识别算法 */
    XM_FACE_ALIVE_COLOR_IA = 9,                 /* 彩色人脸活体判断算法 */

    /* 人形算法占用 100～199 */
    XM_PEDETRIAN_DETECT_SSH_RULE_IA = 101,      /* 规则SSH人形检测 */
    XM_PEDETRIAN_FACE_DETECT_RULE_IA = 102,     /* 规则人形人脸检测算法 */
    XM_PEDETRIAN_FACE_DET_REC_RULE_IA = 103,    /* 规则人形人脸检测识别算法 */
    XM_PED_PET_DETECT_RULE_IA = 105,            /* 规则宠物人形检测 */
    /* 车辆算法占用 200～299 */
    XM_PGS_IA = 200,                            /* 停车诱导系统，包含车辆检测、车牌检测、车牌识别 */
    XM_PV_DETECT_SSH_RULE_IA = 201,             /* 规则SSH机非人检测 */

    /* 摇头机算法占用 300~400 */
    XM_SC_IA = 300,
    XM_OBJECT_TRACKING_IA = 301,                /* 目标跟踪算法 */

    /*物品盗遗算法占用400~500*/
    XM_OBJTL_IA = 400,
    //移动侦测算法
    XM_MD_IA = 500,

    /* 检测类NN */
    XM_PED_BIKE_DET_NN_IA = 600,                /* 人非机动车检测NN */

    /* 检测算法 */
    XM_TARGET_DETECT_RULE_IA = 700,             /* 规则目标检测算法 */
    XM_PET_DETECT_IA = 710,                     /* 宠物检测 */
    /* 语音算法 */
    XM_VAD_IA = 800,                            /* 婴儿哭声检测 */
    XM_KEYWORD_REC_IA = 801,                    /* 关键词识别 */

    /* 交通类算法 */
    XM_TRAFFIC_LIGHT_DET_IA = 900,              /* TLD 交通灯检测 */
    XM_TRAFFIC_SIGN_DET_IA = 901,               /* TSD 交通标志检测 */
    XM_LICENSEPLATE_DET_REC_IA = 902,           /* LPDR 车牌检测识别 */
    XM_ROADSIDE_PARKING_IA = 903,               /* RSP 路侧停车检测识别 */
    XM_SIDE_CAR_DET_IA = 904,                   /* 道路车辆检测 */
    XM_OBJECT_DET_IA = 905,                     /* OBJD 目标检测 */
    XM_VEHICLE_TRACKING_IA = 906,               /* VT 目标跟踪 */
    XM_VEHICLE_DET_TRACKING_IA = 907,           /* VDT 道路车辆检测 */
    /* 图像质量判断算法 */
    XM_IQD_IA = 1000,                       /* 判断图像线束数 */

    XM_ADAS_IA = 1100,                       /* 车载辅助驾驶 */

    XM_SDV_IA = 1200,                           /* 立体视觉算法 */

    XM_MAX_SIZE_IA,
}XM_IA_TYPE_E;

typedef XM_IA_RECT_S XM_IA_LINE_S;
typedef XM_IA_POINT_S XM_IA_IMG_SIZE_S;

/*************************************************
Author: WangZelang
Date: 2021-01-27
Description: 雄迈智能算法硬件初始化
*************************************************/
int XM_IA_Init(void);

/*************************************************
Author: WangZelang
Date: 2018-11-28
Description: 雄迈智能算法创建
INPUT:  eIAtype       算法类型
        initParams    初始化参数结构体
        Handle        算法句柄
*************************************************/
int XM_IA_Create(XM_IA_TYPE_E eIAtype, void* initParams, void **Handle);

/*************************************************
Author: WangZelang
Date: 2018-11-28
Description: 雄迈智能算法工作函数
INPUT:  Handle        算法句柄
        inParams      算法工作入参
OUTPUT: Result        算法结果结构体
*************************************************/
int XM_IA_Run(void* Handle, void* inParams, void* Result);

/*************************************************
Author: WangZelang
Date: 2018-11-28
Description: 配置雄迈智能算法
INPUT:  configParams  算法配置结构体
        Handle        算法句柄
*************************************************/
int XM_IA_Configuration(void* configParams, void* Handle);

/*************************************************
Author: WangZelang
Date: 2018-11-28
Description: 销毁雄迈智能算法
INPUT:  Handle        算法句柄
*************************************************/
int XM_IA_Destruction(void **Handle);

/*************************************************
Author: FengShiYi
Date: 2020-09-27
Description: 雄迈智能算法多线程配置函数
INPUT:  ThreadNum        线程数
        eIAType          算法类型
*************************************************/
int XM_IA_ThreadNum_Set(int ThreadNum, XM_IA_TYPE_E eIAType);

/*************************************************
Author: WangZelang
Date: 2020-10-20
Description: 设置算法使用的资源
INPUT:  eIAType             算法类型
        pstResourceLimit    算法资源限制结构体
*************************************************/
int XM_IA_SetResourceLimit(XM_IA_TYPE_E eIAType, XM_IA_RESOURCE_LIMIT_S *pstResourceLimit);

/*************************************************
Author: WangZelang
Date: 2021-01-13
Description: 获取算法版本信息
INPUT:  eIAType             算法类型
OUTPUT: pstVersion          版本信息
*************************************************/
int XM_IA_GetVersion(XM_IA_TYPE_E eIAType, XM_IA_VERSION_S *pstVersion);

/*************************************************
Author: WuChunXuan
Date: 2020-06-30
Description: 释放雄迈智能算法XM_IA_Init创建的资源
*************************************************/
int XM_IA_UnInit(void);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif

