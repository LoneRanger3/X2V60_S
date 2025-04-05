#ifndef PERIPHERYTHREAD_H
#define PERIPHERYTHREAD_H

#ifndef WIN32
#include "io.h"
#include "adc.h"
#include "car_state_detect.h"
//#include "wdt.h"
#include "adc_presskey.h"
#include "XMThread.h"
#include <stdio.h>
#include <unistd.h>
#include "xm_middleware_def.h"
#include "power.h"

#define GPIO_ADDR_BASE 0x10020000

#define HIGH 0xc00
#define LOW  0x400
#define BITWIDTH 0x04

#define HIGH_POWER 0x3000
#define LOW_POWER  0x1000

	//A端口管脚
#define A0   0x00
#define A1   (A0+BITWIDTH)
#define A2   (A1+BITWIDTH)
#define A3   (A2+BITWIDTH)
#define A4   (A3+BITWIDTH)
#define A5   (A4+BITWIDTH)
#define A6   (A5+BITWIDTH)
#define A7   (A6+BITWIDTH)
#define A8   (A7+BITWIDTH)
#define A9   (A8+BITWIDTH)
#define A10   (A9+BITWIDTH)
#define A11   (A10+BITWIDTH)
#define A12   (A11+BITWIDTH)
#define A13   (A12+BITWIDTH)
#define A14   (A13+BITWIDTH)

//B端口管脚
#define B32   0x80
#define B33   (B32+BITWIDTH)
#define B34   (B33+BITWIDTH)
#define B35   (B34+BITWIDTH)
#define B36   (B35+BITWIDTH)
#define B37   (B36+BITWIDTH)

//C端口管脚
#define C64   0x100
#define C65   (C64+BITWIDTH)
#define C66   (C65+BITWIDTH)
#define C67   (C66+BITWIDTH)
#define C68   (C67+BITWIDTH)
#define C69   (C68+BITWIDTH)
#define C70   (C69+BITWIDTH)
#define C71   (C70+BITWIDTH)
#define C72   (C71+BITWIDTH)
#define C73   (C72+BITWIDTH)
#define C74   (C73+BITWIDTH)
#define C75   (C74+BITWIDTH)
#define C76   (C75+BITWIDTH)
#define C77   (C76+BITWIDTH)
#define C78   (C77+BITWIDTH)
#define C79   (C78+BITWIDTH)
#define C80   (C79+BITWIDTH)
#define C81   (C80+BITWIDTH)
#define C82   (C81+BITWIDTH)
#define C83   (C82+BITWIDTH)
#define C84   (C83+BITWIDTH)
#define C85   (C84+BITWIDTH)
#define C86   (C85+BITWIDTH)
#define C87   (C86+BITWIDTH)
#define C88   (C87+BITWIDTH)
#define C89   (C88+BITWIDTH)
#define C90   (C89+BITWIDTH)
#define C91   (C90+BITWIDTH)

//E端口管脚
#define E128  0x200
#define E129  (E128+BITWIDTH)
#define E130  (E129+BITWIDTH)
#define E131  (E130+BITWIDTH)
#define E132  (E131+BITWIDTH)
#define E133  (E132+BITWIDTH)
#define E134  (E133+BITWIDTH)
#define E135  (E134+BITWIDTH)
#define E136  (E135+BITWIDTH)
#define E137  (E136+BITWIDTH)

//F端口管脚
#define F160  0x280
#define F161  (F160+BITWIDTH)
#define F162  (F161+BITWIDTH)
#define F163  (F162+BITWIDTH)
#define F164  (F163+BITWIDTH)
#define F165  (F164+BITWIDTH)
#define F166  (F165+BITWIDTH)
#define F167  (F166+BITWIDTH)
#define F168  (F167+BITWIDTH)
#define F169  (F168+BITWIDTH)

//E端口管脚
#define H224  0x380
#define H225  (H224+BITWIDTH)
#define H226  (H225+BITWIDTH)
#define H227  (H226+BITWIDTH)
#define H228  (H227+BITWIDTH)
#define H229  (H228+BITWIDTH)
#define H230  (H229+BITWIDTH)
#define H231  (H230+BITWIDTH)
#define H232  (H231+BITWIDTH)
#define H233  (H232+BITWIDTH)
#define H234  (H233+BITWIDTH)
#define H235  (H234+BITWIDTH)
#define H236  (H235+BITWIDTH)
#define H237  (H236+BITWIDTH)

//复用具体功能的定义
#define HOST_CPU_POWER  B33
#define PWM_LCD         (A8+GPIO_ADDR_BASE)
#define GPS_UART0_RXD   A9
#define I2C1_SCL        A10
#define I2C1_SDA        A12
#define DA380_GSENSOR_INT1  (GPIO_ADDR_BASE+A13)
#define ACC_DET (GPIO_ADDR_BASE + E136)
#define USB_DET (GPIO_ADDR_BASE + B36)
#define SD0_DET (GPIO_ADDR_BASE + F166)
#define SPEAK_EN (GPIO_ADDR_BASE + E128)
#define GPS_POWER_EN (GPIO_ADDR_BASE + E134)
#define INT_SET1_DEFAULT 0x10
//#define USB0_WIFI_PWR  (GPIO_ADDR_BASE + F168)
//#define WIFI_OFF   writel(USB0_WIFI_PWR,HIGH)
#define ACC_SIGNAL 0x20000F0C
#define USB_SIGNAL 0x20000F14
#define ACR_BACK_DETECT (GPIO_ADDR_BASE + A14)
#define PWM_LCD_GPIO 8
#define KEY_ON (E130+GPIO_ADDR_BASE)
#define PWM_LCD_INIT 0x88

//用户寄存器
#define USER_REG_ADC            0x20000F10
#define USER_REG_USB            0x20000F14
#define USER_REG_KEYON          0x20000F18
#define USER_REG_GSENSOR        0x20000F08

//ADC
#define AD_IN_DETECT 0
#define  ADC1_BATTERY   1
#define  ADC2_PRESSKEY  2
#define  ADC3_PRESSKEY  3

//管脚拉低拉高的函数

#define SET(offset_v1) writel(GPIO_ADDR_BASE+offset_v1, HIGH)
#define RESET(offset_v1) writel(GPIO_ADDR_BASE+offset_v1,LOW)

//主控电源开关
#define POWER_ON   SET(B33)
#define POWER_OFF  RESET(B33)

#define SPEAK_ON  SET(E128)
#define SPEAK_OFF  RESET(E128)

//ad电源关闭
#define AD_OFF RESET(E137)

//Sensor电源关闭
#define SENSOR_OFF RESET(B35)

//Wifi电源关闭
//#define WIFI_OFF RESET(A14)

//LCD_RESE 电源关闭
//#define LCD_RESE  RESET(C78)

//TP_RST电源关闭
//#define TP_RST RESET(C80)

//红灯开关
#define RED_LED_ON   SET(C65)
#define RED_LED_OFF   RESET(C65)

//绿灯开关
#define GREEN_LED_ON   SET(C64)
#define GREEN_LED_OFF   RESET(C64)

//WIFI指示灯开关
#define WiFi_LED_ON   SET(C66)
#define WiFi_LED_OFF   RESET(C66)

//gps红灯开关
#define GPS_RED_LED_ON   SET(C67)
#define GPS_RED_LED_OFF   RESET(C67)

//gps绿灯开关
#define GPS_GREEN_LED_ON   SET(C68)
#define GPS_GREEN_LED_OFF   RESET(C68)

//PWM_LCD背光屏开光
#define PWM_LCD_ON  SET(A8)
#define PWM_LCD_OFF RESET(A8) 

//IIC通道定义
#define IIC1  1
#define IIC2  2

//用到的全局变量
	//int current_battery_num_v1;
	//unsigned int wdt_timeout = 30;

//函数定义
void PeripheryThreadStart();

#endif //WIN32
#endif 
