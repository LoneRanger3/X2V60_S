/******************************************************************************
 Copyright (C), 2015-2020, XM. Co., Ltd.
******************************************************************************/

#ifndef __XM_DEFINES_H__
#define __XM_DEFINES_H__

#define BITMSK(bit)				(XM_S32)(1 << (bit))
#define MAX(a, b)				((a) > (b) ? (a) : (b))
#define MIN(a, b)				((a) < (b) ? (a) : (b))

#define LINE_LEN_BIT            5
#define LINE_LEN                (1<<LINE_LEN_BIT)
#define LINE_BASE_MASK          (~(LINE_LEN-1))
/* For Sys */
#define DEFAULT_ALIGN    16
#define MAX_MMZ_NAME_LEN 16

#define MAX_NODE_NUM        16

/* For VDA */
#define VDA_MAX_NODE_NUM        32
#define VDA_MAX_INTERNAL        256
#define VDA_CHN_NUM_MAX         32
#define VDA_MAX_WIDTH           960
#define VDA_MAX_HEIGHT          576

/* For VI */
#define VI_MAX_DEV_NUM						4
#define VI_MAX_CHN_NUM						9


/* For VPSS */
#define VPSS_MAX_GRP_NUM      64
#define VPSS_MAX_CHN_NUM	  4


/* For VAR */
#define VAR_MAX_CHN_NUM      64


/* For VO */
#define VO_MAX_DEV_NUM	 	 2
#define VO_MAX_CHN_NUM       32


/* For H264 H265 JPEG */
#define VENC_MAX_CHN_NUM   32
#define VENC_MAX_ROI_NUM   2

/* For Region */
#define OVERLAY_MAX_HANDLE        256
#define COVER_MAX_HANDLE          256
#define LINE_MAX_HANDLE			  256

/* number of channle and device on video input unit of chip
 * Note! VIU_MAX_CHN_NUM is NOT equal to VIU_MAX_DEV_NUM
 * multiplied by VIU_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VIU_MAX_CHN_NUM              3

#define AIO_MAX_CHN_NUM         8		
#define AENC_MAX_CHN_NUM        9	
#define ADEC_MAX_CHN_NUM        3	

#define AI_DEV_MAX_NUM          3		
#define AO_DEV_MIN_NUM          0
#define AO_DEV_MAX_NUM          3		

#endif /* __XM_DEFINES_H__ */

