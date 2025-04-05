#pragma once

#ifdef WIN32
#ifdef API_EXPORTS
#define XM_MIDDLEWARE_API extern "C" __declspec(dllexport)
#else
#define XM_MIDDLEWARE_API extern "C" __declspec(dllimport)
#endif
#define CALLMETHOD __stdcall
#else
#define XM_MIDDLEWARE_API 
#define CALLMETHOD 
#endif

#include "xm_middleware_def.h"

/*------------------------全局-----------------------------*/
//设置通道数，默认为XM_MAX_CHANNEL_NUM。该接口需要在XM_Middleware_Init之前调用
XM_MIDDLEWARE_API int XM_Middleware_SetChannelNum(int channel_num);
//初始化中间件sdk
XM_MIDDLEWARE_API int XM_Middleware_Init();
//获取sdk版本号
XM_MIDDLEWARE_API const char* XM_Middleware_GetVersion();
//退出中间件sdk释放资源
XM_MIDDLEWARE_API int XM_Middleware_Exit();
//设置日志等级
XM_MIDDLEWARE_API int XM_Middleware_SetLogLevel(XMLogLevel level);
//设置录像缓冲帧数、录像最大块数、录像空间满自动覆盖
XM_MIDDLEWARE_API int XM_Middleware_SetParam(int channel, XM_SET_PARAM param, int value);

/*----------------------设置回调---------------------------*/
//设置MPP实时预览数据回调
XM_MIDDLEWARE_API int XM_Middleware_SetRealStreamCallback(XM_MW_RealStream_CallBack proc, int64_t user);
//设置中间件sdk回放流回调
XM_MIDDLEWARE_API int XM_Middleware_SetVodStreamCallback(XM_MW_VodStream_CallBack proc, int64_t user);
//设置中间件sdk事件回调
XM_MIDDLEWARE_API int XM_Middleware_SetEventCallback(XM_MW_Event_CallBack proc, int64_t user);
XM_MIDDLEWARE_API int XM_Middleware_SetFileSystemCallback(XM_MW_FileSystem_CallBack proc, int64_t user);
//设置日志回调
XM_MIDDLEWARE_API int XM_Middleware_SetLogCallback(XM_MW_LOG_CallBack proc, int64_t user);

/*------------------------存储-----------------------------*/
//放在最开始调用。设置坏卡检测超时时间，单位毫秒。默认8000毫秒
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetBadSdcardCheckTime(int time);
//设置sd卡mount路径
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetSDCardConfig(XM_MW_Storage_SDCard_Config* sdcard_cfg);
//获取速度状态
XM_MIDDLEWARE_API int XM_Middleware_Storage_GetSpeedState(XMStorageSDCardNum sdc_num, int* write_speed);
//写速度到文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_WriteSpeedToFile(XMStorageSDCardNum sdc_num, int* write_speed);
//检测sd卡,挂载sd卡及检测文件系统
XM_MIDDLEWARE_API int XM_Middleware_Storage_CheckSDCard(XMStorageSDCardNum sdc_num);
//只挂载sd卡
XM_MIDDLEWARE_API int XM_Middleware_Storage_MountSDCard(XMStorageSDCardNum sdc_num); 
//检测sd卡上的文件系统
XM_MIDDLEWARE_API int XM_Middleware_Storage_CheckFileSystem(XMStorageSDCardNum sdc_num);
//查询sd卡容量
XM_MIDDLEWARE_API int XM_Middleware_Storage_CheckSDSpace(XMStorageSDCardNum sdc_num, XM_MW_SDCard_Space* sdcard_space);
//格式化sd卡
XM_MIDDLEWARE_API int XM_Middleware_Storage_FormatSDCard(XMStorageSDCardNum sdc_num);
//释放所有通道缓冲队列内存
XM_MIDDLEWARE_API int XM_Middleware_Storage_DestroyFrameList(XMStorageSDCardNum sdc_num);
//设置录像时长，1、2、3分钟
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetRecordDuration(XMStorageRecordDuration record_duration);
//设置普通录像延时去切换下一个文件，默认550毫秒
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetFileSwtichDelayTime(int delay);
//设置停车监控唤醒后紧急视频时长，默认30秒, 范围10-100秒
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetAlarmRecordDuration(int dura);
//指定视频通道存储到哪张sd卡
XM_MIDDLEWARE_API int XM_Middleware_Storage_BindSDCardNum(int channel, XMStorageSDCardNum sdc_num);
//设置存储媒体格式，只针对pc版本有效，板子上会自动从mpp模块获取
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetMediaInfo(int channel, XM_MW_Media_Info* media_info);
//开始录像/停止录像, channel为-1，表所有通道开启和关闭。刚插上后拉时，画面不稳定，delay_time用于延迟存储，过滤异常画面
XM_MIDDLEWARE_API int XM_Middleware_Storage_Start(int channel, int delay_time);
//碰撞开机录像
XM_MIDDLEWARE_API int XM_Middleware_Storage_CollisionRecord(int channel);
//录音使能
XM_MIDDLEWARE_API int XM_Middleware_Storage_EnableRecordAudio(bool enable);
//AddVideoFrame\AddAudioFrame\AddPicture只针对pc版本有效，板子上会自动从mpp模块取流
XM_MIDDLEWARE_API int XM_Middleware_Storage_AddVideoFrame(int channel, bool key_frame, uint8_t* data, int len, int64_t pts);
XM_MIDDLEWARE_API int XM_Middleware_Storage_AddAudioFrame(int channel, uint8_t* data, int len, int64_t pts);
XM_MIDDLEWARE_API int XM_Middleware_Storage_AddPicture(int channel, uint8_t* data, int len);
//增加自定义数据，诸如GPS信息
XM_MIDDLEWARE_API int XM_Middleware_Storage_AddUserData(int channel, uint8_t* data, int len, int64_t pts);
//慢回放存储
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetSpeed(int channel, XM_STORAGE_SPEED storage_speed);
XM_MIDDLEWARE_API int XM_Middleware_Storage_Stop(int channel);
XM_MIDDLEWARE_API bool XM_Middleware_Storage_ReallyStop();
//锁定文件，关键录像，不允许被覆盖。锁定历史录像文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_LockHistoryFile(XM_MW_File_Param* file_param);
//锁定所有文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_LockAllHistoryFile(XMStorageSDCardNum sdc_num, int channel, XMRecordType type);
//解锁历史文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_UnLockHistoryFile(XM_MW_File_Param* file_param);
//解锁所有文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_UnLockAllHistoryFile(XMStorageSDCardNum sdc_num, int channel, XMRecordType type);
//锁定当前正在录的一个录像文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_LockCurrentFile(XMStorageSDCardNum sdc_num, int channel, bool lock_file);
//设置加锁文件空间占比
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetLockFilePercent(XM_LOCKFILE_PERCENT lockfile_percent);
//删除文件，删除前需停止播放
XM_MIDDLEWARE_API int XM_Middleware_Storage_DelFile(XM_MW_File_Param* file_param);
//删除所有文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_DelAllFile(XMStorageSDCardNum sdc_num, int channel, XMRecordType type);
//获取当前录像文件已录的时间
XM_MIDDLEWARE_API int XM_Middleware_Storage_GetRecordedTime(XMStorageSDCardNum sdc_num, int channel);
//返回剩余空间,输出参数remain_space（单位MB)，目前有1.5G做为预留空间。函数返回值小于0表失败，一般是尚未开始录像，此时上层不显示剩余录像时间
XM_MIDDLEWARE_API int XM_Middleware_Storage_GetRemainSpace(XMStorageSDCardNum sdc_num, int* remain_space);
//返回录像所需要的sd卡空间
XM_MIDDLEWARE_API int XM_Middleware_Storage_GetRecordSpaceNeed(XMStorageSDCardNum sdc_num, int* space_need);
//获取缩略图png
XM_MIDDLEWARE_API int XM_Middleware_Media_GetThumbnailPng(int channel, char* out_buffer, int* out_len);

/*------------------------回放-----------------------------*/
//mtv播放使能。解码mtv时通知到sdk，sdk内部关闭一些功能，释放更多资源来做mtv软解
XM_MIDDLEWARE_API int XM_Middleware_Playback_MTV(bool enable);
//录像(包括视频和图片)查询
XM_MIDDLEWARE_API int XM_Middleware_Playback_SetUSBPath(const char* dir_path);
//查询图片总数
XM_MIDDLEWARE_API long XM_Middleware_Playback_QueryPicNum(XMStorageSDCardNum sdc_num, int channel);
//录像和图片查询
XM_MIDDLEWARE_API int XM_Middleware_Playback_Query(XM_MW_Record_Query* query_info, XM_MW_Record_List* record_list);
//打开视频录像
XM_MIDDLEWARE_API long XM_Middleware_Playback_Open(XMStorageSDCardNum sdc_num, int channel, XMFileFormat file_format, 
	const char* file_full_name);
//获取录像文件信息
XM_MIDDLEWARE_API int XM_Middleware_Playback_GetFileInfo(long handle, XM_MW_File_Info* file_info);
//解码缩略图。scale_width默认为0，不做缩放，非0做缩放
XM_MIDDLEWARE_API long XM_Middleware_Playback_DecodeJpg(char* jpg_buf, int len,
	char* rgb_buf, int* width, int* height, int scale_width=0, int scale_height =0);

//display_region是渲染区域的位置和大小。play_one为true时只播放一帧
XM_MIDDLEWARE_API int XM_Middleware_Playback_Play(long handle, bool play_one, RECT_S* display_region);
//seek_time，单位毫秒
XM_MIDDLEWARE_API int XM_Middleware_Playback_Seek(long handle, int seek_time);
//pause true表暂停，false表恢复
XM_MIDDLEWARE_API int XM_Middleware_Playback_Pause(long handle, bool pause);
//获取到当前播放进度，单位毫秒
XM_MIDDLEWARE_API int XM_Middleware_Playback_GetCurrentDuration(long handle);
//关闭录像文件
XM_MIDDLEWARE_API int XM_Middleware_Playback_Close(long handle);
//挂载U盘
XM_MIDDLEWARE_API int XM_Middleware_Playback_CheckUDisk();
//格式化u盘
XM_MIDDLEWARE_API int XM_Middleware_Playback_FormatUDisk();
//卸载U盘
XM_MIDDLEWARE_API int XM_Middleware_Playback_UmountUDisk();

/*------------------------图片-----------------------------*/
//解码渲染图片
XM_MIDDLEWARE_API int XM_Middleware_Picture_Play(const char* file_full_name, int file_len, RECT_S* display_region);
//从录像文件中获取缩略图
XM_MIDDLEWARE_API int XM_Middleware_Picture_GetFromRecord(XMFileFormat file_format, 
	const char* file_full_name, char* jpg_buf, int* jpg_len);

/*------------------------算法-----------------------------*/
//初始化算法
XM_MIDDLEWARE_API int XM_Middleware_Smart_Init();
//设置各通道ai算法类型，默认XM_AI_TYPE_BSD
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetAIType(int channel, XMAIType ai_type);
//设置有多少个算法通道
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetAIChannelNum(int channel_num);
//设置adas参数
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetAdasParam(int channel, XM_MW_ADAS_Param* adas_param);
//设置adas模式
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetAdasMode(int channel, int adas_mode);
//设置bsd参数
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetBsdParam(int channel, XM_MW_BSD_Param* bsd_param);
//ai使能
XM_MIDDLEWARE_API int XM_Middleware_Smart_EnableAI(int channel, bool enable);
//获取AI使能
XM_MIDDLEWARE_API bool XM_Middleware_Smart_GetAIEnable(int channel);
//声控使能
XM_MIDDLEWARE_API int XM_Middleware_Smart_EnableKeyword(bool enable);
//笑脸检测使能
XM_MIDDLEWARE_API int XM_Middleware_Smart_EnableFaceClsDetect(bool enable);
//传递车辆运动还是静止的状态
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetCarStopStatus(int status);
//获取车辆运动还是静止的状态
XM_MIDDLEWARE_API int XM_Middleware_Smart_GetCarStopStatus();

/*------------------------报警---------------------------*/
//触发线警报
XM_MIDDLEWARE_API int XM_Middleware_Alarm_AlarmStart();

/*------------------------停车监控-----------------------*/
//设置一秒录几帧
XM_MIDDLEWARE_API int XM_Middleware_ParkRecord_SetTimeLapse(XMTimeLapse time_lapse);
//设置录制时长单位小时
XM_MIDDLEWARE_API int XM_Middleware_ParkRecord_SetTimeLapseDuration(XMTimeLapseDuration time_lapse_duration);
//使能停车监控
XM_MIDDLEWARE_API int XM_Middleware_ParkRecord_Enable(bool enable);

/*----------------------看门狗---------------------------*/
//sec是看门狗超时时间，默认3秒
XM_MIDDLEWARE_API int XM_Middleware_WDT_WatchDogStart(int sec);
XM_MIDDLEWARE_API int XM_Middleware_WDT_WatchDogStop();

/*-----------------------SD卡升级------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_SDCard_Upgrade(const char* file_name);
//判断上一次升级是否是uboot强制升级
XM_MIDDLEWARE_API int XM_Middleware_CheckIsUbootUpgrade(const char* file_name = NULL, const char* md5_name = NULL);
//判断是否能升级
XM_MIDDLEWARE_API int XM_Middleware_SDCard_AlreadyUpgrade(const char* file_name);
#ifndef WIN32
XM_MIDDLEWARE_API int XM_Middleware_SDCard_Upgrade(const char* file_name, const char* md5_name);
XM_MIDDLEWARE_API int XM_Middleware_SDCard_AlreadyUpgrade(const char* file_name, const char* md5_name);
XM_MIDDLEWARE_API int XM_Middleware_SDCard_AlreadyUpgradeWithoutMount(const char* file_name, const char* md5_name);
#endif
//获取升级进度
XM_MIDDLEWARE_API int XM_Middleware_SDCard_GetUpgradeProgress();
//获取当前MD5
XM_MIDDLEWARE_API int XM_Middleware_SDCard_GetMD5(const char* file_name, char* md5_out);
XM_MIDDLEWARE_API void XM_Middleware_SDCard_IsUpdateCompileTime(bool option);

/*-----------------------设置环境变量------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_EnvReadKeyValue(const char *pKey, char *pValue, int ValueLen);
XM_MIDDLEWARE_API int XM_Middleware_EnvWriteKeyValue(const char *pKey, const char *pValue);

/*-----------------------外设事件通知--------------------*/
XM_MIDDLEWARE_API int XM_Middleware_Periphery_Notify(XMEventType event_type, const char* msg, int param, char* out_msg = NULL);

/*-----------------------媒体推流------------------------*/
//MPP媒体推流到SDK内部做存储；相机拍归要返回图片路径
XM_MIDDLEWARE_API int XM_Middleware_Media_VideoStream(int channel, int stream, XM_MW_Media_Frame* media_frame, int width, int height, char* jpg_full_name = NULL);
XM_MIDDLEWARE_API int XM_Middleware_Media_AudioStream(int ai_dev, int channel, XM_MW_Media_Frame* media_frame);
//添加视频缩略图
XM_MIDDLEWARE_API int XM_Middleware_Media_VideoThumbnail(int channel, char* yuv, int width, int height);

/*-----------------------USB功能-------------------------*/
//创建一个USB功能
XM_MIDDLEWARE_API int XM_Middleware_USBFunc_Create(USB_MODE_E mode);
//销毁一个USB功能
XM_MIDDLEWARE_API int XM_Middleware_USBFunc_Destroy(USB_MODE_E mode);
//检测是否是连接电脑
XM_MIDDLEWARE_API int XM_Middleware_USBFunc_ConnectComputer();

/*-----------------------WIFI----------------------------*/
//获取wifi参数
XM_MIDDLEWARE_API int XM_Middleware_WIFI_GetMacParam(WIFI_MAC_PARAM_S* param);
#ifndef WIN32
XM_MIDDLEWARE_API int XM_Middleware_WIFI_GetEthAttr(const char* pEthName, XMSDK_NET_ATTR_S* pstNetAttr);
#endif
/*-----------------------RTSP----------------------------*/
//开启RTSP默认运行在TCP端口554
XM_MIDDLEWARE_API int XM_Middleware_RTSP_Start();
//停止RTSP
XM_MIDDLEWARE_API int XM_Middleware_RTSP_Stop();
//推流
XM_MIDDLEWARE_API int XM_Middleware_RTSP_PushAudioStream(int channel, uint8_t* data, int len);
XM_MIDDLEWARE_API int XM_Middleware_RTSP_PushVideoStream(int channel, int stream, uint8_t* data, int len, bool key_frame);

/*-----------------------时区----------------------------*/
//设置时区
XM_MIDDLEWARE_API int XM_Middleware_TIME_SetTimeZoneInfo(int minuteswest);
//获取时区
XM_MIDDLEWARE_API int XM_Middleware_TIME_GetTimeZoneInfo(int* minuteswest);

/*-----------------------system--------------------------*/
//替代system函数
XM_MIDDLEWARE_API int XM_Middleware_SafeSystem(const char* pCmdLine);
//获取当前OS内存使用情况
XM_MIDDLEWARE_API int XM_Middleware_GetMemoryMsg(uint32_t& used_memory, uint32_t& available_memory);

/*-----------------------mpp-----------------------------*/
//在头文件中定义，在sample中实现，由sdk调用

/*-----------------------GPS----------------------------*/
//传递车辆速度
XM_MIDDLEWARE_API int XM_Middleware_GPS_SetCarSpeedStatus(double speed);
//获取车辆速度
XM_MIDDLEWARE_API double XM_Middleware_GPS_GetCarSpeedStatus();
//传递GPS信号
XM_MIDDLEWARE_API int XM_Middleware_GPS_SetCarGpsStatus(int status);
//获取GPS信号
XM_MIDDLEWARE_API int XM_Middleware_GPS_GetCarGpsStatus();
//传递GPS经纬度
XM_MIDDLEWARE_API int XM_Middleware_GPS_SetCarLonLatStatus(GPS_GNRMC rmc_data);
//获取GPS经纬度
XM_MIDDLEWARE_API GPS_GNRMC XM_Middleware_GPS_GetCarLonLatStatus();

/*-----------------------缩放----------------------------*/
//缩放初始化，format使用XMPixelFormat中的定义
XM_MIDDLEWARE_API int XM_Middleware_Resize_Init(int src_width, int src_height, int src_format, int dst_width, int dst_height, int dst_format);
XM_MIDDLEWARE_API int XM_Middleware_Resize_Convert(long handle, uint8_t* src, int src_width, int src_height, uint8_t* dst, int dst_width, int dst_height);
XM_MIDDLEWARE_API int XM_Middleware_Resize_Clean(long handle);
