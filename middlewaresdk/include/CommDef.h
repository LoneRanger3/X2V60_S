#pragma once
#include <map>
#include "xm_middleware_def.h"
#include "xm_ia_comm.h"
#define MAX_NAL_NUM		10//最大nal单元数
#define MAX_PNG_FILE_LEN 102400 //缩略图最大100KB
#define MAX_USER_DATA_LEN 1024 //gps轨迹信息最大长度
//文件系统返回值定义
#define VFS_RESULT				    int32_t
#define VFS_RESULT_SUCCESS			0
#define VFS_RESULT_UNKNOWN			0xFFFFFFFF
#define VFS_RESULT_FILE_TOO_LARGE	1
#define VFS_RESULT_FILE_WRITE_ERROR	3

extern const char* kSepartorStr;
extern const int kMaxRecordFileLen;
extern const int64_t kMaxRecordFileLenInByte;
extern const int64_t kReserveLen;
extern const int kSpaceReserved;
extern const int kPictureSpace;
extern const int kMaxFrameLen;
extern const int kAiChn;
extern const char* kClassifierPath;
extern const char* kAdasCfgPath;
extern const XM_IA_PLATFORM_E kIAPlatform;
extern const int kViChnMap[];
extern const int kMaxPicWidth;
extern const int kMaxPicHeight;
extern const int kVodDecDev;
extern const int kPicDecDev;
extern const int kFrameRate;
extern const int kFileMinLen;
extern const char* kDevNodeName1;
extern const char* kDevNodeName2;

extern XM_MW_LOG_CallBack g_log_cb;
extern int64_t g_log_user;

struct FragmentationHeader {
	int nal_begin_pos[MAX_NAL_NUM];
	int nal_len[MAX_NAL_NUM];
	int nal_count;
	int sps_index;
	int pps_index;
	int vps_index;
};

typedef std::map<int64_t, XM_MW_Record_Info> TimeRecordInfoMap;
typedef std::multimap<int64_t, XM_MW_Record_Info> TimeRecordInfoMultiMap;