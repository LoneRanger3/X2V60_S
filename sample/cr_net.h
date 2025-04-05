/*************************************************************************
 * Copyright (C) 2023 Xmsilicon Tech. Co., Ltd.
 * @File Name: cr_net.h
 * @Description: 
 * @Author: songliuyang
 * @Mail: songliuyang@xmsilicon.cn
 * @Created Time: 2023.08.10
 * @Modification: 
 ************************************************************************/

#ifndef __NET_H__
#define __NET_H__

//#define ETH_STAT_EXIST	(1 << 0)
//#define ETH_STAT_UP		(1 << 1)


/******************************************************************************
 *	函数: LIBCR_NET_CheckEther
 *	描述: 检查网卡状态
 *	参数: ether	[in]	要检查的网卡名称
 *
 *	返回值: 网卡存在返回0, 检查失败返回-1
 *****************************************************************************/
int LIBCR_NET_CheckEther(const char *ether);


#endif
