#ifndef __MPI_VAR_H__
#define __MPI_VAR_H__

#include "xm_common.h"
#include "xm_comm_video.h"
#include "xm_comm_vb.h"
#include "xm_comm_var.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

XM_S32 XM_MPI_VAR_Init(void);
XM_S32 XM_MPI_VAR_SetRotate(VAR_CHN VarChn, ROTATE_E enRotate);
XM_S32 XM_MPI_VAR_GetRotate(VAR_CHN VarChn, ROTATE_E* penRotate);
XM_S32 XM_MPI_VAR_EnableChn(VAR_CHN VarChn);
XM_S32 XM_MPI_VAR_DisableChn(VAR_CHN VarChn);
XM_S32 XM_MPI_VAR_Bind(MPP_CHN_S *pstSrcChn, MPP_CHN_S *pstDestChn);
XM_S32 XM_MPI_VAR_SetDepth(VAR_CHN VarChn, XM_U32 u32Depth);
XM_S32 XM_MPI_VAR_GetDepth(VAR_CHN VarChn, XM_U32 *pu32Depth);
XM_S32 XM_MPI_VAR_GetChnFrame(VAR_CHN VarChn, VIDEO_FRAME_INFO_S* pstVideoFrame, XM_S32 s32MilliSec);
XM_S32 XM_MPI_VAR_ReleaseChnFrame(VAR_CHN VarChn, const VIDEO_FRAME_INFO_S* pstVideoFrame);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */

