#pragma once
#ifndef WIN32
#include "xm_common.h"
#include "xm_middleware_def.h"

class VideoDecBase
{
public:
    VideoDecBase() {}
    virtual ~VideoDecBase() {}

    virtual int Start(bool vod) {return 0;}
    virtual bool SendFrame(long handle, XM_MW_Media_Frame* media_frame) {return true;}
    virtual void ResetDev() {}
    virtual int Stop() {return 0;}
    virtual void SetWidth(int width, int height) {}
};

class VideoDecUnit : public VideoDecBase
{
public:
    VideoDecUnit(int playback_vpss_grp);
    ~VideoDecUnit();

    virtual int Start(bool vod) override;
    virtual bool SendFrame(long handle, XM_MW_Media_Frame* media_frame) override;
    virtual void ResetDev() override;
    virtual int Stop();
    virtual void SetWidth(int width, int height) override;

protected:
    int StartInternal(int dev_id);
    int StopInternal(int dev_id);

private:
    int width_;
    int height_;
    int playback_vpss_grp_;
    CUR_PLAY_STATUS dec_status_;
};
#endif
