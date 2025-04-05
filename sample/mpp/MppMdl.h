/******************************************************************************
  Copyright (C), 2020, xmsilicon Tech. Co., Ltd.
 ******************************************************************************
  File Name     : MppMdl.cpp
  Author        : xmsilicon R&D
  Description   : mpp功能封装
  History       :
******************************************************************************/
#pragma once

#include "xm_middleware_def.h"
#include<deque>

#ifndef WIN32
#include <mutex>
#include <string>
#include <pthread.h>
#include "xm_common.h"
#include "xm_comm_video.h"
#include "xm_comm_vo.h"
#include "xm_comm_venc.h"
#include "xm_comm_aio.h"
#include "xm_ia_comm.h"
#include "xm_ia_td_interface.h"
#include "xm_middleware_def.h"
#include "mpi_region.h"
#include "XMThread.h"
#include "VideoDecUnit.h"

const int kOSDWidth = 32;
const int kOSDHeight = 64;
const int kMaxTargetNum = 16;
const int kThumbnailWidth = 320;
const int kThumbnailHeight = 180;
const int kMaxOSDNumCount = 21;

#define ALIGN 8
#define ALIGN8(x) ((x)/ALIGN*ALIGN)			/* 8像素向下对齐 */
#define ALIGNUP8(x) (ALIGN8(x + ALIGN - 1))	/* 8像素向上对齐 */

struct BSDTargetInfo {
    char rgn_created[kMaxTargetNum];
    char video_cover_attached[kMaxTargetNum];
    int64_t last_modify_time;
    int target_num_last;
};

struct SystemTime {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    SystemTime() {
        year = 0;
        month = 0;
        day = 0;
        hour = 0;
        min = 0;
        sec = 0;
    }

    int compare(SystemTime& other) {
        if (year != other.year ||
            month != other.month ||
            day != other.day ||
            hour != other.hour ||
            min != other.min ||
            sec != other.sec) {
                return 1;
            }
        return 0;    
    }
};

enum PlayLogo
{
    PlayLogo_Start,
    PlayLogo_Shutdown,
};

//缩略图状态机
enum ThumbnailStatus
{
    Thumbnail_None,
    Thumbnail_VI,       //待获取vi数据
    Thumbnail_PNG,      //已获取vi数据，待回调出去封装成png
    Thumbnail_AI,
};

enum VBStatus
{
    VB_NONE = 0,
    VB_4K_1080,
    VB_2K_1080,
    VB_1080_1080,
};

class AdBase;
class MppMdl
{
public:
    static MppMdl* Instance();
	static void Uninstance();

   	/*---------------对外接口--------------------*/
    int SetChannelNum(int channel_num) { channel_num_ = channel_num; }
    int SetEncInfo(int channel, int stream, const XM_MW_Media_Info& media_info);//stream 0表主码流，1表子码流
    int SetEncInfo(const XM_MW_Encode_Info& enc_info);
    int GetEncInfo(int channel, int stream, XM_MW_Media_Info* media_info);
    int DisplayInit();
    int VBInit(VBStatus vb_st);
    int Init();
    int SetPreviewRegion(int hor_res, int ver_res, int physical_hor_res, int physical_ver_res);
    void* GetImgVirAttr();
    void* SwapImgVirAttr();
    void CommitSwapImg();
    void ShowLogo(PlayLogo logo);
    int SpeakerEnable(bool enable);

    int StartEncProc();
    int StopEncProc();
    int StartADProc();
    int StopADProc();
    int StartGeneralProc();
    int StopGeneralProc();
    int StartAudioProc();
    int StopAudioProc();
    int StartSoftVoProc();
    int StopSoftVOProc();

    int StartPreview();
    int StopPreview();
    int StartPlayback(CUR_PLAY_STATUS play_status, PAYLOAD_TYPE_E codec_type, RECT_S* display_region, bool audio_only);
    bool SendDecFrame(long handle, XM_MW_Media_Frame* media_frame);
    int StopPlayback();
  
    int EnableOsdTime(int channel, bool enable, int x, int y);//使能osd时间
    int ResetDecDev(); //播放一个新文件时，复位解码设备
    int PlaySound(const std::string& sound_file);
    int SetVolume(int sound_volume);
    int StopSound();
    int SetAudioOutInfo(int bit_per_sample,	int samples_per_sec, int audio_channel);
    int ForceIFrame(int channel, int stream=0);
    int SubStreamEnable(bool enable);
    //停车监控
    int SetTimeLapse(XMTimeLapse time_lapse);
    //开启12小时制
    int Enable12H(bool enable);
    //用mpp中的jpg编码，以节省空间占用
    int EncodeJpg(char* raw_buf, int width, int height, char* jpg_buf, int* jpg_len);
    /*--------------内部线程接口--------------------*/
    int VideoEncProc();
    int AudioEncProc();
    int GeneralProc();
    int AudioProc();
    int SoftVoProc(); //软件缩放
    int SetOSDInfos(XM_MW_OSD_INFOS& osd_infos);
    int UpdateOSDInfo(int channel, XM_MW_OSD_UPDATE_INFO& osd_update_info);
 	int OsdTitleCreate(RGN_HANDLE Handle, int nChannel, int width, int height);
    /**********************************************
	Function: Osd
	param:
		Handle:同一通道的每个Osd都拥有独立的Handle，若Handle已经创建则是修改该Handle对应的osd配置
		nChannel:显示Osd的通道
		Titlestr:显示的字符串
		Point:确定显示字符串的位置
    ************************************************/
	int SetOsdTitle(RGN_HANDLE Handle, int nChannel, const SystemTime& sys_time, const POINT_S& Point, bool sub);
    int SetUserOsd(RGN_HANDLE Handle, int nChannel, XM_MW_OSD_INFO& osd_info, bool auto_up);
	int CloseOSD(RGN_HANDLE Handle, int nChannel);
	int GetPicJPEG(int channel);
    int SetMirror(int chn, XM_MIRROR_TYPE mirror_type);
    int CameraSetRefrenceLevel(int level);
    int CameraSetAwb(int chn, bool enable, int level);
    int CameraSetRejectFlicker(int mode);
    int CameraSetWdr(int chn, bool enable, int level);
    bool AdLoss();
    
private:
    int AdInit();
    int VdecConfig(bool init, VDEC_DEV dec_dev_id, PAYLOAD_TYPE_E codec_type);
    int VencConfig(int channel, int stream);
	int JpegConfig(int channel);
    int ViConfig();
    int ViModify(int channel, bool mirror);
    int AiConfig();
    int AoConfig(AUDIO_SAMPLE_RATE_E sample_rate);
    int ChnBind();
    int ChnUnBind();
    int SetEncChnAttr(int channel, int stream, VENC_CHN_ATTR_S& enc_chn_attr);
    int GetJpgPicInternal(int channel);
    int GetAIFrame(int ai_dev, int channel_num, AUDIO_FRAME_S* audio_frame);
    int StartEnc(int channel);
    int StopEnc(int channel);
    int OSDSetNum(uint8_t* buf, int num, int pos, int width, bool sub);//sub为true是子码流osd，长宽各减半
    int SetAIVolume(int sound_volume);
    
    int GetViFrame(int channel);
    int ZoomResolution(int channel, VIDEO_FRAME_INFO_S* src_frame);
    int GetOSDPoint(int channel, POINT_S& point, int width, int height);
    int UpdateOsdTime(int channel, const SystemTime& local_time);
    int UpdateUsrOSD();
    int GetRectInfo(RECT_S& rect_info);
    int SetVolumeInternal(int sound_volume);

private:
    static const int kMaxAudioBufferLen = 1280;
    bool init_;
    int channel_num_;
    int playback_vpss_grp_;
    CUR_PLAY_STATUS play_status_;
    bool ad_loss_;
    std::deque<std::string> sound_file_;
    FILE* sound_fp_;
    bool startup_sound_over_;
    uint8_t audio_play_buffer_[kMaxAudioBufferLen];
    AUDIO_SAMPLE_RATE_E ao_sample_rate_;
    int yuv_width_;
    int yuv_height_;
    int64_t last_target_check_time_;
    std::mutex target_opr_mutex_;
    std::mutex speak_mutex_;
    bool speak_enable_;
    AdBase* ad_proc_;
    VideoDecBase* vdec_proc_;
	VBStatus vb_status_;

    XM_MW_Media_Info enc_medias_[XM_MAX_CHANNEL_NUM][2];
    uint8_t audio_buffer_[XM_MAX_CHANNEL_NUM][kMaxAudioBufferLen];
    int audio_remain_len_[XM_MAX_CHANNEL_NUM];
    //XM_MW_Media_Info dec_medias_[XM_MAX_CHANNEL_NUM][2];
    XM_MIRROR_TYPE mirror_[XM_MAX_CHANNEL_NUM];
    bool last_ad_loss_;
    int64_t ad_loss_time_;
    bool vod_enable_chn_;
    std::mutex mutex_render_ad_;
    std::mutex mutex_ad_vi_;
    bool vdec_;
    std::mutex mutex_vdec_;
    bool ao_config_;
    bool stop_sound_;
    XMTimeLapse time_lapse_;
    uint64_t ad_frame_count_;
    int64_t last_send_audio_time_;
    int user_volume_;
	bool enable_12h_;
    XM_VIDEO_FMT ad_type_;
    //线程相关
    pthread_t thread_video_enc_handle_;
    pthread_t thread_audio_enc_handle_;
    bool thread_enc_init_;
	bool thread_enc_exit_;

    OS_THREAD general_thread_;
    OS_THREAD audio_thread_;
    bool thread_genral_exit_;
    bool thread_audio_exit_;

    OS_THREAD vi_thread_;
    bool thread_vi_exit_;
    //视频输入
    VO_LAYER VoLayer_;
    XM_VOID *p_img_vir_attr_;
    XM_U32 u32ImgPhyAddr_;
    int img_pos_; //双缓冲位置，0是第一块，1是第二块
    //osd相关
    XM_MW_OSD_INFOS user_osd_;
    std::mutex metex_user_osd_;
    // 后拉框属性
    XM_MW_RECT_INFOS user_rect_info_;
    //osd时间
    int64_t last_osd_set_time_;
    int osd_time_x_[XM_MAX_CHANNEL_NUM+4];
    int osd_time_y_[XM_MAX_CHANNEL_NUM+4];
    bool enable_osd_[XM_MAX_CHANNEL_NUM+4];
    SystemTime last_osd_time_;
    uint8_t osd_num_buf_[16][kOSDWidth*kOSDHeight*2];
    uint8_t osd_show_buf_[kOSDWidth*kOSDHeight*2*kMaxOSDNumCount];
    //抓图
    bool catch_pic_[XM_MAX_CHANNEL_NUM];
    bool start_recv_pic_[XM_MAX_CHANNEL_NUM];
    int64_t last_catch_pic_time_[XM_MAX_CHANNEL_NUM];
    ThumbnailStatus thumbnail_status_[XM_MAX_CHANNEL_NUM];
    uint8_t thumbnail_buf_[2][320*240*3/2];
	int thumbnail_width_[XM_MAX_CHANNEL_NUM];
    int thumbnail_height_[XM_MAX_CHANNEL_NUM];
    long resize_handle_[XM_MAX_CHANNEL_NUM];

private:
    MppMdl();
    ~MppMdl();
    static MppMdl* instance_;
};

#endif //#ifndef WIN32