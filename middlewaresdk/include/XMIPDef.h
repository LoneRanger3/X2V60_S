#pragma once

typedef enum _XMDataType 
{
	XM_DATA_STRING = 0,
	XM_DATA_STREAM,
	XM_DATA_UPGRADER,
	XM_DATA_PICTURE,
}XMDataType;

typedef enum _XMIPEventType
{
	XM_EVENT_DISCONNECT = 0,
}XMIPEventType;

typedef struct _XMMsgInfo 
{
	int engineId;
	int connId;
	uint8_t type;
	int len;
	std::string msg;
}XMMsgInfo;

typedef struct _XMMediaInfo
{
	//视频
	int video_codec;
	int width;
	int height;
	int frame_rate; //帧率，一般25
	//音频
	int audio_codec;
	int bit_per_sample;
	int samples_per_sec;
	int audio_channel;
}XMMediaInfo;

typedef void (*XM_Data_CallBack)(int engineId, int connId, uint8_t type, char *data, int len);
typedef void (*XM_Event_CallBack)(int engineId, int connId, XMIPEventType type, char *data, int len);