#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include "io.h"
#include "pwm.h"


static int g_cur_freq = -1;
static int g_cur_dr = -1;

int SetPwm(int gpio, PWM_PARAM_S* pstParam)
{
	int pwm_id;
	switch (gpio)
	{
	case 8:
		pwm_id = 0;
		break;
	case 9:
		pwm_id = 1;
		break;
	case 86:
		pwm_id = 2;
		break;
	case 87:
		pwm_id = 3;
		break;
	default:
		printf("gpio %d not support PWM.", gpio);
		return -1;
	}

	if (pstParam->dr > 100 || pstParam->dr < 0)
	{
		printf("invalid level %d!\n", pstParam->dr);
		return -1;
	}

	if (pstParam->freq <= (PWM_CLK / COUNT_MAX) || pstParam->freq > PWM_CLK)
	{
		printf("invalid freq %d!\n", pstParam->freq);
		return -1;
	}

	if (pstParam->freq == g_cur_freq && pstParam->dr == g_cur_dr)
	{
		return 0;
	}

	// 周期计数值
	int cycle_cnt = PWM_CLK / pstParam->freq - 1;

	// 翻转计数值
	int rev_cnt = cycle_cnt - (cycle_cnt * pstParam->dr / 100);

	if (pstParam->freq == g_cur_freq && pstParam->dr != g_cur_dr)
	{

		writel(PWM0_REV_COUNT + 4 * pwm_id, rev_cnt);
	}
	else
	{
		writel(PWM0_CYC_COUNT + 4 * pwm_id, cycle_cnt);
		writel(PWM0_REV_COUNT + 4 * pwm_id, rev_cnt);
	}

	printf("PWM[%d], gpio:%d, sample:%d, radio:%d,rev_cnt=%#x\n", pwm_id, gpio, pstParam->freq, pstParam->dr, rev_cnt);
	writel(GPIO_BASE + 4 * gpio, 0x8);	// gpio 复用为pwm
	writel(PWM_MODE, 1 << pwm_id);		// 设置为连续模式
	writel(PWM_EN, 1 << pwm_id);		// 使能

	g_cur_freq = pstParam->freq;
	g_cur_dr = pstParam->dr;

	return 0;
}