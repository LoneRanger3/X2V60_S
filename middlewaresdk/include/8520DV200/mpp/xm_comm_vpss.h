#ifndef __XM_COMM_VPSS_H__
#define __XM_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "xm_type.h"
#include "xm_common.h"
#include "xm_errno.h"
#include "xm_comm_video.h"

#define VPSS_SHARPEN_GAIN_NUM        (32)
#define VPSS_AUTO_ISO_STRENGTH_NUM   (16)
#define VPSS_YUV_SHPLUMA_NUM         (32)
#define VPSS_3DNR_MAX_AUTO_PARAM_NUM (16)

#define XM_TRACE_VPSS(level, fmt...) XM_TRACE(level, XM_ID_VPSS,##fmt)

#define XM_ERR_VPSS_NULL_PTR        XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define XM_ERR_VPSS_NOTREADY        XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define XM_ERR_VPSS_INVALID_DEVID   XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define XM_ERR_VPSS_INVALID_CHNID   XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define XM_ERR_VPSS_EXIST           XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define XM_ERR_VPSS_UNEXIST         XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define XM_ERR_VPSS_NOT_SUPPORT     XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define XM_ERR_VPSS_NOT_PERM        XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define XM_ERR_VPSS_NOMEM           XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define XM_ERR_VPSS_NOBUF           XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define XM_ERR_VPSS_ILLEGAL_PARAM   XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define XM_ERR_VPSS_BUSY            XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define XM_ERR_VPSS_BUF_EMPTY       XM_DEF_ERR(XM_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#define VPSS_INVALID_FRMRATE   -1
#define VPSS_CHN0               0
#define VPSS_CHN1               1
#define VPSS_CHN2               2
#define VPSS_CHN3               3
#define VPSS_INVALID_CHN       -1

typedef XM_U32 VPSS_GRP;
typedef XM_U32 VPSS_CHN;

typedef struct xmVPSS_ISP_COMM_ATTR_S
{
	XM_U8 au8Rsv[4];
} VPSS_ISP_COMM_ATTR_S;


typedef struct xmVPSS_YCNR_ATTR_S
{
	XM_U8 u8Thresh;                            /* RW, Range: [0x0, 0xFF]. Noise reduction effect for high spacial frequencies. */
	XM_U8 u8Str;
	XM_U8 au8Rsv[6];
} VPSS_YCNR_ATTR_S;

typedef struct xmVPSS_SHARPEN_ATTR_S
{
	XM_U8 u8SharpenUd;		// 纹理锐化高频强度
	XM_U8 u8SharpenD; 		// 纹理锐化中频强度
	XM_U8 u8TextureThrUd; 	// 纹理锐化阈值(高频)
	XM_U8 u8TextureThrD; 	// 纹理锐化阈值(中频)
	XM_U8 u8SharpenUd_H;	// 水平锐化高频强度
	XM_U8 u8SharpenD_H; 	// 水平锐化中频强度
	XM_U8 u8TextureThrUd_H; // 水平锐化阈值(高频)
	XM_U8 u8TextureThrD_H; 	// 水平锐化阈值(中频)
	XM_U8 u8SharpenEdge;	// 边界锐化强度
	XM_U8 u8EdgeThr;		// 边界锐化阈值
	XM_U8 u8OverShoot;
	XM_U8 u8UnderShoot;
	XM_U8 u8shootSupStr;	// OverShoot/UnderShoot  抑制强度
	XM_U8 u8DetailCtrl;		// 纹理区Shoot强度控制
 	XM_U8 u8EdgeFiltStr;	// 边缘滤波强度
 	XM_U8 au8Rsv[5];
} VPSS_SHARP_ATTR_S;

typedef struct xmVPSS_3DNR_ATTR_S
{ 
	XM_U8 u8TfStrength;		// Reg0x404 Reg0x408
	XM_U8 u8SfStrength;		// Reg0x394
	XM_U8 u8MdTh_a;			// Reg0x234  4Q4
	XM_U8 u8MdTh_b;			// Reg0x1F0~0x230
	XM_U8 u8IdxMax;			// Reg0x2B0
	XM_U8 u8IdxMin;			// Reg0x2B4
	XM_U8 u8RatC; 			// Reg0x0B0  Reg0x0B4
	XM_U8 au8Rsv[5];
} VPSS_3DNR_ATTR_S;	

typedef struct xmVPSS_DEFOG_ATTR_S
{
	XM_U8 u8Strength;
	XM_U8 au8Rsv[3];
}VPSS_DEFOG_ATTR_S;

typedef struct xmVPSS_CSC_ATTR_S
{
	XM_U8 u8Lum;
	XM_U8 u8Contrast;
	XM_U8 u8Sat;
	XM_U8 u8Hue;
	XM_U8 au8Rsv[4];
}VPSS_CSC_ATTR_S;

typedef struct xmVPSS_ISP_ATTR_S
{
	//Comm
	VPSS_ISP_COMM_ATTR_S stComm;
	//YcNr0
	VPSS_YCNR_ATTR_S stYcNr[2];
	//Sharp0
	VPSS_SHARP_ATTR_S stSharp[2];
	//3DNr
	VPSS_3DNR_ATTR_S st3DNr;
	//Defog
	VPSS_DEFOG_ATTR_S stDefog;
	//CSC
	VPSS_CSC_ATTR_S stCsc;
}VPSS_ISP_ATTR_S;

typedef struct xmVPSS_GRP_ATTR_S
{
    XM_U32                     u32MaxW;           /* RW; Range: Width of source image. */
    XM_U32                     u32MaxH;           /* RW; Range: Height of source image. */
    PIXEL_FORMAT_E             enPixelFormat;     /* RW; Pixel format of source image. */
    XM_BOOL                    b3DNrEn;             /* RW; NR enable. */
	XM_BOOL					   bYcNrEn[2];
	XM_BOOL					   bSharpEn[2];
	VPSS_ISP_ATTR_S            stIspAttr;
} VPSS_GRP_ATTR_S;

typedef enum xmVPSS_CHN_MODE_E
{
    VPSS_CHN_MODE_USER  = 0,       /* User mode. */
    VPSS_CHN_MODE_AUTO  = 1        /* Auto mode. */
} VPSS_CHN_MODE_E;

typedef struct xmVPSS_CHN_ATTR_S
{
    VPSS_CHN_MODE_E     enChnMode;          /* RW; Vpss channel's work mode. */
    XM_U32              u32Width;           /* RW; Width of target image. */
    XM_U32              u32Height;          /* RW; Height of target image. */
    PIXEL_FORMAT_E      enPixelFormat;      /* RW; Pixel format of target image. */
    FRAME_RATE_CTRL_S   stFrameRate;        /* Frame rate control info */
    XM_BOOL             bMirror;            /* RW; Mirror enable. */
    XM_BOOL             bFlip;              /* RW; Flip enable. */
    XM_U32              u32Depth;           /* RW; Range: [0, 8]; User get list depth. */
} VPSS_CHN_ATTR_S;

typedef struct xmVPSS_CROP_INFO_S
{
	XM_BOOL bEnable; /* Crop region enable */
	RECT_S stRect; /* Crop region, note: s32X must be multi of 16 */
}VPSS_CROP_INFO_S;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __XM_COMM_VPSS_H__ */

