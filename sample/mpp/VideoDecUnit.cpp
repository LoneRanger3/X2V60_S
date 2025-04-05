#ifndef WIN32
#include "VideoDecUnit.h"
#include <string.h>
#include "mpi_vdec.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_ao.h"
#include "xm_comm_aio.h"
#include "mpi_vpss.h"
#include "Log.h"
#include "CommDef.h"

static const int HOR_RES = 640;
static const int VER_RES = 480;
const int kVodDecDev = 0;
const int kPicDecDev = 1;

VideoDecUnit::VideoDecUnit(int playback_vpss_grp):playback_vpss_grp_(playback_vpss_grp),
     width_(0), height_(0), dec_status_(PLAY_NONE)
{

}

VideoDecUnit::~VideoDecUnit()
{

}

int VideoDecUnit::StartInternal(int dev_id)
{
	VDEC_DEV_ATTR_S stVdecDevAttr;
	memset(&stVdecDevAttr, 0, sizeof(stVdecDevAttr));
	stVdecDevAttr.u32PicWidth = kMaxPicWidth;//宽高设置成最大分辨率，解码设备内部会自适应
	stVdecDevAttr.u32PicHeight = kMaxPicHeight;
	stVdecDevAttr.u32BufSize = kMaxFrameLen;
	stVdecDevAttr.stVdecVideoAttr.enMode = VIDEO_MODE_FRAME;
	if (dev_id == kVodDecDev) {
		stVdecDevAttr.enType = PT_H265;	
		stVdecDevAttr.stVdecVideoAttr.u32RefFrame = 2;	
	} else {
		stVdecDevAttr.enType = PT_JPEG;	
	}

	int ret = XM_MPI_VDEC_CreateDev(dev_id, &stVdecDevAttr);
	if (ret != XM_SUCCESS) {
		XMLogE("XM_MPI_VDEC_CreateDev error, ret=%x", ret);
	}

	ret = XM_MPI_VDEC_SetDepth(dev_id, 0, 1);
	if (ret != XM_SUCCESS) {
		XMLogE("XM_MPI_VDEC_SetDepth error, ret=%x", ret);
	}

	VDEC_CHN_ATTR_S stVdecChnAttr;
	memset(&stVdecChnAttr, 0 , sizeof(stVdecChnAttr));
	stVdecChnAttr.u32OutWidth = HOR_RES;
	stVdecChnAttr.u32OutHeight = VER_RES;
	ret = XM_MPI_VDEC_SetChnAttr(dev_id, 0, &stVdecChnAttr);
    if (ret != XM_SUCCESS) {
        XMLogE("XM_MPI_VDEC_SetDevAttr error, ret=%x", ret);
    }

	ret = XM_MPI_VDEC_StartRecvStream(dev_id);
	if (ret != XM_SUCCESS) {
        XMLogE("XM_MPI_VDEC_StartRecvStream error, ret=%x", ret);
    }
	ret = XM_MPI_VDEC_EnableChn(dev_id, 0);
	if (ret != XM_SUCCESS) {
        XMLogE("XM_MPI_VDEC_EnableChn error, ret=%x", ret);
    }

	return XM_SUCCESS;
}

int VideoDecUnit::StopInternal(int dev_id)
{
	XMLogI("StopInternal, dev id=%d", dev_id);
	int ret = XM_MPI_VDEC_StopRecvStream(dev_id);
	if (ret != XM_SUCCESS) {
		XMLogE("XM_MPI_VDEC_StopRecvStream error, ret=%x", ret);
	}
	ret = XM_MPI_VDEC_SetDepth(dev_id, 0, 0);
	if (ret != XM_SUCCESS) {
		XMLogE("XM_MPI_VDEC_SetDepth error, ret=%x", ret);
	}
	ret = XM_MPI_VDEC_DisableChn(dev_id, 0);
	if (ret != XM_SUCCESS) {
		XMLogE("XM_MPI_VDEC_DisableChn error, ret=%x", ret);
	}
	ret = XM_MPI_VDEC_DestroyDev(dev_id);
	if (ret != XM_SUCCESS) {
		XMLogE("XM_MPI_VDEC_DestroyDev error, ret=%x", ret);
	}
	return 0;
}

int VideoDecUnit::Start(bool vod)
{
	XMLogI("VideoDecUnit start");
	if (vod) {
		if (dec_status_ == PLAY_PLAYBACK_VOD)
			return 0;
		if (dec_status_ == PLAY_PLAYBACK_PICTURE)
			StopInternal(kPicDecDev);
		StartInternal(kVodDecDev);
		dec_status_ = PLAY_PLAYBACK_VOD;
	} else {
		if (dec_status_ == PLAY_PLAYBACK_PICTURE)
			return 0;
		if (dec_status_ == PLAY_PLAYBACK_VOD)
			StopInternal(kVodDecDev);
		StartInternal(kPicDecDev);	
		dec_status_ = PLAY_PLAYBACK_PICTURE;
	}
	return 0;
}

void VideoDecUnit::ResetDev() 
{
    XMLogW("XM_MPI_VDEC_ResetDev");
    XM_MPI_VDEC_StopRecvStream(kVodDecDev);
    int ret = XM_MPI_VDEC_ResetDev(kVodDecDev);
    if (ret != 0) {
        XMLogW("XM_MPI_VDEC_ResetDev error, ret=%d", ret);
    }
    XM_MPI_VDEC_StartRecvStream(kVodDecDev);
}

bool VideoDecUnit::SendFrame(long handle, XM_MW_Media_Frame* media_frame) 
{
	VDEC_STREAM_S dec_stream;
	dec_stream.pu8Addr = media_frame->frame_buffer;
	dec_stream.u32Len = media_frame->frame_size;
	dec_stream.u64PTS = media_frame->timestamp*1000; //从毫秒转换成微妙

	int ret = 0;
	if (media_frame->codec_type == PT_H264 || media_frame->codec_type == PT_H265) {
		ret = XM_MPI_VDEC_SendStream(kVodDecDev, &dec_stream, 0);
	}
	else if (media_frame->codec_type == PT_BUTT) {
		VIDEO_FRAME_INFO_S stVideoFrame;
		memset(&stVideoFrame, 0, sizeof(stVideoFrame));
		int s32Ret = XM_MPI_VB_GetFrame(media_frame->frame_size, &stVideoFrame, 0);
		if (XM_SUCCESS != s32Ret) {
			XMLogE("Get empty frame failed. ret=%x", s32Ret);
			return false;
		}
		stVideoFrame.stVFrame.u32Width = width_;
		stVideoFrame.stVFrame.u32Stride[0] = width_;
		stVideoFrame.stVFrame.u32Height = height_;
		stVideoFrame.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		XM_VOID* pVirAddr = XM_MPI_SYS_Mmap(stVideoFrame.stVFrame.u32PhyAddr[0], media_frame->frame_size);
		if (pVirAddr == NULL) {
			XMLogE("XM_MPI_SYS_Mmap, viraddr is NULL");
			return false;
		}
		memcpy(pVirAddr, media_frame->frame_buffer, media_frame->frame_size);
		s32Ret = XM_MPI_VPSS_SendFrame(playback_vpss_grp_, &stVideoFrame, 0);
		if (XM_SUCCESS != s32Ret) {
			XMLogE("vpss send frame failed. ret=%x", s32Ret);
			return false;
		}
		XM_MPI_SYS_Munmap(pVirAddr, media_frame->frame_size);
	}
	else if (media_frame->codec_type == PT_PCMA) {
		AUDIO_DEV AoDev = 0;
		AO_CHN AoChn = 0;
        AUDIO_FRAME_S audio_frame;
        memset(&audio_frame, 0, sizeof(audio_frame));
        audio_frame.enBitwidth = AUDIO_BIT_WIDTH_16;
        audio_frame.enSoundmode = AUDIO_SOUND_MODE_MONO;
        audio_frame.u64TimeStamp = 1;
        audio_frame.u32Seq = 1;
		audio_frame.pVirAddr[0] = media_frame->frame_buffer;
		audio_frame.u32Len = media_frame->frame_size;
		XM_MPI_AO_SendFrame(AoDev, AoChn, &audio_frame, XM_TRUE);
	}
	else if (media_frame->codec_type == PT_JPEG) {
		ret = XM_MPI_VDEC_DecJpeg(kPicDecDev, &dec_stream);
		XMLogW("XM_MPI_VDEC_DecJpeg, len=%d, ret=%x", dec_stream.u32Len, ret);
	}
	else {
		XMLogE("codec type not support, codec=%d", media_frame->codec_type );
		return true;
	}
	if (ret != 0) {
		XMLogW("dec playback frame error, ret=%x", ret);
	}
	return ret == 0 ? true : false;
}

int VideoDecUnit::Stop()
{
	if (dec_status_ == PLAY_PLAYBACK_VOD)
		StopInternal(kVodDecDev);
	else if (dec_status_ == PLAY_PLAYBACK_PICTURE)
		StopInternal(kPicDecDev);
	dec_status_ = PLAY_NONE;
	return 0;
}

void VideoDecUnit::SetWidth(int width, int height)
{
    width_ = width;
    height_ = height;
}

#endif