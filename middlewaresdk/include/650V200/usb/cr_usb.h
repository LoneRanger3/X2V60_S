/*************************************************************************
 * Copyright (C) 2023 Xmsilicon Tech. Co., Ltd.
 * @File Name: cr_usb.h
 * @Description: 
 * @Author: songliuyang
 * @Mail: songliuyang@xmsilicon.cn
 * @Created Time: 2023.02.09
 * @Modification: 
 ************************************************************************/

#ifndef __CR_USB_H__
#define __CR_USB_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum {
	USB_MODE_WIFI			= 0x0,
	USB_MODE_RNDIS			= 0x1,
	USB_MODE_MASS_STORAGE	= 0x2,
	USB_MODE_NONE,
} USB_MODE_E;

/******************************************************************************
 *	函数: LIBCR_USB_FuncCreate
 *	描述: 创建一个USB功能
 *	参数:	mode    USB模式
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int LIBCR_USB_FuncCreate(USB_MODE_E mode);


/******************************************************************************
 *	函数: LIBCR_USB_FuncDestroy
 *	描述: 销毁一个USB功能
 *	参数:	mode    USB模式
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int LIBCR_USB_FuncDestroy(USB_MODE_E mode);


/******************************************************************************
 *	函数: LIBCR_USB_Detect
 *	描述: 检测USB的连接状态，即USB是否连接了电脑
 *	参数: 无
 *	返回值: 连接电脑时返回1，未连接电脑时返回0.
 *****************************************************************************/
int LIBCR_USB_Detect(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CR_USB_H__ */
