#pragma once
#include <stdint.h>
#ifndef WIN32

class RegCtrl
{
public:
	RegCtrl();
	~RegCtrl();

	int VidecDecCtrl(int en);
	
	uint32_t WriteL(uint32_t val, uint32_t phyaddr);
	uint32_t ReadL(uint32_t phyaddr);

private:
	bool video_dec_enable_;

};

#endif //#ifndef WIN32
