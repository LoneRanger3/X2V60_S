#ifndef PERIPHERY_MANAGER_H
#define PERIPHERY_MANAGER_H

#include "RegCtrl.h"
#include "xm_middleware_def.h"
class PeripheryManager
{
private:
    /* data */
public:
    static PeripheryManager* Instance();
	static void Uninstance();

    int Periphery();
	int GsensorCheck();
	int SensitivitySet(int degree);
	int CarStopSensitivitySet(int degree);
	int CameraLedControl(bool led_state);
	int GetAdcBattery();
	int GsensorModeSelect(int mode);

	//获取实体按键值
	int GetKeyValue();
    //控制屏幕亮度
	int SetLight(int value);
    //关机
	int Shutdown(ShutDownMode shutdownmode);

	//喇叭使能
	int SpeakerEnable(bool enable);
	//解码器使能
	int DecodeEnable(int enable);
	
private:
	RegCtrl reg_ctrl_;
	
    PeripheryManager(/* args */);
    ~PeripheryManager();
    static PeripheryManager* instance_;
};

#endif
