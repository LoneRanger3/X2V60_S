#include "PeripheryThread.h"
#include "power.h"
#include "Log.h"
#include "TimeUtil.h"
#include "io.h"
#include "i2c.h"
#include <unistd.h>
#include <sys/reboot.h>
#include <sys/prctl.h>
#include "adc.h"
#include "pwm.h"
#include "car_state_detect.h"

#define TEST 0

extern bool lcd_check_state;
void IO_Init() {
	writel(ACC_DET, LOW_POWER);
	writel(USB_DET, LOW_POWER);
	writel(ACR_BACK_DETECT, LOW_POWER);
	writel(DA380_GSENSOR_INT1, LOW_POWER);//配置io口为输入模式
	writel(KEY_ON, LOW_POWER);
	writel(AD_IN_DETECT, LOW_POWER);
	writel(PWM_LCD, PWM_LCD_INIT);
	writel(SPEAK_EN, 0xc00);
	writel(GPS_POWER_EN, 0xc00);

    //各种指示灯开机先默认关闭状态
	WiFi_LED_OFF; 
	RED_LED_ON;
	GREEN_LED_OFF;
	GPS_RED_LED_OFF;
	GPS_GREEN_LED_OFF;
}

int periphery_hander_thread() {
	prctl(PR_SET_NAME, "periphery_thread");
	/* int ret_wdt;
	key_init();
	unsigned int adc_signal_value = 0,usb_signal_value=0,keyon_signal_value=0,gsensor_signal_value=0;
	unsigned int acc_signal_value = 0;
	//GPIO初始化
	lcd_check_state = 1; */

	//初始化加速度传感器
	/*if(da380_i2c_read(DEVICE_ADDR, INT_SET1)== INT_SET1_DEFAULT)
	InitSensor();*/

	//主控CPU电、摄像头、背光打开 
	//考虑放在主函数里

	//CAM_ON;
	//PWM_LCD_ON;
    IO_Init();
	int64_t key_shutdown_time = 0; //长按关机时间
	SensitivityDegreeSet(1);//打开灵敏度
	/* writel(0x10020CC0, 0x93);//adc改成内部参考电压
	int64_t usb_disconnect_time = 0; //拔出电源时间
	int64_t last_adc_show_time = 0;
	readl(USER_REG_ADC, &adc_signal_value);
	readl(USER_REG_USB, &usb_signal_value);
	readl(USER_REG_KEYON, &keyon_signal_value);
	readl(USER_REG_GSENSOR, &gsensor_signal_value);
	readl(ACC_SIGNAL, &acc_signal_value);
	XMLogI("adc=%d,usb=%#x,keyon=%#x,gsensor=%#x,acc=%#x\n", adc_signal_value,\
		usb_signal_value, keyon_signal_value, gsensor_signal_value, acc_signal_value); */
    key_init();
	while (true)
	 {
		usleep(5000);

       //按键关机检测
		int ret = PressKeyOpenClose();
		if (ret != 0) {
			XMLogI("press shutdown key, time=%lld", GetTickTime());
			key_shutdown_time = GetTickTime();
			continue;
		}

        PressKeyQuery();

		//WIFI按键
		//WifiKey();

		//碰撞检测、停车检测、倒车检测
		CarStateDetect();
		//汽车打火和熄火检测
		AccDetect();
#ifdef ENABLE_OTHER_FUN
		LcdStatus(lcd_check_state);

		if (key_shutdown_time > 0 || usb_disconnect_time > 0) {
			// int64_t now = GetTickTime();
			// //长按关机键后，主线程超过10秒没来关机，就强制关机。防止主线程卡死的情况
			// if (key_shutdown_time > 0 && now > key_shutdown_time + 10000) {
			// 	XMLogE("force shutdown!!!, should not happen, maybe the main thread dead lock, time=%lld", now);
			// 	//shutDown(ShutDownMode_Key);
			// 	reboot(RB_AUTOBOOT);
			// }
			usleep(5000);
			continue;
		}

		

		//低电量关机检测
		ret = LowBatteryClose();
		if (ret != 0) {
			XMLogI("low battery:exit periphery_hander_thread.");
			continue;
		}

		//USB断开检测
		ret = UsbDetect();
		if (ret < 0) {
			XMLogI("USB disconnect, time=%lld", GetTickTime());
			usb_disconnect_time = GetTickTime();
			continue;
		}
		
		//ADC按键采集
		PressKeyQuery();

		//碰撞检测、停车检测、倒车检测
		CarStateDetect();

		//打印adc值
		int64_t now = GetTickTime();
		if (now > last_adc_show_time + 1000) {
			//XMLogI("adc battery=%d", adc_battery());
			last_adc_show_time = now;	
		}	
#endif
}
	return 0;
}

void PeripheryThreadStart()
{
	OS_THREAD periphery;
	CreateThreadEx(periphery, (LPTHREAD_START_ROUTINE)periphery_hander_thread, NULL);
}

