/*******************************************************************************
Copyright © XmSilicon Tech Co., Ltd. 2022-2022. All rights reserved.
文件名: setgpio.h
作者@编号： 肖建飞
功能描述: setgpio头文件
******************************************************************************/

#ifndef __SETGPIO_H__
#define __SETGPIO_H__

#define REG_SPACE_SIZE      0x1000
#define GPIO_BASE           0x10020000

#define GPIO_MUX1_EN        (1 << 1)
#define GPIO_MUX2_EN        (1 << 2)
#define GPIO_MUX3_EN        (1 << 3)
#define GPIO_OUT_EN         (1 << 10)
#define GPIO_OUT_HIGH       (1 << 11)
#define GPIO_OUT_LOW        (0 << 11)
#define GPIO_IN_EN          (1 << 12)
#define GPIO_IN_HIGH        (1 << 13)
#define GPIO_IN_LOW         (0 << 13)
#define GPIO_DRIVE_2MA      (0 << 6)
#define GPIO_DRIVE_4MA      (1 << 6)
#define GPIO_DRIVE_8MA      (2 << 6)
#define GPIO_DRIVE_12MA     (3 << 6)

int reg_read(unsigned long phyaddr, unsigned int *val);
int reg_write(unsigned long phyaddr, unsigned int val);
int gpio_write(int gpio, unsigned char value);
int gpio_read(int gpio, unsigned char *Value);

#endif


