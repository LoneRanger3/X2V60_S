#include "global_data.h"
#include "CommDef.h"
const char* kTFCardPath = "/mnt/tfcard";
const char* kUDiskPath = "/mnt/usb";
const char* kAudioPath = "/mnt/custom/Audio/";
const char* kUpdateFileName = "/home/netall_XM650V200_CAR_X2V60.bin";
const char* kUSBDeviceUpdateFile = "/home/netall_XM530V200_USB_ETHER.bin";
const char* kUpdateMD5Name = "/mnt/mtd/Config/updateMD5";
const char* kUSBDeviceUpdateMD5File = "/mnt/mtd/Config/USBDEVUpdateMD5";
const char* kUSBDeviceIP = "192.168.2.10";
const char* kSysTime = "/mnt/mtd/Config/sys_time";

const int kFrameRate = 25;
const int k3KHeight = 1800;
const int k4KHeight = 2144;
extern "C"
{
	int LcdInit(void);
	int LcdSetBacklight(int level);
}

GlobalData* GlobalData::instance_ = 0;
GlobalData* GlobalData::Instance()
{
    if (0 == instance_) {
		instance_ = new GlobalData();
	}
	return instance_;
}

void GlobalData::Uninstance()
{
    if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

GlobalData::GlobalData() : g_pImgVirAttr(NULL), version_(""), SDCard_write_speed_(0), 
	upgrading_(false), recording_(false), app_page_(APP_PAGE_PREVIEW),
	stop_screen_refresh_(false), gdb_debug_(false)
{
}

GlobalData::~GlobalData()
{
}

int GlobalData::LcdLightInit()
{
#ifdef BOARD8520DV200
	LcdInit();
#endif
	return 0;
}

int GlobalData::SetLcdLight(int level)
{
#ifdef BOARD8520DV200
	LcdSetBacklight(level);
#endif
	return 0;
}


int GlobalData::SetCurTime(int64_t sys_time)
{
	XMLogI("sys_time = %lld", sys_time);
    FILE* fp = fopen(kSysTime, "w+");
	if (!fp) {
		XMLogE("create %s error!", kSysTime);
		return -1;
	}

	char buf[32] = {};
	sprintf(buf, "%lld\n", sys_time);
	fputs(buf, fp);
    if (fp) {
        fclose(fp);
    }

    return 0;
}

int64_t GlobalData::GetCurTime()
{
	XMLogI("GetCurTime");
	FILE* fp = fopen(kSysTime, "r");
	if (!fp) {
		XMLogE("open %s error!", kSysTime);
		return -1;
	}

	char buf[32] = {};
	fgets(buf, 32 , fp);
    if (fp) {
        fclose(fp);
    }

	return atoll(buf);
}