#pragma once
#include <mutex>
#include "xm_middleware_api.h"
#include "HttpUnit.h"
#include "cJSON.h"

typedef struct APP_PARAM_ITEMS
{
	int record;
	int ir_mode;
	int osd_time;
#if 1//X2V60_S_DEBUG1
	int speaker_volume;
#endif
	int mic;
	int voice_control;
	int sdcard_speed;
	int resolution;
	int circular_record_time;
	int gsr_sensitivity;
	int parking_monitor;
	int screen_standby;
	int timelapse_rate;
	int key_tone;
	int park_record_time;
	char version[32];
}APP_PARAM_ITEMS_S;

class HTTPSerMdl
{
public:
	static HTTPSerMdl* Instance();
	static void Uninstance();
	 
	HTTPSerMdl();
	~HTTPSerMdl();

    // 网络响应信息
    void respMsg(int engineId, int connId, uint8_t type, char* msg, char* connType, int contentLen);
    void CfgInit(APP_PARAM_ITEMS_S* param_items);

/*------------------------基本信息-----------------------------*/
    // 获取产品信息
    void getProductInfo(int engineId, int connId, char * connType);
    // 获取设备信息
    void getDeviceAttr(int engineId, int connId, char * connType);
    // 获取媒体信息
    void getMediaInfo(int engineId, int connId, char * connType);
    // 获取存储（SD/TF卡）信息
    void getSdInfo(int engineId, int connId, char * connType);
    // 获取电量状态
    // void getBatteryInfo(int engineId, int connId, char * connType);
    // 设置时间和日期
    void setSysTime(int engineId, int connId, char* msg, char * connType);
    // 设置时区
    void setTimeZone(int engineId, int connId, char* msg, char * connType);
    // 获取当前录制时长
    void getRecduration(int engineId, int connId, char * connType);
    //进入菜单模式
    void setting(int engineId, int connId, char* msg, char* connType);
    // 进入回放模式
    void playback(int engineId, int connId, char* msg, char* connType);
    //改变支持的分辨率
    int changeSupportResolution(char* resolution);

/*------------------------设备管理-----------------------------*/
    // 修改wifi名称
    void setWifiIssid(int engineId, int connId, char* msg, char * connType);
    // 修改wifi密码
    void setWifiIpwd(int engineId, int connId, char* msg, char* connType);
    //wifi重启
    void WifiReboot(int engineId, int connId, char* msg, char* connType);
    // 格式化TF/SD卡
    void sdForMat(int engineId, int connId, char * connType);
    // 恢复出厂设置
    void reSet(int engineId, int connId, char * connType);
    // 拍照
    void snapShot(int engineId, int connId, char * connType);
    // 锁档
    void lockVideo(int engineId, int connId, char * connType);
    // 重启WiFi
    void wifiReboot(int engineId, int connId, char * connType);
    // 获取设备能力集
    void capAbility(int engineId, int connId, char * connType);
    //升级
    int Upgrader(int engineId, int connId, char* msg, char * connType);
    int OnUpgraderData(int engineId, int connId, char* data, int len);
/*------------------------设备文件管理-----------------------------*/
    // 获取文件列表
    void getFileList(int engineId, int connId, char * msg, char * connType);
    // 分页获取文件列表
    void getFileList_folder(int engineId, int connId, std::string folder, int start, int end, char * connType);
    // 获取文件缩略图
    // void getThumbNail(int engineId, int connId, char * connType);
    // 文件删除
    void deleteFile(int engineId, int connId, char * msg, char * connType);

/*------------------------设置项---------------------------------*/
    // 查询当前所有设置项的参数列表
    void getParamItems(int engineId, int connId, char * msg, char * connType);
    // 查询所有设置项的当前值
    void getParamValue(int engineId, int connId, char * msg, char * connType);
    //设置项
    void setParamValue(int engineId, int connId, char * msg, char * connType);

/*------------------------推送消息---------------------------------*/
    // 录像状态变化的消息
    int tcpRecStatus(int engineId, int status);
    // mic开关的消息
    int tcpMictatus(int engineId, int status);
    // 电池状态变化消息
    // void batteryStatus(int engineId, int connId);
    // 存储卡状态变化消息
    int tcpSdStatus(int engineId, int status, int sdcard_speed);
    // 设备文件删除消息
    int tcpFileDelStatus(int engineId, char* filename, int type);
    //设备文件加锁消息
    int tcpRecLockStatus(int engineId, int status);
    //摄像头切换消息
    int tcpCameraStatus(int engineId, int action, int curcamid, int camnum);
    int tcpWiFiStatus(int engineId);

/*------------------------视频流回调---------------------------------*/
    void OnMWRealTimeStream(int channel, int stream, XM_MW_Media_Frame* media_frame, int64_t user);
    bool OnMWVODStream(long handle, XM_MW_Media_Frame* media_frame, int64_t user);

/*------------------------TCP信息---------------------------------*/
    
    int Run();
    void CloseThread(int connId);
    void CloseAll();

protected:
    // 直播
    void enterRecorder(int engineId, int connId, char * connType);
    // 退出直播
    void exitRecorder(int engineId, int connId, char * connType);
    // 回放
    void playPackFile(int engineId, int connId, char* msg, char* connType);
    //获取缩略图
    void getThumbnail(int engineId, int connId, char* msg, char* connType);

private:
    int getFileListInfo(int chanel, std::string folder, XMStorageSDCardNum sdCardNum, int start, int end, cJSON* infoObject);
    void packParamInfo(const char* name, const char **items_array, int items_len, const int* index_array, cJSON* infoObject);
    void packParamValueInfo(const char* name, int value, cJSON* infoObject);
    void splitMsg(char* msg, std::vector<std::string>& elems);
    void split(const std::string& s, char delim, std::vector<std::string>& elems);
    
    static HTTPSerMdl* instance_;
 
    int rec_;
    int osd_;
    int mic_;
    int speaker_;
    int ir_mode_;
    int voice_control_;
    int adas_;
    int sdcard_speed_;
    int resolution_;
    int rec_split_duration_;
    int gsr_sensitivity_;
    int screen_standby_;
    int parking_monitor_;
    int timelapse_rate_;
    int key_tone_;
    int park_record_time_;
    char support_resolution_[32];
    std::string version_;
    std::mutex thread_map_mutex_;
    std::map<int, HttpUnit*> http_units_;
    OS_THREAD	check_thread_;
    bool thread_exit_;
    FILE* fp_;
    int file_size_;
    int ota_len_;
    int ota_remain_len_;
    char ota_buffer_[128 * 1024];

};
