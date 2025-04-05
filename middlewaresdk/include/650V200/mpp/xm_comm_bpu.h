/******************************************************************************

Copyright (C), 2004-2025, XM Tech. Co., Ltd.

******************************************************************************/

#ifndef __XM_COMM_BPU_H__
#define __XM_COMM_BPU_H__

#include "xm_common.h"
#include "xm_errno.h"
#include "xm_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

typedef XM_S32 BPU_CHN;

/* the attributes of a  channel */
typedef struct xmBpu_CHN_ATTR_S
{
    RECT_S          stCapRect;          /* the capture rect (corresponding to the size of the picture captured by a VI device).
                                                For primary channels, the stCapRect's u32Width and u32Height are static attributes. That is,
                                                the value of them can be changed only after primary and secondary channels are disabled.
                                                For secondary channels, stCapRect is an invalid attribute */
    SIZE_S          stDestSize;         /* Target picture size.
                                                For primary channels, stDestSize must be equal to stCapRect's u32Width and u32Height,
                                                because primary channel doesn't have scale capability. Additionally, it is a static
                                                attribute, That is, the value of stDestSize can be changed only after primary and
                                                secondary channels are disabled.
                                                For secondary channels, stDestSize is a dynamic attribute */

    PIXEL_FORMAT_E  enPixFormat;        /* Pixel storage format. Only the formats semi-planar420 and semi-planar422 are supported */
    XM_BOOL         bMirror;            /* Whether to mirror */
    XM_BOOL         bFlip;              /* Whether to flip */
    XM_BOOL         bChromaResample;    /* Whether to perform chrominance resampling. It is valid only for primary channels */
    XM_S32          s32SrcFrameRate;    /* Source frame rate. The value -1 indicates that the frame rate is not controlled */
    XM_S32          s32FrameRate;       /* Target frame rate. The value -1 indicates that the frame rate is not controlled */
} BPU_CHN_ATTR_S;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef__XM_COMM_VIDEO_IN_H__ */


