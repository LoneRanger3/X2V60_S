#ifndef _DATA_ASSIST_H_
#define _DATA_ASSIST_H_

typedef unsigned int	uint;			///< sizeof(uint) == sizeof(int) == 4
typedef unsigned char	uchar;	

//error code
enum XMSDK_RETURN_CODE	//
{
	XMSDK_SUCCESS	= 0,
	XMSDK_NOT_SUPPORT_STREAM = -1000,
	XMSDK_NOT_SUPPORT_CHANNEL = -1001,
	XMSDK_CAPTURE_FAIL = -1002,
	XMSDK_NO_INIT = -1003,
	XMSDK_ILLEGAL_PARAM = -1004,
	XMSDK_TIMEOUT = -1005,
	XMSDK_INVALID_HANDLE = -1006,
	XMSDK_IN_PLAYING = -1007,
	XMSDK_NOT_FOUND_FILE = -1008,
	XMSDK_GET_DATA_FAIL = -1009,
	XMSDK_NOT_IN_PLAYING = -1010,
	XMSDK_CREATE_FILE_FAIL = -1011,
	XMSDK_SET_DATA_FAIL = -1012,
	XMSDK_VERSION_DISMATCH = -1013,
	XMSDK_NOT_SUPPORT_FUNCTION = -1014,
	XMSDK_OVER_SIZE = -1015,
	XMSDK_NEED_REBOOT = -1016,
	XMSDK_INIT_FAIL = -1017,
	XMSDK_ALLOC_FAIL = -1018,
};

enum XMSDK_STREAM_TYPE {
	SDK_CAPTURE_CHN_MAIN = 0,
	SDK_CAPTURE_CHN_2END = 1,
	SDK_CAPTURE_CHN_3IRD = 2,
	SDK_CAPTURE_CHN_4RTH = 3,
	SDK_CAPTURE_CHN_JPEG = 4,
	SDK_CAPTURE_CHN_NR
};

typedef struct {
	unsigned int	version;
	/*SDK打包日期，格式为YYYYMMDD*/
	unsigned int date;            
	/*SDK打包时间，格式为HHMMSS*/
	unsigned int time;
	/*这个标志如果置上，说明SDK在当前版本基础上有修改,是没有记录的临时版本*/
	unsigned char plus;
	unsigned char	reserved[11];
} XMSDK_VERSION_S;

/// FrameType
enum FRAME_TYPE {
	FRAME_VIDEO = 1,
	FRAME_AUDIO = 2,
	FRAME_DATA = 3
};

/// FrameSubType
enum FRAME_SUB_TYPE {
	I_FRAME = 0,
	P_FRAME = 1,
	B_FRAME = 2,
	S_FRAME = 3,
	DATA_TEXT = 5,
	DATA_TNTL = 6
};


/// 码流类型
enum ENCODE_TYPE {
	StreamTypeEmpty = 0,
	StreamTypeH264 = 2,
	StreamTypeJpeg = 3,
	StreamTypeGeneral = 4,
	StreamTypePCM8 = 7,
	StreamTypeStd = 8,
	StreamTypeH265 = 9
};

typedef struct {

	unsigned char *pHeader;				///< buffer header address
	unsigned char *pContent;            ///< frame data address
	unsigned long nLength;              ///< packet len
	unsigned long nFrameLength;         ///< frame len

	unsigned int FrameType;            ///< refer to FRAME_TYPE
	unsigned int nSubType;             ///< refer to FRAME_SUB_TYPE

	unsigned int nEncodeType;		///ENCODE_TYPE

	unsigned long nYear;				///< timestamp
	unsigned long nMonth;
	unsigned long nDay;
	unsigned long nHour;
	unsigned long nMinute;
	unsigned long nSecond;
	unsigned long nMsecond;	
	unsigned long nTimeStamp;	

	unsigned int  nFrameRate;			///< frame rate
	int nWidth;							///< video resolution，width and height
	int nHeight;
	unsigned long nRequence;

	unsigned int nChannels;
	unsigned int nBitsPerSample;		///< audio sample bit depth
	unsigned int nSamplesPerSecond;		///< audio sample rate

	unsigned long nParam1;		// 
	unsigned long nParam2;		// 
	unsigned long version;		// 版本号
	unsigned long reserved[10];	// 扩展用
} XMSDK_VIDEO_FRAME_INFO_S;

enum SDK_CaptureBitrateControlType 
{
	SDK_CAPTURE_BITRATE_CBR,			///< fixed bitrate
	SDK_CAPTURE_BITRATE_VBR,			///< Variable bitrate
	SDK_CAPTURE_BITRATE_NR				///< 
};

enum SDK_CaptureCompressType 
{
	SDK_CAPTURE_COMPRESS_MJPG,			
	SDK_CAPTURE_COMPRESS_H264,			
	SDK_CAPTURE_COMPRESS_H265,			
	SDK_CAPTURE_COMPRESS_NR				
};

typedef struct {
	int		iCompression;			/*refer to SDK_CaptureCompressType*/
	int     iWidth;                 /*编码图像的宽*/
	int     iHeight;                /*编码图像的高*/
	int		iBitRateControl;		/*refer to SDK_CaptureBitrateControlType*/
	int		iQuality;				/*refer to SDK_CaptureImageQuality*/
	int		nFPS;					/*帧率*/
	int		nBitRate;				
	int		iGOP;					/*I帧间隔*/
	int		iQpMax;
	int		iQpMin;
} XMSDK_VIDEO_FORMAT_S;

typedef struct {
	int bEnable[SDK_CAPTURE_CHN_NR];
	XMSDK_VIDEO_FORMAT_S stFormat[SDK_CAPTURE_CHN_NR];
} XMSDK_VIDEOENCODE_S;

/*
 * 如果需要使用其它编码，请都赋值SDK_AUDIO_ENCODE_PCM
 * 调用外额外的接口实现编解码
 */
enum SDK_AudioEncodeTypes
{
	SDK_AUDIO_ENCODE_NONE = 0,
	SDK_AUDIO_ENCODE_PCM,
	SDK_AUDIO_ENCODE_G711_ALAW,
	SDK_AUDIO_ENCODE_G711_ULAW,
	SDK_AUDIO_ENCODE_TYPES_NR
};

enum SDK_AudioSoundModes
{
    SDK_AUDIO_SOUND_MODE_MONO   =0,/*mono*/
    SDK_AUDIO_SOUND_MODE_STEREO =1,/*stereo*/
};

typedef struct {
	/*采样率，Hz为单位，比如8000Hz,44100Hz*/
	int iSampleRate;   
	/*采样的位深 一般设成16*/
	int iSampleBit;    
	/*编码方式，参照SDK_AudioEncodeTypes定义*/
	int iEncodeType;
	/*单双通道*/
	int iSoundmode;
	/*每帧长度*/
	int iPtNumPerFrm;
} XMSDK_AUDIO_FORMAT_S;

typedef struct {
	int bAecOpen;/*是否开启回音抵消*/	
	int bAnrOpen;/*是否开启语音降噪*/
	int bAgcOpen;/*是否开启自动增益*/
	int reserved[5];
} XMSDK_AUDIO_VQE_S;

typedef struct {
	unsigned char gain;/*增益*/
} XMSDK_VOLUMN_S;

typedef struct {
	unsigned long nYear;				///< timestamp
	unsigned long nMonth;
	unsigned long nDay;
	unsigned long nHour;
	unsigned long nMinute;
	unsigned long nSecond;
	unsigned long nMsecond;
} XMSDK_TIMESTAMP_S;


typedef struct {
	//0对应时间标题，1对应通道标题，2和3是其它标题
	int		index;

	/// 是否显示。0-不显示，其他参数忽略，1-显示。
	int		enable;

	/// 标题左上角x坐标，取8的整数倍，采用相对坐标体系(CIF)。
	unsigned short	x;		

	/// 标题左上角y坐标，采用相对坐标体系(CIF)。
	unsigned short	y;

	/// 标题宽度，即像素点的个数，取8的整数倍。
	unsigned short	width;		

	/// 标题高度，即像素点的个数，取8的整数倍。
	unsigned short	height;	

	/// 前景颜色，16进制表示为0xAABBGGRR，A(Alpha)表示透明度。
	/// text color, rgba 8:8:8:8	
	unsigned int	fg_color;

	/// 背景颜色，16进制表示为0xAABBGGRR，A(Alpha)表示透明度。
	/// background color, rgba 8:8:8:8	
	unsigned int	bg_color;

	//颜色格式，如枚举值GraphicsColorFormat描述的
	unsigned int    color_format;

	/// 标题像素颜色值，如果color_format的值为graphicsColorRGB555，
	/// 则需要两个字节来表示一个像素的颜色，所以pixel保存的数据至少
	/// 应该是 (width * height * 2)字节。
	/// 如果pixel是NULL，并且index为0，则表示由底层库自己产生并设置时间标题。
	unsigned char	*pixel;
} XMSDK_OSD_TITLE_PARAM_S;


//网络摄像头参数
//曝光配置
typedef struct {
	int  level;    //曝光等级
	uint leastTime;//自动曝光时间下限或手动曝光时间，单位微秒
	uint mostTime; //自动曝光时间上限，单位微秒
} XMSDK_EXPOSURE_PARAM_S;

//增益配置
typedef struct{
	int gain;    //自动增益上限(自动增益启用)或固定增益值
	int autoGain;//自动增益是否启用，0:不开启  1:开启
} XMSDK_GAIN_PARAM_S;


typedef enum REJECT_FLICKER_FREQ
{
	REJECT_FLICKER_50HZ,    //当前环境无法做时,设置无效
	REJECT_FLICKER_60HZ,    //当前环境无法做时,设置无效
	REJECT_FLICKER_DISABLE,
	REJECT_FLICKER_FORCE_50HZ,  //强制防闪,当前环境无法做时过曝
	REJECT_FLICKER_FORCE_60HZ   //强制防闪,当前环境无法做时过曝
} XMSDK_REJECT_FLICKER_FREQ_E;

typedef enum {
	XMSDK_IRCUT_SYN_INFRARED,
	XMSDK_IRCUT_SWITCH_AUTO,
	XMSDK_INFRARED_ON,
	XMSDK_INFRARED_OFF
} XMSDK_IRCUT_MODE_E;

//网络摄像头参数
typedef struct {
	uint whiteBalance;          //白平衡
	uint dayNightColor;         //日夜模式，取值有彩色、自动切换和黑白(0自动 1彩 2黑)
	int  elecLevel;             //参考电平值
	uint apertureMode;          //自动光圈模式
	uint BLCMode;               //背光补偿模式
	uint BLCLevel;               //背光补偿强度
    XMSDK_EXPOSURE_PARAM_S exposureParam;//曝光配置
	XMSDK_GAIN_PARAM_S     gainParam;    //增益配置
	uint PictureFlip;		//图片上下翻转
	uint PictureMirror;	//图片左右翻转
	XMSDK_REJECT_FLICKER_FREQ_E RejectFlicker;	//日光灯防闪功能
	uint EsShutter;		//电子慢快门功能
	int ircut_mode;		// XMSDK_IRCUT_MODE_E
	int dnc_thr;			//日夜转换阈值
	int ae_sensitivity;	//ae灵敏度配置
	int Day_nfLevel;		//noise filter 等级，0-5,0不滤波，1-5 值越大滤波效果越明显
	int Night_nfLevel;
	int Ircut_swap;		//ircut 正常序= 0        反序= 1
} XMSDK_CAMERA_PARAM_S;

typedef struct {
    int mode;
    int enable;
    int holdTimeSec;
    int AntiShockEnable;
    int AntiShockTimeSec;
} XMSDK_CAMERA_LED_PARAM_S;

typedef struct {
	unsigned short left; 
	unsigned short top;
	unsigned short right;
	unsigned short bottom;
} XMSDK_RECT_S;

typedef struct {
	XMSDK_RECT_S rect;
	unsigned int color;
	unsigned int enable;
} XMSDK_VIDEO_COVER_PARAM_S;

typedef struct {
	unsigned int x;
    unsigned int y;
    int magn;       //0 - 3 放大倍数,对应1-4倍放大(宽高一起放大)
    int fgTrans;    //0 - 100   (0表示完全透明)
    int bgTrans;    //0 - 100   (0表示完全透明)
    int fgColor;    //0xAABBGGRR   AA=00
    int bgColor;    //0xAABBGGRR  AA=00
} XMSDK_LOGO_PARAM_S;


typedef struct {
	int  year;		///< 年。   
	int  month;		///< 月，January = 1, February = 2, and so on.   
	int  day;			///< 日。   
	int  wday;		///< 星期，Sunday = 0, Monday = 1, and so on   
	int  hour;		///< 时。   
	int  minute;		///< 分。   
	int  second;		///< 秒。   
	int  isdst;		///< 夏令时标识。   
} XMSDK_SYSTEM_TIME_S;

typedef struct {
	int left; 
	int top; 
	int fontSize; //0:16  1:24  2:32  3:48  pixel
    int magn;   //0-16 放大倍数,0默认
	int fgTrans; //0 - 100   (0表示完全透明)
	int bgTrans;//0 - 100   (0表示完全透明)
	int fgColor; //0xAABBGGRR   AA=00
	int bgColor; //0xAABBGGRR  AA=00
	char index;		//用于标记要修改哪个OSD，时间标题不用设置这个值，其它标题取值范围1~5
	char res[3];	//
} XMSDK_OSD_PARAM_S; 

enum ALARM_MSG_TYPE {
	ALARM_IO,              /*IO报警, 状态相应位置高,表示相应IO口报警*/
	ALARM_MOTION,          /*动检报警, 状态1报警*/
	ALARM_LOSS,            /*视频丢失, 状态1报警*/
	ALARM_BLIND,           /*遮挡, 状态1报警*/
	ALARM_IRCUT,           /*IR-CUT切换, 状态1表示滤红外，0表示不滤红外*/
	ALARM_UPDATE,          /*单个分区升级了多少数据*/
	ALARM_NUM
};

typedef struct {
	int nChannel;           /*通道号,目前都为0*/
	int nType;              /*消息类型,见ALARM_MSG_TYPE*/
	unsigned long uStatus;            /*状态*/
} XMSDK_ALARM_MSG_S;

/*目前支持的范围7*7个块*/
#define XMSDK_MD_LINE_MAX 7
#define XMSDK_MD_ROWS_MAX 18 

typedef struct {
	/*
	 * 每一个数组成员表示水平一整行的区域,成员在前的，宏块区域在上
	 * 每一数组成员的一个比特位表示一个宏块区域
	 * 比特位由低位到高位，表示的区域是由左到右,比待位置高表示选上，置低表示未选
	 */
	unsigned int win[XMSDK_MD_ROWS_MAX];

	int level; /*灵敏度等级1~6*/

	int enable;

} XMSDK_MD_PARAM_S;

typedef struct {
    unsigned int enable;
    unsigned int rows;
	unsigned int columns;
} XMSDK_MD_CAPS_S;

typedef struct {
    int level;  /*灵敏度等级,0-3级*/
    int enable;
} XMSDK_BD_PARAM_S;

typedef struct {
    int enable;
} XMSDK_BD_CAPS_S;

typedef enum {
    WIFI_UNKONW,
    WIFI_XM711,
    WIFI_XM712,
    WIFI_RTL8188EU,
    WIFI_SS6051,
    WIFI_XM713G,
    NR_WIFI_TYPE
} XMSDK_WIFI_TYPE_E;

typedef enum {
    WIFI_MODE_MANAG,
    WIFI_MODE_MONIT,
} XMSDK_WIFI_MODE_E;

typedef struct {
    char mac[18];
    char hostIP[16];
    char mask[16];
    char gatewayIP[16];
} XMSDK_NET_ATTR_S;

typedef struct {
    char    bssid[18];     // BSSID xx:xx:xx:xx:xx:xx
    char    ssid[128];      // SSID 
    char    passwd[128];    // 密码
    int     channel;        //
    int     signal;         // 0-100
    int     connected;      //标识设备是否连接 (sdk不处理)
    int     hidden;         //标识设备是否隐藏 (暂不支持)
    int     signal_level;   //信号级别 (暂不支持)
    int     noise_level;    //噪声级别，dBm (暂不支持)
    int     open;           //0: 有密码 1无密码 
} XMSDK_WIFI_S;

typedef struct {
    int count;
    XMSDK_WIFI_S *stWifiBuf;
} XMSDK_WIFI_LIST_S;

typedef struct {
    char ssid[128];
    char passwd[128];
    int channel;
    char hostIP[256];
} XMSDK_AP_S;

typedef enum {
    SD_INEXST = 1,		//SD卡不存在
    SD_ABNORMAL,		//SD卡不正常
    SD_NORMAL,			//SD卡正常
    SD_NEEDFORMAT,		//SD卡需要格式化
    SD_NOTRW,			//SD卡非读写
	SD_NEEDCHECKSPEED,  //SD卡需要测速
	SD_NOSPACE,			//SD卡空间不足
	SD_NO_C10,			//不是class10
} XMSDK_SD_STATUS_E;

typedef enum {
    SDFormat_Sucess = 0,	//SD卡格式化正常
	SDFormat_NOEXST,		//SD卡不存在
    SDFormat_Unmount_Error,	//SD卡卸载失败
    SDFormat_Format_Error,	//SD卡格式化失败
    SDFormat_Mount_Error,	//SD卡挂载失败
} XMSDK_SDFORMAT_STATUS_E;

typedef struct {
    char MountPath[128];	/*SD卡要挂载的路径*/
    char DevName[32];		/*格式化后SD卡的名字,暂只支持fat且8字符长度*/
} XMSDK_SD_INIT_PARAM_S;

typedef struct {
    char DevName[128];          //dev名字
    unsigned long capacity;     //总容量 单位: K bytes
    unsigned long remain;       //剩余容量 
} XMSDK_SD_STORAGE_S;

typedef enum {
    UDISK_INEXST = 1,  //UDISK卡不存在
    UDISK_ABNORMAL,    //UDISK卡不正常
    UDISK_NORMAL       //UDISK卡正常
} XMSDK_UDISK_STATUS_E;

typedef struct {
    char MountPath[128];	/*SD卡要挂载的路径*/
    char DevName[32];		/*格式化后SD卡的名字,暂只支持fat且8字符长度*/
} XMSDK_UDISK_INIT_PARAM_S;

typedef struct {
    char DevName[128];          //dev名字
    unsigned long capacity;     //总容量 单位: K bytes
    unsigned long remain;       //剩余容量 
} XMSDK_UDISK_STORAGE_S;

typedef enum {
    XMSDK_LED_RL,
    XMSDK_LED_RLAUTO,
    XMSDK_LED_WL,
    XMSDK_LED_WLAUTO,
    XMSDK_LED_SMART,
} XMSDK_CAMERA_LED_MODE_E;

typedef struct {
     unsigned int ResultType;  /*结果类型*/
     unsigned int AlarmFlag; /*报警标志*/
     unsigned int TargetNum; /*目标数(人形/车牌) */
} XMSDK_AI_RESULT_S;
   
typedef struct {
    unsigned int id;
    unsigned int width;
    unsigned int height;
    unsigned int PhyAddr;
    char *pVirAddr;
    unsigned int length;
    unsigned long long pts;
} XMSDK_YUV_FRAME_S;

typedef enum {
    _4G_STATE_CONNECTED,		/*4G模块联网成功*/
    _4G_STATE_CONNECT_FAILED, 	/*4G模块联网失败*/
    _4G_STATE_ABNORMAL,			/*4G模块异常*/
    _4G_STATE_UNEXIST,			/*4G模块未识别*/
    _4G_STATE_NO_SIMCARD,		/*4G模块没SIM卡*/
} XMSDK_4G_STATE_TYPE_E;

typedef enum {
    CMD_4G_GET_CARD_ID,			/*获取卡号*/
    CMD_4G_GET_HOST_IP,			/*获取IP地址*/
    CMD_4G_GET_SIGNAL_STRENGTH,	/*获取信号强度, level 0-3*/
    CMD_4G_GET_REGISTER_STATUS,	/*获取注册状态,注册1，没注册0*/
    CMD_4G_GET_NETWORK_TYPE,	/*获取运营商类型， 2/3/4G*/
    MAX_4G_PARAM_TYPE_NR
} XMSDK_4G_CMD_TYPE_E;

typedef struct {
	unsigned int level;         /*占空比 0~100*/
	unsigned int freq;          /*频率 2*12000000/65536 ~ 12000000/100*/
	unsigned int resv[2];
} XMSDK_PWM_PARAM_S;

typedef struct {
	unsigned char	brightness;		/*亮度，取值0-100*/
	unsigned char	contrast;		/*对比度，取值0-100*/
	unsigned char 	saturation;		/*饱和度，取值0-100*/
	unsigned char 	hue;			/*色调，取值0-100*/
	unsigned char 	gain;			/*增益，取值0-100。bit7置位表示自动增益，其他位被忽略*/
	unsigned char	WhiteBalance;	/*自动白电平控制，bit7置位表示开启自动控制.0x0,0x1,0x2分别代表低,中,高等级*/
	unsigned short	acutance;       /*锐度，取值0-15, 第8位表示水平锐度，高8为表示垂直锐度*/
} XMSDK_PICTURE_COLOR_S;

typedef struct {
    char FileName[128];
    unsigned int size;
} XMSDK_LOG_PARAM_S;


typedef enum {
    HOTPLUG_EVENT_NONE,
    HOTPLUG_EVENT_SDIO,         //SDIO事件
    HOTPLUG_EVENT_USB,          //USB事件
} XMSDK_HOTPLUG_EVENT_TYPE_E;

typedef enum {
    HOTPLUG_DEVICE_NONE,
    HOTPLUG_DEVICE_SDCARD,         //SD卡,需要硬件支持
    HOTPLUG_DEVICE_NIC,            //网卡
} XMSDK_HOTPLUG_DEVICE_TYPE_E;

typedef struct {
    int EventType;           /*时间类型，参考: XMSDK_HOTPLUG_EVENT_TYPE_E*/
    int DevType;            /*设备类型，参考: XMSDK_HOTPLUG_DEVICE_TYPE_E*/
    int status;             /*插拔状态, 0: 热拔  1: add 热插*/
    char DevName[32];       /*设备名, 如：mmcblock0/eth2等*/
} XMSDK_HOTPLUG_EVENT_PARAM_S;

typedef enum {
    FILESYSTEM_SD_INEXST,  //SD卡不存在
	FILESYSTEM_UDISK_INEXST, //UDisk不存在
    FILESYSTEM_UNKOWN,  //未知类型
    FILESYSTEM_VFAT,    //fat/fat32
    FILESYSTEM_EXFAT, 
    FILESYSTEM_EXT4,  
    FILESYSTEM_NTFS,  
} XMSD_FILESYSTEM_TYPE_E;

typedef enum {
    SAFESYSTEM_RUN_FAILED = -1,
    SAFESYSTEM_CMD_SUCCESS = 0,
    SAFESYSTEM_CMD_FAILED
} XMSDK_SAFESYSTEM_STATUS_E;

typedef void *hMemory;

/*******************************************************************
 *  回调接口
 *	功能: 分配内存
 * 	传参:[out]phandle 内存句柄
 * 	     [out]buffer [in]size
 * 	     [in]boundary 非0时和memalign接口的boundary一样
 * 	返回：0 成功
 * 		  <0 失败
 *******************************************************************/
typedef int (*fXMSDKMemoryAlloc) (hMemory *phandle, unsigned char **buffer, unsigned int size, unsigned int boundary);


/*******************************************************************
 *  回调接口
 *	功能: 释放内存或减少内存引用计数
 * 	传参:[in]handle 内存句柄
 * 	返回：0 成功
 * 		  <0 失败
 *******************************************************************/
typedef int (*fXMSDKMemoryRelease) (hMemory handle);


/*******************************************************************
 *  回调接口
 *	功能: 增加内存引用计数
 * 	传参:[in]handle 内存句柄
 * 	返回：0 成功
 * 		  <0 失败
 *******************************************************************/
typedef int (*fXMSDKMemoryAddRef) (hMemory handle);


/*******************************************************************
 *  回调接口
 *	功能: 设置内存使用了的长度
 * 	传参:[in]handle 内存句柄
 * 	     [in]len 
 * 	返回：0 成功
 * 		  <0 失败
 *******************************************************************/
typedef int (*fXMSDKMemorySetLength) (hMemory handle, unsigned int len);

typedef struct {
	fXMSDKMemoryAlloc  fMemoryAlloc;
	fXMSDKMemoryRelease fMemoryRelease;
	fXMSDKMemoryAddRef fMemoryAddRef;
	fXMSDKMemorySetLength fMemorySetLength;
} XMSDK_MEMORY_API_S;


/*******************************************************************
 *  回调接口
 *	功能: 注册后，获取到H264/H265数据就会回调该接口
 * 	传参: [in] nChannel
 *        [in] nStream 参考XMSDK_STREAM_TYPE
 *        [in] mhandle
 * 	返回：	0 成功
 * 			<0 失败
 *******************************************************************/
typedef int(*fRealDataCallBack) (int nChannel, int nStream, XMSDK_VIDEO_FRAME_INFO_S *pFrameInfo, hMemory mhandle);


/*******************************************************************
 *  回调接口
 *	功能: 注册后，有相应的报警就会回调该接口
 * 	传参:[in] pMsg 参考XMSDK_ALARM_MSG
 * 	     [in] dwUser 用户自定义数据
 * 	返回：	0 成功
 * 			<0 失败
 *******************************************************************/
typedef int(*fAbnormalCallBack) (XMSDK_ALARM_MSG_S *pMsg, unsigned long dwUser);


/*******************************************************************
 *  回调接口
 *	功能: 注册后，有音频输入就会回调该接口
 * 	传参:[in] mhandle 
 * 	     [in] pTime 参考XMSDK_TIMESTAMP
 * 	返回：	0 成功
 * 			<0 失败
 *******************************************************************/
typedef int(*fAudioInCallBack) (hMemory mhandle, XMSDK_TIMESTAMP_S *pTime);


/*******************************************************************
 *  回调接口
 *	功能: 注册后，获取到YUV数据就会回调该接口
 * 	传参:[in] mhandle
 * 	     [in] dwUser 用户自定义数据
 * 	返回：	0 成功
 * 			<0 失败
 *******************************************************************/
typedef int(*fYUVCallBack) (hMemory mhandle, unsigned long dwUser);

/*******************************************************************
 *  回调接口
 *	功能: 注册后，智能警戒模式会调用该接口
 * 	传参:[in] pstAIRsult 结果属性 
 * 	返回：	0 成功
 * 			<0 失败
 *******************************************************************/
typedef int(*fGetAIResultCallBack) (XMSDK_AI_RESULT_S *pstAIRsult);

/*******************************************************************
 *  回调接口
 *	功能: 注册后，有音频数据输入时会回调该接口
 * 	传参:[in] buf 数据
 * 	     [out] len 数据长度
 * 	返回：	0 成功
 * 			<0 失败
 *******************************************************************/
typedef void(*fGetAudioInDataCallBack) (unsigned char *buf, unsigned int len);

/*******************************************************************
 *  回调接口
 *	功能: 注册后，触发热插拔事件会调用该接口
 * 	传参:[int] pstHotplug 热插拔属性
 * 	返回： 无
 *******************************************************************/
typedef void(*fHotplugEventCallBack) (XMSDK_HOTPLUG_EVENT_PARAM_S *pstHotplug);
#endif

