#ifndef __SET_CONFIG_H__
#define __SET_CONFIG_H__

#include <stdio.h>
#include "xm_common.h"
#include "xm_ia_comm.h"
#include "gps.h"

int AlarmRegionCoorSw(float coor, float length, bool flag);
#define VOICE_CONTROL_EN    0    //0-屏蔽声控，注意默认菜单配置参数要关闭                  1-添加声控  
#define GPS_EN              1    //0不支持GPS               1-支持GPS功能
#define X2V60_S_DEBUG1      1

//#define X2V60_GPS_OSD        0
#define OSD_SHOW_ADJUST     1    //搜不到gps时间水印下移，0关闭，1开启
#define OSD_TIME_ADJUST_Y 0
#define OSD_GPS_ADJUST_Y  420

//配置
struct _time
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
};

enum Language
{
	English,
	SimpChinese,
	TradChinese,
};

enum Car_model
{
	Car,
	SUV,
	Other,
};

enum Sensitivity
{
	Sensitivity_Close,
	Sensitivity_High,
	Sensitivity_Mid,
	Sensitivity_Low,
	Sensitivity_StartUp,
};

typedef enum WIFI_TYPE
{
	WIFI_TYPE_OFF,
	WIFI_TYPE_2_4G,
	WIFI_TYPE_5G,
}WIFI_TYPE_E;

enum XM_CONFIG_VALUE_TYPE
{
	CFG_Operation_Value_Int = 0,
	CFG_Operation_Value_Bool,
	CFG_Operation_Value_Float,
};

enum XM_CONFIG_OPERATION
{
	//系统设置
	CFG_Operation_Lcd_Luminance = 0,
	CFG_Operation_Volume,
	CFG_Operation_Lcd_OffTime,
	CFG_Operation_Fast_Start,
	CFG_Operation_Language,
	CFG_Operation_Key_Voice,
	CFG_Operation_Back_Camera_Image,
	CFG_Operation_Acc_Power_Supply,
	CFG_Operation_Voice_Control,
	CFG_Operation_Contrast_Ratio,
	CFG_Operation_AutoShutdown_Time,
	//行车设置
	CFG_Operation_Circular_Record_Time,
	CFG_Operation_Video_Resolution,
	CFG_Operation_Video_Encode_Format,
	CFG_Operation_Gsensor_Sensitivity,
	CFG_Operation_Silent_Record,
	CFG_Operation_Time_Watermark,
	CFG_Operation_Park_Monitor,
	CFG_Operation_Miniature_Video,
	CFG_Operation_Front_Mirror,
	CFG_Operation_Behind_Mirror,
	CFG_Operation_Left_Mirror,
	CFG_Operation_Right_Mirror,

	//行车记录仪
	CFG_Operation_Lock_Video,
	CFG_Operation_Mute_Video,
	CFG_Operation_Exposure_Compensation,
	CFG_Operation_White_Balance,
	CFG_Operation_Park_Monitor_Sensitivity,
	CFG_Operation_Compact_Record_Fps,
	CFG_Operation_Compact_Record_Duration,
	CFG_Operation_Collision_Sensitivity,
	CFG_Operation_Record_Voice,
	CFG_Operation_Collision_Startup,
	CFG_Operation_Car_Charger,
	CFG_Operation_Wifi_Type,
	CFG_Operation_IR_Mode,
	CFG_Operation_GAMMA_DELTA,
#if 1//X2V60_S_DEBUG1
	CFG_Operation_NEED_REPAIR_SDCARD,
	CFG_Operation_Speaker_Volume,
#endif
};

union XM_CONFIG_VALUE
{
	int int_value;
	bool bool_value;
	float float_value;
};

struct XM_CONFIG_UNIT
{
	XM_CONFIG_OPERATION	 opr_type;
	const char*	obj_str;//默认是"root", 
	const char*	opr_str;
	XM_CONFIG_VALUE_TYPE value_type;
	XM_CONFIG_VALUE default_value;
};

const XM_CONFIG_UNIT CFG_ALL_OPERATION_UNITS[] =
{
	//系统设置初始化
	{ CFG_Operation_Lcd_Luminance,			"",		"lcd_luminance",		CFG_Operation_Value_Int,	55},
	{ CFG_Operation_Volume,					"",		"volume",				CFG_Operation_Value_Int,	50},
	{ CFG_Operation_Lcd_OffTime,			"",		"lcd_offtime",			CFG_Operation_Value_Int,	0},
	{ CFG_Operation_Fast_Start,				"",		"fast_start",			CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Language,				"",		"language",				CFG_Operation_Value_Int,	SimpChinese},
	{ CFG_Operation_Key_Voice,				"",		"key_voice",			CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Back_Camera_Image,		"",		"back_camera_image",	CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Acc_Power_Supply,		"",		"acc_power_supply",		CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Voice_Control,		    "",		"Voice_Control",		CFG_Operation_Value_Int,	false},
	{ CFG_Operation_Contrast_Ratio,			"",		"Contrast_Ratio",		CFG_Operation_Value_Int,	50},
	{ CFG_Operation_AutoShutdown_Time,		"",		"AutoShutdown_Time",	CFG_Operation_Value_Int,	0},
	//行车设置初始化
	{ CFG_Operation_Circular_Record_Time,	"",		"circular_record_time",	CFG_Operation_Value_Int,	1},
	{ CFG_Operation_Video_Resolution,		"",		"video_resolution",		CFG_Operation_Value_Int,	2144},
	{ CFG_Operation_Video_Encode_Format,	"",		"video_encode_format",	CFG_Operation_Value_Int,	PT_H265},
	{ CFG_Operation_Gsensor_Sensitivity,	"",		"gsensor_sensitivity",	CFG_Operation_Value_Int,	3},
	{ CFG_Operation_Silent_Record,			"",		"silent_record",		CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Time_Watermark,			"",		"time_watermark",		CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Park_Monitor,			"",		"park_monitor",			CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Miniature_Video,		"",		"miniature",			CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Front_Mirror,		    "",		"Front_Mirror",			CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Behind_Mirror,			"",		"Behind_Mirror",		CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Left_Mirror,			"",		"Left_Mirror",			CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Right_Mirror,			"",		"Right_Mirror",			CFG_Operation_Value_Bool,	false},

	//行车记录仪
	{ CFG_Operation_Lock_Video,				"",		"lock_video",			CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Mute_Video,				"",		"mute_video",			CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Exposure_Compensation,	"",		"Exposure_Compensation",CFG_Operation_Value_Int,	50},
	{ CFG_Operation_White_Balance,			"",		"White_Balance",		CFG_Operation_Value_Int,	0},
	{ CFG_Operation_Park_Monitor_Sensitivity,	"",		"Park_Monitor_Sensitivity",		CFG_Operation_Value_Int,	Sensitivity_Mid},
	{ CFG_Operation_Compact_Record_Fps,			"",		"Compact_Record_Fps",			CFG_Operation_Value_Int,	1},
	{ CFG_Operation_Compact_Record_Duration,	"",		"Compact_Record_Duration",		CFG_Operation_Value_Int,	0},
	{ CFG_Operation_Collision_Sensitivity,		"",		"Collision_Sensitivity",		CFG_Operation_Value_Int,	Sensitivity_Mid},
	{ CFG_Operation_Record_Voice,				"",		"Record_Voice",					CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Collision_Startup,			"",		"Collision_Startup",			CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Car_Charger,				"",		"Car_Charger",					CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Wifi_Type,					"",		"Wifi_Type",					CFG_Operation_Value_Int,	WIFI_TYPE_2_4G},
	{ CFG_Operation_IR_Mode,					"",		"IRLed",						CFG_Operation_Value_Int,	2},
	{ CFG_Operation_GAMMA_DELTA,				"",		"DELTA",					    CFG_Operation_Value_Int,	100},

#if 1//X2V60_S_DEBUG1
	{ CFG_Operation_NEED_REPAIR_SDCARD,			"",		"Fsck Sdcard",					CFG_Operation_Value_Bool,	false},//需要fsck修复sd卡
	{ CFG_Operation_Speaker_Volume,				"",		"Speaker_Volume",				CFG_Operation_Value_Int,	1},
#endif
};

struct cJSON;
class CarConfig
{
public:
	CarConfig();
	~CarConfig();

	int ReadConfigFromFile();
	int SetDefaultConfig();
	int GetValue(const XM_CONFIG_OPERATION& cfg_opr, XM_CONFIG_VALUE& out_value);
	int SetValue(const XM_CONFIG_OPERATION& cfg_opr, const XM_CONFIG_VALUE& in_value);
	int SetMultiValue(XM_CONFIG_OPERATION cfg_oprs[], XM_CONFIG_VALUE in_values[], int count);

private:
	int SaveToFile(cJSON* json_obj);

private:
	cJSON* json_obj_;
};

char* GetParsedString(const char *string);
void set_language(int language);

#endif
