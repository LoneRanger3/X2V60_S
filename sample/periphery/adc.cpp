/*************************************************************************
 * Copyright (C) 2022 Xmsilicon Tech. Co., Ltd.
 * @File Name: adc_test.c
 * @Description:
 * @Author: songliuyang
 * @Mail: songliuyang@xmsilicon.cn
 * @Created Time: 2022.08.18
 * @Modification:
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "adc.h"


#define ADC_DRV "/dev/adc"
#define ADC_CMD_BASE	0
#define ADC_GET_VALUE	_IOWR(ADC_CMD_BASE, 1, adc_info_s)


int get_adc_val(int id)
{
	static int fd = -1;
	if (fd == -1)
	{
		fd = open(ADC_DRV, O_RDWR);
		if (fd < 0)
		{
			printf("failed to open %s.\n", ADC_DRV);
			return -1;
		}
	}

	adc_info_s adc_info;
	memset(&adc_info, 0, sizeof(adc_info_s));
	adc_info.id = id;
	if (ioctl(fd, ADC_GET_VALUE, &adc_info))
	{
		printf("ioctl failed.\n");
		close(fd);
		return -1;
	}

	return adc_info.value;
}

void usage_exit(char* arg)
{
	printf("usage: %s [id]\n"
		"  id      ADC id.\n", arg);
	exit(1);
}


//int main(int argc, char* argv[])
//{
//	printf("ADC SAMPLE\n");
//	if (argc < 2)
//		usage_exit(argv[0]);
//	int adc_id = atoi(argv[1]);
//	int adc_value;
//
//	while (1)
//	{
//		sleep(1);
//		adc_value = get_adc_val(adc_id);
//		if (adc_value == -1)
//		{
//			printf("Get adc_value failed\n");
//			continue;
//		}
//		else {
//			printf("ADC: %d, value: %#x\n", adc_id, adc_value);
//		}
//	}
//
//	return 0;
//}
