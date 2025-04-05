#ifndef __XM_COMM_VB_H__
#define __XM_COMM_VB_H__

#include "xm_type.h"
#include "xm_errno.h"
#include "xm_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
 
#define VB_MAX_POOLS 32
#define VB_MAX_COMM_POOLS 16
#define VB_MAX_MOD_COMM_POOLS 16

typedef struct xmVB_CONF_S
{
    XM_U32 u32MaxPoolCnt;     /* max count of pools, (0,VB_MAX_POOLS]  */    
    struct xmVB_CPOOL_S
    {
        XM_U32 u32BlkSize;
        XM_U32 u32BlkCnt;
        XM_CHAR acMmzName[MAX_MMZ_NAME_LEN];
    }astCommPool[VB_MAX_COMM_POOLS];
} VB_CONF_S;


#define XM_ERR_VB_NULL_PTR  XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define XM_ERR_VB_NOMEM     XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define XM_ERR_VB_NOBUF     XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define XM_ERR_VB_UNEXIST   XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define XM_ERR_VB_ILLEGAL_PARAM XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define XM_ERR_VB_NOTREADY  XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define XM_ERR_VB_BUSY      XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define XM_ERR_VB_NOT_PERM  XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define XM_ERR_VB_2MPOOLS XM_DEF_ERR(XM_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUTT + 1)

#define XM_TRACE_VB(level,fmt...) XM_TRACE(level, XM_ID_VB,##fmt)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __XM_COMM_VB_H_ */

