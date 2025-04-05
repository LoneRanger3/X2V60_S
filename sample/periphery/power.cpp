#include <unistd.h>
#include <sys/reboot.h>
#include "power.h"
#include "PeripheryThread.h"
#include "TimeUtil.h"
#include "adc_presskey.h"
#include "Log.h"
#include "io.h"
#include "pwm.h"
#include "xm_middleware_api.h"


#define POWER_TEST 0
static int pwm_lcd_open_or_close_flag = 0;
bool AccDisconnectFlag = false;
bool lcd_check_state=0;
static bool g_set_lcd = 0;
int PressKeyOpenClose() {
	static unsigned int power_off_state;
	static bool power_off_flag = false;
	static int64_t last_time = 0;
	static int64_t new_time = 0;
	static int64_t press_time = 0;
	static bool press_pull_flag = false;
	static bool first_detect_high_flag = false;
	static bool long_presskey_exit = false;
	static int64_t last_time_log = 0;
	writel(KEY_ON, LOW_POWER);

	if (readl(KEY_ON, &power_off_state))
	{
		XMLogE("Read fail");
	}
	if (!last_time_log)
	{
		last_time_log = GetTickTime();
	}
	if ((GetTickTime() - last_time_log) > 2000 && last_time_log)
	{
		last_time_log = 0;
	}
	//XMLogI("power_off_state=%#x\n", power_off_state);
	if (power_off_state == HIGH_POWER) {
		usleep(2000);
		writel(KEY_ON, LOW_POWER);
		readl(KEY_ON, &power_off_state);
		if (power_off_state == HIGH_POWER) {
			if (!first_detect_high_flag) {
				last_time = GetTickTime();
				first_detect_high_flag = true;
			}
			new_time = GetTickTime();
			power_off_flag = true;
		}
	}
	else{
		if (power_off_flag == true) {
			press_time = GetTickTime() - last_time;
			power_off_flag = false;
			press_pull_flag = true;
			first_detect_high_flag = false;
			last_time = 0;
			new_time = 0;
		}
	}
	if (press_pull_flag&&press_time > CLOSE_TIME_MIN && press_time < CLOSE_TIME_MAX) {
	#if 0	
		if (!pwm_lcd_open_or_close_flag) {
			XMLogI("1:short press to close LCD");
			pwm_lcd_open_or_close_flag = 1;
			press_pull_flag = false;
			XM_Middleware_Periphery_Notify(XM_EVENT_POWERKEY_SHORTPRESS, "", 0);
			//PWM_LCD_OFF;
		}
		else  if (pwm_lcd_open_or_close_flag) {
			XMLogI("1:short press to open LCD");
			pwm_lcd_open_or_close_flag = 0;
			press_pull_flag = false;
			XM_Middleware_Periphery_Notify(XM_EVENT_POWERKEY_SHORTPRESS, "", 0);
			//PWM_LCD_ON;
		}
	 #else	
	 SetKeyValue(KEYMAP_OK);
	 #endif	
		press_time = 0;
	}
	else if ((new_time-last_time)> CLOSE_TIME_MAX) {
		XMLogI("long press to shut down power");
		long_presskey_exit = true;
		press_time = 0;
		new_time = 0;
		last_time = 0;
		first_detect_high_flag = false;
		power_off_flag = false;
		//POWER_OFF;
		XM_Middleware_Periphery_Notify(XM_EVENT_KEY_SHUTDOWN, "", get_adc_val(ADC1_BATTERY)); // XM_EVENT_POWERKEY_LONGPRESS
		return -1;
		//usleep(3000000);
	}
	return 0;
}

int PressKeyStateAcquire()
{
	unsigned int key_current_state = 0;
	writel(KEY_ON, LOW_POWER);
	if (readl(KEY_ON, &key_current_state))
	{
		XMLogE("wifi key read failed\n");
		return -1;
	}
	if (key_current_state == HIGH_POWER)
	{
		usleep(5);//消抖处理
		writel(KEY_ON, LOW_POWER);
		if (readl(KEY_ON, &key_current_state))
		{
			XMLogE("wifi key read failed\n");
			return -1;
		}
		if (key_current_state == HIGH_POWER)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

int WifiKey()
{
	static int wifi_key_current_state=0;
	static int  wifi_key_last_state = 0;
	static int64_t wifi_key_audio_last_time = 0;
	static int64_t wifi_key_reset_last_time = 0;
	static int64_t tap_time = 0;
	static int press_count = 0;

	if((wifi_key_current_state = PressKeyStateAcquire())==-1)
		return -1;
	if (wifi_key_current_state != wifi_key_last_state)
	{
		if (wifi_key_current_state)//按键按下
		{
			wifi_key_audio_last_time = GetTickTime();
			wifi_key_reset_last_time = GetTickTime();
			tap_time = GetTickTime();
			press_count++;
		}
		else//按键松开
		{

		}
		wifi_key_last_state = wifi_key_current_state;
	}
	if (wifi_key_current_state)//长按不松开检测
	{
		int long_press_reset_time = GetTickTime() - wifi_key_reset_last_time;
		if (long_press_reset_time > 5000 && wifi_key_reset_last_time) //长按大于5s
		{
			tap_time = 0;//防止短按触发
			wifi_key_reset_last_time = 0;
			press_count = 0;
			XMLogI("long press key 5s");

			XM_Middleware_Periphery_Notify(XM_EVENT_KEY_REBOOT, "", 0);
		}
	}
	else //松开检测
	{
		//长按松开检测
		int long_press_audio_time = GetTickTime() - wifi_key_audio_last_time;
		if (long_press_audio_time >= 2000 && long_press_audio_time < 5000 && wifi_key_audio_last_time)//长按大于2s小于5s时松开
		{
			tap_time = 0;//防止短按触发
			wifi_key_audio_last_time = 0;
			press_count = 0;
			XMLogI("long press 2s");
		  	XM_Middleware_Periphery_Notify(XM_EVENT_POWERKEY_LONGPRESS, "", 0);
		}
		
		//短按松开检测
		int tap_time_interval = GetTickTime() - tap_time;
		if (tap_time_interval > 50*10&& tap_time)//按键延迟反应
		{
			if (press_count == 1)//单击
			{
				XMLogI("single tap presskey");
				//处理短按事件
				if (!pwm_lcd_open_or_close_flag) //屏幕亮时
				{
					XMLogI("short press key lock video");
					XM_Middleware_Periphery_Notify(XM_EVENT_POWERKEY_SHORTPRESS, "", 0);
				}
				else //屏幕灭时
				{
					XMLogI("short press key open lcd");
					XM_Middleware_Periphery_Notify(XM_EVENT_POWERKEY_SHORTPRESS, "", 0);
					pwm_lcd_open_or_close_flag = 0;
				}
			}
			if (press_count == 2)//双击
			{
				XMLogI("double tap presskey shut record audio down");
				// XM_Middleware_Periphery_Notify(XM_EVENT_RECORD_SOUND_ENABLE, "", 0);
			}
			tap_time = 0;
			press_count = 0;
			wifi_key_audio_last_time = 0;
		}
	}
	return 0;
}

int LowBatteryClose() {
	static int current_battery_num;
	static bool usb_disconnect = false;
	static int low_battery_times = 0;
	static unsigned int usb_connect_det_battery = 0;
	static bool usb_state_battery = false;
	static bool low_battery_exit = false;
	readl(USB_DET, &usb_connect_det_battery);
	if (usb_connect_det_battery == LOW_POWER&& !low_battery_exit) {
		usleep(2000);
		readl(USB_DET, &usb_connect_det_battery);
		if (usb_connect_det_battery == LOW_POWER) {
			current_battery_num = get_adc_val(ADC1_BATTERY);
			XMLogI("low battery:%d", current_battery_num);
			if (current_battery_num && current_battery_num <= SHUT_DOWN_THRESHOLD) {
				low_battery_times++;
				if (current_battery_num <= 157) {
					POWER_OFF;
					reboot(RB_POWER_OFF);
					return -1;
				}
				if (low_battery_times > LOW_BATTERY_NUM) {
					low_battery_times = 0;
					low_battery_exit = true;
					XM_Middleware_Periphery_Notify(XM_EVENT_VOLTAGE_LOW, "", current_battery_num);
					current_battery_num = 0;
					return -1;
				}
			}
			else if (current_battery_num > RESET_LOW_BATTERY_THRESHOLD)
			{
				low_battery_times = 0;
			}
		}
	}
	return 0;
}

int SetLightValue(int value)
{
	if (value == 0) {
		XMLogI("2:short press to close LCD");
		pwm_lcd_open_or_close_flag = true;
		lcd_check_state = 0;
		//g_set_lcd = 0;
		PWM_LCD_OFF;
	}
	else if (value == 1) {
		XMLogI("2:short press to open LCD");
		pwm_lcd_open_or_close_flag = false;
		lcd_check_state = 1;
		//g_set_lcd = 0;
		PWM_LCD_ON;
	}

	return 0;
}

int CheckLightOn()
{
	if (pwm_lcd_open_or_close_flag) {
		pwm_lcd_open_or_close_flag = false;
		//PWM_LCD_ON;
	}
	return 0;
}

int AccDetect() {
	static unsigned int acc_det = 0;
	static bool acc_state = false;
	static unsigned acc_signal = 0;
	static bool acc_again_connect = false;
	readl(ACC_SIGNAL, &acc_signal);
	writel(ACC_DET, 0x1000);
	readl(ACC_DET, &acc_det);
	//XMLogI("acc_det = %x", acc_det);
	if ((acc_det == HIGH_POWER || acc_signal) && !acc_state) {
		usleep(2000);
		readl(ACC_DET, &acc_det);
		if ((acc_det == HIGH_POWER || acc_signal ) && !acc_state) {
			if (acc_det == HIGH_POWER && !acc_again_connect)
				AccDisconnectFlag = true;
			writel(ACC_SIGNAL, 0);
			XMLogI("Ignition of car");
			XM_Middleware_Periphery_Notify(XM_EVENT_ACC_CONNECT, "", 0);
			acc_state = true;
		}
	}

	if (acc_det == LOW_POWER && acc_state) {
		usleep(2000);
		readl(ACC_DET, &acc_det);
		if (acc_det == LOW_POWER && acc_state) {
			if (AccDisconnectFlag) {
				acc_again_connect = true;
			}
			AccDisconnectFlag = false;
			XMLogI("Stall of car");                                
			XM_Middleware_Periphery_Notify(XM_EVENT_ACC_DISCONNECT, "", 0);
			acc_state = false;
		}
	}
	//XMLogI("acc_det=%d\n", acc_det);
	return 0;
}

int UsbDetect() {
	static unsigned int usb_connect_det = 0;
	static bool usb_state = false;
	static unsigned int usb_signal = 0;
	static int64_t usb_low_time = 0;
	unsigned int sdo_det_value = 0;
	readl(USB_SIGNAL, &usb_signal);
	readl(USB_DET, &usb_connect_det);
	if ((usb_connect_det == HIGH_POWER|| usb_signal==0x3000)&& !usb_state ) {
		usleep(2000);
		readl(USB_DET, &usb_connect_det);
		if ((usb_connect_det == HIGH_POWER|| usb_signal==0x3000)&& !usb_state ) {
			XMLogI("1:USB connect");
			writel(USB_SIGNAL, 0);
			XM_Middleware_Periphery_Notify(XM_EVENT_USB_CONNECT, "", 0);
			usb_state = true;
		}
	}
	if (usb_connect_det == HIGH_POWER) {
		if (usb_low_time != 0) {
			readl(SD0_DET, &sdo_det_value);
			XMLogI("USB stable,sdo_det_value=%#x,usb_connect_det=%#x", sdo_det_value, usb_connect_det);
			XM_Middleware_Periphery_Notify(XM_EVENT_USB_STABLE, "", 0);
		}
		usb_low_time = 0;
	}
	if (usb_connect_det == LOW_POWER && usb_state) {
		if (usb_low_time == 0) {
			readl(SD0_DET, &sdo_det_value);
			XMLogI("USB unstable,sdo_det_value=%#x,usb_connect_det=%#x", sdo_det_value, usb_connect_det);
			XM_Middleware_Periphery_Notify(XM_EVENT_USB_UNSTABLE, "", 0);
			usb_low_time = GetTickTime();
		}
	}
	if (usb_low_time > 0 && (GetTickTime() - usb_low_time) >= USB_LOW_TIME) //2s内检测到usb再次连接就不关机
	{
		usb_low_time = 0;
		readl(USB_DET, &usb_connect_det);
		if (usb_connect_det == LOW_POWER && usb_state) {
			usb_state = false;
			XMLogI("USB disconnect, adc value=%d", get_adc_val(ADC1_BATTERY));
			XM_Middleware_Periphery_Notify(XM_EVENT_USB_DISCONNECT, "", get_adc_val(ADC1_BATTERY));
			return -1;
		}
		else {
			XMLogI("USB has been connected again.");
		}
	}
	return 0;
}

int RecordCameraLedControl(bool camera_led_state){
	// if (camera_led_state)
	// {
	// 	CAM_ON;
	// }
	// else {
	// 	CAM_OFF;
	// }
	return 0;
}

int adc_battery()
{
	return get_adc_val(ADC1_BATTERY);
}

int LcdStatus(bool lcd_status)
{
	static int first_set_pwm = 0;
	if (!g_set_lcd)
	{
		writel(PWM_LCD, PWM_LCD_INIT);
		g_set_lcd = 1;
		PWM_PARAM_S pstParam;
		if (lcd_status)
		{
			XMLogI("pwm on");
			//调节屏幕亮度参数
			pstParam.dr = 85;
			pstParam.freq = 200;
			if (!first_set_pwm)
			{
				first_set_pwm = 1;
				sleep(1);
			}
			SetPwm(PWM_LCD_GPIO, &pstParam);
		}
		else
		{
			XMLogI("pwm off");
			//调节屏幕亮度参数
			pstParam.dr = 0;
			pstParam.freq = 200;
			SetPwm(PWM_LCD_GPIO, &pstParam);
		}
	}
	return 0;
}