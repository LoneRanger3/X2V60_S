/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_venc.h
  Version       : Initial Draft
  Author        : Xm MPP Team
  Created       : 2015/8/21
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/

#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "xm_common.h"
#include "xm_comm_video.h"
#include "xm_comm_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

XM_S32 XM_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_VENC_DestroyChn(VENC_CHN VeChn);

XM_S32 XM_MPI_VENC_StartRecvPic(VENC_CHN VeChn);
XM_S32 XM_MPI_VENC_StopRecvPic(VENC_CHN VeChn);

XM_S32 XM_MPI_VENC_SetChnAttr( VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
XM_S32 XM_MPI_VENC_GetChnAttr( VENC_CHN VeChn, VENC_CHN_ATTR_S *pstAttr);

XM_S32 XM_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, XM_BOOL bBlockFlag);
XM_S32 XM_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);

XM_S32 XM_MPI_VENC_RequestIDR(VENC_CHN VeChn);

XM_S32 XM_MPI_VENC_GetRefParamEx(VENC_CHN VeChn,VENC_PARAM_REF_EX_S *pstRefParam);

XM_S32 XM_MPI_VENC_SetRefParamEx(VENC_CHN VeChn,VENC_PARAM_REF_EX_S *pstRefParam);

XM_S32 XM_MPI_VENC_SelectData(void);
XM_S32 XM_MPI_VENC_EncodeJpeg(VIDEO_FRAME_INFO_S const *pframe, JPEG_ENC_ATTR_S *pstAttr, VENC_STREAM_S *pstStream);
XM_S32 XM_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_PARAM_JPEG_S *pstJpegParam);
XM_S32 XM_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_PARAM_JPEG_S *pstJpegParam);
XM_S32 XM_MPI_VENC_SendFrame(VENC_CHN VeChn, VIDEO_FRAME_INFO_S *pstFrame, XM_S32 s32MilliSec);
XM_S32 XM_MPI_VENC_SetRotation(VENC_CHN VeChn, ROTATE_E enRotate);

XM_S32 XM_MPI_VENC_SetRoiAttr(VENC_CHN VeChn, const VENC_ROI_ATTR_S *pstRoiAttr);
XM_S32 XM_MPI_VENC_GetRoiAttr(VENC_CHN VeChn, XM_U32 index, VENC_ROI_ATTR_S *pstRoiAttr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VENC_H__ */
