#include "HttpSerModule.h"
#include "Log.h"
#include <time.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <thread>
#include <sstream>
#ifndef WIN32
#include <fcntl.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/mount.h>
#endif
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
#include "global_page.h"
#include "network/WifiOpr.h"
#include "GUI/global_data.h"
#include "CommDef.h"

using namespace std;
extern int g_engineId;
extern int g_connId;
extern bool g_realplay;
extern int g_sd_status;

extern bool XM_Middleware_Mpp_ADLoss();
extern int XM_Middleware_Sound_SetVolume(int volume);
#if 1//X2V60_S_DEBUG1
#include "global_data.h"
extern bool g_acc_connected;
extern bool g_app_connect;
#endif
HTTPSerMdl* HTTPSerMdl::instance_ = 0;
HTTPSerMdl* HTTPSerMdl::Instance()
{
	if (0 == instance_) {
		instance_ = new HTTPSerMdl();
	}
	return instance_;
}

void HTTPSerMdl::Uninstance()
{
	if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

HTTPSerMdl::HTTPSerMdl()
{
    rec_ = 1;
    osd_ = 1;
    mic_ = 1;
    speaker_ = 2;
    ir_mode_ = 0;
    voice_control_ = 0;
    adas_ = 0;
    thread_exit_ = false;
    sdcard_speed_ = 0;
    resolution_ = 1080;
    rec_split_duration_ = 1;
    gsr_sensitivity_ = 1;
    screen_standby_ = 1;
    parking_monitor_ = 0;
    timelapse_rate_ = 1;
    version_ = "v1.2.3";
    fp_ = NULL;
    file_size_ = 0;
    ota_remain_len_ = 0;
    memset(support_resolution_, 0, sizeof(support_resolution_));
}

HTTPSerMdl::~HTTPSerMdl()
{
    thread_map_mutex_.lock();
    for (auto it = http_units_.begin(); it != http_units_.end(); ++it) {
        HttpUnit* http_unit = it->second;
        http_unit->Clean();
        delete http_unit;
    }
    thread_map_mutex_.unlock();
}

void CheckHttpUnitsFunc(void* param)
{
    static_cast<HTTPSerMdl*>(param)->Run();
}

void HTTPSerMdl::CfgInit(APP_PARAM_ITEMS_S* param_items)
{
    rec_ = param_items->record;
    ir_mode_ = param_items->ir_mode;
#if 1//X2V60_S_DEBUG1
	speaker_ = param_items->speaker_volume;
#endif
    osd_ = param_items->osd_time;
    mic_ = param_items->mic;
    voice_control_ = param_items->voice_control;
    sdcard_speed_ = param_items->sdcard_speed;
    resolution_ = param_items->resolution;
    rec_split_duration_ = param_items->circular_record_time;
    gsr_sensitivity_ = param_items->gsr_sensitivity;
    screen_standby_ = param_items->screen_standby;
    parking_monitor_ = param_items->parking_monitor;
    timelapse_rate_ = param_items->timelapse_rate;
    version_ = param_items->version;
    key_tone_ = param_items->key_tone;
    park_record_time_ = param_items->park_record_time;

    CreateThreadEx(check_thread_, (LPTHREAD_START_ROUTINE)CheckHttpUnitsFunc, this);
}

// 网络响应信息
void HTTPSerMdl::respMsg(int engineId, int connId, uint8_t type, char * msg, char * connType, int contentLen)
{
	XMLogI("respMsg,engineId=%d,connId=%d,msg=%s,connType=%s",engineId,connId,msg,connType);
    if (strstr(msg, "getproductinfo") != NULL) {
        getProductInfo(engineId, connId,connType);
    } else if (strstr(msg, "getdeviceattr") != NULL) {
        getDeviceAttr(engineId, connId,connType);
    } else if (strstr(msg, "getmediainfo") != NULL) {
        getMediaInfo(engineId, connId,connType);
    } else if (strstr(msg, "getsdinfo") != NULL) {
        getSdInfo(engineId, connId,connType);
    } else if (strstr(msg, "setsystime") != NULL) {
        setSysTime(engineId, connId,msg,connType);
    } else if (strstr(msg, "settimezone") != NULL) {
        setTimeZone(engineId, connId,msg,connType);
    } else if (strstr(msg, "setting") != NULL) {
        setting(engineId, connId, msg, connType);
    } else if (strstr(msg, "playback") != NULL) {
        playback(engineId, connId, msg, connType);
    } else if (strstr(msg, "getrecduration") != NULL) {
        getRecduration(engineId, connId,connType);
    } else if (strstr(msg, "getparamitems") != NULL) {
        getParamItems(engineId, connId, msg,connType);
    } else if (strstr(msg, "getparamvalue") != NULL) {
        getParamValue(engineId, connId, msg,connType);
    } else if (strstr(msg, "capability") != NULL) {
        capAbility(engineId, connId,connType);
    } else if(strstr(msg, "enterrecorder") != NULL) {
        enterRecorder(engineId, connId,connType);
    } else if(strstr(msg, "exitrecorder") != NULL) {
        exitRecorder(engineId, connId,connType);
    } else if (strstr(msg, "getfilelist") != NULL) {
        getFileList(engineId, connId, msg,connType);
    } else if (strstr(msg, "sdformat") != NULL) {
        sdForMat(engineId, connId,connType);
    } else if (strstr(msg, "setparamvalue") != NULL) {
        setParamValue(engineId, connId, msg,connType);
    } else if (strstr(msg, "wifissid") != NULL) {
        setWifiIssid(engineId, connId, msg,connType);
    } else if (strstr(msg, "wifipwd") != NULL) {
        setWifiIpwd(engineId, connId, msg,connType);
    } else if (strstr(msg, "lockvideo") != NULL && strstr(msg, "getlockvideostatus") == NULL) {
        lockVideo(engineId, connId, connType);
    } else if (strstr(msg, "getthumbnail") != NULL) {
        playPackFile(engineId, connId, msg, connType);
    } else if (strstr(msg, "deletefile") != NULL) {
        deleteFile(engineId, connId, msg,connType);
    } else if (strstr(msg, "/mnt/tfcard/") != NULL) {
        XMLogI("------------connId=%d------------------------",connId);
        playPackFile(engineId, connId, msg,connType);
    } else if (strstr(msg, "snapshot") != NULL) {
        snapShot(engineId, connId,connType);
    } else if (strstr(msg, "reset") != NULL) {
        reSet(engineId, connId, connType);
    } else if (strstr(msg, "wifireboot") != NULL) {
        WifiReboot(engineId, connId, msg, connType);
    } else if (strstr(msg, "upload") != NULL) {
        Upgrader(engineId, connId, msg, connType);
    }
    else {
        cJSON* root_res = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_res, "result", 98);
        cJSON_AddStringToObject(root_res, "info", "the operation is unsupport.");
        char* http_body = cJSON_Print(root_res);
        cJSON_Delete(root_res);
        XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
        free(http_body);

        g_engineId = engineId;
        g_connId = connId;
    }
}

const char* kTmpUploadFilePath = "/mnt/tfcard/netall_XM650V200_CAR_CT317.bin.tmp";
const char* kUploadFilePath = "/mnt/tfcard/netall_XM650V200_CAR_CT317.bin";

int HTTPSerMdl::Upgrader(int engineId, int connId, char* msg, char * connType)
{
    XMLogI("Upgrader, msg=%s", msg);    
    if (fp_) return -1;
    
    int ret = sscanf(msg, "/upload/mnt/sdcard/%d", &file_size_);
    XMLogI("Upgrader, file size=%d", file_size_);   
    //升级时，停止录像存储，释放内存
    XM_Middleware_Storage_Stop(-1); 
    bool real_stop = XM_Middleware_Storage_ReallyStop();
    int i;
    for (i = 0; i < 1000; i++) {
        if (real_stop) 
            break;
        usleep(100*1000);
        real_stop = XM_Middleware_Storage_ReallyStop();
    }
    if (i >= 1000) {
        XMLogE("Upgrader storage not stoped");
    }
    XM_Middleware_Storage_DestroyFrameList(XM_STORAGE_SDCard_0);
 
    //接收网络数据，创建临时文件
    fp_ = fopen(kTmpUploadFilePath, "wb");
    if (fp_ == NULL) {
        XMLogE("Upgrader path open file error");
        return -2;
    }

    return 0;
}

int HTTPSerMdl::OnUpgraderData(int engineId, int connId, char* data, int len)
{
    if (fp_) {
        fwrite(data, 1, len, fp_);
        ota_len_ += len;
        XMLogI("OnUpgraderData, recv data, size=%d", len);    
    } else {
        XMLogE("OnUpgraderData, len=%d, but file have not open", len);    
    }

    if (file_size_ > 0 && ota_len_ >= file_size_) {
        fclose(fp_);
        fp_ = NULL;
        XMLogI("Upgrader, recv file over, size=%d", ota_len_);  
         //升级文件存在就先删除
        if (access(kUploadFilePath, F_OK) == 0) {
            char cmd_buf[50] = {0};
            sprintf(cmd_buf, "rm %s", kUploadFilePath);
            system(cmd_buf);
        }
        //文件收全后，重命名升级文件
        rename(kTmpUploadFilePath, kUploadFilePath);
        system("umount /mnt/tfcard");
        
        bool find_dev_node1 = false;
        find_dev_node1 = access(kDevNodeName1, F_OK) == 0 ? true : false;
        const char* mount_dev_node = find_dev_node1 ? kDevNodeName1 : kDevNodeName2;

        //为复用当前逻辑，先重挂载到/home目录下
        int ret = mount(mount_dev_node, "/home", "vfat", MS_NOATIME|MS_NODIRATIME, NULL);
        if (ret != 0) {
            int error_ret = errno;
            XMLogE("mount failed, error ret=%d", error_ret);
            return -1;
        } else {
            XMLogI("remount to /home ok");
        }

        XM_Middleware_WDT_WatchDogStop();
        //升级处理
        system("cp /usr/bin/car_demo /var/");
        char cmd_buf[50];
        sprintf(cmd_buf, "kill -9 %d ; /var/car_demo upgradeMain", getpid());
        system(cmd_buf);
    }
   
    return 0;
}

void HTTPSerMdl::getThumbnail(int engineId, int connId, char* msg, char* connType)
{
    char* file_path = strstr(msg, "=");
    XMLogI("getThumbnail, file_path:%s", file_path);

    playPackFile(engineId, connId, file_path, connType);
}

void HTTPSerMdl::getProductInfo(int engineId, int connId, char * connType)
{
    cJSON* info = cJSON_CreateObject();
    cJSON_AddStringToObject(info, "model", "na");
    cJSON_AddStringToObject(info, "company", "xdsm");
    cJSON_AddStringToObject(info, "soc", "xmsilicon");
    cJSON_AddStringToObject(info, "sp", "XM");

    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::getDeviceAttr(int engineId, int connId, char * connType)
{
    g_engineId = engineId;
    g_connId = connId;
    XM_Middleware_Periphery_Notify(XM_EVENT_APP_GET_DEVATTR, "", 0);  
}

void HTTPSerMdl::getMediaInfo(int engineId, int connId, char * connType)
{
    XMLogI("rtsp getMediaInfo");
    cJSON* info = cJSON_CreateObject();
    cJSON_AddStringToObject(info, "rtsp", "rtsp://192.168.169.1/streamtype=0");
    cJSON_AddStringToObject(info, "transport", "tcp");
    cJSON_AddNumberToObject(info, "port", 2222);
    cJSON_AddNumberToObject(info, "cgiswitch", 0);

    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::getSdInfo(int engineId, int connId, char * connType)
{
    cJSON* info = cJSON_CreateObject();
    XMLogW("g_sd_status = %d", g_sd_status);
    if (XM_SD_NORMAL == g_sd_status) {
        if (sdcard_speed_ >= 1024) {
            XM_MW_SDCard_Space sdcard_space;
            XM_Middleware_Storage_CheckSDSpace(XM_STORAGE_SDCard_0, &sdcard_space);
            cJSON_AddNumberToObject(info, "status", 0);
            cJSON_AddNumberToObject(info, "free", sdcard_space.sdcard_free_space);
            cJSON_AddNumberToObject(info, "total", sdcard_space.sdcard_total_space);
        }
        else {
            cJSON_AddNumberToObject(info, "status", 11);
        }
    }
    else if (XM_SD_NOEXIST == g_sd_status) {
        cJSON_AddNumberToObject(info, "status", 2);
    }
    else if (XM_SD_ABNORMAL == g_sd_status) {
        cJSON_AddNumberToObject(info, "status", 3);
    }
    else if (XM_SD_NEEDFORMAT == g_sd_status) {
        cJSON_AddNumberToObject(info, "status", 1);
    }
    else if (XM_SD_NOTRW == g_sd_status) {
        cJSON_AddNumberToObject(info, "status", 10);
    }
    else if (XM_SD_NEEDCHECKSPEED == g_sd_status) {
        cJSON_AddNumberToObject(info, "status", 1);
    }
    else if (XM_SD_NOSPACE == g_sd_status) {
        cJSON_AddNumberToObject(info, "status", 12);
    }
    
    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    XMLogW("getsdinfo response, %s \r\n", http_body);

    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::setSysTime(int engineId, int connId, char* msg, char* connType)
{
    char* date = strstr(msg, "date");
    string current_time(date);
    struct tm tm_network_time;

    // date的数据格式为“date=20231024130948”,第0-4字符为“date=”
    tm_network_time.tm_year = std::atoi(current_time.substr(5,4).c_str()) - 1900;
    tm_network_time.tm_mon = std::atoi(current_time.substr(9,2).c_str()) - 1;
    tm_network_time.tm_mday = std::atoi(current_time.substr(11,2).c_str());
    tm_network_time.tm_hour = std::atoi(current_time.substr(13,2).c_str());
    tm_network_time.tm_min = std::atoi(current_time.substr(15,2).c_str());
    tm_network_time.tm_sec = std::atoi(current_time.substr(17,2).c_str());
    // XMLogI("tm_network_time = %d-%d-%d-%d-%d-%d", tm_network_time.tm_year, tm_network_time.tm_mon, tm_network_time.tm_mday, tm_network_time.tm_hour, tm_network_time.tm_min, tm_network_time.tm_sec);
    time_t network_time = mktime(&tm_network_time);
    time_t sys_time = time(NULL);  // 获取当前系统时间
    XMLogI("network_time = %ld", network_time);
    XMLogI("system_time = %ld", sys_time);
    // 计算时间差
    int time_diff = abs(static_cast<int>(difftime(network_time, sys_time)));  // 计算绝对值的时间差
    // 时间差不一定是10秒，可按需求自定义
    if (time_diff <= 10)
    {
        XMLogI("The time is similar");
    }
    else
    {
        // 同步系统时间
        // string s("/sbin/hwclock -s");
        // system(s.c_str());
        char buf1[128] = { 0 };
        sprintf(buf1, "date -s \"%d-%d-%d %d:%d:%d\"", 
        tm_network_time.tm_year + 1900,
        tm_network_time.tm_mon + 1,
        tm_network_time.tm_mday,
        tm_network_time.tm_hour,
        tm_network_time.tm_min, 
        tm_network_time.tm_sec);
        XMLogI("buf1 is %s", buf1);
        system(buf1);

        // 同步硬件时间
        string c("/sbin/hwclock -w");
        system(c.c_str());
        XM_Middleware_Periphery_Notify(XM_EVENT_CHANGE_TIME, "", 0);
    }

    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddStringToObject(root_res, "info", "set success.");
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::setTimeZone(int engineId, int connId, char* msg, char * connType)
{
    char* minuteswest = strstr(msg, "timezone");
    string strminuteswest(minuteswest);
    int result = XM_Middleware_TIME_SetTimeZoneInfo(atoi(strminuteswest.substr(9).c_str()));
    XMLogI("system_timezone ret = %d", result);
    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "set success.");
        // XM_Middleware_Periphery_Notify(XM_EVENT_CHANGE_TIME, "", 0);
    }
    else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "set fail.");
    }
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::getRecduration(int engineId, int connId, char * connType)
{
    int duration = XM_Middleware_Storage_GetRecordedTime(XM_STORAGE_SDCard_0, 0);
    cJSON* info = cJSON_CreateObject();
    cJSON_AddNumberToObject(info, "duration", duration);

    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::setting(int engineId, int connId, char* msg, char* connType)
{
    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddStringToObject(root_res, "info", "success.");
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    if (strstr(msg, "exit") != NULL) {
        XM_Middleware_Periphery_Notify(XM_EVENT_APP_PAGE, "", APP_PAGE_PREVIEW);
    }
    else if (strstr(msg, "enter") != NULL) {
        XM_Middleware_Periphery_Notify(XM_EVENT_APP_PAGE, "", APP_PAGE_SET);
    }

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::playback(int engineId, int connId, char* msg, char* connType)
{
    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddStringToObject(root_res, "info", "success.");
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    if (strstr(msg, "exit") != NULL) {
        thread_map_mutex_.lock();
        for (auto it = http_units_.begin(); it != http_units_.end(); ++it) {
            HttpUnit* http_unit = it->second;
            http_unit->Clean();
            delete http_unit;
        }
        http_units_.clear();
        thread_map_mutex_.unlock();
        XM_Middleware_Periphery_Notify(XM_EVENT_APP_PAGE, "", APP_PAGE_PREVIEW);
    }
    else if (strstr(msg, "enter") != NULL) {
        XM_Middleware_Periphery_Notify(XM_EVENT_APP_PAGE, "", APP_PAGE_PLAYBACK);
    }

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::setWifiIssid(int engineId, int connId, char* msg, char * connType)
{
    char* ssid = strstr(msg, "=");
    string wifissid(ssid);
    WIFI_PARAM_S wifi_param;
    memset(&wifi_param, 0, sizeof(WIFI_PARAM_S));
    strncpy(wifi_param.name, wifissid.substr(1).c_str(), sizeof(wifi_param.name) - 1);
    int result = ChangeWifiParam(&wifi_param, 1);
    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "set success.");

        XM_Middleware_Periphery_Notify(XM_EVENT_CHANGE_WIFI_PARAM, "", 0);
    }
    else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "set fail.");
    }
    
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);
    
    g_engineId = engineId;
    g_connId = connId;

#if 0//X2V60_S_DEBUG1
	usleep(3 * 1000 * 1000);
#endif
    //GlobalPage::Instance()->page_main()->Reboot(true);

}

void HTTPSerMdl::setWifiIpwd(int engineId, int connId, char* msg, char * connType)
{
    char* ipwd = strstr(msg, "=");
    string wifiipwd(ipwd);
    WIFI_PARAM_S wifi_param;
    memset(&wifi_param, 0, sizeof(WIFI_PARAM_S));
    strncpy(wifi_param.password, wifiipwd.substr(1).c_str(), sizeof(wifi_param.password) - 1);
    int result = ChangeWifiParam(&wifi_param, 2);
    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "set success.");
    } else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "set fail.");
    }
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
	
	XMLogW("[Wifiset][setWifiIpwd] http_body:\n%s \r\n", http_body);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
	
#if 0//X2V60_S_DEBUG1
	usleep(3 * 1000 * 1000);
#endif
	//GlobalPage::Instance()->page_main()->Reboot(true);
}

void HTTPSerMdl::WifiReboot(int engineId, int connId, char* msg, char* connType)
{
    int result = 0;
    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "set success.");
    }
    else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "set fail.");
    }
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;

#if 0//X2V60_S_DEBUG1
    WifiEnableOpr(false);
    std::chrono::milliseconds dura(1000);
    std::this_thread::sleep_for(dura);
    WifiEnableOpr(true);
#else
	XMLogW("[Wifiset] begin Reboot \r\n");
	usleep(500 * 1000);
	GlobalPage::Instance()->page_main()->Reboot(true);
#endif
}

void HTTPSerMdl::sdForMat(int engineId, int connId, char * connType)
{
    XM_Middleware_Periphery_Notify(XM_EVENT_SDCARD_FORMAT, "", 0);
    int result = XM_Middleware_Storage_FormatSDCard(XM_STORAGE_SDCard_0);
    int write_speed = 0;
    g_sd_status = XM_Middleware_Storage_CheckSDCard(XM_STORAGE_SDCard_0);
    if (g_sd_status == XM_SD_NORMAL) {
        XM_Middleware_Storage_WriteSpeedToFile(XM_STORAGE_SDCard_0, &write_speed);
        sdcard_speed_ = write_speed;
        XMLogI("write_speed : %dKB/s", write_speed);
    }
    XM_Middleware_Periphery_Notify(XM_EVENT_SDCARD_FORMAT, "", write_speed);

    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "set success.");
    } else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "set fail.");
    }
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::reSet(int engineId, int connId, char * connType)
{
    XM_Middleware_Periphery_Notify(XM_EVENT_CFG_RESET, "", 0);
    int result = 1;
    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "set success.");
    } else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "set fail.");
    }
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::snapShot(int engineId, int connId, char * connType)
{
    XM_Middleware_Periphery_Notify(XM_EVENT_CATCH_PIC, "", 0);
    XMLogW("snapShot");
    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddStringToObject(root_res, "info", "snapshot success.");
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::lockVideo(int engineId, int connId, char * connType)
{
    int result = XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, 0, true);
    XMLogW("result:%d", result);
    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "lock success.");
    } else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "lock fail.");
    }
    
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}
#include "../GUI/set_config.h"
void HTTPSerMdl::capAbility(int engineId, int connId, char * connType)
{
    cJSON* root_res = cJSON_CreateObject();
    cJSON* info = cJSON_CreateObject();
    #if GPS_EN
    cJSON_AddStringToObject(info, "value", "201001");
    #else
    cJSON_AddStringToObject(info, "value", "001001");
    #endif
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::getFileList(int engineId, int connId, char * msg, char * connType)
{
    char* pbegin = strstr(msg, "?");

#if 1//X2V60_S_DEBUG1
	int wait_count = 0;

	while(wait_count++ < 60 && !XM_Middleware_Storage_ReallyStop()){
		XMLogW("[getFileList] 0 wait_count:%d\r\n", wait_count);
		usleep(50 * 1000);
	}
#endif
	
	usleep(50 * 1000);
    if (pbegin == NULL) {
        cJSON* info = cJSON_CreateArray();
        cJSON* loop = cJSON_CreateObject();
        cJSON* emr = cJSON_CreateObject();
        cJSON* event = cJSON_CreateObject();
        getFileListInfo(-1, "loop", XM_STORAGE_SDCard_0, -1, -1, loop);
        getFileListInfo(-1, "emr", XM_STORAGE_SDCard_0, -1, -1, emr);
        getFileListInfo(-1, "event", XM_STORAGE_SDCard_0, -1, -1, event);
        cJSON_AddItemToArray(info, loop);
        cJSON_AddItemToArray(info, emr);
        cJSON_AddItemToArray(info, event);
        cJSON* root_res = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddItemToObject(root_res, "info", info);
        char* http_body = cJSON_Print(root_res);
        cJSON_Delete(root_res);
        XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
		//XMLogW("[getFileList] 1 http body:%s \r\n", http_body); //
        free(http_body);

        g_engineId = engineId;
        g_connId = connId;
    } else {
        string_vector_t head_line_vect;
        splitMsg(pbegin, head_line_vect);
        if (head_line_vect.size() < 3) {
            cJSON* root_res = cJSON_CreateObject();
            cJSON_AddNumberToObject(root_res, "result", 99);
            cJSON_AddStringToObject(root_res, "info", "Parameter incompleteness.");
            char* http_body = cJSON_Print(root_res);
            cJSON_Delete(root_res);
            XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
            free(http_body);
            return ;
        }
		    
        string folder = head_line_vect[0].substr(1);
        string strstart = head_line_vect[1].substr(6);
        string strend = head_line_vect[2].substr(4);
        getFileList_folder(engineId, connId, folder, atoi(strstart.c_str()), atoi(strend.c_str()),connType);
    }
}

void HTTPSerMdl::getFileList_folder(int engineId, int connId, string folder, int start, int end, char * connType)
{
    cJSON* info = cJSON_CreateArray();
    cJSON* files = cJSON_CreateObject();
    int result = getFileListInfo(-1, folder, XM_STORAGE_SDCard_0, start, end, files);
    cJSON_AddItemToArray(info, files);
	cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", result);
	cJSON_AddItemToObject(root_res, "info", info);
	char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
	free(http_body);

    g_engineId = engineId;
	g_connId = connId;
}

int HTTPSerMdl::getFileListInfo(int chanel, string folder, XMStorageSDCardNum sdCardNum, int start, int end, cJSON* infoObject)
{
    XM_MW_Record_Query query_info;
    XM_MW_Record_List record_list;
    int file_type = 2;
    if ("emr" == folder) {
        query_info.type = XM_Record_Video;
        file_type = 2;
    } else if ("loop" == folder) {
        query_info.type = XM_Record_Video;
        file_type = 2;
    } else if ("event" == folder) {
        query_info.type = XM_Record_Picture;
        file_type = 1;
    } else {
        return 1;
    }
    cJSON* files = cJSON_CreateArray();
    query_info.channel = chanel;
    query_info.start_time = 0;
    query_info.end_time = 0xffffffff;
    query_info.sdc_num = sdCardNum;
    int result = XM_Middleware_Playback_Query(&query_info, &record_list);
    if (result != 0)
        return 1;
    cJSON_AddStringToObject(infoObject, "folder", folder.c_str());
    int record_count = record_list.record_count;
    
    int start_num = start, end_num = end;
    if (end_num < start_num)
        return 1;

    bool get_all = true;
    if (start_num != -1 && end_num != -1)
        get_all = false;

    record_count = 0;
    int record_count1 = 0;
    char name[256 + 64], time_buf[32];
    cJSON_AddItemToObject(infoObject, "files", files);
    for (int i = 0; i < record_list.record_count; i++) {
        if ("emr" == folder) {
            if (!record_list.records[i].video_info.file_locked) {
                continue;
            }
        }
        else if ("loop" == folder) {
            if (record_list.records[i].video_info.file_locked) {
                continue;
            }
        }

        record_count1++;
        if (!get_all) {
            if (record_count1 <= start_num)
                continue;
            if (record_count1 > end_num + 1)
                break;
        }

        cJSON* file = cJSON_CreateObject();
        memset(name, 0, sizeof(name));
        if (chanel == -1) {
            std::string CC = "F";
            if (record_list.records[i].channel == 1) CC = "R";
            else if (record_list.records[i].channel == 2) CC = "I";
            if (query_info.type == XM_Record_Video) {
                sprintf(name, "/mnt/tfcard/record/record_%s/%s",
                    CC.c_str(), record_list.records[i].file_name);
            }
            else if (query_info.type == XM_Record_Picture) {
                sprintf(name, "/mnt/tfcard/pic/pic_%s/%s",
                    CC.c_str(), record_list.records[i].file_name);
            }
        }
        else {
            strcat(name, record_list.dir_name);
            strcat(name, "/");
            strcat(name, record_list.records[i].file_name);
        }
        cJSON_AddStringToObject(file, "name", name);
        cJSON_AddNumberToObject(file, "size", record_list.records[i].file_len / 1024);
        
        time_t file_time = record_list.records[i].create_time / 1000;
        tm* tm_time = localtime(&file_time);
        memset(time_buf, 0, sizeof(time_buf));
        sprintf(time_buf, "%04d%02d%02d%02d%02d%02d",
            tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
        //XMLogI("name:%s, createtimestr:%s", name, time_buf);
        cJSON_AddStringToObject(file, "createtimestr", time_buf);
        
        cJSON_AddNumberToObject(file, "type", file_type);
        cJSON_AddItemToArray(files, file);

        record_count++;
    }
    cJSON_AddNumberToObject(infoObject, "count", record_count);
    
    return 0;
}

void HTTPSerMdl::deleteFile(int engineId, int connId, char * msg, char * connType)
{
    char* file = strstr(msg, "=");
    XM_MW_File_Param file_param;
    file_param.channel = 0;
    file_param.type = XM_Record_Video;
    file_param.sdc_num = XM_STORAGE_SDCard_0;
    string strfile(file);
    if (strstr(file, "record") != NULL) {
        file_param.type = XM_Record_Video;
        const char* str1 = strfile.substr(29).c_str();   //只需要文件名，不包含路径
        strncpy(file_param.file_name, str1, sizeof(file_param.file_name) - 1);
		
        if (strstr(file, "record_F") != NULL) {
            file_param.channel = 0;
        }
        else if (strstr(file, "record_R") != NULL){
            file_param.channel = 1;
        }else{
            file_param.channel = 2;
		}

		
    }
    else {
        const char* str1 = strfile.substr(23).c_str();   //只需要文件名，不包含路径
        strncpy(file_param.file_name, str1, sizeof(file_param.file_name) - 1);
        file_param.type = XM_Record_Picture;
		
        if (strstr(file, "pic_F") != NULL) {
			
            file_param.channel = 0;
        }else if (strstr(file, "pic_R") != NULL){
        
    		file_param.channel = 1;
        }else {
			
            file_param.channel = 2;
        }
		
    }
    XMLogW("deleteFile-----chn = %d, type = %d, name = %s", file_param.channel, file_param.type, file_param.file_name);

    int result = XM_Middleware_Storage_DelFile(&file_param);
    cJSON* root_res = cJSON_CreateObject();
    if (result == 0) {
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON_AddStringToObject(root_res, "info", "set success.");
    } else {
        cJSON_AddNumberToObject(root_res, "result", 1);
        cJSON_AddStringToObject(root_res, "info", "set fail.");
    }
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    g_engineId = engineId;
    g_connId = connId;
}

void HTTPSerMdl::getParamItems(int engineId, int connId, char* msg, char * connType)
{ 
    if (strstr(msg, "param=all") != NULL) {
        char out_str[4096] = {0};
        XM_Middleware_Periphery_Notify(XM_EVENT_APP_GET_PARAM, "", 0, out_str);
        XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, out_str, strlen(out_str));
    }
    else {
        cJSON* root_res = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_res, "result", 0);
        cJSON* info = cJSON_CreateArray();
        cJSON_AddItemToObject(root_res, "info", info);
        char* http_body = cJSON_Print(root_res);
        cJSON_Delete(root_res);
        XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
        free(http_body);
    }
    g_engineId = engineId;
	g_connId = connId;
}

void HTTPSerMdl::packParamInfo(const char* name, const char **items_array, int items_len, const int* index_array, cJSON* infoObject)
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

void HTTPSerMdl::getParamValue(int engineId, int connId, char* msg, char * connType)
{
    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON* info = cJSON_CreateArray();

    char* param = strstr(msg, "=") + 1;
    XMLogW("getParamValue  param:%s", param);
    uint32_t a, b;
    XM_Middleware_GetMemoryMsg(a, b);
    if (strcmp(param, "all") == 0) {
        packParamValueInfo("mic", mic_, info);
        packParamValueInfo("rec_resolution", resolution_, info);
        packParamValueInfo("osd", osd_, info);
        packParamValueInfo("rec", rec_, info);
        packParamValueInfo("ir_mode", ir_mode_, info);
#if 1//X2V60_S_DEBUG1
		packParamValueInfo("speaker", speaker_, info);
#endif
        if (voice_control_ >= 0)
            packParamValueInfo("voice_control", voice_control_, info);
        packParamValueInfo("rec_split_duration", rec_split_duration_, info);
        packParamValueInfo("gsr_sensitivity", gsr_sensitivity_, info);
        packParamValueInfo("screen_standby", screen_standby_, info);
        packParamValueInfo("parking_monitor", parking_monitor_, info);
        packParamValueInfo("timelapse_rate", timelapse_rate_, info);
        packParamValueInfo("key_tone", key_tone_, info);
        packParamValueInfo("park_record_time", park_record_time_, info);
        cJSON_AddItemToObject(root_res, "info", info);
    } else if (strcmp(param, "rec") == 0) {
        packParamValueInfo(nullptr, rec_, root_res);
    } else if (strcmp(param, "osd") == 0) {
        packParamValueInfo(nullptr, osd_, root_res);
    } else if (strcmp(param, "mic") == 0) {
        packParamValueInfo(nullptr, mic_, root_res);
    } else if (strcmp(param, "speaker") == 0) {
        packParamValueInfo(nullptr, speaker_, root_res);
    } else if (strcmp(param, "voice_control") == 0) {
        packParamValueInfo(nullptr, voice_control_, root_res);
    } else if (strcmp(param, "adas") == 0) {
        packParamValueInfo(nullptr, adas_, root_res);
    }
    else if (strcmp(param, "rec_resolution") == 0) {
        packParamValueInfo(nullptr, resolution_, root_res);
    }
    else if (strcmp(param, "rec_split_duration") == 0) {
        packParamValueInfo(nullptr, rec_split_duration_, root_res);
    }
    else if (strcmp(param, "gsr_sensitivity") == 0) {
        packParamValueInfo(nullptr, gsr_sensitivity_, root_res);
    }
    else if (strcmp(param, "screen_standby") == 0) {
        packParamValueInfo(nullptr, screen_standby_, root_res);
    }
    else if (strcmp(param, "parking_monitor") == 0) {
		if (g_acc_connected){
		
            packParamValueInfo(nullptr, parking_monitor_, root_res);
		}
    }
    else if (strcmp(param, "timelapse_rate") == 0) {
		if (g_acc_connected){
        	packParamValueInfo(nullptr, timelapse_rate_, root_res);
		}
    }
    else if (strcmp(param, "key_tone") == 0) {
        packParamValueInfo(nullptr, key_tone_, root_res);
    }
    else if (strcmp(param, "park_record_time") == 0) {
		if (g_acc_connected){
		
            packParamValueInfo(nullptr, park_record_time_, root_res);
		}
    }

    char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
	free(http_body);

    g_engineId = engineId;
	g_connId = connId;
}

void HTTPSerMdl::packParamValueInfo(const char* name, int value, cJSON* infoObject)
{
    cJSON* object = cJSON_CreateObject();
    // cJSON_AddNumberToObject(object, "value", value);
    if (nullptr != name) {
        cJSON_AddStringToObject(object, "name", name);
        cJSON_AddNumberToObject(object, "value", value);
        cJSON_AddItemToArray(infoObject, object);
    } else {
        cJSON_AddNumberToObject(object, "value", value);
        cJSON_AddItemToObject(infoObject, "info", object);
    }
}

void HTTPSerMdl::enterRecorder(int engineId, int connId, char * connType)
{
    XMLogW("enterRecorder");
    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddStringToObject(root_res, "info", "enterrecorder success.");
    
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);

    XM_Middleware_Periphery_Notify(XM_EVENT_RECORD_START, "", 0);

    g_engineId = engineId;
    g_connId = connId;
    XM_Middleware_RTSP_Stop();
    XM_Middleware_RTSP_Start();
    uint32_t a, b;
    XM_Middleware_GetMemoryMsg(a, b);
    g_realplay = true;
}

void HTTPSerMdl::exitRecorder(int engineId, int connId, char * connType)
{
    XMLogW("exitRecorder");
    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddStringToObject(root_res, "info", "exitrecorder success.");
    
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);
    XMLogW("before XM_Middleware_RTSP_Stop");
    uint32_t a, b;
    XM_Middleware_GetMemoryMsg(a, b);
    XM_Middleware_RTSP_Stop();
    XMLogW("after XM_Middleware_RTSP_Stop");
    XM_Middleware_GetMemoryMsg(a, b);
    g_realplay = false;
}

void HTTPSerMdl::OnMWRealTimeStream(int channel, int stream, XM_MW_Media_Frame* media_frame, int64_t user)
{
    if (!g_realplay)  
		return ;
	// bool ret = true;
	if (media_frame->video) {
        XM_Middleware_RTSP_PushVideoStream(channel, stream, (unsigned char*)media_frame->frame_buffer, media_frame->frame_size, media_frame->key_frame);
    }
	else {
        XM_Middleware_RTSP_PushAudioStream(channel, (unsigned char*)media_frame->frame_buffer, media_frame->frame_size);
	}

#if 1//X2V60_S_DEBUG1
	if(GlobalData::Instance()->app_page_ != APP_PAGE_PREVIEW){
		
		XM_Middleware_Periphery_Notify(XM_EVENT_APP_PAGE, "", APP_PAGE_PREVIEW);
	}
#endif
	
	return;	
}

bool HTTPSerMdl::OnMWVODStream(long handle, XM_MW_Media_Frame* media_frame, int64_t user)
{
    return true;
}

void HTTPSerMdl::setParamValue(int engineId, int connId, char* msg, char * connType)
{
    int ret = 0;
    string_vector_t head_line_vect;
    splitMsg(msg, head_line_vect);
    cJSON* root_res = cJSON_CreateObject();
    XMLogW("parm:%s", head_line_vect[0].substr(1).c_str());
    XMLogW("value:%s", head_line_vect[1].substr(6).c_str());
    if (head_line_vect.size() < 2) {
        cJSON_AddNumberToObject(root_res, "result", 99);
        cJSON_AddStringToObject(root_res, "info", "Parameter incompleteness.");
    }
    else {
        string param = head_line_vect[0].substr(1);
        string value = head_line_vect[1].substr(6);
        if (strcmp(param.c_str(), "switchcam") == 0) {
            int channel = atoi(value.c_str());
            XMLogW("switchcam!!!!!!! , %s, channel=%d", msg, channel);
            if (channel == 2 && XM_Middleware_Mpp_ADLoss()) {
                ret = -1;
            }
            else {
                XM_Middleware_Periphery_Notify(XM_EVENT_CAMERA_SWITCH, "", channel);
            }
        }
        else if (strcmp(param.c_str(), "rec") == 0) {
            if (strstr(value.c_str(), "1") != NULL) {
                if (sdcard_speed_ >= 1024 && g_sd_status == XM_SD_NORMAL) {
                    XM_Middleware_Periphery_Notify(XM_EVENT_RECORD_START, "", 0);
                    rec_ = 1;
                }
                else {
                    ret = -1;
                }
            } else {
                XM_Middleware_Periphery_Notify(XM_EVENT_RECORD_CLOSE, "", 0);
                rec_ = 0;
            }
        }
        else if (strcmp(param.c_str(), "ir_mode") == 0) {
            if (strstr(value.c_str(), "1") != NULL) {
                XM_Middleware_Periphery_Notify(XM_EVENT_IR_MODE, "", 1);
                ir_mode_ = 1;
            } else if (strstr(value.c_str(), "2") != NULL) {
                XM_Middleware_Periphery_Notify(XM_EVENT_IR_MODE, "", 2);
                ir_mode_ = 2;
            } else {
                XM_Middleware_Periphery_Notify(XM_EVENT_IR_MODE, "", 0);
                ir_mode_ = 0;
            }
        }
        else if (strcmp(param.c_str(), "mic") == 0) {
            if (strstr(value.c_str(), "1") != NULL) {
                XM_Middleware_Periphery_Notify(XM_EVENT_RECORD_SOUND_ENABLE, "", 1);
                mic_ = 1;
            } else {
                XM_Middleware_Periphery_Notify(XM_EVENT_RECORD_SOUND_ENABLE, "", 0);
                mic_ = 0;
            }
        }
        else if (strcmp(param.c_str(), "osd") == 0) {
            if (strstr(value.c_str(), "1") != NULL) {
                osd_ = 1;
            } else {
                osd_ = 0;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_OSD_ENABLE, "", osd_);
        }
        else if (strcmp(param.c_str(), "speaker") == 0) {
            #if 1
            if (strstr(value.c_str(), "0") != NULL) {
                speaker_ = 0;
            } else if (strstr(value.c_str(), "1") != NULL) {
                speaker_ = 1;
            } else if (strstr(value.c_str(), "2") != NULL) {
                speaker_ = 2;
            } else {
                speaker_ = 4;
            }

			XMLogI("[setParamValue] speaker_: %d \r\n", speaker_);
			XM_Middleware_Periphery_Notify(XM_EVENT_SPEAKER_VOLUME, "", speaker_);

            #else
            if (strstr(value.c_str(), "0") != NULL) {
                XM_Middleware_Sound_SetVolume(0);
                speaker_ = 0;
            } else if (strstr(value.c_str(), "1") != NULL) {
                XM_Middleware_Sound_SetVolume(33);
                speaker_ = 1;
            } else if (strstr(value.c_str(), "2") != NULL) {
                XM_Middleware_Sound_SetVolume(66);
                speaker_ = 2;
            } else {
                XM_Middleware_Sound_SetVolume(100);
                speaker_ = 4;
            }
            #endif
        }
        else if (strcmp(param.c_str(), "voice_control") == 0) {
            if (strstr(value.c_str(), "1") != NULL) {
                XM_Middleware_Periphery_Notify(XM_EVENT_VOICE_CONTROL_ENABLE, "", 1);
                voice_control_ = 1;
            } else {
                XM_Middleware_Periphery_Notify(XM_EVENT_VOICE_CONTROL_ENABLE, "", 0);
                voice_control_ = 0;
            }
        }
        else if (strcmp(param.c_str(), "adas") == 0) {
            if (strstr(value.c_str(), "1") != NULL) {
                // XM_Middleware_Smart_EnableKeyword(true);
                adas_ = 1;
            } else {
                // XM_Middleware_Smart_EnableKeyword(false);
                adas_ = 0;
            }
        }
        else if (strcmp(param.c_str(), "rec_resolution") == 0) {
            if (strstr(value.c_str(), "1080") != NULL) {
                resolution_ = 1080;
            }
            else if (strstr(value.c_str(), "1440") != NULL) {
                resolution_ = 1440;
            }
            else {
                resolution_ = kMaxPicHeight;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_CHANGE_RESOLUTION, "", resolution_);
        }
        else if (strcmp(param.c_str(), "rec_split_duration") == 0) {
            if (strstr(value.c_str(), "1") != NULL) {
                rec_split_duration_ = 1;
            }
            else if (strstr(value.c_str(), "2") != NULL) {
                rec_split_duration_ = 2;
            }
            else {
                rec_split_duration_ = 3;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_REC_SPLIT_DUR, "", rec_split_duration_);
        }
        else if (strcmp(param.c_str(), "gsr_sensitivity") == 0) {
            if (strstr(value.c_str(), "0") != NULL) {
                gsr_sensitivity_ = 0;
            }
            else if (strstr(value.c_str(), "1") != NULL) {
                gsr_sensitivity_ = 1;
            }
            else if (strstr(value.c_str(), "2") != NULL) {
                gsr_sensitivity_ = 2;
            }
            else {
                gsr_sensitivity_ = 3;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_GSR_SENSITIVITY, "", gsr_sensitivity_);
        }
        else if (strcmp(param.c_str(), "screen_standby") == 0) {
            if (strstr(value.c_str(), "0") != NULL) {
                screen_standby_ = 0;
            }
            else if (strstr(value.c_str(), "1") != NULL) {
                screen_standby_ = 1;
            }
            else if (strstr(value.c_str(), "3") != NULL) {
                screen_standby_ = 3;
            }
            else {
                screen_standby_ = 5;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_SCREEN_STANDBY, "", screen_standby_);
        }
        else if (strcmp(param.c_str(), "parking_monitor") == 0) {
            if (strstr(value.c_str(), "0") != NULL) {
                parking_monitor_ = 0;
            }
            else {
                parking_monitor_ = 1;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_PARKING_MONITOR, "", parking_monitor_);
        }
        else if (strcmp(param.c_str(), "timelapse_rate") == 0) {
            if (strstr(value.c_str(), "0") != NULL) {
                timelapse_rate_ = 0;
            }
            else if (strstr(value.c_str(), "1") != NULL) {
                timelapse_rate_ = 1;
            }
            else if (strstr(value.c_str(), "2") != NULL) {
                timelapse_rate_ = 2;
            }
            else {
                timelapse_rate_ = 5;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_TIMELAPSE_RATE, "", timelapse_rate_);
        }
        else if (strcmp(param.c_str(), "key_tone") == 0) {
            if (strstr(value.c_str(), "0") != NULL) {
                key_tone_ = 0;
            }
            else {
                key_tone_ = 1;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_KEYTONE_ENABLE, "", key_tone_);
        }
        else if (strcmp(param.c_str(), "park_record_time") == 0) {
            if (strstr(value.c_str(), "0") != NULL) {
                park_record_time_ = 0;
            }
            else if (strstr(value.c_str(), "8") != NULL) {
                park_record_time_ = 8;
            }
            else if (strstr(value.c_str(), "16") != NULL) {
                park_record_time_ = 16;
            }
            else {
                park_record_time_ = 24;
            }
            XM_Middleware_Periphery_Notify(XM_EVENT_PARK_RECORD_TIME, "", park_record_time_);
        }

        if (ret == 0) {
            cJSON_AddNumberToObject(root_res, "result", 0);
            cJSON_AddStringToObject(root_res, "info", "set success.");
        }
        else if (ret == -1) {
            cJSON_AddNumberToObject(root_res, "result", 1);
            cJSON_AddStringToObject(root_res, "info", "set fail.");
        }
    }
    
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(engineId, connId, StatusCode::Ok, http_body, strlen(http_body));
    free(http_body);
}

int HTTPSerMdl::tcpSdStatus(int engineId, int status, int sdcard_speed)
{
    sdcard_speed_ = sdcard_speed;
    cJSON* root_res = cJSON_CreateObject();
    cJSON* info = cJSON_CreateObject();
    cJSON_AddNumberToObject(info, "status", status);
    cJSON_AddStringToObject(root_res, "msgid", "sd");
    cJSON_AddItemToObject(root_res, "info", info);
    tm tmp_time;
	time_t timep;
	time(&timep);
	tmp_time = *(localtime(&timep));
	int64_t t = mktime(&tmp_time);
    cJSON_AddNumberToObject(info, "time", t);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendTcpBuffer(engineId, XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
    return 0;
}

int HTTPSerMdl::tcpRecStatus(int engineId, int status)
{
    rec_ = status;
    cJSON* root_res = cJSON_CreateObject();
    cJSON* info = cJSON_CreateObject();
    cJSON_AddNumberToObject(info, "value", status);
    cJSON_AddStringToObject(root_res, "msgid", "rec");
    cJSON_AddItemToObject(root_res, "info", info);
    tm tmp_time;
	time_t timep;
	time(&timep);
	tmp_time = *(localtime(&timep));
	int64_t t = mktime(&tmp_time);
    cJSON_AddNumberToObject(info, "time", t);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendTcpBuffer(engineId, XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
    return 0;
}

#if 1//X2V60_S_DEBUG1
int HTTPSerMdl::tcpWiFiStatus(int engineId)
{
    cJSON* root_res = cJSON_CreateObject();

    cJSON_AddStringToObject(root_res, "msgid", "wifi_disconnect");

    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
	
    XM_Middleware_Network_SendTcpBuffer(engineId, XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
    return 0;
}

#endif

int HTTPSerMdl::tcpMictatus(int engineId, int status)
{
    mic_ = status;
    cJSON* root_res = cJSON_CreateObject();
    cJSON* info = cJSON_CreateObject();
    cJSON_AddNumberToObject(info, "value", status);
    cJSON_AddStringToObject(root_res, "msgid", "mic");
    cJSON_AddItemToObject(root_res, "info", info);
    tm tmp_time;
	time_t timep;
	time(&timep);
	tmp_time = *(localtime(&timep));
	int64_t t = mktime(&tmp_time);
    cJSON_AddNumberToObject(info, "time", t);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendTcpBuffer(engineId, XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
    return 0;
}

int HTTPSerMdl::tcpFileDelStatus(int engineId, char* filename, int type)
{
    cJSON* root_res = cJSON_CreateObject();
    cJSON* info = cJSON_CreateObject();
    cJSON_AddStringToObject(info, "name", filename);
    cJSON_AddNumberToObject(info, "type", type);
    cJSON_AddStringToObject(root_res, "msgid", "file_del");
    cJSON_AddItemToObject(root_res, "info", info);
    tm tmp_time;
	time_t timep;
	time(&timep);
	tmp_time = *(localtime(&timep));
	int64_t t = mktime(&tmp_time);
    cJSON_AddNumberToObject(info, "time", t);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendTcpBuffer(engineId, XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
    return 0;
}

int HTTPSerMdl::tcpRecLockStatus(int engineId, int status)
{
    cJSON* root_res = cJSON_CreateObject();
    cJSON* info = cJSON_CreateObject();
    cJSON_AddNumberToObject(info, "value", status);
    cJSON_AddStringToObject(root_res, "msgid", "rec_lock");
    cJSON_AddItemToObject(root_res, "info", info);
    tm tmp_time;
	time_t timep;
	time(&timep);
	tmp_time = *(localtime(&timep));
	int64_t t = mktime(&tmp_time);
    cJSON_AddNumberToObject(info, "time", t);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendTcpBuffer(engineId, XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
    return 0;
}

int HTTPSerMdl::tcpCameraStatus(int engineId, int action, int curcamid, int camnum)
{
    cJSON* root_res = cJSON_CreateObject();
    cJSON* info = cJSON_CreateObject();
    cJSON_AddNumberToObject(info, "action", action);
    cJSON_AddNumberToObject(info, "curcamid", curcamid);
    cJSON_AddNumberToObject(info, "camnum", camnum);
    cJSON_AddStringToObject(root_res, "msgid", "cam_plugin");
    cJSON_AddItemToObject(root_res, "info", info);

    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendTcpBuffer(engineId, XM_DATA_STRING, http_body, strlen(http_body));
    free(http_body);
    return 0;
}

int HTTPSerMdl::Run()
{
#ifndef WIN32
	prctl(PR_SET_NAME, "http_mdl");
#endif    
    std::chrono::milliseconds dura(50);
    while (1) {
        bool have_thread_exit = false;
        thread_map_mutex_.lock();
        auto it = http_units_.begin();
        for (; it != http_units_.end(); it++) {
            HttpUnit* unit = it->second;
            if (unit->thread_exit()) {
                http_units_.erase(it);
                thread_map_mutex_.unlock();
                unit->Clean();
                delete unit;
                have_thread_exit = true;
                break;
            }
        }
        if (!have_thread_exit) {
            thread_map_mutex_.unlock();
            std::this_thread::sleep_for(dura);
        }
    }
	return 0;
}

void HTTPSerMdl::playPackFile(int engineId, int connId, char* msg, char * connType)
{
    thread_map_mutex_.lock();
    if (http_units_.find(connId) == http_units_.end()) {
        XMMsgInfo msg_info;
        msg_info.engineId = engineId;
        msg_info.connId = connId;
        msg_info.msg = msg;
        msg_info.connType = connType;
        XM_Middleware_Network_SendOnly(engineId, connId);
        HttpUnit* unit = new HttpUnit();
        unit->Init(msg_info);
        http_units_[connId] = unit;
        XMLogW("playPackFile, total count=%d", http_units_.size());
        uint32_t a, b;
        XM_Middleware_GetMemoryMsg(a, b);
    }
    thread_map_mutex_.unlock();
}

void HTTPSerMdl::CloseAll()
{
    XMLogW("Close All http unit");
    thread_map_mutex_.lock();
    auto it = http_units_.begin();
    for (; it != http_units_.end(); it++) {
        HttpUnit* unit = it->second;
        unit->Clean();
        delete unit;
    }
    http_units_.clear();
    thread_map_mutex_.unlock(); 
    XM_Middleware_Network_CloseHttpUnit();
}

void HTTPSerMdl::CloseThread(int connId)
{
    thread_map_mutex_.lock();
    auto it = http_units_.find(connId);
    if (it != http_units_.end()) {
        HttpUnit* unit = it->second;
        http_units_.erase(it);
        thread_map_mutex_.unlock();
        unit->Clean();
        delete unit;
        XMLogW("connid=%d close thread", connId);
    }
    else {
        thread_map_mutex_.unlock();
    }
}

void HTTPSerMdl::split(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
}

void HTTPSerMdl::splitMsg(char* msg, vector<string>& elems)
{
    char* ptmp = strstr(msg, "=");
    string strtmp(ptmp);
    split(strtmp, '&', elems);
}



int HTTPSerMdl::changeSupportResolution(char* resolution)
{
    memset(support_resolution_, 0, sizeof(support_resolution_));
    memcpy(support_resolution_, resolution, strlen(resolution));
    return 0;
}
