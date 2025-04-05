#ifndef __MPI_VPSS_H__
#define __MPI_VPSS_H__

#include "xm_common.h"
#include "xm_comm_video.h"
#include "xm_comm_vb.h"
#include "xm_comm_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

XM_S32 XM_MPI_VPSS_CreateGrp(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S* pstGrpAttr);
XM_S32 XM_MPI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);

XM_S32 XM_MPI_VPSS_StartGrp(VPSS_GRP VpssGrp);
XM_S32 XM_MPI_VPSS_StopGrp(VPSS_GRP VpssGrp);

XM_S32 XM_MPI_VPSS_ResetGrp(VPSS_GRP VpssGrp);

XM_S32 XM_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstGrpAttr);
XM_S32 XM_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S* pstGrpAttr);

XM_S32 XM_MPI_VPSS_SetGrpCrop(VPSS_GRP VpssGrp, const VPSS_CROP_INFO_S* pstCropInfo);
XM_S32 XM_MPI_VPSS_GetGrpCrop(VPSS_GRP VpssGrp, VPSS_CROP_INFO_S* pstCropInfo);

XM_S32 XM_MPI_VPSS_SendFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S* pstVideoFrame, XM_S32 s32MilliSec);

/*XM_S32 XM_MPI_VPSS_GetGrpFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe,  VIDEO_FRAME_INFO_S* pstVideoFrame);*/
/*XM_S32 XM_MPI_VPSS_ReleaseGrpFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe, const VIDEO_FRAME_INFO_S* pstVideoFrame);*/

/*XM_S32 XM_MPI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);*/
/*XM_S32 XM_MPI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);*/


/* Chn Settings */
XM_S32 XM_MPI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CHN_ATTR_S* pstChnAttr);
XM_S32 XM_MPI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S* pstChnAttr);

XM_S32 XM_MPI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
XM_S32 XM_MPI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

XM_S32 XM_MPI_VPSS_SetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CROP_INFO_S* pstCropInfo);
XM_S32 XM_MPI_VPSS_GetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CROP_INFO_S* pstCropInfo);

XM_S32 XM_MPI_VPSS_SetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, XM_U32 u32Depth);
XM_S32 XM_MPI_VPSS_GetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, XM_U32 *pu32Depth);
XM_S32 XM_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S* pstVideoFrame, XM_S32 s32MilliSec);
XM_S32 XM_MPI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VIDEO_FRAME_INFO_S* pstVideoFrame);

/* Module Param Settings */
/*XM_S32 XM_MPI_VPSS_SetModParam(const VPSS_MOD_PARAM_S *pstModParam);*/
/*XM_S32 XM_MPI_VPSS_GetModParam(VPSS_MOD_PARAM_S *pstModParam);*/

/*XM_S32 XM_MPI_VPSS_GetChnFd(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);*/
/*XM_S32 XM_MPI_VPSS_CloseFd(XM_VOID);*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */

