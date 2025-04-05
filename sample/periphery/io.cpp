/*************************************************************************
 * Copyright (C) 2021 Xmsilicon Tech. Co., Ltd.
 * @File Name: io.c
 * @Description: Register read/write demo.
 * @Author: XMsilicon R&D
 * @History: [1]. 2021.8.14 created
 *
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "io.h"

#define REG_SPACE_SIZE 0x1000

unsigned int writel(unsigned long phyaddr, unsigned int val)
{
	int fd;
	unsigned long virtaddr;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
	{
		printf("mem dev Open Failed\n");
		return -1;
	}

	virtaddr = (unsigned long)mmap((void*)0, REG_SPACE_SIZE,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, (phyaddr & 0xfffff000));
	if (virtaddr == 0xffffffff)
	{
		printf("Memory Map Failed For Address\n");
		close(fd);
		return -1;
	}

	*((volatile unsigned int*)(virtaddr + (phyaddr & 0x00000fff))) = val;

	munmap((void*)virtaddr, REG_SPACE_SIZE);

	close(fd);
	return 0;
}


unsigned int readl(unsigned long phyaddr, unsigned int* val)
{
	int fd;
	unsigned long virtaddr;
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
	{
		printf("mem dev Open Failed\n");
		return -1;
	}
	virtaddr = (unsigned long)mmap((void*)0, REG_SPACE_SIZE,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, (phyaddr & 0xfffff000));

	if (virtaddr == 0xffffffff)
	{
		printf("Memory Map Failed For Address\n");
		close(fd);
		return -1;
	}
	*val = *((volatile unsigned int*)(virtaddr + (phyaddr & 0x00000fff)));

	munmap((void*)virtaddr, REG_SPACE_SIZE);
	close(fd);
	return 0;
}
