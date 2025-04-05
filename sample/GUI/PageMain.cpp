#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/reboot.h>

#include "global_data.h"
#include "global_page.h"
#include "PageMain.h"
#include "cr_lcd.h"
#include "TimeUtil.h"
#include "usbdev/UsbDevice.h"
#include "periphery/PeripheryManager.h"
#include "periphery/PeripheryThread.h"
#include "network/HttpSerModule.h"
#include "mpp/MppMdl.h"
#include "network/WifiOpr.h"
#include "CommDef.h"

extern std::string g_upgrade_type;
extern int g_sd_status;
extern int g_engineId;
extern bool g_app_connect;
extern bool g_realplay;
extern bool g_storage_started;
extern void OnNetworkMsg(int engineId, int connId, uint8_t type, char* msg, char* connType, int contentLen);
extern void closeThread(int connId);
extern void lv_ignore_signals();

#if 1//X2V60_S_DEBUG1
extern bool g_acc_connected;
extern bool g_first_record;
extern bool g_wifi_man_oper;

//int LIBCR_SDIO1_Status();
#include "cr_sdio.h"

#endif

PageMain::PageMain() :wifi_enable_(true), wifi_img_(NULL), mic_img_(NULL), tfcard_img_(NULL), collision_record_timer_(NULL)
, pre_sys_time_(0), pre_time_(0), pre_record_time_(0), wifi_id_label_(NULL), rec_time_label_(NULL), car_charger_(true)
, acc_disconnect_(true), acc_disconnect_timer_(NULL), acc_connect_timer_(NULL), compact_recording_(false), record_start_time_(0)
, lock_current_recording_file_(false), pre_date_(0), audio_flag_(1), sd_formatting_(false), judge_app_timer_(NULL), collision_startup_(false)
{

}

PageMain::~PageMain()
{

}

int PageMain::OpenPage()
{
	XMLogI("OpenPage, lcd width=%d, height=%d", MY_DISP_HOR_RES, MY_DISP_VER_RES);
	/* open */
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
	static lv_style_t main_page_style_;
	lv_style_init(&main_page_style_);
	lv_style_set_border_width(&main_page_style_, 0);
	lv_style_set_bg_color(&main_page_style_, lv_color_black());
	lv_style_set_text_font(&main_page_style_, lv_font_all);
	lv_style_set_text_color(&main_page_style_, lv_color_white());

	main_page_ = GlobalData::Instance()->gui()->CreatePage(lv_scr_act(), SCREEN_WIDTH, SCREEN_HEIGHT, &main_page_style_);
	lv_obj_set_pos(main_page_, 0, 0);

	time_t t = time(NULL);
	pre_sys_time_ = t;
	struct tm* current_time = localtime(&t);
	char buf[16] = { 0 };
	//time
	sprintf(buf, "%02d:%02d:%02d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
	time_label_ = GlobalData::Instance()->gui()->CreateLabel(main_page_, buf);
	lv_obj_set_style_text_color(time_label_, lv_color_make(0, 255, 255), 0);
	lv_obj_set_style_text_font(time_label_, &lv_font_montserrat_48, 0);
	lv_obj_align(time_label_, LV_ALIGN_LEFT_MID, size_w(60), size_h(10));
	pre_time_ = current_time->tm_hour + current_time->tm_min + current_time->tm_sec;

	//date
	sprintf(buf, "%04d-%02d-%02d", current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday);
	date_label_ = GlobalData::Instance()->gui()->CreateLabel(main_page_, buf);
	lv_obj_set_style_text_font(date_label_, &lv_font_montserrat_20, 0);
	lv_obj_align(date_label_, LV_ALIGN_BOTTOM_RIGHT, size_w(-40), size_h(-16));
	pre_date_ = current_time->tm_year + current_time->tm_mon + current_time->tm_mday;

	// record_time
	rec_time_label_ = GlobalData::Instance()->gui()->CreateLabel(main_page_, "STOP");
	// lv_obj_set_style_text_color(rec_time_label_, lv_color_make(255, 0, 0), 0);
	lv_obj_set_style_text_font(rec_time_label_, &lv_font_montserrat_20, 0);
	lv_obj_align_to(rec_time_label_, date_label_, LV_ALIGN_OUT_LEFT_MID, size_w(-85), 0);

	red_circle_img_ = lv_img_create(main_page_);
	lv_img_set_src(red_circle_img_, image_path"red_circle.png");
	lv_obj_align(red_circle_img_, LV_ALIGN_TOP_LEFT, size_w(36), size_h(23));
	lv_obj_add_flag(red_circle_img_, LV_OBJ_FLAG_HIDDEN);
	
#if 1//X2V60_S_DEBUG1
	sync();
	g_first_record = true;
#endif

	SDCardImg();

#if 1//X2V60_S_DEBUG1
	//卡修复
	int tep = 0;
	XM_CONFIG_VALUE get_cfg_value;
	tep = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_NEED_REPAIR_SDCARD, get_cfg_value);
	
	if(tep < 0){
		
		tep = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_NEED_REPAIR_SDCARD, get_cfg_value);
	}
	
	if(tep >= 0 && get_cfg_value.bool_value) 
	{
	#if 1
		XMLogW("sdio_status = %d \r\n", LIBCR_SDIO1_Status());
		if(LIBCR_SDIO1_Status())
		{
			XM_Middleware_WDT_WatchDogStop();
			XMLogW("before fsck sdcard");
			const char* kDevNode1 = "/dev/mmcblk0p1";
			const char* kDevNode2 = "/dev/mmcblk0";
			bool find_dev_node1 = access(kDevNode1, F_OK) == 0 ? true : false;
			const char* sdcard_dev_node = find_dev_node1 ? kDevNode1 : kDevNode2;
			char str_cmd[64] = {0};
			sprintf(str_cmd, "fsck.fat -V -a -w %s", sdcard_dev_node);
			system(str_cmd);
			XM_Middleware_WDT_WatchDogStart(6);
		}
   #endif
		XMLogW("after fsck sdcard");
		get_cfg_value.bool_value = false;
		GlobalData::Instance()->car_config()->SetValue(CFG_Operation_NEED_REPAIR_SDCARD, get_cfg_value);
	}
	
	//5分钟无操作关闭WiFi ExitCompactRecordTimer wifi_5min_timer_ 
	wifi_5min_timer_ = lv_timer_create(Wifi5MinDelayTimer, 5*60*1000, NULL);
	//GlobalPage::Instance()->page_main()->PlaySdCardStatus();

	
#endif

	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	MICImg(cfg_value.bool_value);
	WIFIImg();

	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
	bool parking_monitor = cfg_value.bool_value;

#if 1//
    if (parking_monitor && !g_acc_connected)
#else
	if (parking_monitor)
#endif
	{
		collision_startup_ = PeripheryManager::Instance()->GsensorCheck();
	}
	XMLogI("GsensorCheck collision_startup_=%d", collision_startup_);
	
	if (collision_startup_)
	{
		if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024)
		{
			StartRecord();
		}
		else
		{
			int collision_startup = 1;
			lv_timer_create(DelayShutDownTimer, 2000, (void *)collision_startup);
		}
	}
	else
	{
		StartRecord();
	}
	lv_timer_create(UpdateTime, 100, NULL);
	return 0;
}

void PageMain::WIFIID() {
	WIFI_MAC_PARAM_S wifi_param;
	GetMacParam(&wifi_param);
	if (!strlen(wifi_param.ssid)) return;
	
	if (wifi_id_label_ == NULL) {
		wifi_id_label_ = lv_label_create(main_page_);
		lv_obj_set_style_text_font(wifi_id_label_, &lv_font_montserrat_20, 0);
		lv_label_set_text(wifi_id_label_, wifi_param.ssid);
		lv_obj_align_to(wifi_id_label_, time_label_, LV_ALIGN_OUT_TOP_MID, 0, -size_h(10));
	}
	else {
		lv_label_set_text(wifi_id_label_, wifi_param.ssid);
	}
}

void PageMain::WIFIImg()
{
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = WIFI_TYPE_5G;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Wifi_Type, cfg_value);

	if(cfg_value.int_value && !compact_recording_)
		WiFi_LED_ON;
	else
		WiFi_LED_OFF;

#if 0//X2V60_S_DEBUG1
	if (wifi_img_ == NULL) {
		wifi_img_ = lv_img_create(main_page_);
		if (wifi_enable_ && g_app_connect) {
			if (cfg_value.int_value == WIFI_TYPE_5G)
				lv_img_set_src(wifi_img_, image_path"5Gwifi1.png");
			else
				lv_img_set_src(wifi_img_, image_path"2.4Gwifi1.png");
		}
		else {
			if (cfg_value.int_value == WIFI_TYPE_5G)
				lv_img_set_src(wifi_img_, image_path"5Gwifi0.png");
			else
				lv_img_set_src(wifi_img_, image_path"2.4Gwifi0.png");
		}
		lv_obj_align_to(wifi_img_, mic_img_, LV_ALIGN_OUT_LEFT_MID, -size_w(35), 0);
	}
	else{
		if (wifi_enable_ && g_app_connect) {
			if (cfg_value.int_value == WIFI_TYPE_5G)
				lv_img_set_src(wifi_img_, image_path"5Gwifi1.png");
			else
				lv_img_set_src(wifi_img_, image_path"2.4Gwifi1.png");
		}
		else {
			if (cfg_value.int_value == WIFI_TYPE_5G)
				lv_img_set_src(wifi_img_, image_path"5Gwifi0.png");
			else
				lv_img_set_src(wifi_img_, image_path"2.4Gwifi0.png");
		}
	}
#endif
}

void PageMain::MICImg(bool enable)
{
	if(mic_img_ == NULL){
		mic_img_ = lv_img_create(main_page_);
		if (enable)
			lv_img_set_src(mic_img_, image_path"mic_on.png");
		else
			lv_img_set_src(mic_img_, image_path"mic_off.png");
		lv_obj_align_to(mic_img_, tfcard_img_, LV_ALIGN_OUT_LEFT_MID, -size_w(35), 0);
	}
	else{
		if (enable) {
			if(strcmp((char*)lv_img_get_src(mic_img_), image_path"mic_on.png") != 0)
				lv_img_set_src(mic_img_, image_path"mic_on.png");
		}
		else {
			if (strcmp((char*)lv_img_get_src(mic_img_), image_path"mic_off.png") != 0)
				lv_img_set_src(mic_img_, image_path"mic_off.png");
		}
	}
}

void PageMain::SDCardImg()
{
	if (tfcard_img_ == NULL)
	{
		tfcard_img_ = lv_img_create(main_page_);
		if (g_sd_status == XM_SD_NORMAL || g_sd_status == XM_SD_NEEDFORMAT || g_sd_status == XM_SD_NEEDCHECKSPEED)
			lv_img_set_src(tfcard_img_, image_path "TFcard.png");
		else if (g_sd_status == XM_SD_NOEXIST)
			lv_img_set_src(tfcard_img_, image_path "TFcard_No.png");
		else if (g_sd_status == XM_SD_ABNORMAL || g_sd_status == XM_SD_NOTRW || g_sd_status == XM_SD_NOSPACE)
		{
			XMLogW("TFcard abnormal, g_sd_status=%d", g_sd_status);
			lv_img_set_src(tfcard_img_, image_path "TFcard_Broken.png");
		}

		lv_obj_align(tfcard_img_, LV_ALIGN_TOP_RIGHT, -size_w(36), size_h(20));
	}
	else
	{
		// 缺少SD卡异常提示音，所以状态图标不一定与语音提示完全对应

		// 清除HIDDEN，预防闪烁
		if (lv_obj_has_flag(tfcard_img_, LV_OBJ_FLAG_HIDDEN))
		{
			lv_obj_clear_flag(tfcard_img_, LV_OBJ_FLAG_HIDDEN);
		}

		// 白色图标显示,需要格式化的闪烁动作在时间更新模块处理
		if (g_sd_status == XM_SD_NORMAL || g_sd_status == XM_SD_NEEDFORMAT || g_sd_status == XM_SD_NEEDCHECKSPEED)
		{
			if (strcmp((char *)lv_img_get_src(tfcard_img_), image_path "TFcard.png") != 0)
			{
				lv_img_set_src(tfcard_img_, image_path "TFcard.png");
			}
		}
		// 灰色图标显示
		else if (g_sd_status == XM_SD_NOEXIST)
		{
			if (strcmp((char *)lv_img_get_src(tfcard_img_), image_path "TFcard_No.png") != 0)
			{
				lv_img_set_src(tfcard_img_, image_path "TFcard_No.png");
			}
		}
		// 其余状态都认为SD卡异常，橙色图标显示
		else if (g_sd_status == XM_SD_ABNORMAL || g_sd_status == XM_SD_NOTRW || g_sd_status == XM_SD_NOSPACE)
		{
			if (strcmp((char *)lv_img_get_src(tfcard_img_), image_path "TFcard_Broken.png") != 0)
				lv_img_set_src(tfcard_img_, image_path "TFcard_Broken.png");
		}
	}
}

void PageMain::UpdateTime(lv_timer_t* timer)
{
	PageMain* object = GlobalPage::Instance()->page_main();
	static int last_tm_sec = 0;

	//开启sta模式
#if 1
	static int wifi_sta_mode=0;
	static int time_cnt=0;
	FILE *file=NULL;
	if(time_cnt<100){
      	time_cnt++;
	}
	if(!wifi_sta_mode&&time_cnt==100){
		file = fopen("/mnt/tfcard/wpa.conf","r");
		wifi_sta_mode=-1;
		if(file){
			XMLogI("wpa.conf open success");	
			wifi_sta_mode=1;
			std::string b("wpa_supplicant -ieth2 -Dnl80211 -c /mnt/tfcard/wpa.conf -B");
			system(b.c_str());
			fclose(file);
			WifiEnableOpr(false);
		}	
	}
	if(GlobalPage::Instance()->page_main()->connect_computer_ && time_cnt==4*10){
		if(g_sd_status == XM_SD_NORMAL)
			GlobalPage::Instance()->page_usb()->SetUsbFunc(USB_MODE_MASS_STORAGE);
		else
            GlobalPage::Instance()->page_usb()->SetUsbFunc(USB_MODE_RNDIS);
	}
	 
 if(time_cnt==10*9){
	file = fopen("/mnt/tfcard/iperf","r");
		if(file){
		//std::string b("/usr/bin/iperf -s &");
		std::string b("/mnt/tfcard/iperf -s &");
		system(b.c_str());
		fclose(file);
		}
  }
#endif
#if 1
static int last_Record_Voice_status=-1;
static int last_gps_online_flag=-1;
	if(gps_insert_flag){
		if(last_gps_online_flag!=gps_online_flag){
			last_gps_online_flag=gps_online_flag;
			if(gps_online_flag){
				GPS_GREEN_LED_ON;
				GPS_RED_LED_OFF;
			}else{
				GPS_GREEN_LED_OFF;
				GPS_RED_LED_ON;
			}
		}
	}else{
		if(last_Record_Voice_status!=object->Record_Voice_status && last_Record_Voice_status != -1)
		{
			last_Record_Voice_status=object->Record_Voice_status;
			if(object->Record_Voice_status){
			   GPS_GREEN_LED_ON;
			   GPS_RED_LED_OFF;
			}else{
			   GPS_GREEN_LED_OFF;
			   GPS_RED_LED_ON;
			}
		}
	}

	if (GlobalData::Instance()->recording_) {

	    if (GlobalPage::Instance()->page_main()->lock_current_recording_file_ == false){
		//if (!lock_current_recording_file_) {
			
			GREEN_LED_ON;
			RED_LED_OFF;
		}
	}
#endif
	time_t t = time(NULL);
	struct tm* current_time = localtime(&t);

	if (t > object->pre_sys_time_) {
		object->pre_sys_time_ = t;
		int time = current_time->tm_hour + current_time->tm_min + current_time->tm_sec;
		if (time != object->pre_time_) {
			object->pre_time_ = time;
			lv_label_set_text_fmt(object->time_label_, "%02d:%02d:%02d",
				current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
		}

		int date = current_time->tm_year + current_time->tm_mon + current_time->tm_mday;
		if (date != object->pre_date_) {
			object->pre_date_ = date;
			lv_label_set_text_fmt(object->date_label_, "%04d-%02d-%02d",
				current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday);
		}
	}


	if (last_tm_sec != current_time->tm_sec)
	{
		last_tm_sec = current_time->tm_sec;
		// SD卡图标1秒闪烁一次
		if (g_sd_status == XM_SD_NEEDFORMAT ||
			g_sd_status == XM_SD_NEEDCHECKSPEED ||
			g_sd_status == XM_SD_ABNORMAL ||
			g_sd_status == XM_SD_NOTRW ||
			g_sd_status == XM_SD_NOSPACE)
		{
			if (lv_obj_has_flag(object->tfcard_img_, LV_OBJ_FLAG_HIDDEN))
			{
				lv_obj_clear_flag(object->tfcard_img_, LV_OBJ_FLAG_HIDDEN);
			}
			else
			{
				lv_obj_add_flag(object->tfcard_img_, LV_OBJ_FLAG_HIDDEN);
			}
		} 
		// 10秒循环播报SD卡状态语音
		GlobalPage::Instance()->page_main()->audio_flag_++;
		if (GlobalPage::Instance()->page_main()->audio_flag_ % 11 == 0)
		{
			GlobalPage::Instance()->page_main()->PlaySdCardStatus();
		}

	}

	if (GlobalData::Instance()->recording_) {
		if (object->compact_recording_) {
			int64_t start_time = GlobalPage::Instance()->page_main()->record_start_time_;
			int64_t now = GetTickTime();
			int record_time = (now - start_time) / 1000; //从毫秒切换到秒

			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = 8;
			GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
			if (cfg_value.int_value != 0 && record_time >= cfg_value.int_value * 3600) {
				XMLogW("Compact recording completed!");
				GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_Acc, true, true);
				lv_timer_del(timer);
				return;
			}
		}
		
		int time = XM_Middleware_Storage_GetRecordedTime(XM_STORAGE_SDCard_0, Direction_Front) / 1000;
		if (object->pre_record_time_ != time) {
			object->pre_record_time_ = time;
			//录像时红点闪烁
			if (lv_obj_has_flag(object->red_circle_img_, LV_OBJ_FLAG_HIDDEN))
				lv_obj_clear_flag(object->red_circle_img_, LV_OBJ_FLAG_HIDDEN);
			else
				lv_obj_add_flag(object->red_circle_img_, LV_OBJ_FLAG_HIDDEN);

			if (object->rec_time_label_)
			{
				char rec_time_buf[16];
				if (GlobalData::Instance()->recording_)
				{
					lv_obj_set_style_text_color(object->rec_time_label_, lv_color_make(255, 0, 0), 0);
					sprintf(rec_time_buf, "%02d:%02d", time / 60, time % 60);
				}
				else
				{
					lv_obj_set_style_text_color(object->rec_time_label_, lv_color_make(255, 255, 255), 0);
					sprintf(rec_time_buf, "STOP");
				}
				lv_label_set_text(object->rec_time_label_, rec_time_buf);
			}
		}
	}
}

int PageMain::CfgInit()
{
	XM_CONFIG_VALUE cfg_value;
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	int ret = 0;

	//同步系统时间
	std::string a("/sbin/hwclock -s");
	system(a.c_str());

	//按键音
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Key_Voice, cfg_value);
	GlobalData::Instance()->key_tone_ = cfg_value.bool_value;

	//使能osd时间
	cfg_value.bool_value = true;
	ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Time_Watermark, cfg_value);
	if(ret < 0){
		XMLogW("GetValue Failed!");
	}
	bool enable = cfg_value.bool_value;
	//MppMdl::Instance()->EnableOsdTime(enable, 0, 8192);
	//UsbDevice::Instance()->SetOSDenable(enable);
	XMLogI("XM_Middleware_Encode_EnableOsdTime, enable=%d", (int)enable);

	//录音开关
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	XM_Middleware_Storage_EnableRecordAudio(cfg_value.bool_value);
	GlobalPage::Instance()->page_main()->Record_Voice_status=cfg_value.bool_value;

#if 1//def CT317_OLD_SDK_TRANS
	XM_Middleware_Smart_EnableKeyword(false);
	
	cfg_value.int_value = 1;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Speaker_Volume, cfg_value);

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
			sound_val = 100;
			break;
		default:
			sound_val = 1;
			break;
			
	}
	
	//XM_Middleware_Sound_SetVolume(sound_val);
	MppMdl::Instance()->SetVolume(sound_val);
#endif

	//循环录影时长
	cfg_value.int_value = 1;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Circular_Record_Time, cfg_value);
	XMStorageRecordDuration record_dur = XM_STORAGE_Record_Duration_60;
	if (cfg_value.int_value == 1) {
		record_dur = XM_STORAGE_Record_Duration_60;
	}
	else if (cfg_value.int_value == 2) {
		record_dur = XM_STORAGE_Record_Duration_120;
	}
	else if (cfg_value.int_value == 3) {
		record_dur = XM_STORAGE_Record_Duration_180;
	}
	XM_Middleware_Storage_SetRecordDuration(record_dur);

	//声控
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	event_inparam.smart_enable_keyword.enable = cfg_value.bool_value;
	GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);

	//读取车充配置
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Car_Charger, cfg_value);
	GlobalPage::Instance()->page_main()->car_charger_ = cfg_value.bool_value;

	//缩时录影fps
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
	XMTimeLapse time_lapse = XM_TIME_LAPSE_Closed;
	if (cfg_value.int_value == 0) {
		time_lapse = XM_TIME_LAPSE_Closed;
	}
	else if (cfg_value.int_value == 1) {
		time_lapse = XM_TIME_LAPSE_Frame_1;
	}
	else if (cfg_value.int_value == 2) {
		time_lapse = XM_TIME_LAPSE_Frame_2;
	}
	else if (cfg_value.int_value == 5) {
		time_lapse = XM_TIME_LAPSE_Frame_5;
	}
	XM_Middleware_ParkRecord_SetTimeLapse(time_lapse);

	//缩时录影时长
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
	XMTimeLapseDuration dur = XM_TIME_LAPSE_Duration_Closed;
	if (cfg_value.int_value == 0) {
		dur = XM_TIME_LAPSE_Duration_Closed;
	}
	else if (cfg_value.int_value == 8) {
		dur = XM_TIME_LAPSE_Duration_8;
	}
	else if (cfg_value.int_value == 16) {
		dur = XM_TIME_LAPSE_Duration_16;
	}
	else if (cfg_value.int_value == 24) {
		dur = XM_TIME_LAPSE_Duration_24;
	}
	XM_Middleware_ParkRecord_SetTimeLapseDuration(dur);

	//碰撞灵敏度
	cfg_value.int_value = Sensitivity_Mid;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Sensitivity, cfg_value);
	PeripheryManager::Instance()->SensitivitySet(cfg_value.int_value);
	if (cfg_value.int_value == Sensitivity_Close) {
		PeripheryManager::Instance()->GsensorModeSelect(2);
	}
	else {
		PeripheryManager::Instance()->GsensorModeSelect(0);
	}
}

void PageMain::OpenUpgraderProgressPage()
{
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(80, 150, 190), 0);
	int page_width = size_w(260);
	
	lv_obj_t* upgrade_progress_page = lv_obj_create(lv_scr_act());
	lv_obj_set_size(upgrade_progress_page, page_width, size_h(86));
	lv_obj_set_style_bg_color(upgrade_progress_page, lv_color_make(38, 38, 38), 0);
	lv_obj_set_style_radius(upgrade_progress_page, 0, 0);
	lv_obj_set_style_border_width(upgrade_progress_page, 2, 0);
	lv_obj_set_scrollbar_mode(upgrade_progress_page, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(upgrade_progress_page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_all(upgrade_progress_page, 0, 0);
	lv_obj_center(upgrade_progress_page);

	upgrade_bar_ = lv_bar_create(upgrade_progress_page);
	lv_obj_set_size(upgrade_bar_, page_width, size_h(86));
	lv_obj_center(upgrade_bar_);
	lv_bar_set_range(upgrade_bar_, 0, 100);
	lv_obj_set_style_radius(upgrade_bar_, 0, 0);
	lv_obj_set_style_radius(upgrade_bar_, 0, LV_PART_INDICATOR);

	upgrade_start_time_ = GetTickTime();
	lv_timer_create(SetBarValue, 500, NULL);
}

void PageMain::SetBarValue(lv_timer_t* timer)
{
	PageMain* object = GlobalPage::Instance()->page_main();

	int64_t start_time = object->upgrade_start_time_;
	int64_t now = GetTickTime();
	int upgrader_time = (now - start_time) / 1000;

	static int cnt=0;
	cnt++;
	if(cnt%2)RED_LED_ON;
	else RED_LED_OFF;

	int progress = 0;
	if (g_upgrade_type == "upgradeSub") {
		progress = UsbDevice::Instance()->GetUpgraderProgress();
	}
	else if (g_upgrade_type == "upgradeMain") {
		progress = XM_Middleware_SDCard_GetUpgradeProgress();
	}
	else if (g_upgrade_type == "upgradeAll") {
		int usbdev_progress = UsbDevice::Instance()->GetUpgraderProgress();
		if (usbdev_progress < 150) {
			progress = usbdev_progress/2;
		}
		else {
			int main_progress = XM_Middleware_SDCard_GetUpgradeProgress();
			if (main_progress < 150) {
				progress = main_progress/2 + 50;
			}
			else
				progress = 150;
		}
	}

	//进度150代表升级完成，MD5文件生成
	XMLogI("Get upgrade progress=%d", progress);
	if (progress == 150) {
		reboot(RB_AUTOBOOT); // 重启系统
		return;
	}

	lv_bar_set_value(object->upgrade_bar_, progress, LV_ANIM_ON);
}

int PageMain::StartRecord(bool compact_rec, bool play_audio)
{
	if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024 && !GlobalData::Instance()->recording_) {
		if ((g_app_connect && GlobalData::Instance()->app_page_ != APP_PAGE_PREVIEW) ||GlobalPage::Instance()->page_main()->connect_computer_){
			
		    XMLogW("[StartRecord] return \r\n");
			return 0;
		}

#if 1//X2V60_S_DEBUG1
		if (g_first_record){
			
        	usleep(3 * 1000 * 1000);
		    g_first_record = false;
		}
#endif
		
		GlobalData::Instance()->recording_ = true;
		GlobalPage::Instance()->page_main()->pre_record_time_ = 0;
		record_start_time_ = GetTickTime();

		std::string sound_file = kAudioPath;
		if (compact_rec) {
			XM_Middleware_ParkRecord_Enable(true);
			// XMLogI("red_circle_img_, yellow");
			// lv_img_set_src(red_circle_img_, image_path"yellow_circle.png");
			sound_file += "Start time-lapse video_16k.pcm";
		}
		else {
			sound_file += "Start recording_16k.pcm";
		}

		if (play_audio)
			MppMdl::Instance()->PlaySound(sound_file.c_str());

		if (!MppMdl::Instance()->AdLoss()){
			
			GlobalData::Instance()->ui_event_cb_(XM_UI_START_STORAGE, NULL, NULL);
		}else {
			
			XM_MW_Media_Info media_info;
			MppMdl::Instance()->GetEncInfo(0, 0, &media_info);
			media_info.frame_rate = kFrameRate;	
			XM_Middleware_Storage_SetMediaInfo(0, &media_info);
			XM_Middleware_Storage_Start(0, 0);
			XM_Middleware_Storage_Start(2, 0);
			g_storage_started = true;	
		}

		if (!rec_time_label_) {
			rec_time_label_ = GlobalData::Instance()->gui()->CreateLabel(main_page_, "00:00");
			lv_obj_set_style_text_color(rec_time_label_, lv_color_make(255, 0, 0), 0);
			lv_obj_set_style_text_font(rec_time_label_, &lv_font_montserrat_20, 0);
			lv_obj_align_to(rec_time_label_, date_label_, LV_ALIGN_OUT_LEFT_MID, size_w(-85), 0);
		}
		HTTPSerMdl::Instance()->tcpRecStatus(g_engineId, 1);
		GREEN_LED_ON;  
	    RED_LED_OFF;  
	}
	
	if (collision_startup_)
	{
		LockCurrentFile();
		record_start_time_ = GetTickTime();
		collision_record_timer_ = lv_timer_create(CollisionRecordTime, 100, NULL);
	}
	return 0;
}

void PageMain::CollisionRecordTime(lv_timer_t *timer)
{
	int64_t start_time = GlobalPage::Instance()->page_main()->record_start_time_;
	int64_t now = GetTickTime();
	int record_time = (now - start_time) / 1000; // 从毫秒切换到秒

	if (record_time >= 60)
	{
		XMLogW("Collision recording completed!");
		GlobalPage::Instance()->page_main()->CollisionStartupWork();
		lv_timer_del(GlobalPage::Instance()->page_main()->collision_record_timer_);
		GlobalPage::Instance()->page_main()->collision_record_timer_ = NULL;
	}
	
	return;
}

int PageMain::CollisionStartupWork(void)
{
	if (collision_startup_)
	{
		collision_startup_ = false;
		XMLogI("CloseRecord();");

		CloseRecord();

		XMLogI("[CollisionStartupWork] 1 acc_disconnect_:%d\r\n", g_acc_connected);
		if (g_acc_connected)
		{
			
		    XMLogI("[CollisionStartupWork] 2 before StartRecord, g_acc_connected:%d \r\n", g_acc_connected);
			StartRecord(false, true);
		}else

        {
		   lv_timer_create(DelayShutDownTimer, 4000, NULL);
	    }
	}

	return 0;
}

int PageMain::CloseRecord(bool compact_rec, bool play_audio)
{
	if (GlobalData::Instance()->recording_) {
		GlobalData::Instance()->recording_ = false;
		pre_record_time_ = 0;
		record_start_time_ = 0;
		GlobalData::Instance()->ui_event_cb_(XM_UI_STOP_STORAGE, NULL, NULL);
		XMLogI("CloseRecord();");

		std::string sound_file = kAudioPath;
		if (compact_rec) {
			XM_Middleware_ParkRecord_Enable(false);
			XMLogI("red_circle_img_, red");
			lv_img_set_src(red_circle_img_, image_path"red_circle.png");
			sound_file += "Stop time-lapse video_16k.pcm";
		}
		else {
			sound_file += "Stop recording_16k.pcm";
		}

		if (play_audio)
			MppMdl::Instance()->PlaySound(sound_file.c_str());

		lv_obj_add_flag(red_circle_img_, LV_OBJ_FLAG_HIDDEN);

		// 客户定制UI不需要录像时长
		if (rec_time_label_) {
			lv_obj_del(rec_time_label_);
			rec_time_label_ = NULL;
		}

		// 客户定制UI不需要录像时长
		if (!rec_time_label_)
		{
			rec_time_label_ = GlobalData::Instance()->gui()->CreateLabel(main_page_, "STOP");
			lv_obj_set_style_text_color(rec_time_label_, lv_color_make(255, 255, 255), 0);
			lv_obj_set_style_text_font(rec_time_label_, &lv_font_montserrat_20, 0);
			lv_obj_align_to(rec_time_label_, date_label_, LV_ALIGN_OUT_LEFT_MID, size_w(-85), 0);
		}
		HTTPSerMdl::Instance()->tcpRecStatus(g_engineId, 0);
	}
    GREEN_LED_OFF;	
	RED_LED_ON;
	return 0;
}

void PageMain::JudgeAppConnectTimer(lv_timer_t* timer)
{
	if (GlobalPage::Instance()->page_main()->sd_formatting_) {
		return;

	}
	
	g_app_connect = false;
	HTTPSerMdl::Instance()->CloseAll();
	XM_Middleware_RTSP_Stop();
	g_realplay = false;
	GlobalPage::Instance()->page_main()->WIFIImg();

	GlobalData::Instance()->app_page_ = APP_PAGE_PREVIEW;
	MppMdl::Instance()->SubStreamEnable(false);
	GlobalPage::Instance()->page_main()->StartRecord();

	GlobalPage::Instance()->page_main()->judge_app_timer_ = NULL;
	lv_timer_del(timer);
	XMLogW("App disconnect!");
}

void PageMain::RecordVoiceEnable(bool enable)
{
	XM_CONFIG_VALUE cfg_value;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	GlobalPage::Instance()->page_main()->Record_Voice_status=enable;
	if (cfg_value.bool_value == enable) return;
	cfg_value.bool_value = enable;
	XM_Middleware_Storage_EnableRecordAudio(cfg_value.bool_value);

	if (enable) {
		std::string sound_file = kAudioPath;
		sound_file += "Mute on_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	}
	else {
		std::string sound_file = kAudioPath;
		sound_file += "Mute off_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	}
	
	int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Record_Voice, cfg_value);
	if (ret < 0) {
		XMLogE("set config error, opr=CFG_Operation_Record_Voice");
		return;
	}
	MICImg(cfg_value.bool_value);
}

int PageMain::WifiEnable(bool enable)
{
	XMLogI("WifiEnable enable = %d", enable);
	if (wifi_enable_ == enable) {
		XMLogE("WifiEnable error");
		return -1;
	}
    XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value =enable;
	std::string command_dhcp;
	if (enable) {
		GlobalData::Instance()->app_page_ = APP_PAGE_PREVIEW;
		command_dhcp = "/sbin/ifconfig eth2 up 192.168.169.1";
		system(command_dhcp.c_str());
		//cfg_value.int_value = WIFI_TYPE_5G;
		//GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Wifi_Type, cfg_value);
		if (cfg_value.int_value == WIFI_TYPE_5G) {
			command_dhcp = "/usr/sbin/hostapd /mnt/mtd/Config/hostapd_5G.conf -B -ddd";
		}
		else {
			command_dhcp = "/usr/sbin/hostapd /mnt/mtd/Config/hostapd.conf -B -ddd";
		}
		system(command_dhcp.c_str());
		command_dhcp = "/sbin/udhcpd -fS /usr/sbin/udhcpd.conf &";
		system(command_dhcp.c_str());

		//开启server
		XM_Middleware_Network_SetDataCallback(OnNetworkMsg);
		XM_Middleware_Network_CloseThreadCallback(closeThread);
		AppParamInit();
		
		XM_Middleware_Network_StartServer(80, 2222);
		lv_ignore_signals();
	}
	else {
		
#if 1//X2V60_S_DEBUG1
		HTTPSerMdl::Instance()->tcpWiFiStatus(g_engineId);
        sleep(1);
#endif

		XM_Middleware_Network_StopServer();
		XM_Middleware_RTSP_Stop();
		g_realplay = false;
		MppMdl::Instance()->SubStreamEnable(false);
		if (judge_app_timer_) {
			lv_timer_del(judge_app_timer_);
			judge_app_timer_ = NULL;
		}

		g_app_connect = false;
		HTTPSerMdl::Instance()->CloseAll();
		XM_Middleware_RTSP_Stop();
		GlobalData::Instance()->app_page_ = APP_PAGE_PREVIEW;
		WifiEnableOpr(false);
		command_dhcp = "/sbin/ifconfig eth2 down";
		system(command_dhcp.c_str());
	}
	
	wifi_enable_ = enable;
	#if 1//X2V60_S_DEBUG1
	if (!compact_recording_) {
		
    	cfg_value.int_value=enable;
    	GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Wifi_Type, cfg_value);
	}
	#endif
	
	WIFIImg();
}

void PageMain::AppParamInit()
{
	//传递初始化状态
	APP_PARAM_ITEMS_S param_items;
	param_items.record = GlobalData::Instance()->recording_ ? 1 : 0;
	param_items.sdcard_speed = GlobalData::Instance()->SDCard_write_speed_;
	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = true;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_IR_Mode, cfg_value);
	param_items.ir_mode = -1; //cfg_value.int_value;

#if 1//X2V60_S_DEBUG1
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Speaker_Volume, cfg_value);
	param_items.speaker_volume = cfg_value.int_value;
#endif
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Time_Watermark, cfg_value);
	param_items.osd_time = cfg_value.bool_value ? 1 : 0;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	param_items.mic = cfg_value.bool_value ? 1 : 0;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	#if VOICE_CONTROL_EN
	param_items.voice_control = cfg_value.int_value;
    #else
	param_items.voice_control = -1;
	#endif
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
	param_items.resolution = cfg_value.int_value;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Circular_Record_Time, cfg_value);
	param_items.circular_record_time = cfg_value.int_value;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Sensitivity, cfg_value);
	if (cfg_value.int_value == Sensitivity_Close)
		param_items.gsr_sensitivity = 0;
	else if (cfg_value.int_value == Sensitivity_Low)
		param_items.gsr_sensitivity = 1;
	else if (cfg_value.int_value == Sensitivity_Mid)
		param_items.gsr_sensitivity = 2;
	else if (cfg_value.int_value == Sensitivity_High)
		param_items.gsr_sensitivity = 3;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
	param_items.parking_monitor = cfg_value.bool_value ? 1 : 0;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
	param_items.timelapse_rate = cfg_value.int_value;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Key_Voice, cfg_value);
	param_items.key_tone = cfg_value.bool_value;

	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
	param_items.park_record_time = cfg_value.int_value;

	strncpy(param_items.version, GlobalData::Instance()->version().c_str(), sizeof(param_items.version) - 1);
	HTTPSerMdl::Instance()->CfgInit(&param_items);
}

int PageMain::OnADEvent(bool ad_connect)
{
	if (ad_connect) {
		if (GlobalData::Instance()->recording_) {
			XM_Middleware_Storage_Stop(-1);
			GlobalData::Instance()->ui_event_cb_(XM_UI_START_STORAGE, NULL, NULL);
		}
	}
	else {
		if (GlobalData::Instance()->recording_) {
			XM_Middleware_Storage_Stop(-1);
			XM_MW_Media_Info media_info;
			MppMdl::Instance()->GetEncInfo(0, 0, &media_info);
			media_info.frame_rate = kFrameRate;	
			XM_Middleware_Storage_SetMediaInfo(0, &media_info);
			XM_Middleware_Storage_Start(0, 0);
			XM_Middleware_Storage_Start(2, 0);	
			g_storage_started = true;
		}
	}
}

void ShowShutdownLogo()
{
	int width = MY_DISP_HOR_RES;
	int height = MY_DISP_VER_RES;
	std::string logo_path = "/mnt/custom/Pic/172x320_shutdown.rgb";
	FILE* fp = fopen(logo_path.c_str(), "rb");
	if (!fp) {
		XMLogE("open logo file error");
		return;
	}

	uint8_t* logo_buf = new uint8_t[width * height * 2];
	int bytes = fread(logo_buf, width * height * 2, 1, fp);
	if (bytes <= 0) {
		XMLogE("read logo file error");
		return;
	}
#if 0
	lcd_data_s lcd_data = {0};
	memset(&lcd_data, 0, sizeof(lcd_data_s));
	lcd_data.x = 0;
	lcd_data.y = 0;
	lcd_data.width = width;
	lcd_data.height = height;
	lcd_data.data = logo_buf;
	lcd_data.size = width * height * 2;
	LIBCR_LCD_Display(&lcd_data);
#endif	
	fclose(fp);
	delete logo_buf;
}

void PageMain::ShutDownTimer_PullPowerOn(lv_timer_t* timer)
{
	    	static int cnt=0;
			cnt++;
			XMLogI("ShutDownTimer_PullPowerOn");
			if(cnt==1){
			POWER_OFF;
			}else if(cnt>2){
			SPEAK_OFF;
			 lv_timer_del(timer);
			}
	   
}

void PageMain::DelayTimer_SpkOff(lv_timer_t* timer)
{
	SPEAK_OFF;
}

void PageMain::DelayTimer_SpkOn(lv_timer_t* timer)
{
	SPEAK_ON;
}

void PageMain::ShutDown(ShutDownMode shutdownmode, bool play_pic, bool close_preview)
{
	if (play_pic) {
		//停止lvgl画面刷新
		XMLogI("before stop refresh screen!");
		RED_LED_OFF;
		GREEN_LED_OFF;
		WiFi_LED_OFF;
		GPS_RED_LED_OFF;
		GPS_GREEN_LED_OFF;
		
#if 1//X2V60_S_DEBUG1
        lv_timer_create(DelayTimer_SpkOff, 700, NULL);
        lv_timer_create(ShutDownTimer_PullPowerOn, 600, (void*)shutdownmode);
#else
		lv_timer_create(ShutDownTimer_PullPowerOn, 400, (void*)shutdownmode);
#endif

		GlobalData::Instance()->stop_screen_refresh_ = true;
		//关闭录像
		GlobalData::Instance()->ui_event_cb_(XM_UI_STOP_STORAGE, NULL, NULL);
		//显示关机图片
		XMLogI("before ShowShutdownLogo");
		ShowShutdownLogo();
		//播放关机音乐,如果是电池供电不放音乐
		XMLogI("before MppMdl::Instance()->PlaySound");
		std::string sound_file = kAudioPath;
		sound_file += "shutdown_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	
		//释放资源
		XMLogI("before XM_Middleware_Exit");
		XM_Middleware_Exit();
		//关闭热点
		WifiEnableOpr(false);
		//关机前将碰撞灵敏度设为高，若碰撞开机为关的话则关闭
		XMLogI("before SensitivitySet");
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = true;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
		if (cfg_value.bool_value) {
			PeripheryManager::Instance()->SensitivitySet(Sensitivity_StartUp);
			PeripheryManager::Instance()->GsensorModeSelect(1);
		}
		else {
			PeripheryManager::Instance()->SensitivitySet(Sensitivity_Close);
			PeripheryManager::Instance()->GsensorModeSelect(2);
		}
		
		XMLogI("before Create ShutDownTimer");
		
#if 1//X2V60_S_DEBUG1

        lv_timer_create(ShutDownTimer, 400, (void*)shutdownmode);
#else
		lv_timer_create(ShutDownTimer, 1500, (void*)shutdownmode);
#endif
		XMLogI("after Create ShutDownTimer");
	}
	else {
		XMLogI("before XM_Middleware_Exit");
		XM_Middleware_Exit();
		XMLogI("Shutdown");
		PeripheryManager::Instance()->Shutdown(shutdownmode);
	}
}

void PageMain::Reboot( bool play_pic)
{
	if (play_pic) {
		//停止lvgl画面刷新
		XMLogI("before stop refresh screen!");
		GlobalData::Instance()->stop_screen_refresh_ = true;
		//关闭录像
		GlobalData::Instance()->ui_event_cb_(XM_UI_STOP_STORAGE, NULL, NULL);
		//显示关机图片
		XMLogI("before ShowShutdownLogo");
		ShowShutdownLogo();
		//播放关机音乐,如果是电池供电不放音乐
		XMLogI("before MppMdl::Instance()->PlaySound");
		std::string sound_file = kAudioPath;
		sound_file += "shutdown_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	
		//释放资源
		XMLogI("before XM_Middleware_Exit");
		XM_Middleware_Exit();
		//关闭热点
		WifiEnableOpr(false);
		//关机前将碰撞灵敏度设为高，若碰撞开机为关的话则关闭
		XMLogI("before SensitivitySet");
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = true;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
		if (cfg_value.bool_value) {
			PeripheryManager::Instance()->SensitivitySet(Sensitivity_StartUp);
			PeripheryManager::Instance()->GsensorModeSelect(1);
		}
		else {
			PeripheryManager::Instance()->SensitivitySet(Sensitivity_Close);
			PeripheryManager::Instance()->GsensorModeSelect(2);
		}
		sleep(3);
		reboot(RB_AUTOBOOT);
	}
	else {
		XMLogI("before XM_Middleware_Exit");
		XM_Middleware_Exit();
		XMLogI("Reboot");
		sleep(3);
		reboot(RB_AUTOBOOT);
	}
}

void PageMain::ShutDownTimer(lv_timer_t* timer)
{
	XMLogI("Enter ShutDownTimer");
	int shutdownmode = (int)timer->user_data;
	XMLogI("Shutdown");
	PeripheryManager::Instance()->Shutdown((ShutDownMode)shutdownmode);
	lv_timer_del(timer);
}

void PageMain::DelayShutDownTimer(lv_timer_t* timer)
{
	GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_Acc, true, true);
	lv_timer_del(timer);
}

void PageMain::OpenTipBox(const char* string, uint32_t timer_period, bool switch_string, int w, int h)
{
	lv_obj_t* tip_box = GlobalData::Instance()->gui()->CreatePage(lv_scr_act(), size_w(w), size_h(h),
		lv_palette_main(LV_PALETTE_BLUE), 0, 0, lv_font_all, lv_color_white());
	lv_obj_align(tip_box, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t* label = NULL;
	if (switch_string) {
		label = GlobalData::Instance()->gui()->CreateLabel(tip_box, GetParsedString(string), size_w(w),
			LV_TEXT_ALIGN_CENTER);
	}
	else {
		label = GlobalData::Instance()->gui()->CreateLabel(tip_box, string, size_w(w),
			LV_TEXT_ALIGN_CENTER);
	}

	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

	lv_timer_t* tip_timer = lv_timer_create(CloseTipBoxTimer, timer_period, (void*)tip_box);
}

void PageMain::CloseTipBoxTimer(lv_timer_t* timer)
{
	lv_obj_t* user_data = (lv_obj_t*)timer->user_data;
	lv_obj_del(user_data);
	lv_timer_del(timer);
}

void PageMain::ChangeCarChargerCfg(bool car_charger)
{
	if (car_charger == GlobalPage::Instance()->page_main()->car_charger_)
		return;

	GlobalPage::Instance()->page_main()->car_charger_ = car_charger;
	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = car_charger;
	int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Car_Charger, cfg_value);
	if (ret < 0) {
		XMLogE("set config error, opr=CFG_Operation_Car_Charger");
	}
}

void PageMain::AccDisconnectWork()
{
	if (compact_recording_ || sd_formatting_) {
		XMLogW("Retracting video recording!");
		return;
	}
	
	//检测缩时录影FPS
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
	int compact_record_fps = cfg_value.int_value;
	//检测缩时录影时长
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
	int compact_record_dur = cfg_value.int_value;

	if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024 && compact_record_fps > 0) {
		OpenTipBox("About to enter minitype video mode");
		if (acc_disconnect_timer_) {
			lv_timer_del(acc_disconnect_timer_);
			acc_disconnect_timer_ = NULL;
		}

		UsbDevice::Instance()->SetFPS(compact_record_fps);
		acc_disconnect_timer_ = lv_timer_create(CompactRecordTimer, 2000, NULL);
	}
	else {
		ShutDown(ShutDownMode_Acc, true, true);
	}
}

void PageMain::CompactRecordTimer(lv_timer_t* timer)
{
	if (GlobalPage::Instance()->page_main()->acc_disconnect_) {
		GlobalPage::Instance()->page_main()->CompactRecord();
	}

	GlobalPage::Instance()->page_main()->acc_disconnect_timer_ = NULL;
	lv_timer_del(timer);
}

void PageMain::CompactRecord()
{
	compact_recording_ = true;

	//退出wifi
	if (wifi_enable_) {
		WifiEnable(false);
	}

	//关闭录像
	CloseRecord(false, false);

	//关闭声控
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	event_inparam.smart_enable_keyword.enable = false;
	GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);

	//进行缩时录影
	StartRecord(true);
}

void PageMain::AccConnectWork()
{
	if (compact_recording_) {
		OpenTipBox("About to exit minitype video mode");
		if (acc_connect_timer_) {
			lv_timer_del(acc_connect_timer_);
			acc_connect_timer_ = NULL;
		}
		UsbDevice::Instance()->SetFPS(0);
		acc_connect_timer_ = lv_timer_create(ExitCompactRecordTimer, 2000, NULL);
	}
}

void PageMain::ExitCompactRecordTimer(lv_timer_t* timer)
{
	if (!GlobalPage::Instance()->page_main()->acc_disconnect_ && GlobalPage::Instance()->page_main()->compact_recording_) {
		GlobalPage::Instance()->page_main()->ExitCompactRecord();
	}

	GlobalPage::Instance()->page_main()->acc_connect_timer_ = NULL;
	lv_timer_del(timer);
}

void PageMain::ExitCompactRecord()
{
	//关闭缩时录影
	CloseRecord(true);

	//恢复声控
	XM_CONFIG_VALUE cfg_value;
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	event_inparam.smart_enable_keyword.enable = cfg_value.bool_value;
	GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);

	//进行正常录影
	compact_recording_ = false;
	StartRecord(false, false);

	//开启wifi
	if (!wifi_enable_) {
		WifiEnable(true);
	}
}

void PageMain::LockCurrentFile()
{
	if (!lock_current_recording_file_) {
		if (GlobalData::Instance()->recording_) {
			lock_current_recording_file_ = true;
			if (red_circle_img_) {
				XMLogI("red_circle_img_, yellow");
				lv_img_set_src(red_circle_img_, image_path"yellow_circle.png");
			}

			XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, 0, true);
			XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, 1, true);
			XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, 2, true);

			std::string sound_file = kAudioPath;
			sound_file += "Video locked_16k.pcm";
			MppMdl::Instance()->PlaySound(sound_file.c_str());
			RED_LED_ON;
		}
	}
	else {
		if (red_circle_img_) {
			XMLogI("red_circle_img_, red");
			lv_img_set_src(red_circle_img_, image_path"red_circle.png");
		}

        RED_LED_OFF;
		lock_current_recording_file_ = false;
		XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, 0, false);
		XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, 1, false);
		XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, 2, false);
	}
}

void PageMain::PlaySdCardStatus()
{
	std::string sound_file = kAudioPath;
	// 10秒循环重新计时
	GlobalPage::Instance()->page_main()->audio_flag_ = 1;

	if (g_sd_status == XM_SD_NEEDFORMAT || g_sd_status == XM_SD_NEEDCHECKSPEED)
	{
		sound_file += "Please format the SD card_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	}
	else if (g_sd_status == XM_SD_ABNORMAL || g_sd_status == XM_SD_NOTRW || g_sd_status == XM_SD_NOSPACE)
	{
		sound_file += "memory card error.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	}
#if 1//X2V60_S_DEBUG1
	else if (g_sd_status == XM_SD_NOEXIST) {// || !sd_exit
		
		sound_file += "Please insert the SD card_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	
	}
#endif
}

#if 1//X2V60_S_DEBUG1
void PageMain::Wifi5MinDelayTimer(lv_timer_t* timer)
{

	//return;

	if(g_app_connect) {

		return;
	}
	
  #if 1
  if (GlobalPage::Instance()->page_main()->wifi_prepared_) {
  	
	  if(GlobalPage::Instance()->page_main()->wifi_enable_ && !g_wifi_man_oper){
	  	//
	    GlobalPage::Instance()->page_main()->WifiEnable(!GlobalPage::Instance()->page_main()->wifi_enable_);
	  }
   }
  
	GlobalPage::Instance()->page_main()->wifi_5min_timer_ = NULL;
	lv_timer_del(timer);
	#endif
}
#endif


