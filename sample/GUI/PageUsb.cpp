#include "global_data.h"
#include "global_page.h"
#include "PageUsb.h"
#include "cJson/cJSON.h"
#include "TimeUtil.h"
#include "xm_middleware_network.h"
extern "C" {
#include "cr_vo.h"
}
extern int g_sd_status;
const int kSignalPort = 2222;
const int kMediaPort = 2223;


PageUsb::PageUsb():usb_page_(NULL), usb_mode_(USB_MODE_NONE), user_selected_usb_mode_(USB_MODE_NONE),memory_mode_page_(NULL)
{
    realplay_ = false;
    engineId_ = 0;
    connId_ = 0;
    frame_id_ = 0;
    channel_ = 0;
	first_frame_time_ = 0;
}

PageUsb::~PageUsb()
{

}
int PageUsb::SetUsbFunc(USB_MODE_E mode)
{
	XMLogI("SetUsbFunc mode = %d", mode);
	int ret = 0;
	if (usb_mode_ != USB_MODE_NONE) {
		ret = XM_Middleware_USBFunc_Destroy(usb_mode_);
		if (ret < 0) {
			XMLogW("XM_Middleware_USBFunc_Destroy Failed! ret = %d", ret);
			return -1;
		}
	}
	usb_mode_ = USB_MODE_NONE;
	
	if (mode != USB_MODE_NONE) {
		ret = XM_Middleware_USBFunc_Create(mode);
		if (ret < 0) {
			XMLogW("XM_Middleware_USBFunc_Create Failed! ret = %d", ret);
			return -1;
		}
		if (mode == USB_MODE_RNDIS) {
			int ret = XM_Middleware_Network_XMIP_StartServer(kSignalPort, kMediaPort);
			if (ret < 0) {
				XMLogE("start server error");
			}	
		}
	}
	usb_mode_ = mode;

	return 0;
}

int PageUsb::OnRealPlay(int engineId, int connId, int channel, int stream, int id)
{
	//调整屏幕色彩时，要显示视频画面
	//GlobalPage::Instance()->page_usb()->Exit();
	//回复
	cJSON* params = cJSON_CreateObject();
	cJSON_AddNumberToObject(params, "width", 1280);
	cJSON_AddNumberToObject(params, "height", 720);
	cJSON_AddNumberToObject(params, "fps", 25);
	cJSON_AddNumberToObject(params, "sample_rate", 8000);
	cJSON_AddStringToObject(params, "video_codec", "h265");
	cJSON_AddStringToObject(params, "audio_codec", "g711a");

	cJSON* root_res = cJSON_CreateObject();
	cJSON_AddStringToObject(root_res, "version", "1.0");
	cJSON_AddStringToObject(root_res, "type", "response");
	cJSON_AddStringToObject(root_res, "op", "realplay");
	cJSON_AddItemToObject(root_res, "param", params);
	char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	XM_Middleware_Network_XMIP_SendBuffer(engineId, connId, (uint8_t)XM_DATA_STRING, http_body, strlen(http_body));
	free(http_body);

	engineId_ = engineId;
	connId_ = connId;
	frame_id_ = id;
	realplay_ = true;
	first_frame_time_ = 0;
	channel_ = channel;
	XM_Middleware_Network_XMIP_SetFrameList(100);
	XM_Middleware_Network_XMIP_ClearFrameList();
	//做强制I帧
	return 0;
}
int AdjustColor(int engineId, int connId, lcd_vo_param* vo_param)
{
 	//回复
	cJSON* params = cJSON_CreateObject();
	cJSON_AddNumberToObject(params, "channel", 0);
	cJSON_AddNumberToObject(params, "stream", 0);

	cJSON* root_res = cJSON_CreateObject();
	cJSON_AddStringToObject(root_res, "version", "1.0");
	cJSON_AddStringToObject(root_res, "type", "response");
	cJSON_AddStringToObject(root_res, "op", "adjustcolor");
	cJSON_AddItemToObject(root_res, "param", params);
	char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	XM_Middleware_Network_SendBuffer(engineId, connId, (uint8_t)XM_DATA_STRING, http_body, strlen(http_body));
	free(http_body);
	LIBCR_VO_SetParam(vo_param);

	return 0;
}
int GetColor(int engineId, int connId)
{
	lcd_vo_param vo_param;
	LIBCR_VO_GetParam(&vo_param);

 	//回复
	cJSON* params = cJSON_CreateObject();
	cJSON_AddNumberToObject(params, "channel", 0);
	cJSON_AddNumberToObject(params, "stream", 0);
	
	cJSON_AddNumberToObject(params, "ygain", vo_param.ygain);
    cJSON_AddNumberToObject(params, "ugain", vo_param.ugain);
    cJSON_AddNumberToObject(params, "vgain", vo_param.vgain);

    cJSON_AddNumberToObject(params, "rgain", vo_param.rgain);
    cJSON_AddNumberToObject(params, "ggain", vo_param.ggain);
    cJSON_AddNumberToObject(params, "bgain", vo_param.bgain);

    cJSON_AddNumberToObject(params, "yoffs", vo_param.yoffs);
    cJSON_AddNumberToObject(params, "uoffs", vo_param.uoffs);
    cJSON_AddNumberToObject(params, "voffs", vo_param.voffs);

    cJSON_AddNumberToObject(params, "roffs", vo_param.roffs);
    cJSON_AddNumberToObject(params, "goffs", vo_param.goffs);
    cJSON_AddNumberToObject(params, "boffs", vo_param.boffs);

    cJSON_AddNumberToObject(params, "rcoe0", vo_param.rcoe[0]);
    cJSON_AddNumberToObject(params, "rcoe1", vo_param.rcoe[1]);
    cJSON_AddNumberToObject(params, "rcoe2", vo_param.rcoe[2]);

    cJSON_AddNumberToObject(params, "gcoe0", vo_param.gcoe[0]);
    cJSON_AddNumberToObject(params, "gcoe1", vo_param.gcoe[1]);
    cJSON_AddNumberToObject(params, "gcoe2", vo_param.gcoe[2]);

    cJSON_AddNumberToObject(params, "bcoe0", vo_param.bcoe[0]);
    cJSON_AddNumberToObject(params, "bcoe1", vo_param.rcoe[1]);
    cJSON_AddNumberToObject(params, "bcoe2", vo_param.bcoe[2]);

	XM_CONFIG_VALUE cfg_value;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_GAMMA_DELTA, cfg_value);
    cJSON_AddNumberToObject(params, "gamaDelta", cfg_value.int_value);

	cJSON* root_res = cJSON_CreateObject();
	cJSON_AddStringToObject(root_res, "version", "1.0");
	cJSON_AddStringToObject(root_res, "type", "response");
	cJSON_AddStringToObject(root_res, "op", "getcolor"); 
	cJSON_AddItemToObject(root_res, "param", params);
	char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	XM_Middleware_Network_SendBuffer(engineId, connId, (uint8_t)XM_DATA_STRING, http_body, strlen(http_body));
	free(http_body);

	return 0;
}

// 关闭直播回复
int PageUsb::OffRealPlay(int engineId, int connId, int channel, int stream, int id)
{
    //回复
	cJSON* params = cJSON_CreateObject();
	cJSON_AddNumberToObject(params, "channel", channel);
	cJSON_AddNumberToObject(params, "stream", stream);
    cJSON_AddNumberToObject(params, "id", id);

	cJSON* root_res = cJSON_CreateObject();
	cJSON_AddStringToObject(root_res, "version", "1.0");
	cJSON_AddStringToObject(root_res, "type", "response");
	cJSON_AddStringToObject(root_res, "op", "realplay_close");
	cJSON_AddItemToObject(root_res, "param", params);
	char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	XM_Middleware_Network_XMIP_SendBuffer(engineId, connId, (uint8_t)XM_DATA_STRING, http_body, strlen(http_body));
	free(http_body);

	realplay_ = false;
    frame_id_ = 0;   
	first_frame_time_ = 0; 
	XM_Middleware_Network_XMIP_ClearFrameList();
	return 0;
}

void PageUsb::OnNetworkMsg(int engineId, int connId, uint8_t type, char * msg, int contentLen)
{
	XMLogI("respMsg,engineId=%d,connId=%d,msg=%s,contentLen=%d",engineId,connId,msg,contentLen);
	cJSON* root_req = cJSON_Parse(msg);
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
	
	if (str_oper == "realplay") {
		cJSON* channel_item = cJSON_GetObjectItem(param_item, "channel");
		if (!channel_item) {
			XMLogE("can't find channel_item");
			cJSON_Delete(root_req);
			return;
		}
		cJSON* stream_item = cJSON_GetObjectItem(param_item, "stream");
		if (!channel_item) {
			XMLogE("can't find stream_item");
			cJSON_Delete(root_req);
			return;
		}
		cJSON* id_item = cJSON_GetObjectItem(param_item, "id");
		if (!id_item) {
			XMLogE("can't find id_item");
			cJSON_Delete(root_req);
			return;
		}

		int id = id_item->valueint;
		int channel = channel_item->valueint;
		int stream = stream_item->valueint;
		OnRealPlay(engineId, connId, channel, stream, id);
	}
	else if ("realplay_close" == str_oper) {
        cJSON* channel_item = cJSON_GetObjectItem(param_item, "channel");
		if (!channel_item) {
			XMLogE("can't find channel_item");
			cJSON_Delete(root_req);
			return;
		}
		cJSON* stream_item = cJSON_GetObjectItem(param_item, "stream");
		if (!channel_item) {
			XMLogE("can't find stream_item");
			cJSON_Delete(root_req);
			return;
		}
        cJSON* id_item = cJSON_GetObjectItem(param_item, "id");
		if (!id_item) {
			XMLogE("can't find id_item");
			cJSON_Delete(root_req);
			return;
		}

        int id = id_item->valueint;
		int channel = channel_item->valueint;
		int stream = stream_item->valueint;
		OffRealPlay(engineId, connId, channel, stream, id);
	}
	else if (str_oper == "adjustcolor") {
		cJSON* ygain = cJSON_GetObjectItem(param_item, "ygain");
		cJSON* ugain = cJSON_GetObjectItem(param_item, "ugain");
        cJSON* vgain = cJSON_GetObjectItem(param_item, "vgain");
		cJSON* rgain = cJSON_GetObjectItem(param_item, "rgain");
		cJSON* ggain = cJSON_GetObjectItem(param_item, "ggain");
        cJSON* bgain = cJSON_GetObjectItem(param_item, "bgain");

		cJSON* yoffs = cJSON_GetObjectItem(param_item, "yoffs");
		cJSON* uoffs = cJSON_GetObjectItem(param_item, "uoffs");
        cJSON* voffs = cJSON_GetObjectItem(param_item, "voffs");
		cJSON* roffs = cJSON_GetObjectItem(param_item, "roffs");
		cJSON* goffs = cJSON_GetObjectItem(param_item, "goffs");
        cJSON* boffs = cJSON_GetObjectItem(param_item, "boffs");

		cJSON* rcoe0 = cJSON_GetObjectItem(param_item, "rcoe0");
		cJSON* rcoe1 = cJSON_GetObjectItem(param_item, "rcoe1");
        cJSON* rcoe2 = cJSON_GetObjectItem(param_item, "rcoe2");
		cJSON* gcoe0 = cJSON_GetObjectItem(param_item, "gcoe0");
		cJSON* gcoe1 = cJSON_GetObjectItem(param_item, "gcoe1");
        cJSON* gcoe2 = cJSON_GetObjectItem(param_item, "gcoe2");
		cJSON* bcoe0 = cJSON_GetObjectItem(param_item, "bcoe0");
		cJSON* bcoe1 = cJSON_GetObjectItem(param_item, "bcoe1");
        cJSON* bcoe2 = cJSON_GetObjectItem(param_item, "bcoe2");

		if (!ygain || !ugain || !vgain) {
			XMLogE("can't find ygain or ugain or vgain");
			cJSON_Delete(root_req);
			return;
		}

		lcd_vo_param vo_param = {0};
		vo_param.ygain = ygain->valueint;
		vo_param.ugain = ugain->valueint;
		vo_param.vgain = vgain->valueint;
		vo_param.rgain = rgain->valueint;
		vo_param.ggain = ggain->valueint;
		vo_param.bgain = bgain->valueint;
		XMLogI("ygain, %d %d %d %d %d %d", vo_param.ygain, vo_param.ugain, vo_param.vgain,
			vo_param.rgain, vo_param.ggain, vo_param.bgain);
		vo_param.yoffs = yoffs->valueint;
		vo_param.uoffs = uoffs->valueint;
		vo_param.voffs = voffs->valueint;
		vo_param.roffs = roffs->valueint;
		vo_param.goffs = goffs->valueint;
		vo_param.boffs = boffs->valueint;
		XMLogI("yoffs, %d %d %d %d %d %d", vo_param.yoffs, vo_param.uoffs, vo_param.voffs,
			vo_param.roffs, vo_param.goffs, vo_param.boffs);
		vo_param.rcoe[0] = rcoe0->valueint;
		vo_param.rcoe[1] = rcoe1->valueint;
		vo_param.rcoe[2] = rcoe2->valueint;
		vo_param.gcoe[0] = gcoe0->valueint;
		vo_param.gcoe[1] = gcoe1->valueint;
		vo_param.gcoe[2] = gcoe2->valueint;
		vo_param.bcoe[0] = bcoe0->valueint;
		vo_param.bcoe[1] = bcoe1->valueint;
		vo_param.bcoe[2] = bcoe2->valueint;

		XMLogI("rcoe, %d %d %d %d %d %d %d %d %d", vo_param.rcoe[0], vo_param.rcoe[1], vo_param.rcoe[2],
			vo_param.gcoe[0], vo_param.gcoe[1], vo_param.gcoe[2], vo_param.bcoe[0], vo_param.bcoe[1], vo_param.bcoe[2]);

		cJSON* gamma = cJSON_GetObjectItem(param_item, "gamma");
		if (gamma && gamma->type == cJSON_Array) {
			int size = cJSON_GetArraySize(gamma);
			for (int i = 0; i < size; i++) {
				cJSON* item = cJSON_GetArrayItem(gamma, i);
				vo_param.gamma[i] = item->valueint;
				printf("%d ", item->valueint);
			}
			XMLogI("total gama count=%d", size);
		}
		cJSON* gamma_delta = cJSON_GetObjectItem(param_item, "gamaDelta");
		if (gamma_delta) {
			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = gamma_delta->valueint;
			XMLogI("gama delta=%d", gamma_delta->valueint);
			GlobalData::Instance()->car_config()->SetValue(CFG_Operation_GAMMA_DELTA, cfg_value);
		}
		AdjustColor(engineId, connId, &vo_param);
	}
	else if (str_oper == "getcolor") {
		GetColor(engineId, connId);
	}
	cJSON_Delete(root_req);
}

void PageUsb::OnMWRealTimeStreamCallBack(int channel, int stream, XM_MW_Media_Frame* media_frame, int64_t user)
{
	if (!realplay_ || channel != 0 || stream != 0)
		return;

	int64_t now = GetTickTime();
	if (first_frame_time_ <= 0) {
		first_frame_time_ = now;
	}

	int pts = (int)(now - first_frame_time_);
	if (media_frame->video) {
		XM_Middleware_Network_XMIP_SendFrame(engineId_, connId_, (char*)media_frame->frame_buffer,
			media_frame->frame_size, true, media_frame->key_frame, media_frame->timestamp, frame_id_);
		// if (media_frame->key_frame) 
		// 	XMLogI("key frame, len=%d", media_frame->frame_size);
	}
	else {
		XM_Middleware_Network_XMIP_SendFrame(engineId_, connId_, (char*)media_frame->frame_buffer,
			media_frame->frame_size, false, false, media_frame->timestamp, frame_id_);
	}
}