/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.
******************************************************************************/

#ifndef __MPI_BPU_H__
#define __MPI_BPU_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "xm_common.h"
#include "xm_comm_video.h"
#include "xm_comm_vb.h"
#include "xm_comm_bpu.h"

XM_S32 XM_MPI_BPU_SetChnAttr(BPU_CHN BpuChn, const BPU_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_BPU_GetChnAttr(BPU_CHN BpuChn, BPU_CHN_ATTR_S *pstAttr);

XM_S32 XM_MPI_BPU_EnableChn(BPU_CHN BpuChn);
XM_S32 XM_MPI_BPU_DisableChn(BPU_CHN BpuChn);


XM_S32 XM_MPI_BPU_SetFrameDepth(BPU_CHN BpuChn, XM_U32 u32Depth);
XM_S32 XM_MPI_BPU_GetFrameDepth(BPU_CHN BpuChn, XM_U32 *u32Depth);

XM_S32 XM_MPI_BPU_GetFrame(BPU_CHN BpuChn, VIDEO_FRAME_INFO_S *pstFrameInfo, XM_S32 s32MilliSec);
XM_S32 XM_MPI_BPU_ReleaseFrame(BPU_CHN BpuChn, VIDEO_FRAME_INFO_S *pstFrameInfo);

XM_S32 XM_MPI_BPU_Init(void);
XM_S32 XM_MPI_BPU_Bind(MPP_CHN_S *pstSrcChn, MPP_CHN_S *pstDestChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_BPU_H__ */



