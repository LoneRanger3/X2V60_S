#ifndef POWER_H
#define POWER_H

#define CLOSE_TIME_MAX 1000
#define CLOSE_TIME_MIN 5
#define CLOSE_BATTERY_THRESHOLD 10
#define LOW_BATTERY_NUM 2
#define SHUT_DOWN_THRESHOLD 161
#define RESET_LOW_BATTERY_THRESHOLD 165
#define USB_LOW_TIME 2000
extern bool AccDisconnectFlag;

int PressKeyOpenClose();
int LowBatteryClose();
int SetLightValue(int value);
int CheckLightOn();
int AccDetect();
int UsbDetect();
int RecordCameraLedControl(bool camera_led_state);
int adc_battery();
int LcdStatus(bool lcd_status);
int WifiKey();
#endif