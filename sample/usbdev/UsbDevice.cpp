/******************************************************************************
  Copyright (C), 2020, xmsilicon Tech. Co., Ltd.
 ******************************************************************************
  File Name     : UsbDevice.cpp
  Author        : xmsilicon R&D
  Description   : usb摄像头相关的处理模块，连接、升级、取流
  History       :
******************************************************************************/

#include "UsbDevice.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "xm_middleware_network.h"
#include "xm_middleware_api.h"
#include "Log.h"
#include "cJson/cJSON.h"
#include "global_data.h"
#include "network/HttpSerModule.h"

UsbDevice* UsbDevice::instance_ = 0;
UsbDevice* UsbDevice::Instance()
{
    if (0 == instance_) {
		instance_ = new UsbDevice();
	}
	return instance_;
}

void UsbDevice::Uninstance()
{
    if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

UsbDevice::UsbDevice() :upgrader_over_(false)
{

}

UsbDevice::~UsbDevice()
{
    
}

void OnClientNetworkEvent(int engineId, int connId, XMIPEventType type, char *data, int len)
{
	if (XM_EVENT_DISCONNECT == type) {
		XMLogE("OnClientNetworkEvent, DISCONNECT");
	}
}

uint8_t g_last_video_seq = 0;
bool g_recv_i_frame = false;
int g_progress = 0;

int XMIRLedEnable(bool enable)
{
#ifndef WIN32
	if (enable)
	{
		XM_Middleware_SafeSystem("regs 1002020c c00");
	}
	else
	{
		XM_Middleware_SafeSystem("regs 1002020c 400");
	}
#endif
	return 0;
}

void OnClientNetworkMsg(int engineId, int connId, uint8_t type, char* data, char* connType, int len) 
{
    XM_MW_Media_Frame media_frame;
	if (type == XM_DATA_STREAM) {
		char* frame_buf = NULL;
		int frame_len = 0;
		int frame_time = 0;
		uint8_t* ptr = (uint8_t*)data;
		uint8_t video_seq = 0;
		if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 1) {
			if (ptr[3] == 0xFD) {
				//I
                media_frame.key_frame = true;
				g_recv_i_frame = true;
				video_seq = ptr[5];
				frame_buf = data + 20;
				media_frame.codec_type = PT_H265;
				media_frame.video = true;
				ptr += 16;
				frame_len = ptr[3] << 24 | ptr[2] << 16 | ptr[1] << 8 | ptr[0];
				frame_time = ptr[7] << 24 | ptr[6] << 16 | ptr[5] << 8 | ptr[4];
				if (frame_len + 20 != len) {
					XMLogE("frame len error, frame len=%d, len=%d", frame_len, len);
				}
				//XMLogI("i frame, len=%d", frame_len);
			}
			else if (ptr[3] == 0xFC) {
				//P
                media_frame.key_frame = false;
				video_seq = ptr[4];
				frame_buf = data + 14;
				media_frame.codec_type = PT_H265;
				media_frame.video = true;
				ptr += 6;
				frame_time = ptr[3] << 24 | ptr[2] << 16 | ptr[1] << 8 | ptr[0];
				ptr += 4;
				frame_len = ptr[3] << 24 | ptr[2] << 16 | ptr[1] << 8 | ptr[0];
				if (frame_len + 14 != len) {
					XMLogE("frame len error, frame len=%d, len=%d", frame_len, len);
				}
			}
			else if (ptr[3] == 0xFA) {
				media_frame.codec_type = PT_PCMA;
				media_frame.video = false;
				frame_buf = data + 14;
                ptr += 12;
                frame_len = ptr[1] << 8 | ptr[0];
                if (frame_len + 14 != len) {
                    XMLogE("frame len error, frame len=%d, len=%d", frame_len, len);
                }
			}
			else if (ptr[3] == 0xFE) {
				//缩略图
				frame_buf = data + 12;
				frame_len = ptr[11] << 24 | ptr[10] << 16 | ptr[9] << 8 | ptr[8];
				if (frame_len != (320*180*3/2)) {
					XMLogE("thumbnail len error, frame len=%d, len=%d, %02x-%02x-%02x-%02x-%02x-%02x", 
						frame_len, len,
						ptr[6], ptr[7], ptr[8], ptr[9], ptr[10], ptr[11]);
					return;
				}
				int storage_channel = 2;
				//缩略图从yuv420P转成nv12，mpp中的jpg编码只支持nv12
				char* src_u = frame_buf + 320*180;
				char* src_v = src_u + 320*180/4;
				char dst_uv[320*180/2];
				char* ptr_uv = dst_uv;
				for (int i = 0; i < 90; i++) 
					for (int j = 0; j < 160; j++) {
						*ptr_uv++ = *src_u++;
						*ptr_uv++ = *src_v++;
					}
				memcpy(frame_buf + 320*180, dst_uv, 320*180/2);	
				XM_Middleware_Media_VideoThumbnail(storage_channel, frame_buf, 320, 180);
				return;
			}
			else if (ptr[3] == 0xFB) {
				//jpg图片
				frame_buf = data + 12;
				frame_len = ptr[11] << 24 | ptr[10] << 16 | ptr[9] << 8 | ptr[8];
				if (frame_len + 12 != len) {
					XMLogE("jpg len error, frame len=%d, len=%d", 
						frame_len, len);
					return;
				}
		
				int storage_channel = 2;
				XM_MW_Media_Frame media_frame = {0};
				media_frame.video = true;
				media_frame.codec_type = PT_JPEG;
				media_frame.frame_buffer = (uint8_t*)frame_buf;
				media_frame.frame_size = frame_len;
				int width = 1920;
				int height = 1080;
				XM_Middleware_Media_VideoStream(storage_channel, 0, &media_frame, width, height);

				return;
			}
		}
		
		if (frame_buf) {
			if (!media_frame.key_frame) {
				if (g_last_video_seq != 0 
				    && video_seq != g_last_video_seq+1 
				    && !(g_last_video_seq==255 && video_seq==0)) {
					//帧序不连续，要重定位i帧，防止花屏
					g_recv_i_frame = false;
					//processIFrame();
					XMLogW("lost frame, cur seq=%d, last seq=%d", video_seq, g_last_video_seq);
				}
			}

			g_last_video_seq = video_seq;
			if (g_recv_i_frame == false) {//如果没找到I帧，不解码
				return;
			}	
			media_frame.frame_buffer = (uint8_t*)frame_buf;
			media_frame.frame_size = frame_len;
			media_frame.timestamp = frame_time;

			int storage_channel = 2;
			int realplay_channel = 2;//1
			if (media_frame.video) {
				if (g_storage_started) {
					XM_Middleware_Storage_AddVideoFrame(storage_channel, media_frame.key_frame, 
						media_frame.frame_buffer, media_frame.frame_size, media_frame.timestamp);
				}

				if (g_realplay && realplay_channel == g_realplay_channel) {
					HTTPSerMdl::Instance()->OnMWRealTimeStream(realplay_channel, 0, &media_frame, 0);
				}
			}
			else {
				//暂时不用内录的音频
				// XM_Middleware_Storage_AddAudioFrame(channel, media_frame.frame_buffer, 
				// 	media_frame.frame_size, media_frame.timestamp);
				if (g_realplay && realplay_channel == g_realplay_channel) {
					HTTPSerMdl::Instance()->OnMWRealTimeStream(realplay_channel, 0, &media_frame, 0);
				}
			}
			// int ret = XM_Middleware_TCPVideo_Play(&media_frame);
			// if (ret < 0)
			// 	processIFrame();
		}
	} else if (type == XM_DATA_STRING) {
        XMLogI("OnClientMsg, msg=%s", data);
		cJSON* root_req = cJSON_Parse(data);
		if(!root_req){
			XMLogE("json parse error");
			return ;
		}
		cJSON* op_item = cJSON_GetObjectItem(root_req,"op");
		if(!op_item){
			XMLogE("can't find op opration");
			cJSON_Delete(root_req);
			return ;
		}
		std::string str_oper = op_item->valuestring;
		cJSON* param_item = cJSON_GetObjectItem(root_req,"param");
		if(!param_item){
			XMLogE("can't find param_item opration");
			cJSON_Delete(root_req);
			return ;
		}

		if (str_oper == "heartbeat") {
			cJSON* wifispeed_item = cJSON_GetObjectItem(param_item, "wifispeed");
			if (!wifispeed_item) {
				XMLogE("can't find wifispeed");
				cJSON_Delete(root_req);
				return;
			}
			// int wifispeed_value = wifispeed_item->valueint;  //wifi信号，分五档，越大越好。1:0；2:0-49；3:49-79；4:80-100；5:100
			// OnMWEventCallBack(0, XM_EVENT_CAMERA_CONNECT, NULL, wifispeed_value, 0);
		} else if (str_oper == "upgrader_progress") {
            cJSON* progress_item = cJSON_GetObjectItem(param_item, "progress");
			if (progress_item) {
                g_progress = progress_item->valueint;
                XMLogI("g_progress=%d", g_progress);
            }else {
                XMLogI("g_progress error");
            }
		} else if (str_oper == "set_ir_led") {
            cJSON* progress_item = cJSON_GetObjectItem(param_item, "state");
			if (progress_item) {
                int ir_led_state = progress_item->valueint;
                XMLogI("ir_led_state=%d", ir_led_state);
				if (ir_led_state == 0) {
					XMIRLedEnable(false);
				} else {
					XMIRLedEnable(true);
				}
            }else {
                XMLogE("set_ir_led para invalid");
            }
		}
        else {
            XMLogW("unknow opr=%s", str_oper.c_str());
        }
        cJSON_Delete(root_req);
	}
	return;
}

int UsbDevice::Init()
{
    XM_Middleware_Network_Client_SetEventCallback(OnClientNetworkEvent);
    XM_Middleware_Network_Client_SetDataCallback(OnClientNetworkMsg);
    return 0;
}

int UsbDevice::ConnectServer(const char* server_ip, int signal_port, int media_port)
{
    int ret = XM_Middleware_Network_Client_StartClient((char*)server_ip, signal_port, media_port);
    XMLogI("connect usb device, ip=%s, ret=%d", server_ip, ret);
    if (ret < 0) {
        XM_Middleware_Network_Client_StopClient();
        usleep(5000*1000);
        ret = XM_Middleware_Network_Client_StartClient((char*)server_ip, signal_port, media_port);
    }
    return ret;
}

int UsbDevice::ForceIFrame(int channel, int stream)
{
	cJSON* params = cJSON_CreateObject();
    cJSON_AddNumberToObject(params, "channel", channel);
    cJSON_AddNumberToObject(params, "stream", stream);
    cJSON* root_req = cJSON_CreateObject();
    cJSON_AddStringToObject(root_req, "version", "1.0");
    cJSON_AddStringToObject(root_req, "type", "request");
    cJSON_AddStringToObject(root_req, "op", "forceIFrame");
    cJSON_AddItemToObject(root_req, "param", params);
    char* http_body = cJSON_Print(root_req);
    cJSON_Delete(root_req);

    uint8_t type = 0;
    XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
	return 0;
}

int UsbDevice::Realplay(int channel, int stream)
{
	cJSON* params = cJSON_CreateObject();
    cJSON_AddNumberToObject(params, "channel", channel);
    cJSON_AddNumberToObject(params, "stream", stream);
    cJSON_AddNumberToObject(params, "id", 0);
    cJSON* root_req = cJSON_CreateObject();
    cJSON_AddStringToObject(root_req, "version", "1.0");
    cJSON_AddStringToObject(root_req, "type", "request");
    cJSON_AddStringToObject(root_req, "op", "realplay");
    cJSON_AddItemToObject(root_req, "param", params);
    char* http_body = cJSON_Print(root_req);
    cJSON_Delete(root_req);

    uint8_t type = 0;
    XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);

	//使能osd时间
	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = true;
	int ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Time_Watermark, cfg_value);
	if(ret < 0) {
		XMLogW("Get OSD Value Failed!");
	} else {
		bool enable = cfg_value.bool_value;
		SetOSDenable(enable);
	}
	return 0;
}

int UsbDevice::SyncSystemTime()
{
	char str_time[128] = {0};
    struct timeval tv = { 0 };
	gettimeofday(&tv, 0);
    tm nowTime;
    localtime_r(&tv.tv_sec, &nowTime);
    sprintf(str_time, "%4d-%02d-%02d %02d:%02d:%02d", nowTime.tm_year + 1900,
            nowTime.tm_mon + 1, nowTime.tm_mday, nowTime.tm_hour, nowTime.tm_min,
            nowTime.tm_sec);

    XMLogI("SyncTime, %s", str_time);

	cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "date", str_time);
    cJSON* root_req = cJSON_CreateObject();
    cJSON_AddStringToObject(root_req, "version", "1.0");
    cJSON_AddStringToObject(root_req, "type", "request");
    cJSON_AddStringToObject(root_req, "op", "setsystime");
    cJSON_AddItemToObject(root_req, "param", params);
    char* http_body = cJSON_Print(root_req);
    cJSON_Delete(root_req);

    uint8_t type = 0;
    XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
	return 0;
}

int UsbDevice::CatchPic()
{
	cJSON* params = cJSON_CreateObject();
	cJSON_AddNumberToObject(params, "channel", 0);
    cJSON_AddNumberToObject(params, "stream", 0);
    cJSON_AddNumberToObject(params, "id", 0);
    cJSON* root_req = cJSON_CreateObject();
    cJSON_AddStringToObject(root_req, "version", "1.0");
    cJSON_AddStringToObject(root_req, "type", "request");
    cJSON_AddStringToObject(root_req, "op", "catchpic");
    cJSON_AddItemToObject(root_req, "param", params);
    char* http_body = cJSON_Print(root_req);
    cJSON_Delete(root_req);

    uint8_t type = 0;
    XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
	return 0;
}
    
int UsbDevice::Upgrader(const char* file_name)
{
	upgrader_over_ = false;
	struct stat statbuf;
    if (stat(file_name, &statbuf) != 0)
    {
        XMLogE("stat file %s info err", file_name);
        return -1;
    }
   
    int file_len = statbuf.st_size;
    XMLogI("upgrade usb device, file len=%d, file name=%s", file_len, file_name);
    FILE* fp = fopen(file_name, "rb");
    if (!fp) {
        XMLogE("open upgrader file error, name=%s", file_name);
        return 0;
    }

    char md5[64] = {0};
    int ret = XM_Middleware_SDCard_GetMD5(file_name, md5);
    XMLogI("upgrade usb device, md5=%s, ret=%d", md5, ret);
    cJSON* params = cJSON_CreateObject();
	cJSON_AddStringToObject(params, "status", "start");
    cJSON_AddStringToObject(params, "md5", md5);
	cJSON_AddNumberToObject(params, "size", file_len);

	cJSON* root_res = cJSON_CreateObject();
	cJSON_AddStringToObject(root_res, "version", "1.0");
	cJSON_AddStringToObject(root_res, "type", "request");
	cJSON_AddStringToObject(root_res, "op", "upgrader");
	cJSON_AddItemToObject(root_res, "param", params);
	char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
	free(http_body);

    static const int BUFLEN = 1024;
    char buffer[BUFLEN];
    while(1) {
        int len = fread(buffer, 1, BUFLEN, fp);
        if (len <= 0) {
            fclose(fp);
            break;
        }
        int ret = XM_Middleware_Network_Client_SendData(XM_DATA_UPGRADER, buffer, len);
        while (ret < 0) {
            usleep(20*1000);
            ret = XM_Middleware_Network_Client_SendData(XM_DATA_UPGRADER, buffer, len);
        }
    }

    usleep(200*1000);
    params = cJSON_CreateObject();
	cJSON_AddStringToObject(params, "status", "over");
	cJSON_AddNumberToObject(params, "size", file_len);

	root_res = cJSON_CreateObject();
	cJSON_AddStringToObject(root_res, "version", "1.0");
	cJSON_AddStringToObject(root_res, "type", "request");
	cJSON_AddStringToObject(root_res, "op", "upgrader");
	cJSON_AddItemToObject(root_res, "param", params);
	http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
	free(http_body);

    XMLogI("upgrade usb device, file send over");
    //等待UVC摄像头接收完成
    while(1) {
        if (g_progress >= 100) {
            XMLogI("upgrade write md5, g_progress=%d", g_progress);
            fp = fopen(kUSBDeviceUpdateMD5File, "w+");
            if (fp) {
                XMLogI("writing usb dev md5 = %s", md5);
                fwrite(md5, strlen(md5), 1, fp);
                fclose(fp);
				sync();
            } else {
                XMLogE("writing usb dev md5 failed, name=%s", kUSBDeviceUpdateMD5File);
            }
            break;
        }
        usleep(200*1000);
	}
	upgrader_over_ = true;
	XMLogI("Upgrader usb dev over!");
    return 0;
}

int UsbDevice::GetUpgraderProgress()
{
	if (upgrader_over_) {
		return 150;
	}
	return g_progress;
}


/// @brief 设置子模块帧率
/// @param fps 
/// @return 0
int UsbDevice::SetFPS(int fps)
{
	cJSON* params = cJSON_CreateObject();
    cJSON_AddNumberToObject(params, "fps", fps);
    cJSON* root_req = cJSON_CreateObject();
    cJSON_AddStringToObject(root_req, "version", "1.0");
    cJSON_AddStringToObject(root_req, "type", "request");
    cJSON_AddStringToObject(root_req, "op", "setFPS");
    cJSON_AddItemToObject(root_req, "param", params);
    char* http_body = cJSON_Print(root_req);
    cJSON_Delete(root_req);

    uint8_t type = 0;
    XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
	return 0;
}

/// @brief 设置OSD使能
/// @param enable 
/// @return 0
int UsbDevice::SetOSDenable(int enable, int hour12)
{
	cJSON* params = cJSON_CreateObject();
    cJSON_AddNumberToObject(params, "enable", enable);
	cJSON_AddNumberToObject(params, "12H", hour12);
    cJSON* root_req = cJSON_CreateObject();
    cJSON_AddStringToObject(root_req, "version", "1.0");
    cJSON_AddStringToObject(root_req, "type", "request");
    cJSON_AddStringToObject(root_req, "op", "setosd");
    cJSON_AddItemToObject(root_req, "param", params);
    char* http_body = cJSON_Print(root_req);
    cJSON_Delete(root_req);

    uint8_t type = 0;
    XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
	
	return 0;
}

/// @brief 设置红外灯模式
/// @param mode：0(off),1(on),2(auto)
/// @return 0
int UsbDevice::SetIRMode(int mode)
{
	cJSON* params = cJSON_CreateObject();
    cJSON_AddNumberToObject(params, "mode", mode);
    cJSON* root_req = cJSON_CreateObject();
    cJSON_AddStringToObject(root_req, "version", "1.0");
    cJSON_AddStringToObject(root_req, "type", "request");
    cJSON_AddStringToObject(root_req, "op", "set_ir_mode");
    cJSON_AddItemToObject(root_req, "param", params);
    char* http_body = cJSON_Print(root_req);
    cJSON_Delete(root_req);

    uint8_t type = 0;
    XM_Middleware_Network_Client_SendData(XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
	
	return 0;
}