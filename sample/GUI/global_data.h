#ifndef __GLOBAL_DATA_H__
#define __GLOBAL_DATA_H__

#include <string>
#include <string.h>
#include "set_config.h"
#include "xm_type.h"
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
#include "Log.h"
#include "GUI.h"

#ifdef WIN32
#define image_path "A:D:\\resource\\image\\"
#else 
#define image_path "A:/mnt/custom/Pic/"
#endif

#define SCREEN_WIDTH 		320
#define SCREEN_HEIGHT		172
#define size_w(w) ((SCREEN_WIDTH * w) / 320)
#define size_h(h) ((SCREEN_HEIGHT * h) / 172)
#define start_y 0

extern const char* kTFCardPath;
extern const char* kUDiskPath;
extern const char* kAudioPath;
extern const char* kUpdateFileName;
extern const char* kUSBDeviceUpdateFile;
extern const char* kUpdateMD5Name;
extern const char* kUSBDeviceUpdateMD5File;

extern const char* kUSBDeviceIP;
extern bool g_storage_started;
extern bool g_realplay;
extern int g_realplay_channel;
extern const int k3KHeight;
extern const int k4KHeight;
typedef enum _XMUIVideoDirection
{
	Direction_Front = 0,
	Direction_Behind,
    Direction_Total,
}XMUIVideoDirection;

typedef enum _UIMode
{
	UIMode_Videotape,
	UIMode_Photograph,
	UIMode_Playback,
	UIMode_Picture,
	UIMode_CompactRecord,
}UIMode;

typedef enum _XMUIEventType
{
	XM_UI_START_STORAGE = 0,
    XM_UI_STOP_STORAGE,
    XM_UI_QUERY_RECORD,
    XM_UI_VOD_OPEN,
    XM_UI_VOD_GET_FILE_INFO,            //获取文件总时间
    XM_UI_VOD_PLAY,
    XM_UI_VOD_PAUSE,
    XM_UI_VOD_SEEK,
    XM_UI_VOD_GET_CURRENT_DURATION,     //当前播放时间
	XM_UI_VOD_STOP,
    XM_UI_VOD_CLOSE,
	XM_UI_VOD_LOCK_FILE,
	XM_UI_VOD_UNLOCK_FILE,
	XM_UI_VOD_LOCK_HISTORY_FILE,
	XM_UI_VOD_UNLOCK_HISTORY_FILE,
	XM_UI_SET_STORAGE_CONFIG,
    XM_UI_SET_ENCODE_PARAM,
	XM_UI_PICTURE_RENDER,
	XM_UI_PICTURE_CLOSE,
	XM_UI_SMART_SET_AI_TYPE,
	XM_UI_SMART_SET_BSD_PARAM,
	XM_UI_SMART_ENABLE_AI,
	XM_UI_SMART_ENABLE_KEYWORD,
	XM_UI_SPLIT_SET_GRID_RECT,
	XM_UI_SPLIT_SET_SPLID,
	XM_UI_STORAGE_DEL_FILE,
	XM_UI_SOUND_SET_VOLUME,
	XM_UI_PREVIEW_STOP,
	XM_UI_MIRROR_SET_MIRROR,
	XM_UI_SYS_UPGRADE,
	XM_UI_GET_UPGRADE_PROGRESS,
	XM_UI_UPGRADE_QUERY,
}XMUIEventType;

//录像查询入参
typedef struct _XMUIQueryRecordInparam
{
    XM_MW_Record_Query rec_query;
}XMUIQueryRecordInparam;

//录像查询出参
typedef struct _XMUIQueryRecordOutparam
{
    XM_MW_Record_List rec_list;
}XMUIQueryRecordOutparam;

//开始点播入参
typedef struct _XMUIVodStartInparam
{
    XMStorageSDCardNum sdcard_num;
	XMFileFormat file_format;
    int channel;
    char file_full_name[XM_MAX_STORAGE_PATH_LEN];
}XMUIVodStartInparam;

//开始点播出参
typedef struct _XMUIVodStartOutparam
{
    int handle;
}XMUIVodStartOutparam;

//获取文件信息入参
typedef struct _XMUIGetFileInfoInparam
{
    int handle;
}XMUIGetFileInfoInparam;

//获取文件信息出参
typedef struct _XMUIGetFileInfoOutparam
{
    int duration;
	XM_MW_Media_Info media_info;
}XMUIGetFileInfoOutparam;

//开始播放入参
typedef struct _XMUIVodPlayInparam
{
	int handle;
	bool play_one;
	RECT_S dst_rc; //渲染的位置
}XMUIVodPlayInparam;

//开始播放出参
typedef struct _XMUIVodPlayOutparam
{
}XMUIVodPlayOutparam;

//点播暂停入参
typedef struct _XMUIVodPauseInparam
{
    int handle;
    bool pause; //true表暂停, false表恢复
}XMUIVodPauseInparam;

//点播暂停出参
typedef struct _XMUIVodPauseOutparam
{
}XMUIVodPauseOutparam;

//点播拖动入参
typedef struct _XMUIVodSeekInparam
{
    int handle;
    int seek_time; //单位是毫秒
}XMUIVodSeekInparam;

//点播拖动出参
typedef struct _XMUIVodSeekOutparam
{
}XMUIVodSeekOutparam;

//点播获取进度入参
typedef struct _XMUIVodGetCurDurationInparam
{
    int handle;
}XMUIVodGetCurDurationInparam;

//点播获取进度出参
typedef struct _XMUIVodGetCurDurationOutparam
{
    int cur_duration;
}XMUIVodGetCurDurationOutparam;

//停止点播入参
typedef struct _XMUIVodStopInparam
{
    int handle;
}XMUIVodStopInparam;

//停止点播出参
typedef struct _XMUIVodStopOutparam
{
}XMUIVodStopOutparam;

//锁定文件入参
typedef struct _XMUIVodLockFileInparam
{
	XMStorageSDCardNum sdc_num;
	int channel;
}XMUIVodLockFileInparam;

//锁定文件出参
typedef struct _XMUIVodLockFileOutparam
{
}XMUIVodLockFileOutparam;

//解锁文件入参
typedef struct _XMUIVodUnlockFileInparam
{
	XMStorageSDCardNum sdc_num;
	int channel;
}XMUIVodUnlockFileInparam;

//解锁文件出参
typedef struct _XMUIVodUnlockFileOutparam
{
}XMUIVodUnlockFileOutparam;

//锁定历史文件入参
typedef struct _XMUIVodLockHistoryFileInparam
{
	XM_MW_File_Param file_param;
}XMUIVodLockHistoryFileInparam;

//锁定历史文件出参
typedef struct _XMUIVodLockHistoryFileOutparam
{
}XMUIVodLockHistoryFileOutparam;

//解锁历史文件入参
typedef struct _XMUIVodUnlockHistoryFileInparam
{
	XM_MW_File_Param file_param;
}XMUIVodUnlockHistoryFileInparam;

//解锁历史文件出参
typedef struct _XMUIVodUnlockHistoryFileOutparam
{
}XMUIVodUnlockHistoryFileOutparam;

//设置录像时长入参
typedef struct _XMUISetRecordDurationInparam
{
	XMStorageRecordDuration record_duration;
}XMUISetRecordDurationInparam;

//设置录像时长出参
typedef struct _XMUISetRecordDurationOutparam
{
}XMUISetRecordDurationOutparam;

//编码属性设置入参
typedef struct _XMUISetEncodeInparam
{
	PAYLOAD_TYPE_E video_codec_type;
    int width;
	int height;
    int bit_rate;
	int channel;
}XMUISetEncodeInparam;

//编码属性设置出参
typedef struct _XMUISetEncodeOutparam
{
}XMUISetEncodeOutparam;

//图片渲染入参
typedef struct _XMUIPictureRenderInparam
{
	char file_full_name[XM_MAX_STORAGE_PATH_LEN];
	int file_len;
}XMUIPictureRenderInparam;

//图片渲染出参
typedef struct _XMUIPictureRenderOutparam
{

}XMUIPictureRenderOutparam;

//设置AI算法类型入参
typedef struct _XMUISmartSetAITypeInparam
{
	XMAIType ai_type;
}XMUISmartSetAITypeInparam;

//设置AI算法类型出参
typedef struct _XMUISmartSetAITypeOutparam
{

}XMUISmartSetAITypeOutparam;

//设置BSD参数入参
typedef struct _XMUISmartSetBSDParamInparam
{
	int channel;
	XM_MW_BSD_Param bsd_param;
}XMUISmartSetBSDParamInparam;

//设置BSD参数出参
typedef struct _XMUISmartSetBSDParamOutparam
{

}XMUISmartSetBSDParamOutparam;

//设置AI使能入参
typedef struct _XMUISmartEnableAIInparam
{
	bool enable;
}XMUISmartEnableAIInparam;

//设置AI使能出参
typedef struct _XMUISmartEnableAIOutparam
{

}XMUISmartEnableAIOutparam;

//分屏入参
typedef struct _XMUISplitSetSplitInparam
{
	XM_MW_SPLIT_Param split_param;
}XMUISplitSetSplitInparam;

//分屏出参
typedef struct _XMUISplitSetSplitOutparam
{
	int result;
}XMUISplitSetSplitOutparam;

//分屏grid入参
typedef struct _XMUISplitSetGridRectInparam
{
	XM_MW_SPLIT_RECT_Param split_rect_param;
}XMUISplitSetGridRectInparam;

//分屏grid出参
typedef struct _XMUISplitSetGridRectOutparam
{
	XM_MW_SPLIT_Param split_param;
}XMUISplitSetGridRectOutparam;

//删除文件入参
typedef struct _XMUIStorageDelFileInparam
{
	XM_MW_File_Param file_param;
}XMUIStorageDelFileInparam;

//删除文件出参
typedef struct _XMUIStorageDelFileOutparam
{
	
}XMUIStorageDelFileOutparam;

//音量输入
typedef struct _XMUISoundSetVolumeInparam
{
	int sound_volume;
}XMUISoundSetVolumeInparam;

//音量输出
typedef struct _XMUISoundSetVolumeOutparam
{

}XMUISoundSetVolumeOutparam;

//镜像输入
typedef struct _XMUISetMirrorInparam
{
	int chn;
	bool open_mirror;
}XMUISetMirrorInparam;

//升级入参
typedef struct _XMUISysUpgradeInparam
{
	char file_full_name[XM_MAX_STORAGE_PATH_LEN];
}XMUISysUpgradeInparam;

//升级出参
typedef struct _XMUISysUpgradeOutparam
{
	int result;
}XMUISysUpgradeOutparam;

//升级进度入参
typedef struct _XMUIGetUpgradeProgressInparam
{
	
}XMUIGetUpgradeProgressInparam;

//升级进度出参
typedef struct _XMUIGetUpgradeProgressOutparam
{
	int current_duration;
}XMUIGetUpgradeProgressOutparam;

//升级文件入参
typedef struct _XMUIUpgradeQueryInparam
{

}XMUIUpgradeQueryInparam;

//升级文件出参
typedef struct _XMUIUpgradeQueryOutparam
{
	XM_MW_Record_List rec_list;
}XMUIUpgradeQueryOutparam;

//声控入参
typedef struct _XMUISmartEnableKeywordInparam
{
	bool enable;
}XMUISmartEnableKeywordInparam;

typedef struct _XMUIEventInParam
{
    union 
    {
        XMUIQueryRecordInparam query_record;
        XMUIVodStartInparam vod_start;
        XMUIGetFileInfoInparam vod_getfileinfo; 
        XMUIVodPlayInparam vod_play;
        XMUIVodPauseInparam vod_pause;
        XMUIVodSeekInparam vod_seek;
        XMUIVodGetCurDurationInparam vod_getcurduration;
        XMUIVodStopInparam vod_stop;
        XMUISetEncodeInparam set_enc;
		XMUIVodLockFileInparam vod_lockfile;
		XMUIVodUnlockFileInparam vod_unlockfile;
		XMUIVodLockHistoryFileInparam vod_lockhistoryfile;
		XMUIVodUnlockHistoryFileInparam vod_unlockhistoryfile;
		XMUISetRecordDurationInparam set_record_duraion;
		XMUIPictureRenderInparam picture_render;
		XMUISmartSetAITypeInparam set_AI_type;
		XMUISmartSetBSDParamInparam set_BSD_param;
		XMUISmartEnableAIInparam enable_AI;
		XMUISplitSetSplitInparam set_split;
		XMUISplitSetGridRectInparam set_grid_rect;
		XMUIStorageDelFileInparam storage_del_file;
		XMUISoundSetVolumeInparam sound_set_volume;
		XMUISetMirrorInparam set_mirror;
		XMUISysUpgradeInparam sys_upgrade;
		XMUIGetUpgradeProgressInparam get_upgrade_progress;
		XMUIUpgradeQueryInparam upgrade_query;
		XMUISmartEnableKeywordInparam smart_enable_keyword;
    };  
}XMUIEventInParam;

typedef struct _XMUIEventOutParam
{
    union 
    {
        XMUIQueryRecordOutparam query_record;
        XMUIVodStartOutparam vod_start;
        XMUIGetFileInfoOutparam vod_getfileinfo; 
        XMUIVodPlayOutparam vod_play;
        XMUIVodPauseOutparam vod_pause;
        XMUIVodSeekOutparam vod_seek;
        XMUIVodGetCurDurationOutparam vod_getcurduration;
        XMUIVodStopOutparam vod_stop;
        XMUISetEncodeOutparam set_enc;
		XMUIVodLockFileOutparam vod_lockfile;
		XMUIVodUnlockFileOutparam vod_unlockfile;
		XMUIVodLockHistoryFileOutparam vod_lockhistoryfile;
		XMUIVodUnlockHistoryFileOutparam vod_unlockhistoryfile;
		XMUISetRecordDurationOutparam set_record_duraion;
		XMUIPictureRenderOutparam picture_render;
		XMUISmartSetAITypeOutparam set_AI_type;
		XMUISmartSetBSDParamOutparam set_BSD_param;
		XMUISmartEnableAIOutparam enable_AI;
		XMUISplitSetSplitOutparam set_split;
		XMUISplitSetGridRectOutparam set_grid_rect;
		XMUIStorageDelFileOutparam storage_del_file;
		XMUISoundSetVolumeOutparam sound_set_volume;
		XMUISysUpgradeOutparam sys_upgrade;
		XMUIGetUpgradeProgressOutparam get_upgrade_progress;
		XMUIUpgradeQueryOutparam upgrade_query;
    };  
}XMUIEventOutParam;

typedef int (*XM_UI_EVENT_CallBack)(XMUIEventType ui_event_type, XMUIEventInParam* in_param, XMUIEventOutParam* out_param);

class GlobalData
{
public:
    static GlobalData* Instance();
	static void Uninstance();

	std::string version() {return version_;}
	void set_version(std::string version) {version_=version;}

	int LcdLightInit();
	int SetLcdLight(int level);
    //用于取代g_cfg_bus_dvr_menu，做为配置
	CarConfig* car_config() { return &car_config_; }
	GUI* gui() { return &gui_; }
	int SetCurTime(int64_t sys_time);
	int64_t GetCurTime();

public:
    void* g_pImgVirAttr;
	RECT_S dst_rc;
	bool BSD_alarm_sound_;
	bool key_tone_;
	int SDCard_write_speed_;
	bool upgrading_;
	bool recording_;
	bool stop_screen_refresh_;
	bool gdb_debug_;
	APP_PAGE app_page_;
    XM_UI_EVENT_CallBack ui_event_cb_;
	lv_group_t* group;

private:
    CarConfig car_config_;
	std::string version_;
	GUI gui_;

private:
    GlobalData();
    ~GlobalData();
    static GlobalData* instance_;
};

#endif