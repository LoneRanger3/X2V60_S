#pragma once
#ifndef WIN32
#include <pthread.h>
#include "AdBase.h"
#include "MediaFormat.h"
#include "xm_middleware_def.h"

class DvrAD : public AdBase
{
public:
    DvrAD();
    ~DvrAD();

    virtual int Init(int channel_num) override;
    virtual int Start() override;
    virtual int Stop() override;
    virtual int Clean() override;
    virtual int GetCapSolution(int channel, int& width, int& height) override;

    int ADProc();

private:
    uint8_t video_format_[XM_MAX_CHANNEL_NUM];
    MEDIA_FORMATE_S ad_media_format_[256];
    pthread_t thread_ad_handle_;
    bool thread_ad_init_;
	bool thread_ad_exit_;
    int channel_num_;
};

#endif