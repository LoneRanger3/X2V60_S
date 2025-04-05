#include "RegCtrl.h"
#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include "Log.h"
#define REG_SPACE_SIZE			0x1000

RegCtrl::RegCtrl() : video_dec_enable_(false) 
{

}

RegCtrl::~RegCtrl()
{

}


int RegCtrl::VidecDecCtrl(int en)
{
	XMLogI("VidecDecCtrl, en=%d", en);
	if (en == 1) {
		if (video_dec_enable_) {
			return 0;
		}
		WriteL(0x1, 0x20000000);
		WriteL(ReadL(0x20000078) | (1 << 8), 0x20000078);
		WriteL(0x0, 0x20000000);
		usleep(10);
		while (((ReadL(0x20000078) >> 8) & 0x1) != 1);
		video_dec_enable_ = true;
		XMLogI("enable VDEC clock.");
	}
	else {
		if (!video_dec_enable_) {
			return 0;
		}
		WriteL(0x01, 0x20000000);
		WriteL(ReadL(0x20000078) & (~(1 << 8)), 0x20000078);
		WriteL(0x0, 0x20000000);
		usleep(10);
		while (((ReadL(0x20000078) >> 8) & 0x1) != 0);
		video_dec_enable_ = false;
		XMLogI("disable VDEC clock.");
	}

	return 0;
}

uint32_t RegCtrl::WriteL(uint32_t val, uint32_t phyaddr)
{
	int fd;
	unsigned long virtaddr;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
	{
		XMLogE("mem dev Open Failed");
		return -1;
	}

	virtaddr = (unsigned long)mmap((void*)0, REG_SPACE_SIZE,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, (phyaddr & 0xfffff000));
	if (virtaddr == 0xffffffff)
	{
		XMLogE("Memory Map Failed For Address");
		close(fd);
		return -1;
	}

	*((volatile unsigned int*)(virtaddr + (phyaddr & 0x00000fff))) = val;

	munmap((void*)virtaddr, REG_SPACE_SIZE);

	close(fd);
	return 0;
}

uint32_t RegCtrl::ReadL(uint32_t phyaddr)
{
	int fd;
	unsigned long virtaddr;
	unsigned int val = 0;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
	{
		XMLogE("mem dev Open Failed");
		return -1;
	}
	virtaddr = (unsigned long)mmap((void*)0, REG_SPACE_SIZE,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, (phyaddr & 0xfffff000));

	if (virtaddr == 0xffffffff)
	{
		XMLogE("Memory Map Failed For Address");
		close(fd);
		return -1;
	}
	val = *((volatile unsigned int*)(virtaddr + (phyaddr & 0x00000fff)));

	munmap((void*)virtaddr, REG_SPACE_SIZE);
	close(fd);
	return val;
}

#endif //#ifndef WIN32
