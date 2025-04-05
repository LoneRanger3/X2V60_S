/*************************************************************************
 * Copyright (C) 2024 Xmsilicon Tech. Co., Ltd.
 * @File Name: uvc.h
 * @Description: 
 * @Author: songliuyang
 * @Mail: songliuyang@xmsilicon.cn
 * @Created Time: 2024.07.18
 * @Modification: 
 ************************************************************************/

#ifndef __UVC_H__
#define __UVC_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#define VIDEO_SHM_FIFO_KEY		1234
#define VIDEO_SHM_FIFO_DEPTH	4
#define VIDEO_SHM_NODE_SIZE		(1024 * 1024)

typedef void XMUVC_Handle_t;

typedef struct jpeg_stream
{
	int size;
	unsigned char data[VIDEO_SHM_NODE_SIZE];
} XMUVC_Stream_t;

/*****************************************************************************
 * Function: XMUVC_Create
 * Description: Create XMUVC
 * Parameter: none
 *
 * Return: Retuen A pointer on success, Otherwise, NULL is returned. 
 *****************************************************************************/
XMUVC_Handle_t* XMUVC_Create(void);

/*****************************************************************************
 * Function: XMUVC_Send
 * Description: Create XMUVC
 * Parameter:	@handle		XMUVC handle
 *				@stream		Video stream
 *
 * Return: Retuen A pointer on success, Otherwise, NULL is returned. 
 *****************************************************************************/
int XMUVC_Send(XMUVC_Handle_t *handle, XMUVC_Stream_t *stream);

/*****************************************************************************
 * Function: XMUVC_Destroy
 * Description: Destroy XMUVC
 * Parameter: none
 *
 * Return: Retuen 0 on success, Otherwise, -1 is returned. 
 *****************************************************************************/
int XMUVC_Destroy(XMUVC_Handle_t *handle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
