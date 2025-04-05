/******************************************************************************
******************************************************************************/
#ifndef __MPI_VB_H__
#define __MPI_VB_H__

#include "xm_comm_vb.h"
#include "xm_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


XM_S32 XM_MPI_VB_Init(XM_VOID);
XM_S32 XM_MPI_VB_Exit(XM_VOID);
XM_S32 XM_MPI_VB_SetConf(const VB_CONF_S *pstVbConf);
XM_S32 XM_MPI_VB_GetConf(VB_CONF_S *pstVbConf);
XM_S32 XM_MPI_VB_GetFrame(XM_U32 u32Size, VIDEO_FRAME_INFO_S *pstFrameInfo, XM_S32 s32MilliSec);
XM_S32 XM_MPI_VB_ReleaseFrame(VIDEO_FRAME_INFO_S *pstFrameInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_VB_H__ */

