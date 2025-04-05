#include "adc_presskey.h"
#include "adc.h"
#include "stdio.h"
#include <unistd.h>
#include "Log.h"
#include "PeripheryThread.h"
#include "io.h"
#include <unistd.h>
#include <sys/reboot.h>
#include "i2c.h"
#include "power.h"
#include "log/LogFileManager.h"
#include "xm_middleware_api.h"
#include <sys/reboot.h>

static int inindex = 0;
static int outindex = 0;
static int Key_Value[16] = { 0 };
static int last_value = 0, new_value = 0, last_value_occur_times = 0, new_value_occur_times = 0;
static int last_value_1 = 0, new_value_1 = 0, last_value_occur_times_1 = 0, new_value_occur_times_1 = 0;


void key_init()
{
	for (int i = 0; i < 16; i++) {
		Key_Value[i] = -1;
	}
}

//unsigned char adc_query(unsigned short battery_value) {
//	if (0 <= battery_value && battery_value < 50) return 1;//26 up and shock
//	if (60 < battery_value && battery_value < 150)  return 2; //118 down and ok
//	if (160 < battery_value && battery_value < 240) return 3; //217 menu and mode
//	if (245 < battery_value) return 4;
//	return 0;
//}

// unsigned char adc_query(unsigned short battery_value) {
// 	if (0 <= battery_value && battery_value < 50) return 1;//26 up and shock
// 	if (60 < battery_value && battery_value < 150)  return 2; //118 down and ok
// 	if (160 < battery_value && battery_value < 240) return 3; //217 menu and mode
// 	if (245 < battery_value) return 4;
// 	return 0;
// }

unsigned char adc_query(unsigned short battery_value) { 
	if (0 <= battery_value && battery_value < 40) return 1;//
	if (40 <= battery_value && battery_value <100 )  return 2; //75 down and ok
	if (140 <= battery_value && battery_value < 240) return 3; //216 menu and mode
	if (245 < battery_value) return 5;
	return 0;
}

void PressKeyChannelSelect(int channel) {
	static int key_new_value_record = 0;
	static int key_last_value_record = 0;
	new_value = adc_query(get_adc_val(channel));
	key_new_value_record = get_adc_val(channel);
	if (new_value) {
		if (new_value == KEY_NONE && last_value_occur_times > count_max) {
			if (last_value < KEY_NONE) {
				printf("keyvalue=%d\n", key_last_value_record);
				if (channel == ADC2_PRESSKEY) {
					if (last_value == 1) {
						Key_Value[inindex] = KEYMAP_DEF;
						XMLogI("key value=KEYMAP_DEF");
						//XMLogI("adc battery=%d", adc_battery());
						if (inindex < 15)
							inindex++;
						else
							inindex = 0;
					}
					else if (last_value == 3) {
						Key_Value[inindex] = KEYMAP_WIFI;
						XMLogI("key value=KEYMAP_WIFI");
						//XMLogI("adc battery=%d", adc_battery());
						if (inindex < 15)
							inindex++;
						else
							inindex = 0;
					}
					else if (last_value == KEY_MENU) {
						XMLogW("last_value_occur_times = %d", last_value_occur_times);
						if (last_value_occur_times > 30) {
							Key_Value[inindex] = KEYMAP_MENU_LONG;
							XMLogI("key value=MENU_LONG");
						}
						else {
							Key_Value[inindex] = KEYMAP_MENU;
							XMLogI("key value=MENU");
						}
						
						if (inindex < 15)
							inindex++;
						else
							inindex = 0;
					}
					else if (last_value == KEY_OK) {
						Key_Value[inindex] = KEYMAP_OK;
						XMLogI("key value=OK");
						if (inindex < 15)
							inindex++;
						else
							inindex = 0;
					}
				}
			}
			last_value_occur_times = 0;
			new_value_occur_times = 0;
			last_value = 0;
		}
		if (new_value == last_value) {
			last_value_occur_times++;
		}
		else {
			new_value_occur_times++;
		}
		if (last_value_occur_times < new_value_occur_times) {
			last_value = new_value;
			key_last_value_record = key_new_value_record;
		}
	}
}


void PressKeyQuery() {
	PressKeyChannelSelect(ADC2_PRESSKEY);
}

int getKeyValue()
{
	int value = Key_Value[outindex];
	if (value != -1)
	{
		Key_Value[outindex] = -1;
		if (outindex < 15)
			outindex++;
		else
			outindex = 0;
	}
	return value;
}
void SetKeyValue(int key)
{
	 Key_Value[inindex] = key;
		if (inindex < 15)
			inindex++;
		else
			inindex = 0;
}
int shutDown(ShutDownMode shutdownmode)
{
	switch (shutdownmode)
	{
	case ShutDownMode_Key:XMLogI("shutdownmode = auto shut down"); break;
	case ShutDownMode_PressKey:XMLogI("shutdownmode = presskey shut down"); break;
	case ShutDownMode_Acc:XMLogI("shutdownmode = Acc disconnect shut down"); break;
	case ShutDownMode_USBDisconnect:XMLogI("shutdownmode = USB disconnect shut down"); break;
	default:XMLogE("ShutDownMode error");
		break;
	}
	UsbAccGsensorCheckShutDown(shutdownmode);
	return 0;
}

int UsbAccGsensorCheckShutDown(ShutDownMode ShutType) {
	static unsigned int usb_connect_check = 0;
	static bool usb_state_check = false;
	static unsigned int acc_check = 0;
	static bool acc_state_high_check = false;
	static bool acc_state_low_check = false;
	static unsigned char interrupt_signal_check = 0;
	static  bool collision_flag_check = false;
	interrupt_signal_check = da380_i2c_read(DEVICE_ADDR, MOTION_FLAG);

	readl(ACC_DET, &acc_check);
	readl(USB_DET, &usb_connect_check);
	if (usb_connect_check == HIGH_POWER 
		&& ShutType != ShutDownMode_PressKey 
		&& ShutType != ShutDownMode_Acc
		&& ShutType != ShutDownMode_Key) {
		usleep(2000);
		readl(USB_DET, &usb_connect_check);
		if (usb_connect_check == HIGH_POWER 
			&& ShutType != ShutDownMode_PressKey 
			&& ShutType != ShutDownMode_Acc
			&& ShutType != ShutDownMode_Key) {
			XMLogI("2:USB connect");
			reboot(RB_AUTOBOOT);
			usb_state_check = true;
		}
	}
	else if (acc_check == HIGH_POWER && (!AccDisconnectFlag && ShutType != ShutDownMode_Key)) {
		usleep(2000);
		readl(ACC_DET, &acc_check);
		if (acc_check == HIGH_POWER && (!AccDisconnectFlag && ShutType != ShutDownMode_Key)) {
			XMLogI("2:Ignition of car");
			reboot(RB_AUTOBOOT);
		}
	}
	else if (interrupt_signal_check) {
		XMLogI("2:collision power on");
		reboot(RB_AUTOBOOT);
	}
	else
	{
		XMLogI("SHUT DOWN");
		PWM_LCD_OFF;
		//LCD_RESE;
		//TP_RST;
		//CAM_OFF;
		for (int check_count = 0; check_count < 20; check_count++) {
			usleep(10000);
			if (readl(USB_DET, &usb_connect_check) != 0) {
				XMLogE("Read usb_state failed");
			}
			if (usb_connect_check == HIGH_POWER 
				&& ShutType != ShutDownMode_PressKey 
				&& ShutType != ShutDownMode_Acc
				&& ShutType != ShutDownMode_Key) {
					XMLogI("3:USB connect");
					reboot(RB_AUTOBOOT);           
			}
			if (readl(ACC_DET, &acc_check)!= 0) {
				XMLogE("Read acc_state failed");
			}
			if (acc_check == HIGH_POWER && (!AccDisconnectFlag && ShutType != ShutDownMode_Key)) {
				XMLogI("3:Ignition of car");
				reboot(RB_AUTOBOOT);
			}
			interrupt_signal_check = da380_i2c_read(DEVICE_ADDR, MOTION_FLAG);
			if (interrupt_signal_check) {
				XMLogI("3:collision power on");
				reboot(RB_AUTOBOOT);
			}
		}
		XMLogI("usb_state=%#x", usb_connect_check);
		usleep(10 * 1000);
		AD_OFF;
		usleep(10 * 1000);
		SENSOR_OFF;
		usleep(10 * 1000);
		POWER_OFF;
		reboot(RB_POWER_OFF);
		for (int check_count = 0; check_count < 20; check_count++) {
			usleep(10000);
			if (readl(USB_DET, &usb_connect_check) != 0) {
				XMLogE("Read usb_state failed");
			}
			if (usb_connect_check == HIGH_POWER 
				&& ShutType != ShutDownMode_PressKey 
				&& ShutType != ShutDownMode_Acc
				&& ShutType != ShutDownMode_Key) {
				XMLogI("4:USB connect");
				reboot(RB_AUTOBOOT);
			}
			if (readl(ACC_DET, &acc_check) != 0) {
				XMLogE("Read acc_state failed");
			}
			if (acc_check == HIGH_POWER && (!AccDisconnectFlag && ShutType != ShutDownMode_Key)) {
				XMLogI("4:Ignition of car");
				reboot(RB_AUTOBOOT);
			}
			interrupt_signal_check = da380_i2c_read(DEVICE_ADDR, MOTION_FLAG);
			if (interrupt_signal_check) {
				XMLogI("4:collision power on");
				reboot(RB_AUTOBOOT);
			}
		}
		XMLogI("1:usb_state=%#x", usb_connect_check);
	}
}

