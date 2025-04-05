/*************************************************************************
 * Copyright (C) 2021 Xmsilicon Tech. Co., Ltd.
 * @File Name: uart.h
 * @Description: 
 * @Author: songliuyang
 * @Mail: songliuyang@xmsilicon.cn
 * @Created Time: 2021.12.13
 * @Modification: 
 ************************************************************************/

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/// 串口属性结构
typedef struct COMM_ATTR
 {
	unsigned int	baudrate;	///< 实际的波特率值。
	unsigned char	databits;	///< 实际的数据位数。
	unsigned char	parity;		///< 奇偶校验选项，取comm_parity_t类型的枚举值。
	unsigned char	stopbits;	///< 停止位数，取comm_stopbits_t类型的枚举值。
	unsigned char	reserved;	///< 保留	
} COMM_ATTR;

/// 串口停止位类型
enum comm_stopbits_t 
{
	COMM_ONESTOPBIT,	///< 1 stop bit
	COMM_ONE5STOPBITS,	///< 1.5 stop bit
	COMM_TWOSTOPBITS	///< 2 stop bit
};

/// 串口校验位类型
enum comm_parity_t 
{
	COMM_NOPARITY,	///< No parity
	COMM_ODDPARITY,	///< Odd parity
	COMM_EVENPARITY,///< Even parity
	COMM_MARK,		///< 
	COMM_SPACE		///< 
};


int UartOpen(int index);

int UartInit(int id);

int UartSetAttribute(int index, COMM_ATTR *pattr);

int UartRead(int index, void *pdata, unsigned int nbytes);

int UartWrite(int index, void *pdata, unsigned int len);

int Uart1RecvData(unsigned char* pData, unsigned int maxlen);

int Uart1SendData(unsigned char* pData, unsigned int datalen);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
