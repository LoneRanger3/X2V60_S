/******************************************************************************
  Copyright (C), 2020, xmsilicon Tech. Co., Ltd.
 ******************************************************************************
  File Name     : demo.c
  Author        : xmsilicon R&D
  Description   : Video output demo 
  History       :
******************************************************************************/
#include <sys/time.h>
#include <sys/times.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <deque>
#include <string>
#include <mutex>
#include <algorithm>
#include "lvgl.h"
#include "global_data.h"
#include "porting/lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
#include "xm_ia_td_interface.h"
#include "Log.h"
#include "TimeUtil.h"
#include "XMThread.h"
#include "global_page.h"
#include "cJson/cJSON.h"
#include "periphery/PeripheryManager.h"
#include "log/LogFileManager.h"
#include "usbdev/UsbDevice.h"
#include "network/HttpSerModule.h"
#include "network/WifiOpr.h"
#include "mpp/MppMdl.h"
#include "CommDef.h"
#include "DemoDef.h"
#include "cr_lcd.h"
extern "C" {
#include "cr_net.h"
}
#include "cr_sdio.h"
#include "AudioEnc.h"
#include "osd_user.h"
const int kMainPicWidth = MY_DISP_HOR_RES;
const int kMainPicHeight = MY_DISP_VER_RES;
const int kChannelNum = 2;//3

int g_sd_status = XM_SD_NOEXIST;
bool g_storage_started = false;
bool g_realplay = false;
int g_engineId = 0;
int g_connId = 0;
int g_frame_id = 0;
int g_realplay_channel = 0;//0是前摄  1是后拉 2是usb内录
int64_t g_first_frame_time = 0;
bool g_app_connect = false;
bool g_wait_IFrame = false;

#if 1//X2V60_S_DEBUG1
bool g_acc_connected = false;
bool g_first_record = false;
#endif

struct MWEventInfo {
	long handle;
	XMEventType event_type;
	int param;
	std::string msg;
};

std::deque<MWEventInfo> g_mw_event_infos;
std::mutex g_event_mutex;

void OnMWEventCallBack(long handle, XMEventType event_type, const char* msg, int param, int64_t user, char* out_param);

void OnNetworkEvent(int engineId, int connId, XMIPEventType type, char *data, int len)
{
	if (type == XM_EVENT_DISCONNECT) {
		XMLogW("OnNetworkEvent");
		g_realplay = false;
	}
	XMLogW("after OnNetworkEvent, type=%d", (int)type);
}

void OnNetworkMsg(int engineId, int connId, uint8_t type, char* msg, char* connType, int contentLen) 
{
	if (GlobalPage::Instance()->page_main()->connect_computer_) {
		GlobalPage::Instance()->page_usb()->OnNetworkMsg(engineId, connId, type, msg, contentLen);
		return;
	}
	if (!g_app_connect) {
		MppMdl::Instance()->SubStreamEnable(true);
	}
	g_app_connect = true;

	if (type == XM_DATA_UPGRADER) {
		HTTPSerMdl::Instance()->OnUpgraderData(engineId, connId, msg, contentLen);
		return;
	}
	
	//XMLogW("respMsg,engineId=%d,connId=%d,msg=%s,connType=%s", engineId, connId, msg, connType);

	MWEventInfo event_info;
	event_info.handle = 0;
	event_info.event_type = XM_EVENT_NETWORK_MSG;
	event_info.param = 0;
	event_info.msg = "";
	g_event_mutex.lock();
	g_mw_event_infos.push_back(event_info);
	g_event_mutex.unlock();

	HTTPSerMdl::Instance()->respMsg(engineId, connId, type, msg, connType, contentLen);
}

void closeThread(int connId)
{
	HTTPSerMdl::Instance()->CloseThread(connId);
}

void lv_ignore_signal_handler(int sig)
{
	g_realplay = false;
	XMLogW("lv_ignore_signal_handler");
}

void lv_ignore_signals()
{
	signal(SIGPIPE, lv_ignore_signal_handler);
	signal(SIGTERM, lv_ignore_signal_handler);
	signal(SIGHUP, lv_ignore_signal_handler);
}

void run_project()
{
	XMLogI("====================Start Run UI=====================");
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
	
	GlobalPage::Instance()->page_main()->CfgInit();
	GlobalPage::Instance()->page_main()->OpenPage();

	if (GlobalPage::Instance()->page_main()->connect_computer_) {
		XMLogI("Usb connect computer!");
		XM_Middleware_Network_XMIP_SetDataCallback(OnNetworkMsg);
		//XM_Middleware_Network_SetEventCallback(OnNetworkEvent);	
		//GlobalPage::Instance()->page_usb()->OpenPage();
	}
}

//设置编码参数
int SetEncodeParam(int channel, PAYLOAD_TYPE_E video_codec_type, int width, int height, int frame_rate, int bit_rate)
{
	XM_MW_Media_Info media_info;
	media_info.video_codec = video_codec_type == PT_H265 ? PT_H265 : PT_H264;
	media_info.width = width;
	media_info.height = height;
	media_info.bit_rate = bit_rate;
	media_info.frame_rate = frame_rate;		
	media_info.audio_codec = PT_PCMA;
	media_info.audio_channel = 1;
	media_info.bit_per_sample = 16;
	media_info.samples_per_sec = 16000;
	MppMdl::Instance()->SetEncInfo(channel, 0, media_info);
	return 0;
}

void PackParamInfo(const char* name, const char **items_array, int items_len, const int* index_array, cJSON* infoObject)
{
    cJSON* object = cJSON_CreateObject();
    cJSON* items = cJSON_CreateStringArray(items_array, items_len);
    cJSON* index = cJSON_CreateIntArray(index_array, items_len);
    if (nullptr != name) {
        cJSON_AddStringToObject(object, "name", name);
        cJSON_AddItemToObject(object, "items", items);
        cJSON_AddItemToObject(object, "index", index);
        cJSON_AddItemToArray(infoObject, object);
    } else {
        cJSON_AddItemToObject(object, "items", items);
        cJSON_AddItemToObject(object, "index", index);
        cJSON_AddItemToObject(infoObject, "info", object);
    }
}

int ProcessGetParam(char* res_str)
{
	cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON* info = cJSON_CreateArray();

	const char *mic_items_array[2] = {"on", "off"};
	const int mic_index_array[2] = {1, 0};
	PackParamInfo("mic", mic_items_array, 2, mic_index_array, info);

	const char* resolution_items_array[3] = { "1080P", "2K", "4K"};
	const int resolution_index_array[3] = { 1080, 1440, 2144 };
	PackParamInfo("rec_resolution", resolution_items_array, 3, resolution_index_array, info);
	
	const char* rec_dur_items_array[3] = { "1MIN", "2MIN", "3MIN" };
	const int rec_dur_index_array[3] = { 1, 2, 3 };
	PackParamInfo("rec_split_duration", rec_dur_items_array, 3, rec_dur_index_array, info);

	const char *osd_items_array[2] = {"on", "off"};
	const int osd_index_array[2] = {1, 0};
	PackParamInfo("osd", osd_items_array, 2, osd_index_array, info);

#if 1//X2V60_S_DEBUG1
    const char* speaker_items_array[4] = { "off", "low", "middle", "high"};
    const int speaker_index_array[4] = {0, 1, 2, 4};
    PackParamInfo("speaker", speaker_items_array, 4, speaker_index_array, info);
#endif

	const char* gsr_sensitivity_items_array[4] = { "off", "low", "middle", "high" };
	const int gsr_sensitivity_index_array[4] = { 0, 1, 2, 3 };
	PackParamInfo("gsr_sensitivity", gsr_sensitivity_items_array, 4, gsr_sensitivity_index_array, info);

	/* const char* screen_standby_items_array[4] = { "off", "1MIN", "3MIN", "5MIN" };
	const int screen_standby_index_array[4] = { 0, 1, 3, 5 };
	PackParamInfo("screen_standby", screen_standby_items_array, 4, screen_standby_index_array, info); */

	const char* parking_monitor_items_array[2] = { "off", "on" };
	const int parking_monitor_index_array[2] = { 0, 1 };
	if (g_acc_connected){
		
	    PackParamInfo("parking_monitor", parking_monitor_items_array, 2, parking_monitor_index_array, info);
	}

	const char* timelapse_rate_items_array[4] = { "off", "1fps", "2fps", "5fps" };
	const int timelapse_rate_index_array[4] = { 0, 1, 2, 5 };
	if (g_acc_connected){
		
	    PackParamInfo("timelapse_rate", timelapse_rate_items_array, 4, timelapse_rate_index_array, info);
	}

	const char *rec_items_array[2] = {"on", "off"};
	const int rec_index_array[2] = {1, 0};
	PackParamInfo("rec", rec_items_array, 2, rec_index_array, info);
#if 0
	const char *ir_mode_items_array[3] = { "off", "on", "auto" };;
	const int ir_mode_index_array[3] = {0, 1, 2};
	PackParamInfo("ir_mode", ir_mode_items_array, 3, ir_mode_index_array, info);
#endif	

#if VOICE_CONTROL_EN
	const char *voice_control_items_array[2] = {"on", "off"};
	const int voice_control_index_array[2] = {1, 0};
	PackParamInfo("voice_control", voice_control_items_array, 2, voice_control_index_array, info);
#endif
	const char* keytone_items_array[2] = { "on", "off" };
	const int keytone_index_array[2] = { 1, 0 };
	PackParamInfo("key_tone", keytone_items_array, 2, keytone_index_array, info);

	//缩时录影时长
	const char* park_record_time_items_array[4] = { "off", "8H", "16H", "24H" };
	const int park_record_time_index_array[4] = { 0, 8, 16, 24 };
	if (g_acc_connected){
		
	    PackParamInfo("park_record_time", park_record_time_items_array, 4, park_record_time_index_array, info);
	}

	cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	//http_body长度不要超过4K
	strcpy(res_str, http_body);
	free(http_body);
	return 0;
}

int ProcessGetDevAttr()
{
	WIFI_MAC_PARAM_S net_parm;
    memset(net_parm.ssid, 0, sizeof(net_parm.ssid));
    memset(net_parm.mac, 0, sizeof(net_parm.mac));
    GetMacParam(&net_parm);
    std::string ssid = net_parm.ssid;
    std::string mac = net_parm.mac;
    mac.erase(remove_if(mac.begin(), mac.end(), [](char ch) {return ch == ':'; }), mac.end());

    cJSON* info = cJSON_CreateObject();
    cJSON_AddStringToObject(info, "uuid", mac.c_str());
    cJSON_AddStringToObject(info, "softver", GlobalData::Instance()->version().c_str());
    cJSON_AddStringToObject(info, "otaver", "v1.20230312.1");
    cJSON_AddStringToObject(info, "hwver", "v1.2");
    cJSON_AddStringToObject(info, "ssid", ssid.c_str());
    cJSON_AddStringToObject(info, "bssid", mac.c_str());
    if (!MppMdl::Instance()->AdLoss()) {
        cJSON_AddNumberToObject(info, "camnum", 2);
    }
    else {
        cJSON_AddNumberToObject(info, "camnum", 1);
    }
    
    cJSON_AddNumberToObject(info, "curcamid", 0);
#if 1//X2V60_S_DEBUG1
    cJSON_AddNumberToObject(info, "wifireboot", 1);
#endif

    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(g_engineId, g_connId, 0, http_body, strlen(http_body));
    free(http_body);
	return 0;
}

//中间件sdk事件回调
void OnMWEventCallBack(long handle, XMEventType event_type, const char* msg, int param, int64_t user, char* out_msg)
{
	//app来获取参数能力集，同步返回
	if (XM_EVENT_APP_GET_PARAM == event_type) {
		XMLogI("ProcessGetParam");
		ProcessGetParam(out_msg);
		return;
	}

	//app来获取设备属性，可以异步处理，直接返回，等设备切换到预览模式再回复
	if (XM_EVENT_APP_GET_DEVATTR == event_type) {
		XMLogI("ProcessGetDevAttr");
		ProcessGetDevAttr();
		return;
	}

	if (XM_EVENT_CATCH_PIC == event_type) {
		GlobalPage::Instance()->page_main()->PlaySdCardStatus();
		if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
			std::string sound_file = kAudioPath;
			sound_file += "Picture success_16k.pcm";
			MppMdl::Instance()->PlaySound(sound_file.c_str());
			MppMdl::Instance()->GetPicJPEG(-1);
			UsbDevice::Instance()->CatchPic();
		}
		return;
	}

	if (XM_EVENT_USBCAM_ForceIFrame == event_type) {
		UsbDevice::Instance()->ForceIFrame(0, 0);
		return;
	}

	if (GlobalPage::Instance()->page_main()->compact_recording_
		&& (event_type == XM_EVENT_POWERKEY_LONGPRESS || event_type == XM_EVENT_POWERKEY_SHORTPRESS)) {
		XMLogW("Retracting video recording!");
		return;
	}
#if GPS_EN	
    if (XM_EVENT_RECORDFILE_START == event_type) {
		XMLogI("XM_EVENT_RECORD_START, channel=%d", handle);
		if(handle==0){
		write_start=1;
		}else if(handle==1){
		write_startback=1;
		}
		return;
	}
	if (XM_EVENT_RECORDFILE_END == event_type) {
        XMLogI("XM_EVENT_RECORDFILE_END, channel=%d, file name=%s", handle, msg); //msg.c_str()
			if(handle==0){
				write_start=0;
				get_gps_data(msg);
			}else if(handle==1){
				write_startback=0;
				get_gps_back_data(msg);
			}	
		return;
    }
#endif
	MWEventInfo event_info;
	event_info.handle = handle;
	event_info.event_type = event_type;
	event_info.param = param;
	event_info.msg = msg;
	g_event_mutex.lock();
	g_mw_event_infos.push_back(event_info);
	g_event_mutex.unlock();

	if (event_type == XM_EVENT_KEY_SHUTDOWN) {
		XMLogI("XM_EVENT_KEY_SHUTDOWN power = %d", param);
	}

	if (event_type >= XM_EVENT_KEYWORD_XIANSHIQIANLU && event_type <= XM_EVENT_KEYWORD_GUANBIREDIAN) {
		// if (event_type != XM_EVENT_KEYWORD_GUANBIPINGMU) {
		// 	GlobalPage::Instance()->page_main()->OpenScreen();
		// }

		/* if (event_type != XM_EVENT_KEYWORD_ZHUAPAIZHAOPIAN) {
			std::string sound_file = kAudioPath;
			sound_file += "haode_16k.pcm";
			MppMdl::Instance()->PlaySound(sound_file.c_str());
		} */
	}
}

void OnMWLogCallback(char* log_buf, int log_len, int64_t user)
{
	// LogFileManager::Instance()->Write(log_buf);
	LogFileManager::Instance()->get_log_data(log_buf,log_len,user);
}

#if OSD_SHOW_ADJUST
extern int osd_time_ofs_y;
#endif
int ProcessEvent(long handle, XMEventType event_type, const std::string& msg, int param) 
{
 	switch (event_type)
    {
    case XM_EVENT_Play_Over:
        XMLogI("play over, handle=%ld\n", handle);
        break;
    case XM_EVENT_DEV_ADD:
		{
			XMLogI("SD ADD, sd status=%d", g_sd_status);
			if (g_sd_status == XM_SD_NORMAL) {
				XMLogW("SD card is detected to be inserted! g_sd_status = %d", g_sd_status);
				break;
			}
			
			g_sd_status = XM_Middleware_Storage_MountSDCard(XM_STORAGE_SDCard_0);
			if (g_sd_status == XM_SD_NORMAL) {
				GlobalPage::Instance()->page_main()->SDCardImg();
				int write_speed = 0;
				int ret = XM_Middleware_Storage_GetSpeedState(XM_STORAGE_SDCard_0, &write_speed);
				XMLogI("write_speed, ret=%d, speed=%d", ret, write_speed);
				GlobalData::Instance()->SDCard_write_speed_ = write_speed;
				if (ret < 0) {
					g_sd_status = XM_SD_NEEDCHECKSPEED;
				}
				else {
					g_sd_status = XM_Middleware_Storage_CheckFileSystem(XM_STORAGE_SDCard_0);
					if (g_sd_status == XM_SD_NORMAL) {
						if (write_speed >= 1024) {
							if(!g_app_connect){
								
							    GlobalPage::Instance()->page_main()->StartRecord();
							}
							HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, 0, GlobalData::Instance()->SDCard_write_speed_);
						}
						else {
							HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, 11, GlobalData::Instance()->SDCard_write_speed_);
						}
					}
				}
			}
			
			if (g_sd_status != XM_SD_NORMAL) {
				int status = 0;
				if (XM_SD_NOEXIST == g_sd_status) {
					status = 2;
				}
				else if (XM_SD_ABNORMAL == g_sd_status) {
					status = 3;
				}
				else if (XM_SD_NEEDFORMAT == g_sd_status) {
					status = 1;
				}
				else if (XM_SD_NOTRW == g_sd_status) {
					status = 10;
				}
				else if (XM_SD_NEEDCHECKSPEED == g_sd_status) {
					status = 1;
				}
				else if (XM_SD_NOSPACE == g_sd_status) {
					status = 12;
				}
				else {
					status = 99;
				}
				HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, status, GlobalData::Instance()->SDCard_write_speed_);
			}
			GlobalPage::Instance()->page_main()->SDCardImg();
			GlobalPage::Instance()->page_main()->PlaySdCardStatus();
		}
        break;
	case XM_EVENT_SDCARD_READ_ONLY:
		{
#if 1//X2V60_S_DEBUG1
    		//卡修复
    		XM_CONFIG_VALUE get_cfg_value;
    		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_NEED_REPAIR_SDCARD, get_cfg_value);
    		
    		get_cfg_value.bool_value = true;
    		GlobalData::Instance()->car_config()->SetValue(CFG_Operation_NEED_REPAIR_SDCARD, get_cfg_value);
			XMLogI("READ_ONLY, g_sd_status=%d \r\n", g_sd_status);
#endif
        break;
	    }
    case XM_EVENT_DEV_REMOVE:
		{
			//缩时录影时拔出SD卡，直接关机
			if (GlobalPage::Instance()->page_main()->compact_recording_) {
				GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_Acc, true, true);
				break;
			}
			
			LogFileManager::Instance()->Enable(false);
			XMLogI("SD REMOVE");
#if 1//X2V60_S_DEBUG1
			//卡修复
			XM_CONFIG_VALUE get_cfg_value;
			GlobalData::Instance()->car_config()->GetValue(CFG_Operation_NEED_REPAIR_SDCARD, get_cfg_value);
			
			get_cfg_value.bool_value = true;
			GlobalData::Instance()->car_config()->SetValue(CFG_Operation_NEED_REPAIR_SDCARD, get_cfg_value);
#endif
			g_sd_status = XM_SD_NOEXIST;
			HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, 2, GlobalData::Instance()->SDCard_write_speed_);
			GlobalPage::Instance()->page_main()->CloseRecord();
			GlobalPage::Instance()->page_main()->SDCardImg();
			GlobalPage::Instance()->page_main()->PlaySdCardStatus();
		}
        break;
    case XM_EVENT_USB_ADD:
		{
		}
		break;
	case XM_EVENT_USB_REMOVE:
		{
		}
		break;
     case XM_EVENT_LOCALALARM_START:
		{
			XMLogI("LocalAlarm Start chn:%d", handle);
		}
     	break;
     case XM_EVENT_LOCALALARM_STOP:
		{
			XMLogI("LocalAlarm Stop chn:%d", handle);
		}
     	break;
	case XM_EVENT_KEYWORD_XIANSHIQIANLU:
		{
		}
		break;
	case XM_EVENT_KEYWORD_XIANSHIHOULU:
		{
		}
		break;
	case XM_EVENT_KEYWORD_CHAKANQUANBU:
		{
		}
		break;
	case XM_EVENT_KEYWORD_DAKAIPINGMU:
		{
			XMLogI("open screen");
			// GlobalPage::Instance()->page_main()->OpenScreen();
		}
		break;
	case XM_EVENT_KEYWORD_GUANBIPINGMU:
		{
			XMLogI("close screen");
			// GlobalPage::Instance()->page_main()->CloseScreen();
		}
		break;
	case XM_EVENT_KEYWORD_ZHUAPAIZHAOPIAN:
		{
			GlobalPage::Instance()->page_main()->PlaySdCardStatus();
			if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
				std::string sound_file = kAudioPath;
				sound_file += "Picture success_16k.pcm";
				MppMdl::Instance()->PlaySound(sound_file.c_str());
				MppMdl::Instance()->GetPicJPEG(-1);
				UsbDevice::Instance()->CatchPic();
			}
		}
		break;
	case XM_EVENT_KEYWORD_DAKAILUYIN:
		{
			XMLogI("da kai lu yin");
			GlobalPage::Instance()->page_main()->RecordVoiceEnable(true);
			HTTPSerMdl::Instance()->tcpMictatus(g_engineId, 1);
		}
		break;
	case XM_EVENT_KEYWORD_GUANBILUYIN:
		{
			XMLogI("guan bi lu yin");
			GlobalPage::Instance()->page_main()->RecordVoiceEnable(false);
			HTTPSerMdl::Instance()->tcpMictatus(g_engineId, 0);
		}
		break;
	case XM_EVENT_KEYWORD_DAKAIREDIAN:
		{
			XMLogI("da kai re dian");
			// if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape && !GlobalPage::Instance()->page_main()->wifi_enable_
			// 	&& GlobalPage::Instance()->page_main()->wifi_prepared_) {
			// 	lv_event_send(GlobalPage::Instance()->page_main()->wifi_img_, LV_EVENT_CLICKED, NULL);
			// }
		}
		break;
	case XM_EVENT_KEYWORD_GUANBIREDIAN:
		{
			XMLogI("guan bi re dian");
			// if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape && GlobalPage::Instance()->page_main()->wifi_enable_
			// 	&& GlobalPage::Instance()->page_main()->wifi_prepared_) {
			// 	lv_event_send(GlobalPage::Instance()->page_main()->wifi_img_, LV_EVENT_CLICKED, NULL);
			// }
		}
		break;
	case XM_EVENT_KEYWORD_JINJILUXIANG:
	case XM_EVENT_KEYWORD_WOYAOLUXIANG:
		{
			GlobalPage::Instance()->page_main()->StartRecord();
			GlobalPage::Instance()->page_main()->PlaySdCardStatus();

			if (event_type == XM_EVENT_KEYWORD_JINJILUXIANG) {
				XMLogI("jin ji lu xiang");
				if (GlobalPage::Instance()->page_main()->lock_current_recording_file_ == false) 
					GlobalPage::Instance()->page_main()->LockCurrentFile();
			} else {
				XMLogI("wo yao lu xiang");
			}
		}
		break;
	case XM_EVENT_PLAY_First_I_frame:
		{
		}
		break;
	case XM_EVENT_VOLTAGE_LOW:
		{
		}

	case XM_EVENT_KEY_SHUTDOWN:
		{
			XMLogI("XM_EVENT_KEY_SHUTDOWN");
			GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_PressKey, true, true);
		}
		break;
	case XM_EVENT_KEY_REBOOT:
		{
			XMLogI("XM_EVENT_KEY_REBOOT");
			GlobalPage::Instance()->page_main()->Reboot(true);
		}
		break;
	case XM_EVENT_USB_DISCONNECT:
		{
			XMLogI("XM_EVENT_USB_DISCONNECT");
			//有USB断开则判断为是车充
			// GlobalPage::Instance()->page_main()->ChangeCarChargerCfg(true);
			// GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_USBDisconnect, true, true);
		}
		break;
	case XM_EVENT_USB_CONNECT:
		{
			XMLogI("XM_EVENT_USB_CONNECT");
			//USB连接则不是电池供电
			// GlobalPage::Instance()->page_main()->battery_powered_ = false;
			// GlobalPage::Instance()->page_main()->check_battery_ = false;
			// lv_obj_t* battery_img = GlobalPage::Instance()->page_main()->battery_img_;
			// if (battery_img && strcmp((char*)lv_img_get_src(battery_img), image_path"i_char.png"))
			// 	lv_img_set_src(battery_img, image_path"i_char.png");
		}
		break;
	case XM_EVENT_AD_EXIT:
		{
			XMLogI("XM_EVENT_AD_EXIT");
			GlobalPage::Instance()->page_main()->OnADEvent(false);
			HTTPSerMdl::Instance()->tcpCameraStatus(g_engineId, 0, 0, 1);//2
			// if (GlobalPage::Instance()->page_main()->wifi_enable_) {
			// 	MppMdl::Instance()->SubStreamEnable(true);
			// }
			//ad被拔出，且当前app正在预览ad画面时，自动切到前摄
			if (g_realplay_channel == 1) {//2
				g_wait_IFrame = true;
				g_realplay_channel = 0;	
			}
		}
		break;
	case XM_EVENT_AD_CONNECT:
		{
			XMLogI("XM_EVENT_AD_CONNECT");
			GlobalPage::Instance()->page_main()->OnADEvent(true);
			HTTPSerMdl::Instance()->tcpCameraStatus(g_engineId, 1, g_realplay_channel, 2);//3
			// if (GlobalPage::Instance()->page_main()->wifi_enable_) {
			// 	MppMdl::Instance()->SubStreamEnable(true);
			// }
		}
		break;
	case XM_EVENT_ACC_DISCONNECT:
		{
			XMLogI("XM_EVENT_ACC_DISCONNECT");
			g_acc_connected = false;
			//有acc连接或断开则判断为不是车充
			GlobalPage::Instance()->page_main()->ChangeCarChargerCfg(false);
			GlobalPage::Instance()->page_main()->acc_disconnect_ = true;
			GlobalPage::Instance()->page_main()->AccDisconnectWork();
		}
		break;
	case XM_EVENT_ACC_CONNECT:
		{
			XMLogI("XM_EVENT_ACC_CONNECT");
			g_acc_connected = true;
			//有acc连接或断开则判断为不是车充
			GlobalPage::Instance()->page_main()->ChangeCarChargerCfg(false);
			GlobalPage::Instance()->page_main()->acc_disconnect_ = false;
			GlobalPage::Instance()->page_main()->AccConnectWork();
		}
		break;
	case XM_EVENT_POWERKEY_SHORTPRESS:
		{
			XMLogI("XM_EVENT_POWERKEY_SHORTPRESS");
			if (GlobalData::Instance()->key_tone_) {
				std::string sound_file = kAudioPath;
				sound_file += "dianji_16k.pcm";
				MppMdl::Instance()->PlaySound(sound_file.c_str());
			}
			if (GlobalPage::Instance()->page_main()->lock_current_recording_file_ == false)
				GlobalPage::Instance()->page_main()->LockCurrentFile();
		}
		break;
	case XM_EVENT_POWERKEY_LONGPRESS:
		{
			XMLogI("XM_EVENT_POWERKEY_LONGPRESS");
			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = WIFI_TYPE_5G;
			GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Wifi_Type, cfg_value);
			if (cfg_value.int_value == WIFI_TYPE_5G) {
				cfg_value.int_value = WIFI_TYPE_2_4G;
			}
			else {
				cfg_value.int_value = WIFI_TYPE_5G;
			}

			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Wifi_Type, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Wifi_Type");
			}
			GlobalPage::Instance()->page_main()->WifiEnable(false);
			GlobalPage::Instance()->page_main()->WifiEnable(true);
		}
		break;
	case XM_EVENT_COLLISION_DETECTED:
		{
			XMLogI("XM_EVENT_COLLISION_DETECTED, lock_current_recording_file_=%d",
				GlobalPage::Instance()->page_main()->lock_current_recording_file_);
			if (GlobalPage::Instance()->page_main()->lock_current_recording_file_ == false
				&& !GlobalPage::Instance()->page_main()->compact_recording_)
				GlobalPage::Instance()->page_main()->LockCurrentFile();
		}
		break;
	case XM_EVENT_RECORD_END:
		{
			XMLogI("XM_EVENT_RECORD_END");
			if (GlobalPage::Instance()->page_main()->red_circle_img_ && !GlobalPage::Instance()->page_main()->compact_recording_) {
				XMLogI("red_circle_img_, red");
				lv_img_set_src(GlobalPage::Instance()->page_main()->red_circle_img_, image_path"red_circle.png");
			}

			GlobalPage::Instance()->page_main()->lock_current_recording_file_ = false;
			//定期保存系统时间
			GlobalData::Instance()->SetCurTime(time(NULL));
			#if 1//X2V60_S_DEBUG1
			XM_Middleware_Storage_SetFileSwtichDelayTime(200);
			#endif
		}
		break;
	case XM_EVENT_SDCARD_NOENOUGH_SPACE:
		{
			XMLogI("XM_EVENT_SDCARD_NOENOUGH_SPACE");
			//缩时录影时SD卡没有空间直接关机
			if (GlobalPage::Instance()->page_main()->compact_recording_) {
				GlobalPage::Instance()->page_main()->OpenTipBox("SD card space is insufficient", 4000);
				lv_timer_create(GlobalPage::Instance()->page_main()->DelayShutDownTimer, 1000, NULL);
				break;
			}
			// //碰撞录影时若SD卡空间不足且熄火直接关机
			// if (GlobalPage::Instance()->page_main()->collision_startup_) {
			// 	int ret = GlobalPage::Instance()->page_main()->CollisionStartupWork(CollisionStartup_SDCardError);
			// 	if (ret < 0) 
			// 		break;
			// }

			GlobalPage::Instance()->page_main()->CloseRecord();
			GlobalPage::Instance()->page_main()->OpenTipBox("SD card space is insufficient");
		}
		break;
	case XM_EVENT_REVERSE_DETECTED:
		{
			// XMLogI("XM_EVENT_REVERSE_DETECTED");
			// GlobalPage::Instance()->page_main()->ReverseEvent(true);
		}
		break;
	case XM_EVENT_REVERSE_END:
		{
			// XMLogI("XM_EVENT_REVERSE_END");
			// GlobalPage::Instance()->page_main()->ReverseEvent(false);
		}
		break;
	case XM_EVENT_CHANGE_TIME:
		{
			XMLogI("XM_EVENT_CHANGE_TIME");
			GlobalPage::Instance()->page_main()->CloseRecord();
			GlobalPage::Instance()->page_main()->StartRecord();
			time_t t1 = time(NULL);
			GlobalPage::Instance()->page_main()->pre_sys_time_ = t1;
			//子设备也同步下时间
			UsbDevice::Instance()->SyncSystemTime();
			//保存系统时间到配置文件
			GlobalData::Instance()->SetCurTime(time(NULL));
		}
		break;
	case XM_EVENT_RECORD_START:
		{
			XMLogI("XM_EVENT_RECORD_START");
			g_wait_IFrame = true;
			GlobalPage::Instance()->page_main()->StartRecord();
		}
		break;
	case XM_EVENT_RECORD_CLOSE:
		{
			XMLogI("XM_EVENT_RECORD_CLOSE");
			GlobalPage::Instance()->page_main()->CloseRecord();
		}
		break;
	case XM_EVENT_RECORDFILE_START:
		{
			XMLogI("XM_EVENT_RECORD_START, channel=%d", handle);
		
		}
		break;
	case XM_EVENT_RECORDFILE_END:
		{
			XMLogI("XM_EVENT_RECORDFILE_END, channel=%d, file name=%s", handle, msg.c_str());
		}
		break;	
	case XM_EVENT_RECORD_SOUND_ENABLE:
		{
			XMLogI("XM_EVENT_RECORD_SOUND_ENABLE  param==%d",param);
			bool enable = (param == 0) ? false : true;
			GlobalPage::Instance()->page_main()->RecordVoiceEnable(enable);
		}
		break;
	case XM_EVENT_VOICE_CONTROL_ENABLE:
		{
			XMLogI("XM_EVENT_VOICE_CONTROL_ENABLE");
			bool enable = (param == 0) ? false : true;
			XM_Middleware_Smart_EnableKeyword(enable);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = enable;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Voice_Control, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Voice_Control");
			}
		}
		break;
	case XM_EVENT_SDCARD_FORMAT:
		{
			XMLogI("XM_EVENT_SDCARD_FORMAT");
			GlobalPage::Instance()->page_main()->sd_formatting_ = !GlobalPage::Instance()->page_main()->sd_formatting_;
			GlobalData::Instance()->SDCard_write_speed_ = param;
			if (!GlobalPage::Instance()->page_main()->sd_formatting_) {
				std::string sound_file = kAudioPath;
				sound_file += "Formatting is completed_16k.pcm";
				MppMdl::Instance()->PlaySound(sound_file.c_str());
			}

			// SD卡状态变化后，更新UI图标显示
			if (g_sd_status != XM_SD_NEEDFORMAT)
			{
				GlobalPage::Instance()->page_main()->SDCardImg();
			}
		}
		break;
	case XM_EVENT_CAMERA_SWITCH:
		{
			g_wait_IFrame = true;
			XMLogI("XM_EVENT_CAMERA_SWITCH, camera = %d", param);
			g_realplay_channel = param;	
		}
		break;
	case XM_EVENT_CFG_RESET:
		{
			XMLogI("XM_EVENT_CFG_RESET");
			
			XM_Middleware_SafeSystem("rm /mnt/mtd/Config/hostapd.conf");
			GlobalData::Instance()->car_config()->SetDefaultConfig();
			
			std::string sound_file = kAudioPath;
			sound_file += "Factory default success_16k.pcm";
			MppMdl::Instance()->PlaySound(sound_file.c_str());

			MppMdl::Instance()->StopADProc();
			MppMdl::Instance()->StopEncProc();
			XM_Middleware_Exit();

#if 1//X2V60_S_DEBUG1
    		usleep(3 * 1000 * 1000);
#endif
			reboot(RB_AUTOBOOT);
		}
		break;
	case XM_EVENT_CHANGE_RESOLUTION:
		{
			XMLogI("XM_EVENT_CHANGE_RESOLUTION, resolution = %d", param);
			XM_CONFIG_VALUE cfg_value;
			XMUIEventInParam event_inparam;
			XMUIEventOutParam event_outparam;

			if (param == 2144) {
				event_inparam.set_enc.width = 3840;
				event_inparam.set_enc.height = 2144;
				event_inparam.set_enc.bit_rate = 20480;
				cfg_value.int_value = 2144;
			}
			else if (param == 1440) {
				event_inparam.set_enc.width = 2560;
				event_inparam.set_enc.height = 1440;
				event_inparam.set_enc.bit_rate = 20480;
				cfg_value.int_value = 1440;
			}
			else if (param == 1080) {
				event_inparam.set_enc.width = 1920;
				event_inparam.set_enc.height = 1080;
				event_inparam.set_enc.bit_rate = 4096;
				cfg_value.int_value = 1080;
			}

			event_inparam.set_enc.video_codec_type = PT_H265;
			event_inparam.set_enc.channel = Direction_Front;

			GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Video_Resolution, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Video_Resolution");
			}
			
			MppMdl::Instance()->SubStreamEnable(true);
			
			#if 0//OSD_SHOW_ADJUST
			
			if(osd_time_ofs_y == 100){
				
				osd_time_ofs_y = 450;
			}else{
			
			    osd_time_ofs_y = 100;
			}

			MppMdl::Instance()->EnableOsdTime(4,1, 128, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight + osd_time_ofs_y); //APP时间水印
			
			#else
			osd_data_init();
			#endif
		}
		break;
	case XM_EVENT_REC_SPLIT_DUR:
		{
			XMLogI("XM_EVENT_REC_SPLIT_DUR, param = %d", param);
			XMStorageRecordDuration dur = XM_STORAGE_Record_Duration_60;
			if (param == 1) {
				dur = XM_STORAGE_Record_Duration_60;
			}
			else if (param == 2) {
				dur = XM_STORAGE_Record_Duration_120;
			}
			else if (param == 3) {
				dur = XM_STORAGE_Record_Duration_180;
			}
			XM_Middleware_Storage_SetRecordDuration(dur);

			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Circular_Record_Time, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Circular_Record_Time");
			}
		}
		break;
	case XM_EVENT_GSR_SENSITIVITY:
		{
			XMLogI("XM_EVENT_GSR_SENSITIVITY, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			if (param == 0)
				cfg_value.int_value = Sensitivity_Close;
			else if (param == 1)
				cfg_value.int_value = Sensitivity_Low;
			else if (param == 2)
				cfg_value.int_value = Sensitivity_Mid;
			else if (param == 3)
				cfg_value.int_value = Sensitivity_High;
			
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Collision_Sensitivity, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Collision_Sensitivity");
			}

			PeripheryManager::Instance()->SensitivitySet(cfg_value.int_value);
			if (cfg_value.int_value == Sensitivity_Close) {
				PeripheryManager::Instance()->GsensorModeSelect(2);
			}
			else {
				PeripheryManager::Instance()->GsensorModeSelect(0);
			}
		}
		break;
	case XM_EVENT_SCREEN_STANDBY:
		{

		}
		break;
	case XM_EVENT_PARKING_MONITOR:
		{
			XMLogI("XM_EVENT_PARKING_MONITOR, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Collision_Startup, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Collision_Startup");
			}
		}
		break;
	case XM_EVENT_TIMELAPSE_RATE:
		{
			XMLogI("XM_EVENT_TIMELAPSE_RATE, param = %d", param);
			XMTimeLapse time_lapse;
			if (param == 0)
				time_lapse = XM_TIME_LAPSE_Closed;
			else if (param == 1)
				time_lapse = XM_TIME_LAPSE_Frame_1;
			else if (param == 2)
				time_lapse = XM_TIME_LAPSE_Frame_2;
			else if (param == 5)
				time_lapse = XM_TIME_LAPSE_Frame_5;

			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Compact_Record_Fps");
			}
			XM_Middleware_ParkRecord_SetTimeLapse(time_lapse);
		}
		break;
	case XM_EVENT_OSD_ENABLE:
		{
			XMLogI("XM_EVENT_OSD_ENABLE, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = param == 1 ? true : false;
			//MppMdl::Instance()->EnableOsdTime(cfg_value.bool_value, 0, 8192);
			UsbDevice::Instance()->SetOSDenable(cfg_value.bool_value);
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Time_Watermark, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Time_Watermark");
			}
			osd_data_init();
		}
		break;
	case XM_EVENT_CHANGE_WIFI_PARAM:
		{
			XMLogI("XM_EVENT_CHANGE_WIFI_PARAM, param = %d", param);
			if (GlobalPage::Instance()->page_main()->wifi_id_label_) {
				WIFI_MAC_PARAM_S wifi_param;
				memset(wifi_param.ssid, 0, sizeof(wifi_param.ssid));
				memset(wifi_param.mac, 0, sizeof(wifi_param.mac));
				GetMacParam(&wifi_param);

				lv_label_set_text(GlobalPage::Instance()->page_main()->wifi_id_label_, wifi_param.ssid);
			}
		}
		break;
	case XM_EVENT_NETWORK_MSG:
		{
			if (GlobalPage::Instance()->page_main()->judge_app_timer_) {
				lv_timer_reset(GlobalPage::Instance()->page_main()->judge_app_timer_);
			}
			else {
				GlobalPage::Instance()->page_main()->WIFIImg();
				GlobalPage::Instance()->page_main()->judge_app_timer_ = lv_timer_create(GlobalPage::Instance()->page_main()->JudgeAppConnectTimer
					, 16 * 1000, NULL);
			}
		}
		break;
	case XM_EVENT_KEYTONE_ENABLE:
		{
			XMLogI("XM_EVENT_KEYTONE_ENABLE, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = param == 1 ? true : false;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Key_Voice, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Key_Voice");
			}

			GlobalData::Instance()->key_tone_ = cfg_value.bool_value;
		}
		break;
	case XM_EVENT_IR_MODE:
		{
			XMLogI("XM_EVENT_IR_MODE, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			// param值可以是0(off),1(on),2(auto);
			UsbDevice::Instance()->SetIRMode(param);

			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_IR_Mode, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=XM_EVENT_IR_MODE");
			}
		}
		break;
#if 1//def CT317_OLD_SDK_TRANS
	case XM_EVENT_SPEAKER_VOLUME:
		{
			XMLogI("XM_EVENT_SPEAKER_VOLUME, param = %d", param);
			XM_CONFIG_VALUE cfg_value;

			//XM_Middleware_Sound_SetVolume(param * 100 / 4);
			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Speaker_Volume, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=XM_EVENT_SPEAKER_VOLUME");
			}

			int sound_val = 0;
			switch(cfg_value.int_value)
			{
				case 0:
					sound_val = 0;
					break;
				case 1:
					sound_val = 33;
					break;
				case 2:
					sound_val = 66;
					break;
				case 4:
					sound_val = 100;//100 150
					break;
				default:
					sound_val = 0;
					break;
					
			}
			//XM_Middleware_Sound_SetVolume(sound_val);
			MppMdl::Instance()->SetVolume(sound_val);
		}
		break;
#endif
	case XM_EVENT_PARK_RECORD_TIME:
		{
			XMLogI("XM_EVENT_PARK_RECORD_TIME, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Compact_Record_Duration");
			}

			XMTimeLapseDuration dur = XM_TIME_LAPSE_Duration_Closed;
			if (param == 0) {
				dur = XM_TIME_LAPSE_Duration_Closed;
			}
			else if (param == 8) {
				dur = XM_TIME_LAPSE_Duration_8;
			}
			else if (param == 16) {
				dur = XM_TIME_LAPSE_Duration_16;
			}
			else if (param == 24) {
				dur = XM_TIME_LAPSE_Duration_24;
			}

			XM_Middleware_ParkRecord_SetTimeLapseDuration(dur);
		}
		break;
	case XM_EVENT_APP_PAGE:
		{
			XMLogI("XM_EVENT_APP_PAGE, param = %d", param);
			GlobalData::Instance()->app_page_ = (APP_PAGE)param;
		}
		break;
	default:
        break;
    }
	return 0;
}

//中间件sdk回放流回调
bool OnMWVODStreamCallBack(long handle, XM_MW_Media_Frame* media_frame, int64_t user)
{
    return true;
}

uint8_t g_audio_enc_buffer[2048];

//MPP实时预览数据回调
void OnMWRealTimeStreamCallBack(int channel, int stream, XM_MW_Media_Frame* media_frame, int64_t user)
{
	if (GlobalPage::Instance()->page_main()->connect_computer_) {
		GlobalPage::Instance()->page_usb()->OnMWRealTimeStreamCallBack(channel, stream, media_frame, user);
		return;
	}
	if (!g_realplay)
		return;

	if (channel == 0 && stream == 0 && !media_frame->video && !g_wait_IFrame) {
		//推送音频
		int out_len;
		g711a_Encode((char*)media_frame->frame_buffer, (char*)g_audio_enc_buffer, media_frame->frame_size, &out_len);
		media_frame->frame_buffer = g_audio_enc_buffer;
		media_frame->frame_size = out_len;
		HTTPSerMdl::Instance()->OnMWRealTimeStream(channel, 0, media_frame, user);
		return;
	}

	if ((channel == 0 && g_realplay_channel != 0) || (channel == 1 && g_realplay_channel != 1)) { //2
		return;
	}
	if (g_realplay_channel == 0) {
		if (stream != 1)
			return;
	}

	if (g_wait_IFrame) {
		if (media_frame->key_frame) {
			g_wait_IFrame = false;
		}
		else {
			return;
		}
	}
	HTTPSerMdl::Instance()->OnMWRealTimeStream(channel, 0, media_frame, user);
}

//UI界面事件回调
int OnUIEventCallback(XMUIEventType ui_event_type, XMUIEventInParam* in_param, XMUIEventOutParam* out_param)
{
	if (XM_UI_VOD_GET_CURRENT_DURATION != ui_event_type)
		XMLogI("ui_event_type=%d", ui_event_type);

	switch (ui_event_type)
	{
	case XM_UI_START_STORAGE:
		{
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_MW_Media_Info media_info;
				//内录的媒体信息不从mpp模块获取
				if (channel != 2) {
					MppMdl::Instance()->GetEncInfo(channel, 0, &media_info);
					media_info.frame_rate = i == 0 ? kFrameRate : 25;	
					XM_Middleware_Storage_SetMediaInfo(channel, &media_info);
				}
				XM_Middleware_Storage_Start(channel, 0);
			}
			g_storage_started = true;
		}
		break;
	case XM_UI_STOP_STORAGE:
		{
			g_storage_started = false;
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_Middleware_Storage_Stop(channel);
			}
		}
		break;
	case XM_UI_QUERY_RECORD:
		XM_Middleware_Playback_Query(&in_param->query_record.rec_query, &out_param->query_record.rec_list);
		break;
    case XM_UI_VOD_OPEN:
		{
			XMStorageSDCardNum sdcard_num = in_param->vod_start.sdcard_num;
			int channel = in_param->vod_start.channel;
			const char* file_full_name = (const char*)in_param->vod_start.file_full_name;
			XMFileFormat file_format = in_param->vod_start.file_format;
			long handle = XM_Middleware_Playback_Open(sdcard_num, channel, file_format, file_full_name);
			//返回播放句柄
			out_param->vod_start.handle = handle;
		}
		break;
	case XM_UI_VOD_GET_FILE_INFO:
		{
			XM_MW_File_Info file_info;
			long handle = in_param->vod_getfileinfo.handle;
			XM_Middleware_Playback_GetFileInfo(handle, &file_info);
			XMLogI("XM_Middleware_Playback_GetFileInfo, width=%d, height=%d, rate=%d, duration=%d", 
				file_info.media_info.width, file_info.media_info.height, file_info.media_info.frame_rate, file_info.duration);
			//返回文件长度，单位毫秒
			out_param->vod_getfileinfo.duration = file_info.duration;
			out_param->vod_getfileinfo.media_info = file_info.media_info;
		}
		break;
    case XM_UI_VOD_PLAY:
		{
			long handle = in_param->vod_play.handle;
			bool play_one = in_param->vod_play.play_one;
			RECT_S rc = in_param->vod_play.dst_rc; //回放界面位置和大小
			XM_Middleware_Playback_Play(handle, play_one, &rc);
		}
		break;
    case XM_UI_VOD_PAUSE:
		{
			long handle = in_param->vod_pause.handle;
			bool pause = in_param->vod_pause.pause;
			XM_Middleware_Playback_Pause(handle, pause);
		}
		break;
    case XM_UI_VOD_SEEK:
		{
			long handle = in_param->vod_seek.handle;
			int seek_time = in_param->vod_seek.seek_time;
			XM_Middleware_Playback_Seek(handle, seek_time);
		}
		break;
	case XM_UI_VOD_GET_CURRENT_DURATION:
		{
			long handle = in_param->vod_getcurduration.handle;
			int cur_duration = XM_Middleware_Playback_GetCurrentDuration(handle);
			out_param->vod_getcurduration.cur_duration = cur_duration;
		}
		break;
	case XM_UI_VOD_STOP:
		{
			long handle = in_param->vod_stop.handle;
			XM_Middleware_Playback_Close(handle);
		}
		break;
	case XM_UI_VOD_CLOSE:
		{
			long handle = in_param->vod_stop.handle;
			XM_Middleware_Playback_Close(handle);
			//关闭回放后，直接开启预览
			MppMdl::Instance()->StartPreview();
		}
		break;

	case XM_UI_VOD_LOCK_HISTORY_FILE:
		{
			XM_Middleware_Storage_LockHistoryFile(&in_param->vod_lockhistoryfile.file_param);
		}
		break;
	case XM_UI_VOD_UNLOCK_HISTORY_FILE:
		{
			XM_Middleware_Storage_UnLockHistoryFile(&in_param->vod_unlockhistoryfile.file_param);
		}
		break;
	case XM_UI_SET_STORAGE_CONFIG:
		{
			XMStorageRecordDuration record_duration = in_param->set_record_duraion.record_duration;
			XM_Middleware_Storage_SetRecordDuration(record_duration);
		}
		break;
	case XM_UI_SET_ENCODE_PARAM:
		{
			PAYLOAD_TYPE_E video_codec_type = in_param->set_enc.video_codec_type;
			int width = in_param->set_enc.width;
			int height = in_param->set_enc.height;
			int bit_rate = in_param->set_enc.bit_rate;
			int channel = in_param->set_enc.channel;
			SetEncodeParam(channel, video_codec_type, width, height, 25, bit_rate);//height>=2144 ? 25 : 30

			XMLogI("-------------encode:%d, width:%d, height:%d, bit_rate:%d", video_codec_type, width, height, bit_rate);
		}
		break;	
	case XM_UI_PICTURE_RENDER:
		{
		}
		break;
	case XM_UI_PICTURE_CLOSE:
		{
			MppMdl::Instance()->StartPreview();
		}
		break;
	case XM_UI_SMART_SET_AI_TYPE:
		{
			XMAIType ai_type = in_param->set_AI_type.ai_type;
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_Middleware_Smart_SetAIType(channel, ai_type);
			}
		}
		break;
	case XM_UI_SMART_SET_BSD_PARAM:
		{
			int channel = in_param->set_BSD_param.channel;
			XM_Middleware_Smart_SetBsdParam(channel, &in_param->set_BSD_param.bsd_param);

		}
		break;
	case XM_UI_SMART_ENABLE_AI:
		{
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_Middleware_Smart_EnableAI(channel, in_param->enable_AI.enable);
			}
		}
		break;
	case XM_UI_SPLIT_SET_GRID_RECT:
		{
		}
		break;
	case XM_UI_STORAGE_DEL_FILE:
		{
			XM_Middleware_Storage_DelFile(&in_param->storage_del_file.file_param);
		}
		break;
	case XM_UI_SOUND_SET_VOLUME:
		{
			MppMdl::Instance()->SetVolume(in_param->sound_set_volume.sound_volume);
		}
		break;
	case XM_UI_PREVIEW_STOP:
		{
			MppMdl::Instance()->StopPreview();
		}
		break;
	case XM_UI_MIRROR_SET_MIRROR:
		{
		}
		break;
	case XM_UI_SMART_ENABLE_KEYWORD:
		{
			bool enable = in_param->smart_enable_keyword.enable;
			XM_Middleware_Smart_EnableKeyword(enable);
		}
		break;
	default:
		return -1;
	}

	if (XM_UI_VOD_GET_CURRENT_DURATION != ui_event_type)
		XMLogI("after process event, ui_event_type=%d", ui_event_type);
	return 0;
}

// Retrieve year info
#define OS_YEAR     ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
                                     + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))
 
// Retrieve month info
#define OS_MONTH    (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
                                 : __DATE__ [2] == 'b' ? 2 \
                                 : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
                                 : __DATE__ [2] == 'y' ? 5 \
                                 : __DATE__ [2] == 'l' ? 7 \
                                 : __DATE__ [2] == 'g' ? 8 \
                                 : __DATE__ [2] == 'p' ? 9 \
                                : __DATE__ [2] == 't' ? 10 \
                                 : __DATE__ [2] == 'v' ? 11 : 12)
 
// Retrieve day info
#define OS_DAY      ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
                                 + (__DATE__ [5] - '0'))

#define OS_HOUR     ((__TIME__ [0] - '0') * 10 + (__TIME__ [1] - '0'))
 
// Retrieve minute info
#define OS_MINUTE   ((__TIME__ [3] - '0') * 10 + (__TIME__ [4] - '0'))

// Retrieve second info
#define OS_SECOND   ((__TIME__ [6] - '0') * 10 + (__TIME__ [7] - '0'))

void get_current_time(char* str)
{
	static char date_buf[22] = {0};//19
	
	memset(date_buf, 0, 22);
	sprintf(date_buf, "  %04d.%02d.%02d %02d:%02d:%02d", 
		OS_YEAR, OS_MONTH, OS_DAY, OS_HOUR, OS_MINUTE, OS_SECOND);

	memcpy(str, date_buf, 22);
}

void* timer_hander_thread(void* args)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "timer_hander_thread");
#endif	
	while (1) {
		lv_timer_handler();
		usleep(4 * 1000);
		int size = g_mw_event_infos.size();
		for (int i = 0; i < size; i++) {
			g_event_mutex.lock();
			MWEventInfo ev_info = g_mw_event_infos.front();
			g_mw_event_infos.pop_front();
			g_event_mutex.unlock();
			ProcessEvent(ev_info.handle, ev_info.event_type, ev_info.msg, ev_info.param);
		}
	}
}

bool executeCommand(const char *command)
{
	FILE *pipe = popen(command, "r");
	if (!pipe)
	{
		return false; 
	}

	char buffer[128];
	std::string result = "";
	while (!feof(pipe))
	{
		if (fgets(buffer, 128, pipe) != nullptr)
		{
			result += buffer;
		}
	}

	int status = pclose(pipe);
	if (WEXITSTATUS(status) != 0)
	{
		return false;
	}

	return true;
}

void WifiThread(void* param)
{
	bool success = false;
	int retries = 0;
	const int MAX_RETRIES = 3;
	std::string cmd_ifconfig = "/sbin/ifconfig -a";

#if 1//X2V60_S_DEBUG1 //WIFI_SPEED_DEBUG1
	//设置模拟网卡IP
	XMLogI("before set eth1 ip");
	for (int i = 0; i < 100; i++) {
		int ret = LIBCR_NET_CheckEther("eth1");
		if (ret >= 0) {
			XMLogI("find eth1");
			system(cmd_ifconfig.c_str());
			break;
		}

        #if 1//WIFI_SPEED_DEBUG1
		if(i<5 || i>=95){
			
		    XMLogI("retry, ret=%d, i=%d \r\n", ret, i);
		}
		#endif
		
		usleep(20*1000);
	}

	//eth1
	const char *command_eth_ip = "/sbin/ifconfig eth1 192.168.2.15 netmask 255.255.255.0";
	while (!executeCommand(command_eth_ip) && retries < MAX_RETRIES)
	{
		usleep(200 * 1000);
		retries++;
	}
	if (retries >= MAX_RETRIES)
	{
		XMLogE("usb dev connect error, Failed to set eth1 IP address");
		GlobalPage::Instance()->page_main()->OpenTipBox("usb dev connect error");
	}else{
		
		XMLogI("[ip netmask][eth1] retries=%d, successed! \r\n", retries);
	}
#endif
	
	const char *gwCommand = "/sbin/route add default gw 192.168.2.1";
	if (!executeCommand(gwCommand) && retries < MAX_RETRIES)
	{
		XMLogE("Failed to add default gw\n");
	}else{
		
		XMLogE("[default gw][eth1] retries=%d, sucessed! \r\n", retries);
	}
	system(cmd_ifconfig.c_str());

	UsbDevice::Instance()->Init();
	UsbDevice::Instance()->ConnectServer(kUSBDeviceIP, 3333, 2223);		
	UsbDevice::Instance()->Realplay(0, 0);
	UsbDevice::Instance()->SyncSystemTime();
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_IR_Mode, cfg_value);
	UsbDevice::Instance()->SetIRMode(cfg_value.int_value);
	XMLogI("Set IRMode = %d", cfg_value.int_value);

	//wlan0
	XMLogI("before set wlan0 ip");
	for (int i = 0; i < 100; i++) {
		int ret = LIBCR_NET_CheckEther("wlan0");
		if (ret >= 0) {
			XMLogI("find wlan0");
			break;
		}
		usleep(20*1000);
	}

	retries = 0;
	const char *command_up = "/sbin/ifconfig wlan0 up 192.168.169.1";//"/sbin/ifconfig eth2 up 192.168.169.1";
	while (!executeCommand(command_up) && retries < MAX_RETRIES)
	{
		usleep(200 * 1000);
		XMLogI("wlan0 up again");
		retries++;
	}
	if (retries >= MAX_RETRIES)
	{
		XMLogE("Failed to executeCommand");
		#if 0
		GlobalPage::Instance()->page_main()->OpenTipBox("wlan0 up error");
		return;
		#endif
	}else{
	
		XMLogI("[wlan0] retries=%d, sucessed! \r\n", retries);
	}

	system(cmd_ifconfig.c_str());
	usleep(500*1000);

	int retries_id = 0;
	XMSDK_NET_ATTR_S pstNetAttr;
	int ret = XM_Middleware_WIFI_GetEthAttr("eth2", &pstNetAttr);
	if(!ret){
	GlobalPage::Instance()->page_main()->wifi_prepared_=true;
	
	WIFI_PARAM_S wifi_param = {0};
	while(retries_id++ < 4 && ret){

    	usleep(200*1000);
		ret = XM_Middleware_WIFI_GetEthAttr("eth2", &pstNetAttr);
	}
	
	char id[5] = { 0 };
	strncat(id, &pstNetAttr.mac[12], 2);
	strncat(id, &pstNetAttr.mac[15], 2);
	//默认wifi名为CT317加mac地址后4位，密码是12345678
	std::string wifi_name = "X2V60_";
	wifi_name += id;
	strcpy(wifi_param.name, wifi_name.c_str());
	strcpy(wifi_param.password, "12345678");

	CreateWifiParamFile(&wifi_param);
	
	// XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = WIFI_TYPE_5G;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Wifi_Type, cfg_value);
	std::string command_dhcp;
	GlobalPage::Instance()->page_main()->wifi_enable_=cfg_value.int_value;
  if(cfg_value.int_value>WIFI_TYPE_OFF){
	if (cfg_value.int_value == WIFI_TYPE_5G) {
		command_dhcp = "/usr/sbin/hostapd /mnt/mtd/Config/hostapd_5G.conf -B -dddd";
	}
	else {
		command_dhcp = "/usr/sbin/hostapd /mnt/mtd/Config/hostapd.conf -B -dddd";
	}
	system(command_dhcp.c_str());
	command_dhcp = "/sbin/udhcpd -fS /usr/sbin/udhcpd.conf &";
	system(command_dhcp.c_str());
	GlobalPage::Instance()->page_main()->WIFIImg();
	XM_Middleware_Network_StartServer(80, 2222);
	}
  }
}

std::string g_upgrade_type = "upgradeAll";
void UpgraderThread(void* param)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "upgrader");
#endif
	if (g_upgrade_type == "upgradeAll" || g_upgrade_type == "upgradeSub") {
		//升级usb摄像头
		UsbDevice::Instance()->Init();
		UsbDevice::Instance()->ConnectServer(kUSBDeviceIP, 3333, 2223);
		UsbDevice::Instance()->Upgrader(kUSBDeviceUpdateFile);
	}

	if (g_upgrade_type == "upgradeSub") {
		return;
	}

	//升级主控
	XM_Middleware_SDCard_Upgrade(kUpdateFileName, kUpdateMD5Name);
}

#include "periphery/PeripheryThread.h"
int main(int argc, char *argv[ ])
{
	XMLogI("============XM_Middleware_SetLogCallback, argc=%d", argc);
	XM_Middleware_SetLogCallback(OnMWLogCallback, 0);

#if 1//X2V60_S_DEBUG1
    SPEAK_OFF;
#endif

	//父进程
	if (argc == 1) {
		XMLogI("car app start");

		//同步系统时间
		std::string a("/sbin/hwclock -s");
		system(a.c_str());

		//时间处理
        time_t t = time(NULL);
        struct tm* current_time = localtime(&t);
        if ((current_time->tm_year + 1900) < 2023) {
			//rtc时钟出现异常
			char buf1[128] = { 0 };
			//读取配置文件中保存的时间
			int64_t pretime = GlobalData::Instance()->GetCurTime();
			if (pretime < 0) {
				sprintf(buf1, "date -s \"%d-%d-%d %d:%d:%d\"", 2023, 1, 1, 0, 0, 0);
			}	
            else {
				time_t pre = pretime;
				struct tm* pre_time = localtime(&pre);
				sprintf(buf1, "date -s \"%d-%d-%d %d:%d:%d\"", pre_time->tm_year + 1900, pre_time->tm_mon,
				pre_time->tm_mday, pre_time->tm_hour, pre_time->tm_min, pre_time->tm_sec);
			}
            
            system(buf1);
            //同步硬件时间
            std::string a("/sbin/hwclock -w");
            system(a.c_str());
        }

		//开启看门狗
		XM_Middleware_WDT_WatchDogStart(10);

		//检测是否是连接电脑
		/* XMLogI("before connect_computer check");
		GlobalPage::Instance()->page_main()->connect_computer_ = (bool)XM_Middleware_USBFunc_ConnectComputer();
		XMLogI("after connect_computer_ = %d", GlobalPage::Instance()->page_main()->connect_computer_); */

		int i,j;
		XM_Middleware_SetChannelNum(kChannelNum);
		MppMdl::Instance()->SetChannelNum(2); //mpp只有2路
		MppMdl::Instance()->DisplayInit();
		//初始化中间件sdk
		XM_Middleware_Init();
		XM_Middleware_Smart_Init();
		//XM_Middleware_SetLogLevel(XM_LOG_VERBOSE);
		XM_Middleware_SetEventCallback(OnMWEventCallBack, 0);
		XM_Middleware_SetVodStreamCallback(OnMWVODStreamCallBack, 0);
		XM_Middleware_SetRealStreamCallback(OnMWRealTimeStreamCallBack, 0);
		// XM_Middleware_SetLogCallback(OnMWLogCallback, 0);
		
		XM_Middleware_SetParam(0, PARAM_REC_FRAMECOUNT, 100);
		XM_Middleware_SetParam(0, PARAM_REC_TOTAL_BLOCKCOUNT, 5000);
		//播放开机音频
#if 1//X2V60_S_DEBUG1
        SPEAK_OFF;
#endif
		std::string sound_file = kAudioPath;
		sound_file += "kaiji_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
		
		//设置版本号
		std::string version = "X2V60_";
	#ifdef BOARD8520DV200   
		version += "8520dv200.";
	#endif
		//version += "00-" ;//VSP，定义00为通用
		version += XM_Middleware_GetVersion(); //sdk版本号
		char date_version[32] = {0};
		sprintf(date_version, "-%04d%02d%02d-%02d:%02d:%02d", 
			OS_YEAR, OS_MONTH, OS_DAY, OS_HOUR, OS_MINUTE, OS_SECOND);
		version += date_version;
		GlobalData::Instance()->set_version(version);
		XMLogW("program version is [%s]", version.c_str());

		//设置存储路径，分区路径和mount路径
		XM_MW_Storage_SDCard_Config sdcard_cfg = { 0 };
		sdcard_cfg.sdcard_count = 1;
		strcpy(sdcard_cfg.mount_path[XM_STORAGE_SDCard_0], kTFCardPath);
		//设置sd卡格式化名称
		strcpy(sdcard_cfg.sdcard_format_name, "CarDVR");
		//设置文件夹总目录名
		//strcpy(sdcard_cfg.root_dir_name, "");
		XMLogI("before XM_Middleware_Storage_SetSDCardConfig");
		XM_Middleware_Storage_SetSDCardConfig(&sdcard_cfg);

		XMLogI("before ReadConfigFromFile");
		//读取配置文件
		GlobalData::Instance()->car_config()->ReadConfigFromFile();

		//读取并设置录像单文件时长
		XMStorageRecordDuration record_duration = XM_STORAGE_Record_Duration_60;
		XM_CONFIG_VALUE cfg_value;
		int ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Circular_Record_Time, cfg_value);
		if (ret >= 0) {
			if (cfg_value.int_value == 1) {
				record_duration = XM_STORAGE_Record_Duration_60;
			}
			else if (cfg_value.int_value == 2) {
				record_duration = XM_STORAGE_Record_Duration_120;
			}
			else if (cfg_value.int_value == 3) {
				record_duration = XM_STORAGE_Record_Duration_180;
			}
		}
		XM_Middleware_Storage_SetRecordDuration(record_duration);

		//设置编码类型，目前暂只支持h265
		PAYLOAD_TYPE_E video_codec_type = PT_H265;
		ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Encode_Format, cfg_value);
		if (ret >= 0) {
			if (cfg_value.int_value == PT_H264) {
				video_codec_type = PT_H264;
			}
			else {
				video_codec_type = PT_H265;
			}
		}
		MppMdl::Instance()->Init();
		//设置分辨率
		int width = kMainPicWidth;
		int height = kMainPicHeight;
		int bit_rate = 2048;
		ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
		if (ret >= 0) {
			if (cfg_value.int_value == 1080) {
				width = 1920;
				height = 1080;
				bit_rate = 4096;
			}
			else if (cfg_value.int_value == 1440) {
				width = 2560;
				height = 1440;
				bit_rate = 20480;
			}
			else if (cfg_value.int_value == 2144) {
				width = 3840;
				height = 2144;	
				bit_rate = 20480;
			}
		}
		XM_MW_Encode_Info enc_info;
		enc_info.channel_num = 2;
		enc_info.enc_medias[0][0].video_codec = video_codec_type;
		enc_info.enc_medias[0][0].width = width;
		enc_info.enc_medias[0][0].height = height;
		enc_info.enc_medias[0][0].bit_rate = bit_rate;
		enc_info.enc_medias[0][0].frame_rate = 25;//height >= 2144 ? 25 : 30;
		enc_info.enc_medias[0][1].video_codec = video_codec_type;
		enc_info.enc_medias[0][1].width = kSubStreamWidth;
		enc_info.enc_medias[0][1].height = kSubStreamHeight;
		enc_info.enc_medias[0][1].bit_rate = 4096;//4096 2048 1024
		enc_info.enc_medias[0][1].frame_rate = 25;//
		enc_info.enc_medias[1][0].video_codec = video_codec_type;//
		enc_info.enc_medias[1][0].width = 1920;
		enc_info.enc_medias[1][0].height = 1080;
		enc_info.enc_medias[1][0].bit_rate = 4096;
		enc_info.enc_medias[1][0].frame_rate = 25;//height >= 2144 ? 25 : 30;
		for (int i = 0; i < kChannelNum; i++) {
			enc_info.enc_medias[i][0].audio_codec = PT_PCMA;
			enc_info.enc_medias[i][0].audio_channel = 1;
			enc_info.enc_medias[i][0].bit_per_sample = 16;
			enc_info.enc_medias[i][0].samples_per_sec = 16000;
		}
		MppMdl::Instance()->SetEncInfo(enc_info);

		//设置3录的媒体属性
		XM_MW_Media_Info media_info;
		media_info.width = 1920;
		media_info.height = 1080;
		media_info.video_codec = PT_H265;
		media_info.frame_rate = 25;
		media_info.bit_rate = 2048;
		media_info.audio_codec = PT_PCMA;
		media_info.bit_per_sample = 16;
		media_info.samples_per_sec = 16000;
		media_info.audio_channel = 1;
		XM_Middleware_Storage_SetMediaInfo(2, &media_info);

		lv_init();
		lv_port_disp_init();
		lv_port_indev_init();

		//相关外设调用
		PeripheryManager::Instance()->Periphery();

		//SD卡升级;检测可以升级就杀死当前应用程序然后在/var下重新运行应用程序并传入传输参数upgrade
		XMLogI("before XM_Middleware_SDCard_AlreadyUpgrade");

		std::string c("ls /dev");
		system(c.c_str());
		int upgrade_flag = 0;	
		ret = XM_Middleware_SDCard_AlreadyUpgrade(kUpdateFileName, kUpdateMD5Name);	
		if (!ret) {
			upgrade_flag |= 1;
		}
		ret = XM_Middleware_SDCard_AlreadyUpgrade(kUSBDeviceUpdateFile, kUSBDeviceUpdateMD5File);
		if (!ret) {
			upgrade_flag |= 2;
		}

		XMLogI("upgrade_flag=%d", upgrade_flag);
		
		if (upgrade_flag != 0) {
			XM_Middleware_WDT_WatchDogStop();
			//升级处理
			system("cp /usr/bin/car_demo /var/");
			char cmd_buf[50];
			if (upgrade_flag == 1) 
				sprintf(cmd_buf, "kill -9 %d ; /var/car_demo upgradeMain", getpid());
			else if (upgrade_flag == 2) 
				sprintf(cmd_buf, "kill -9 %d ; /var/car_demo upgradeSub", getpid());
			else if (upgrade_flag == 3)  
				sprintf(cmd_buf, "kill -9 %d ; /var/car_demo upgradeAll", getpid());	
			system(cmd_buf);
			return XM_SUCCESS;
		}
		else {
			umount("/home");
			//设置UI界面操作事件回调函数，ui界面中开始存储、停止存储、查询查询、点播等操作，可以通过GlobalData::Instance()->ui_event_cb_回调出来
			GlobalData::Instance()->ui_event_cb_ = OnUIEventCallback;
			//检测是否是连接电脑
			XMLogI("before connect_computer check");
			GlobalPage::Instance()->page_main()->connect_computer_ = (bool)XM_Middleware_USBFunc_ConnectComputer();
			XMLogI("after connect_computer_ = %d", GlobalPage::Instance()->page_main()->connect_computer_);

			//MppMdl::Instance()->SetRealTimeStreamCallback(OnMppRealTimeStreamCallBack, 0);
			//开启预览
			MppMdl::Instance()->StartPreview();
			//MppMdl::Instance()->SetRealTimeStreamCallback(OnMppRealTimeStreamCallBack, 0);		
		
			//GlobalData::Instance()->gdb_debug_ = true;
			if (GlobalData::Instance()->gdb_debug_) {	
				//gdb调试时，应用程序放在sd卡上，此时不要再去挂载sd卡
				g_sd_status = XM_SD_NORMAL;
				XM_Middleware_Storage_CheckFileSystem(XM_STORAGE_SDCard_0);
			}
			else {
				XMLogI("before XM_Middleware_Storage_MountSDCard");
				g_sd_status = XM_Middleware_Storage_MountSDCard(XM_STORAGE_SDCard_0);
			}

			XMLogI("g_sd_status = %d", g_sd_status);
			if (g_sd_status == XM_SD_NORMAL) {
				int write_speed = 0;
				if (GlobalData::Instance()->gdb_debug_) {
					write_speed = 10922;
					ret = 0;
				}
				else {
					ret = XM_Middleware_Storage_GetSpeedState(XM_STORAGE_SDCard_0, &write_speed);
				}
				XMLogI("write_speed, ret=%d, speed=%d", ret, write_speed);
				GlobalData::Instance()->SDCard_write_speed_ = write_speed;

				if (ret < 0) {
					g_sd_status = XM_SD_NEEDCHECKSPEED;
				}
				else {
					g_sd_status = XM_Middleware_Storage_CheckFileSystem(XM_STORAGE_SDCard_0);
					XMLogI("after check file system, ret=%d", ret);
					if (write_speed < 1024)
					{

					}
					else {
						LogFileManager::Instance()->Enable(true);
						LogFileManager::Instance()->write_log_sd_set(1,1);
					}
				}
			} else if (g_sd_status == XM_SD_NOEXIST) {
				int sd_exit = LIBCR_SDIO1_Status();
				XMLogI("LIBCR_SDIO1_Status, ret=%d", sd_exit);
				//sd卡设备节点不存在，但sd卡实际是插入的
				if (sd_exit == 1) {
					//ui做个提示，或播放报警声音
					XMLogE("!!!!!!!!sdcard exist, but can't find in /dev, should warning!!!!");
				}
			}
            osd_data_init();
			#if 0
			XM_MW_OSD_INFOS osd_infos;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 2; j++) {
					osd_infos.osd[i][j].enable=true;
					osd_infos.osd[i][j].x=j*4096;
					osd_infos.osd[i][j].y=4096;
					osd_infos.osd[i][j].width=XM_MAX_OSD_WIDTH;
					osd_infos.osd[i][j].height=XM_MAX_OSD_HEIGHT;
					for (int k = 0; k < XM_MAX_OSD_HEIGHT; k++) {
						memset(osd_infos.osd[i][j].osd_buf+k*XM_MAX_OSD_WIDTH*2, 255-k, XM_MAX_OSD_WIDTH*2);
					}
					osd_infos.osd[i][j].up=false;
				}
			}
			//临时调试用
			//XM_Middleware_Encode_SetOSDInfos(osd_infos);	
          #endif
			XM_Middleware_Network_SetDataCallback(OnNetworkMsg);
			XM_Middleware_Network_CloseThreadCallback(closeThread);

			GlobalPage::Instance()->page_main()->AppParamInit();
			//开启wifi,ap模式
			OS_THREAD thread_wifi;
			CreateThreadEx(thread_wifi, (LPTHREAD_START_ROUTINE)WifiThread, NULL);
			
            #if 1//X2V60_S_DEBUG1
			XM_Middleware_Storage_SetFileSwtichDelayTime(200);
            #endif
			
			XMLogI("run_project");
			run_project();
		}
	}
	//子进程升级
	else if (argc == 2) {
		g_upgrade_type = argv[1];
		XMLogI("upgrade start, argv=%s", argv[1]);

		if (g_upgrade_type == "upgradeAll" || g_upgrade_type == "upgradeSub") {
		
		std::string aa = "/sbin/ifconfig eth1 192.168.2.15 netmask 255.255.255.0";
#if 1//X2V60_S_DEBUG1
			//设置模拟网卡IP
			XMLogI("before set ip");
			for (int i = 0; i < 100; i++) {
				int ret = LIBCR_NET_CheckEther("eth1");
				if (ret >= 0) {
					XMLogI("find eth1");
					std::string bb("/sbin/ifconfig -a");
					system(bb.c_str());
					break;
				}
				usleep(20*1000);
			}

			//std::string aa = "/sbin/ifconfig eth1 192.168.2.15 netmask 255.255.255.0";
			system(aa.c_str());
#endif
			
			aa = "/sbin/route add default gw 192.168.2.1";
			system(aa.c_str());
			XMLogI("after set ip");
			std::string bb("/sbin/ifconfig -a");
			system(bb.c_str());
		}
		//开启看门狗
		XM_Middleware_WDT_WatchDogStart(60);
		//读取配置文件
		GlobalData::Instance()->car_config()->ReadConfigFromFile();
	#if 0	
		lcd_param_s stLcdPara;
		memset(&stLcdPara, 0, sizeof(lcd_param_s));
		stLcdPara.init_flag = 0;
		stLcdPara.spi_cs = 65;
		stLcdPara.lcd_pwd = 8;
		stLcdPara.lcd_rst = 69;
		stLcdPara.lcd_width = MY_DISP_HOR_RES;
		stLcdPara.lcd_height = MY_DISP_VER_RES;
		stLcdPara.type = HSD015;
		LIBCR_LCD_Open(&stLcdPara);
	#endif	
		//lvgl初始化
		lv_init();
		lv_port_disp_init();

		OS_THREAD thread_upgrader;
		CreateThreadEx(thread_upgrader, (LPTHREAD_START_ROUTINE)UpgraderThread, NULL);

		//SD卡升级
		XMLogI("before XM_Middleware_SDCard_Upgrade");
		GlobalData::Instance()->upgrading_ = true;
		g_mw_event_infos.clear();
		GlobalPage::Instance()->page_main()->OpenUpgraderProgressPage();
	}

	XMLogI("timer_hander_thread");
	OS_THREAD tick;
	CreateThreadEx(tick, (LPTHREAD_START_ROUTINE)timer_hander_thread, NULL);
	#if GPS_EN
	//串口GPS外设调用	
		UartGpsThreadStart();
	#endif 
	int64_t dur = 0;
	while (1) {
		int64_t t1 = GetTickTime();
		usleep(1 * 1000);
		int64_t t2 = GetTickTime();
		dur = t2 - t1;
		if (dur > 0 && dur < 100) {
			lv_tick_inc(dur);
		}
	}
	return XM_SUCCESS;
}
