/*
* xm_cv_interface.h
*
*  Created on: 2017年09月14日
*      Author: wangzelang
*/

#ifndef _XM_CV_INTERFACE_H_
#define _XM_CV_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xm_ia_comm.h"
#ifdef _TS_
#define XM_API_CreateImageFuncPtr IA_API_CreateImageFuncPtr
#define XM_API_DestroyImageFuncPtr IA_API_DestroyImageFuncPtr
#define XM_API_Resize IA_API_Resize
#define XM_API_CSC IA_API_CSC
#define XM_API_AffineTransform IA_API_AffineTransform
#define XM_API_CreateMallocImage IA_API_CreateMallocImage
#define XM_API_CreateImage IA_API_CreateImage
#define XM_API_CreateMmzImage IA_API_CreateMmzImage
#define XM_API_CreateMmzImageAlgn IA_API_CreateMmzImageAlgn
#define XM_API_DestroyMmzImage IA_API_DestroyMmzImage
#define XM_API_DestroyMallocImage IA_API_DestroyMallocImage
#define XM_API_DestroyFakeImage IA_API_DestroyFakeImage
#define XM_API_RotateImg IA_API_RotateImg
#define XM_API_WriteBMP IA_API_WriteBMP
#define XM_API_ScaleRect_withoutWHRatioLimit IA_API_ScaleRect_withoutWHRatioLimit
#endif
/* 缩放模式 */
typedef enum tagResizeMode
{
    RESIZE_MODE_NEAR = 0x0,                 /* 最近邻插值算法 */
    RESIZE_MODE_LINEAR = 0x1,               /* 双线性插值算法 */
    RESIZE_MODE_AREA = 0x2,                 /* 区域插值算法 */
    RESIZE_MODE_LADDER = 0x3,               /* 双线性阶梯插值算法 */
    RESIZE_MODE_NEAR_ROI = 0x4,             /* 最近邻插值算法ROI模式 */
    RESIZE_MODE_LINEAR_ROI = 0x5,           /* 双线性插值算法ROI模式 */
    RESIZE_MODE_AREA_ROI = 0x6,             /* 区域性插值算法ROI模式 */
    RESIZE_MODE_LADDER_ROI = 0x7,           /* 双线性阶梯插值算法ROI模式 */
    RESIZE_MODE_HIFI = 0x8,                 /* 高保真缩放算法*/
    RESIZE_MODE_HIFI_ROI = 0x9,             /* 高保真缩放算法ROI模式 */
    RESIZE_MODE_NUM = 0xa

}XM_RESIZE_MODE_E;

/* 图像旋转模式 */
typedef enum
{
    XM_ROTATE_NO,                   /* 不旋转 */
    XM_ROTATE_CLOCKWISE,            /* 顺时针旋转 */
    XM_ROTATE_CLOCKWISE_INVERT,     /* 逆时针旋转 */
    XM_ROTATE_INVERT,               /* 翻转 */
    XM_ROTATE_NUM                   /* 旋转方式数 */
}XM_ROTATE_MODE_E;

/* 颜色空间转换模式 */
typedef enum
{
    CSC_MODE_RGB_pkg2RGB_pln,               /* RGB pkg格式转RGB pln格式 */
    CSC_MODE_RGB_pln2RGB_pkg,               /* RGB pln格式转RGB pkg格式 */
    CSC_MODE_YUV420SP2RGB_pln,              /* YUV420SP格式转RGB planer格式(暂未实现) */
    CSC_MODE_NV12ToRGB_pkg,                 /* YUV420SP格式转RGB package格式 */
    CSC_MODE_NV12ToRGB_pln,                 /* YUV420SP格式转RGB planer格式 */
    CSC_MODE_NV12ToBGR_pln,                 /* YUV420SP格式转BGR planer格式 */
    CSC_MODE_NV21ToRGB_pkg,                 /* NV21格式转RGB package格式 */
    CSC_MODE_NV21ToRGB_pln,                 /* NV21格式转RGB planer格式 */
    CSC_MODE_NV21ToBGR_pln,                 /* NV21格式转BGR planer格式 */
    CSC_MODE_RGB_plnToGray,                 /* BGR planer格式转灰度格式 */
    ROI_CSC_MODE_NV12ToRGB_pkg,             /* ROI NV12格式转RGB package格式 */
    ROI_CSC_MODE_NV12ToRGB_pln,             /* ROI NV12格式转RGB planer格式 */
    ROI_CSC_MODE_NV21ToRGB_pkg,             /* ROI NV21格式转RGB package格式 */
    ROI_CSC_MODE_NV21ToRGB_pln,             /* ROI NV21格式转RGB planer格式 */
    CSC_MODE_BGR_plnToNV12,                 /* BGR planer格式转NV12格式 */
    CSC_MODE_YUV_plnToBGR_pln,             /* YUV planer格式转BGR planer格式 */
    CSC_MODE_NUM

}XM_CSC_MODE_E;

/* 图像DDR通道模式 */
typedef enum 
{
    IMGDDR_MODE_DDR0_DDR0,                  /* 输入图像在DDR0,输出图像在DDR0 */
    IMGDDR_MODE_DDR2_DDR2,                  /* 输入图像在DDR2,输出图像在DDR2 */
}XM_IA_IMGDDR_MODE_E;

/* 拷贝图像API控制结构体 */
typedef struct
{
    XM_IA_RECT_S stRoi;                     /* 感兴趣区域ROI */

} XM_API_COPY_IMG_CTRL_S;

/* 保存图像API控制结构体 */
typedef struct
{
    XM_IA_RECT_S stRoi;                     /* 感兴趣区域ROI */

} XM_API_SAVE_IMG_CTRL_S;

/* 缩放API控制结构体 */
typedef struct tagResizeCtrl
{
    XM_RESIZE_MODE_E eMode;                 /* 缩放模式 */
    XM_IA_RECT_S stRoi;                     /* ROI缩放模式下，感兴趣区域 */
    XM_IA_IMGDDR_MODE_E eDDRMode;           /* 输入输出图像所对应的DDR */

} XM_API_RESIZE_CTRL_S;

/* 图像旋转控制结构体 */
typedef struct
{
    XM_ROTATE_MODE_E eRotateMode;
}XM_API_ROTATE_CTRL_S;

/*人脸放射变换控制结构体*/
typedef struct tagtransformCtrl
{
    float afCoef[9];                        /* 变换参数 */
    XM_IA_RECT_S stRoi;                     /* 输入感兴趣区域 */
    XM_RESIZE_MODE_E eMode;                 /* 缩放模式 */

}XM_API_TRANSFORM_CTRL_S;

/* CSC控制结构体 */
typedef struct tagColorSpaceConvertCtrl
{
    XM_CSC_MODE_E eMode;                    /* 颜色空间转换模式 */
    XM_IA_RECT_S stRoiRect;                 /* 输入感兴趣区域。ROI模式需保证输入ROI的大小与输出图相同 */

} XM_API_CSC_CTRL_S;


/* 创建图像函数的统一指针 */
typedef int(*XM_API_CreateImageFuncPtr)(XM_IA_IMAGE_S*, XM_IA_IMG_TYPE_E, unsigned short, unsigned short);
typedef int(*XM_API_DestroyImageFuncPtr)(XM_IA_IMAGE_S*);

/*************************************************
Author: WangZelang
Date: 2016.10.09
Description: 图像缩放
INPUT:  pstImgI       输入源图像数据
        pstResizeCtrl 缩放控制结构体
OUTPUT: pstImgO       输出缩放图像
*************************************************/
int XM_API_Resize(XM_IA_IMAGE_S *pstImgI, XM_IA_IMAGE_S *pstImgO, XM_API_RESIZE_CTRL_S *pstResizeCtrl);

/*************************************************
Author: WangZelang
Date: 2017.09.18
Description: 图像空间格式转换
INPUT:  pstImgI       输入源图像数据
        pstCscCtrl 缩放控制结构体
OUTPUT: pstImgO       输出转换后图像
*************************************************/
int XM_API_CSC(XM_IA_IMAGE_S *pstImgI, XM_IA_IMAGE_S *pstImgO, XM_API_CSC_CTRL_S *pstCscCtrl);

/*************************************************
Author: WangZelang
Date: 2018.9.30
Description: 图像旋转
INPUT:  pstImgSrc       输入图像
        pstRotateCtrl   旋转模式控制
OUTPUT: pstImgDst       输出图像
*************************************************/
int XM_API_RotateImg(XM_IA_IMAGE_S *pstImgSrc, XM_IA_IMAGE_S *pstImgDst, XM_API_ROTATE_CTRL_S *pstRotateCtrl);

/*************************************************
Author: WangZelang
Date: 2019-02-19
Description: 仿射变换
INPUT:  pstInImg            输入图像
        pstTransformCtrl    变换控制参数
OUTPUT: pstOutImg           输出图像
*************************************************/
int XM_API_AffineTransform(XM_IA_IMAGE_S *pstInImg, XM_API_TRANSFORM_CTRL_S *pstTransformCtrl, XM_IA_IMAGE_S *pstOutImg);

/*************************************************
Author: WangZelang
Date: 2020-07-23
Description: 使用系统malloc创建图像空间
INPUT:  enType          图像数据类型
        u16Width        图像宽
        u16Height       图像高
OUTPUT: pstImg          图像结构体信息
*************************************************/
int XM_API_CreateMallocImage(XM_IA_IMAGE_S *pstImg, XM_IA_IMG_TYPE_E enType, unsigned short u16Width, unsigned short u16Height);

/*************************************************
Author: WangZelang
Date: 2018.5.14
Description: 创建图像空间
INPUT:  enType          图像数据类型
        u16Width        图像宽
        u16Height       图像高
OUTPUT: pstImg          图像结构体信息
*************************************************/
int XM_API_CreateImage(XM_IA_IMAGE_S *pstImg, XM_IA_IMG_TYPE_E enType, unsigned short u16Width, unsigned short u16Height);

/*************************************************
Author: WangZelang
Date: 2020-08-05
Description: 使用MMZ创建图像空间
INPUT:  enType          图像数据类型
        u16Width        图像宽
        u16Height       图像高
OUTPUT: pstImg          图像结构体信息
*************************************************/
int XM_API_CreateMmzImage(XM_IA_IMAGE_S *pstImg, XM_IA_IMG_TYPE_E enType, unsigned short u16Width, unsigned short u16Height);

/*************************************************
Author: WangZelang
Date: 2020-08-15
Description: 使用MMZ创建图像空间,跨距16对齐
INPUT:  enType          图像数据类型
        u16Width        图像宽
        u16Height       图像高
OUTPUT: pstImg          图像结构体信息
*************************************************/
int XM_API_CreateMmzImageAlgn(XM_IA_IMAGE_S *pstImg, XM_IA_IMG_TYPE_E enType, unsigned short u16Width, unsigned short u16Height);

/*************************************************
Author: WangZelang
Date: 2020-08-06
Description: 销毁MMZ创建的图像空间
INPUT:  pstImg          图像结构体信息
OUTPUT: iRet
*************************************************/
int XM_API_DestroyMmzImage(XM_IA_IMAGE_S *pstImg);


/*************************************************
Author: WangZelang
Date: 2020-07-23
Description: 销毁系统malloc图像空间
INPUT:  pstImg          图像结构体信息
OUTPUT: iRet
*************************************************/
int XM_API_DestroyMallocImage(XM_IA_IMAGE_S *pstImg);

/*************************************************
Author: WangZelang
Date: 2022-03-07
Description: 销毁Fake图像空间,实际没有作用
INPUT:  pstImg          图像结构体信息
OUTPUT: iRet
*************************************************/
int XM_API_DestroyFakeImage(XM_IA_IMAGE_S *pstImg);

/*************************************************
Author: Madengpan
Date: 2021_04-25
Description: 将图片打印成 bmp
*************************************************/
int XM_API_WriteBMP(XM_IA_IMAGE_S *pstImage, const char* filename);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif /* _IA_GMM_INTERFACE_H_ */