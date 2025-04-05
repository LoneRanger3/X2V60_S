#ifndef __PAGEMAIN_H__
#define __PAGEMAIN_H__

#include "lvgl.h"
#include <time.h>
#include <string>
#include "xm_middleware_def.h"

class PageMain
{
public:
	PageMain();
	~PageMain();

	int OpenPage();
	int CfgInit();
	void OpenUpgraderProgressPage();
	int StartRecord(bool compact_rec = false, bool play_audio = true);
	int CloseRecord(bool compact_rec = false, bool play_audio = true);
	void RecordVoiceEnable(bool enable);
	int WifiEnable(bool enable);
	void WIFIID();
	void WIFIImg();
	void MICImg(bool enable);
	void SDCardImg();
	int OnADEvent(bool ad_connect);
	void ShutDown(ShutDownMode shutdownmode, bool play_pic, bool close_preview);
	void Reboot(bool play_pic);
	void ChangeCarChargerCfg(bool car_charger);
	void AccDisconnectWork();
	void AccConnectWork();
	void OpenTipBox(const char* string, uint32_t timer_period = 2000, bool switch_string = true, int w = 320, int h = 172);
	void LockCurrentFile();
	void AppParamInit();
	void PlaySdCardStatus();
	int CollisionStartupWork(void);
	static void DelayShutDownTimer(lv_timer_t* timer);
	static void JudgeAppConnectTimer(lv_timer_t* timer);
    static void ShutDownTimer_PullPowerOn(lv_timer_t* timer);
	static void Wifi5MinDelayTimer(lv_timer_t* timer);
    static void DelayTimer_SpkOff(lv_timer_t* timer);
    static void DelayTimer_SpkOn(lv_timer_t* timer);

private:
	void CompactRecord();
	void ExitCompactRecord();
	static void SetBarValue(lv_timer_t* timer);
	static void UpdateTime(lv_timer_t* timer);
	static void ShutDownTimer(lv_timer_t* timer);
	static void CloseTipBoxTimer(lv_timer_t* timer);
	static void CompactRecordTimer(lv_timer_t* timer);
	static void ExitCompactRecordTimer(lv_timer_t* timer);
	static void CollisionRecordTime(lv_timer_t* timer);

public:
	bool wifi_enable_;
	time_t pre_sys_time_;
	int pre_time_;
	int pre_date_;
	int audio_flag_;
	lv_obj_t* main_page_;
	lv_obj_t* red_circle_img_;
	lv_obj_t* wifi_img_;
	lv_obj_t* mic_img_;
	lv_obj_t* tfcard_img_;
	lv_obj_t* wifi_id_label_;
	lv_obj_t* time_label_;
	lv_obj_t* date_label_;

	lv_timer_t* collision_record_timer_;
	lv_timer_t* acc_disconnect_timer_;
	lv_timer_t* acc_connect_timer_;
	lv_timer_t* wifi_5min_timer_;
	
	bool acc_disconnect_;				//acc是否断开变量
	bool lock_current_recording_file_;	//是否正在录制加锁录像（加锁键控制）
	int collision_startup_;
	bool car_charger_;					//是否是车充
	bool compact_recording_;
	bool sd_formatting_;
	lv_timer_t* judge_app_timer_;
	bool wifi_prepared_;
	bool connect_computer_;
	bool Record_Voice_status;
private:
	lv_obj_t* upgrade_bar_;
	int64_t upgrade_start_time_;
	int pre_record_time_;
	lv_obj_t* rec_time_label_;
	int64_t record_start_time_;
};

#endif // !__PAGEMAIN_H__
