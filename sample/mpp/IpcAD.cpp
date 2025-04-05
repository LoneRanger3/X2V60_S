#ifndef WIN32
#include "IpcAD.h"
#include <string.h>
#include <unistd.h>
#include "xm_common.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vo.h"
#include "mpi_vpss.h"
#include "mpi_vi.h"
#include "mpi_var.h"
#include "mpi_camera.h"
#include "xm_comm_vo.h"
#include "xm_comm_vi.h"
#include "fvideo.h"
#include "adr.h"
#include "Log.h"
#include "RegCtrl.h"

extern "C"
{
#include "mpi_ispapp.h"    
}

extern "C" void Fvideo_SetInit(void);
extern "C" void Fvideo_ChannelSetting(unsigned char channel, int video_mode, int fps);

IpcAD::IpcAD()
{

}

IpcAD::~IpcAD()
{

}

typedef enum {
  SENSOR_CHIP_UNKNOW  =  0,
  SENSOR_CHIP_SC1330T =  91,
  SENSOR_CHIP_H42   =  92,
  SENSOR_CHIP_H66   =  93,
  SENSOR_CHIP_SC035GS  =  94,
  SENSOR_CHIP_SC120AT  =  95,
  SENSOR_CHIP_SC1035  =  96, 
  SENSOR_CHIP_SC1037  =  97,
  SENSOR_CHIP_H81   =  98,
  SENSOR_CHIP_H65   =  99,
  
  SENSOR_CHIP_SP140A  =  81,
  SENSOR_CHIP_H62   =  82,
  SENSOR_CHIP_BG0703  =  83,
  SENSOR_CHIP_SC1145  =  84,
  SENSOR_CHIP_SC1135  =  85,
  SENSOR_CHIP_OV9732  =  86,
  SENSOR_CHIP_OV9750  =  87,
  SENSOR_CHIP_SP1409  =  88,
  SENSOR_CHIP_MIS1002 =  89,
  
  SENSOR_CHIP_SC2045  =  21,
  SENSOR_CHIP_IMX222  =  22,
  SENSOR_CHIP_IMX322  =  23,
  SENSOR_CHIP_AR0237_IR  =  24,
  SENSOR_CHIP_SC2035  =  25,
  SENSOR_CHIP_F02   =  26,
  SENSOR_CHIP_AR0237  =  27,
  SENSOR_CHIP_IMX323  =  28,
  SENSOR_CHIP_PS5220  =  29,
  
  SENSOR_CHIP_SC2335  =  30,
  SENSOR_CHIP_H63   =  31,
  SENSOR_CHIP_SC1346  =  36,  //sc1345h
  SENSOR_CHIP_SC133GS  =  37,
  SENSOR_CHIP_SC1345  =  38,  
  SENSOR_CHIP_SC1335T =  39,
  SENSOR_CHIP_SC1336  =  40,
  
  SENSOR_CHIP_SC307C  =  70, //对应SC2232
  SENSOR_CHIP_SC2135  =  71,
  SENSOR_CHIP_F22   =  72,
  SENSOR_CHIP_BG0803  =  73,
  SENSOR_CHIP_PS5230  =  74,
  SENSOR_CHIP_PS3210  =  75,
  SENSOR_CHIP_GC2023  =  76,
  SENSOR_CHIP_SC2145  =  77,
  SENSOR_CHIP_SC200AI =  78,
  SENSOR_CHIP_SC2310  =  79,

  SENSOR_CHIP_SC223A  =  41, 
  SENSOR_CHIP_GC3003 =  42,
  SENSOR_CHIP_OV2718  =  48,
  SENSOR_CHIP_SC2239  =  49,
  
  SENSOR_CHIP_SC1235  =  50,
  SENSOR_CHIP_BF3016  =  51,
  SENSOR_CHIP_IMX307  =  52,
  SENSOR_CHIP_SC2235E =  53, 
  SENSOR_CHIP_307H  =  54,
  SENSOR_CHIP_F37   =  55,
  SENSOR_CHIP_SP2305  =  56,
  SENSOR_CHIP_F37P    =   57,
  SENSOR_CHIP_SC307P  =  58, //对应SC2300
  SENSOR_CHIP_MIS2006 =  59, 
  
  SENSOR_CHIP_BG0806  =  60,
  SENSOR_CHIP_IMX291  =  61,
  SENSOR_CHIP_PS5250  =  62,
  SENSOR_CHIP_SC2235  =  63,
  SENSOR_CHIP_SC2145H =  64,
  SENSOR_CHIP_GC2033  =  65,
  SENSOR_CHIP_F28   =  66,
  SENSOR_CHIP_SC2235P =  67,
  SENSOR_CHIP_MIS2003 =  68,//对应MIS2236
  SENSOR_CHIP_MIS2236 =  68,
  SENSOR_CHIP_SC307E  =  69,
  SENSOR_CHIP_SC2336  =  102,
  SENSOR_CHIP_AR0330  =  130,
  SENSOR_CHIP_SC3035  =  131,
  SENSOR_CHIP_SC3235  =  132, //530平台为SENSOR_CHIP_AUGE
  SENSOR_CHIP_Q03   =  133,
  SENSOR_CHIP_SC3335  =  135,
  SENSOR_CHIP_SC3338  =  136,
  SENSOR_CHIP_OV4689  =  140,
  SENSOR_CHIP_SC4236  =  141,
  SENSOR_CHIP_K02   =  142,
  SENSOR_CHIP_IMX347  =  143,
  SENSOR_CHIP_PS5510  =  150,
  SENSOR_CHIP_K03   =  151,
  SENSOR_CHIP_SC5035  =  152,
  SENSOR_CHIP_SC5235  =  153,
  SENSOR_CHIP_SC335E  =  154,
  SENSOR_CHIP_IMX335  =  155,
  SENSOR_CHIP_SC5239  =  156,
  SENSOR_CHIP_Doris  =  157,
  SENSOR_CHIP_K05   =  158,
  SENSOR_CHIP_SC5332  =  159,
  SENSOR_CHIP_SC5335P   =  159,
  SENSOR_CHIP_MIS5001   =  160,
  SENSOR_CHIP_SC500AI   =  161,
  SENSOR_CHIP_SC401AI   =  162,
  SENSOR_CHIP_SC4210    = 163,
  SENSOR_CHIP_SC4336     =  164,
  SENSOR_CHIP_OV12895   =  180,
  SENSOR_CHIP_APOLLO  =  181,
  SENSOR_CHIP_IMX415  =  182,
}SENSOR_CHIP;

int IpcAD::Init(int channel_num)
{
    //isp
    ISPAPP_TASK_CFG stIspTaskCfg;
	memset(&stIspTaskCfg, 0, sizeof(ISPAPP_TASK_CFG));
	stIspTaskCfg.u32FunExt = 0x60;
	//stIspTaskCfg.u32Fps = (30<<0)|(0<<8)|(0<<16)|(0<<24);
    stIspTaskCfg.au32IrCut[0] = (0xff<<0) | (1<<31);
	stIspTaskCfg.u32StdType = 1;  
    stIspTaskCfg.u32RsltType = 6;
    //stIspTaskCfg.u32SensorType = SENSOR_CHIP_SC401AI;
    XMLogI("before XM_MPI_ISPAPP_Run");
	int ret = XM_MPI_ISPAPP_Run(stIspTaskCfg);
    if (ret != 0) {
        XMLogE("XM_MPI_ISPAPP_Run error, ret=%d", ret);
    }  
    XMLogI("after XM_MPI_ISPAPP_Run");
    RegCtrl reg_ctrl;
    // while(reg_ctrl.ReadL(0x30001000) != 1) {
    //     usleep(10*1000);
    // }
    reg_ctrl.WriteL(5, 0x30001000);
	/* 显示彩色 */
	CAM_DEV CamDev0 = 0;
	CAM_DNC_MODE_E enMode = DNC_MODE_MULTICOLOR; 	/* 彩色 */
	XM_MPI_CAM_SetDncMode(CamDev0, enMode);
    XMLogI("before Fvideo_SetInit");
    Fvideo_SetInit();
    XMLogI("before Fvideo_ChannelSetting");
	Fvideo_ChannelSetting(0, AHD_1080, FPS30);
    XMLogI("after Fvideo_ChannelSetting");
    return 0;
}

int IpcAD::Start()
{
    return 0;
}

int IpcAD::Stop()
{
    return 0;
}

int IpcAD::Clean()
{
    return 0;
}

int IpcAD::GetCapSolution(int channel, int& width, int& height) 
{
    return 0;
}
#endif