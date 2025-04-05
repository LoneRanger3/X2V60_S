/*************************************************************************
 * Copyright (C) 2021 Xmsilicon Tech. Co., Ltd.
 * @File Name: io.h
 * @Description: Register read/write demo header
 * @Author: XMsilicon R&D
 * @History: [1]. 2021.8.14 created
 *
 ************************************************************************/

#ifndef __IO_H__
#define __IO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

unsigned int writel(unsigned long phyaddr, unsigned int val);
unsigned int readl(unsigned long phyaddr, unsigned int *val);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
