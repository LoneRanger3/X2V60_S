/*************************************************************************
 * Copyright (C) 2021 Xmsilicon Tech. Co., Ltd.
 * @File Name: wdt.c
 * @Description: Watchdog usage demo.
 * @Author:XMsilicon R&D
 * @History: [1]. 2021.8.14 created
 *
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define WDT_DEV		"/dev/wdt"

#define WDT_IOCTL_BASE			'W'
#define CMD_WDT_START			_IO(WDT_IOCTL_BASE, 0)
#define CMD_WDT_SETTIMEOUT		_IOW(WDT_IOCTL_BASE, 1, unsigned int)
#define CMD_WDT_FEED			_IO(WDT_IOCTL_BASE, 2)
#define CMD_WDT_STOP			_IO(WDT_IOCTL_BASE, 3)

static int wdt_fd = -1;

int WdtCreate(void)
{
	int timeout;
	int ret;

	if (wdt_fd > 0)
	{
		return 0;
	}

	wdt_fd = open(WDT_DEV, O_RDWR);
	if (wdt_fd < 0)
	{
		perror("open watchdog");
		return -1;
	}

	ret = ioctl(wdt_fd, CMD_WDT_START);
	if (ret < 0)
	{
		printf("Watchdog start failed\n");
		return -1;
	}
	timeout = 60;

	ret = ioctl(wdt_fd, CMD_WDT_SETTIMEOUT, &timeout);
	if (ret < 0)
	{
		printf("Watchdog settimeout failed\n");
		return -1;
	}
	return 0;
}

int WdtSetTimeout(unsigned int timeout)
{
	int ret;
	ret = ioctl(wdt_fd, CMD_WDT_SETTIMEOUT, &timeout);
	if (ret < 0)
	{
		printf("Watchdog settimeout failed\n");
		return -1;
	}
	return 0;
}

int WdtDestory(void)
{
	int ret;
	if (wdt_fd > 0)
	{
		ret = ioctl(wdt_fd, CMD_WDT_STOP);
		if (ret < 0)
		{
			printf("Watchdog Feed Failed\n");
			return -1;
		}
		close(wdt_fd);
	}
	wdt_fd = -1;
	return 0;
}

int WdtFeed(void)
{
	int ret = 0;

	if (wdt_fd < -1)
	{
		printf("Watchdog Not Created\n");
		return -1;
	}

	ret = ioctl(wdt_fd, CMD_WDT_FEED);
	if (ret < 0)
	{
		printf("Watchdog Feed Failed\n");
		return -1;
	}

	return 0;
}

//int main(int argc, char* argv[])
//{
//	int ret;
//	ret = WdtCreate();
//	if (ret != 0)
//	{
//		printf("Watchdog create failed.\n");
//		return -1;
//	}
//	printf("Watchdog create.\n");
//
//	unsigned int timeout = 30;
//	ret = WdtSetTimeout(timeout);
//	if (ret != 0)
//	{
//		printf("Watchdog set timeout failed.\n");
//		goto END;
//	}
//	printf("Set timeout 30s.\n");
//
//	while (1)
//	{
//		ret = WdtFeed();
//		if (ret != 0)
//		{
//			printf("Watchdog feed failed.\n");
//			break;
//		}
//		printf("feed.\n");
//		sleep(5);
//	}
//
//END:
//	WdtDestory();
//
//	return 0;
//}
