#ifndef __ISP_COMMON_H__
#define __ISP_COMMON_H__

#include "isp_type.h"




typedef struct stISP_SIZE_S
{
    ISP_U32 u32Width;
    ISP_U32 u32Height;
}ISP_SIZE_S;

typedef struct stISP_RECT_S
{
    ISP_S32 s32X;
    ISP_S32 s32Y;
    ISP_U32 u32Width;
    ISP_U32 u32Height;
}ISP_RECT_S;



typedef ISP_S32 AI_CHN;
typedef ISP_S32 AO_CHN;
typedef ISP_S32 AENC_CHN;
typedef ISP_S32 ADEC_CHN;
typedef ISP_S32 AUDIO_DEV;
typedef ISP_S32 AVENC_CHN;
typedef ISP_S32 VI_DEV;
typedef ISP_S32 VI_WAY;
typedef ISP_S32 VI_CHN;
typedef ISP_S32 VO_DEV;
typedef ISP_S32 VO_LAYER;
typedef ISP_S32 VO_CHN;
typedef ISP_S32 VENC_CHN;
typedef ISP_S32 VDEC_CHN;
typedef ISP_S32 VENC_GRP;
typedef ISP_S32 VO_GRP;
typedef ISP_S32 VDA_CHN;
typedef ISP_S32 IVE_HANDLE;
typedef ISP_S32 ISP_DEV;
typedef ISP_S32 SENSOR_ID;
typedef ISP_S32 MIPI_CHN;
typedef ISP_U32 VPSS_GRP;


typedef enum stISP_MOD_ID_E
{
    MOD_ID_CMPI    = 0,
    MOD_ID_VB      = 1,
    MOD_ID_SYS     = 2,
    MOD_ID_VALG    = 3,

    MOD_ID_CHNL    = 4,   
    MOD_ID_VDEC    = 5,
    MOD_ID_GROUP   = 6,
    MOD_ID_VENC    = 7,
    MOD_ID_VPSS    = 8,
    MOD_ID_VDA     = 9,
    
    MOD_ID_H264E   = 10,
    MOD_ID_JPEGE   = 11,
    MOD_ID_MPEG4E  = 12,
    MOD_ID_H264D   = 13,
    MOD_ID_JPEGD   = 14,
    MOD_ID_VOU     = 15,

    MOD_ID_VIU     = 16,
    MOD_ID_DSU     = 17,
    MOD_ID_RGN	  = 18,
    MOD_ID_RC      = 19,

    MOD_ID_SIO     = 20,
    MOD_ID_AI      = 21,
    MOD_ID_AO      = 22,
    MOD_ID_AENC    = 23,
    MOD_ID_ADEC    = 24,

    MOD_ID_AVENC   = 25,

    MOD_ID_PCIV    = 26,
    MOD_ID_PCIVFMW = 27,

    MOD_ID_ISP	  = 28,

    MOD_ID_IVE	  = 29,
    /* there is a hole */    

    MOD_ID_DCCM    = 31,
    MOD_ID_DCCS    = 32,

    MOD_ID_PROC    = 33,
    MOD_ID_LOG     = 34,
    MOD_ID_MST_LOG = 35,
    MOD_ID_VD      = 36,

    MOD_ID_VCMP    = 38,
    MOD_ID_FB      = 39,

    
    MOD_ID_HDMI    = 40,
    MOD_ID_VOIE    = 41,
    MOD_ID_TDE     = 42,

    MOD_ID_USR     = 43,

    MOD_ID_VEDU   = 44,
    
    MOD_ID_BUTT,
} ISP_MOD_ID_E;

typedef struct stISP_CHN_S
{
    ISP_MOD_ID_E    enModId;
    ISP_S32      s32DevId;
    ISP_S32      s32ChnId;
} ISP_CHN_S;


#endif  /* __ISP_COMMON_H_ */

