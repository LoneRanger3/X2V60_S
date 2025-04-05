/*************************************************************************
 * Copyright (C) 2021 Xmsilicon Tech. Co., Ltd.
 * @File Name: pwm.h
 * @Description: PWM output test demo header
 * @Author: XMsilicon R&D
 * @History: [1]. 2021.8.14 created
 *
 ************************************************************************/

#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define GPIO_BASE                   0x10020000
#define PWM_BASE                    (0x10024000)
#define PWM_EN						(PWM_BASE)
#define PWM_POL						(PWM_BASE + 0x4)
#define PWM_MODE					(PWM_BASE + 0x8)
#define PWM0_OUT_COUNT				(PWM_BASE + 0xC)
#define PWM1_OUT_COUNT				(PWM_BASE + 0x10)
#define PWM2_OUT_COUNT				(PWM_BASE + 0x14)
#define PWM3_OUT_COUNT				(PWM_BASE + 0x18)
#define PWM0_CYC_COUNT				(PWM_BASE + 0x1C)
#define PWM1_CYC_COUNT				(PWM_BASE + 0x20)
#define PWM2_CYC_COUNT				(PWM_BASE + 0x24)
#define PWM3_CYC_COUNT				(PWM_BASE + 0x28)
#define PWM0_REV_COUNT				(PWM_BASE + 0x2C)
#define PWM1_REV_COUNT				(PWM_BASE + 0x30)
#define PWM2_REV_COUNT				(PWM_BASE + 0x34)
#define PWM3_REV_COUNT				(PWM_BASE + 0x38)

#define PWM_CLK  24000000
#define COUNT_MAX 0xffffff

// PWM signal paramater
typedef struct {
	unsigned int dr;		// Duty rate, 0~100
	unsigned int freq;		// Frequency, 12000000/65536 ~ 12000000/100
}PWM_PARAM_S;


int set_screen_luminance(int gpio, int luminance);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
