/******************************************************************************

  Copyright (C), 2015-2025, XM Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_region.h
  Version       : Initial Draft
  Author        : XM multimedia software group
  Created       : 2015/9/17
  Description   : 
******************************************************************************/

#ifndef __MPI_REGION_H__
#define __MPI_REGION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#include "xm_comm_region.h"

XM_S32 XM_MPI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);
XM_S32 XM_MPI_RGN_Destroy(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

XM_S32 XM_MPI_RGN_GetAttr(RGN_HANDLE Handle,RGN_ATTR_S *pstRegion);
XM_S32 XM_MPI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

XM_S32 XM_MPI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);
XM_S32 XM_MPI_RGN_AttachToChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_TYPE_E enType);
XM_S32 XM_MPI_RGN_DetachFromChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_TYPE_E enType);
XM_S32 XM_MPI_RGN_FormatConvert(const RGN_CONVERT_S *stConvert);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __MPI_REGION_H__ */


