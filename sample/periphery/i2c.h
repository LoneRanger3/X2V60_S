
/*************************************************************************
* Copyright (C) 2022 Xmsilicon Tech. Co., Ltd.
* @File Name: i2c.h
* @Description:
* @Author: songliuyang
* @Mail: songliuyang@xmsilicon.cn
* @Created Time: 2022.11.24
* @Modification:
************************************************************************/

#ifndef __IIC_H__
#define __IIC_H__

#define CMD_I2C_WRITE		0x01
#define CMD_I2C_READ		0x03

typedef struct i2c_data
{
	unsigned char	dev_addr;
	unsigned int	reg_addr;
	unsigned int	addr_byte_num;
	unsigned int	data;
	unsigned int	data_byte_num;
} i2c_data_s;


int da380_i2c_write(unsigned char device_addr, unsigned char reg_addr, unsigned char reg_val);
unsigned char da380_i2c_read(unsigned char device_addr, unsigned char reg_addr);

/*int pthread_mutex_lock_i2c(unsigned char i2c_lock);
int pthread_mutex_unlock_i2c(unsigned char i2c_lock);*/

#endif /* __IIC_H__ */
