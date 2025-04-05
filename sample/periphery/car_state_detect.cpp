#include "car_state_detect.h"
#include "i2c.h"
#include "PeripheryThread.h"
#include "Log.h"
#include "io.h"
#include "xm_middleware_api.h"

#define TEST 0
#define LAST_GSENSOR 1
#define NEW_GSENSOR 2
#define NORMAL 1
#define INNORMAL 2

enum XM_GSENSOR_MODE
{
	NORMAL_MODE,
	LOW_POWER_MODE,
	SUSPENDE_MODE
};

int GsensorCompatible()
{
	unsigned char device_address = da380_i2c_read(DEVICE_ADDR, CHIPID);
	if (device_address == DEVICE_DEFAULT_ADDRESS)
		XMLogI("G-sensor init success, adress=%x", device_address);
	else
	{
		XMLogI("G-sensor init fail,adress=%x", device_address);
		return -1;
	}
	da380_i2c_write(DEVICE_ADDR, 0x7f, 0x83);
	da380_i2c_write(DEVICE_ADDR, 0x7f, 0x69);
	da380_i2c_write(DEVICE_ADDR, 0x7f, 0xBD);
	unsigned char da380_id = da380_i2c_read(device_address, 0x8f) & 0xfd;
	da380_i2c_write(DEVICE_ADDR, 0x8f, da380_id);
	unsigned char distingush_register = da380_i2c_read(DEVICE_ADDR, 0xC0);
	if ((distingush_register & 0x38) >> 3 == 0x03) {
		XMLogI("last gsensor\n");
		da380_i2c_write(DEVICE_ADDR, ODR_AXIS, 0xf8);//使能xyz轴，输出频率为125hz
		return LAST_GSENSOR;
	}
	else if ((distingush_register & 0x38) >> 3 == 0x01) {
		XMLogI("new gsensor\n");
		//da380_i2c_write(DEVICE_ADDR, ODR_AXIS, 0xf6);//使能xyz轴，输出频率为100hz
		//da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x04);//选择低功耗模式
		da380_i2c_write(DEVICE_ADDR, ODR_AXIS, 0xf8);//失能xyz轴，输出频率为125hz
		unsigned char MODE_BW_test = da380_i2c_read(DEVICE_ADDR, MODE_BW);
		XMLogI("*******************MODE_BW_test=%x\n", MODE_BW_test);
		return NEW_GSENSOR;
	}
	return 0;
}

//关机:停车监控模式
void ShutMode()
{
	XMLogI("New Gsensor select low power mode");
	da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x52);//选择低功耗模式
}

//开机:重力感应模式
void NormalMode()
{
	XMLogI("New Gsensor select normal mode");
	da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x12);//选择正常模式
}

//停车监控和重力感应都关闭，选择悬挂模式
void SuspendMode()
{
	XMLogI("New Gsensor select suspend mode");
	da380_i2c_write(DEVICE_ADDR, MODE_BW, 0xf2);//选择悬挂模式
}

void LastGsensorMode(int mode)
{
	if (mode == NORMAL)
	{
		XMLogI("Last Gsensor select normal mode");
		da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x05);//选择正常模式
	}
	else if (mode == INNORMAL)
	{
		XMLogI("Last Gsensor select abnormal mode");
		da380_i2c_write(DEVICE_ADDR, MODE_BW, 0x85);//选择悬挂模式
	}
}
int GsensorMode(int mode)
{
	int ret = GsensorCompatible();
	if (ret == NEW_GSENSOR)
	{
		if (mode == NORMAL_MODE)
		{
			NormalMode();
		}
		else if (mode == LOW_POWER_MODE)
		{
			ShutMode();
		}
		else if (mode == SUSPENDE_MODE)
		{
			SuspendMode();
		}
	}
	else if (ret == LAST_GSENSOR)
	{
		if (mode == NORMAL_MODE || mode == LOW_POWER_MODE)
		{
			LastGsensorMode(NORMAL);
		}
		else if (mode == SUSPENDE_MODE)
		{
			LastGsensorMode(INNORMAL);
		}
	}
	return 0;
}


void InitSensor() {
	da380_i2c_write(DEVICE_ADDR, INT_CONFIG, 0x01);//中断配置
	da380_i2c_write(DEVICE_ADDR, INT_LTACH, 0x04);//设置成临时中断锁存2s
}
void InitSensorHighSensitivity() {
	int gsensor_ret = GsensorCompatible();
	if (gsensor_ret == LAST_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, HIGH_SENSITIVITY);//设置传感器精度+-4g
		da380_i2c_write(DEVICE_ADDR, ACTIVE_THS, 0x3f);//运行检测中断阈值配置高2g
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0xff);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0xff);//中断映射使能
	}
	else if(gsensor_ret == NEW_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, LOW_SENSITIVITY);//设置传感器精度+-16g
		da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x0F);//点击中断阈值配置
		da380_i2c_write(DEVICE_ADDR, TAP_DUR, 0x10);//点击持续时间
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0x30);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0x30);//中断映射使能
	}
	InitSensor();
}

void InitSensorMediumSensitivity() {
	int gsensor_ret=GsensorCompatible();
	if (gsensor_ret == LAST_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, MEDIUM_SENSITIVITY);//设置传感器精度+-8g
		da380_i2c_write(DEVICE_ADDR, ACTIVE_THS, 0x3f);//运行检测中断阈值配置中4g
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0xff);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0xff);//中断映射使能
	}
	else if (gsensor_ret == NEW_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, LOW_SENSITIVITY);//设置传感器精度+-16g
	    da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x19);//点击中断阈值配置 19
		da380_i2c_write(DEVICE_ADDR, TAP_DUR, 0x10);//点击持续时间
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0x30);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0x30);//中断映射使能
	}
	InitSensor();
}

void InitSensorLowSensitivity() {
	int gsensor_ret = GsensorCompatible();
	if (gsensor_ret == LAST_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, LOW_SENSITIVITY);//设置传感器精度+-16g
		da380_i2c_write(DEVICE_ADDR, ACTIVE_THS, 0x3f);//运行检测中断阈值配置低8g
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0xff);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0xff);//中断映射使能
	}
	else if (gsensor_ret == NEW_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, LOW_SENSITIVITY);//设置传感器精度+-16g
		da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x1F);//点击中断阈值配置
		da380_i2c_write(DEVICE_ADDR, TAP_DUR, 0x10);//点击持续时间
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0x30);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0x30);//中断映射使能
	}
	InitSensor();
}

void InitSensorShutDownSensitivity() {
	GsensorCompatible();
	da380_i2c_write(DEVICE_ADDR, INT_SET1, 0x00);//中断使能
	da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0x00);//中断映射使能
	InitSensor();
}

void CarStopSensorHighSensitivity() {
	int gsensor_ret = GsensorCompatible();
	if (gsensor_ret == LAST_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, HIGH_SENSITIVITY);//设置传感器精度+-4g
		da380_i2c_write(DEVICE_ADDR, ACTIVE_THS, 0x2f);//运行检测中断阈值配置高1.5g
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0xff);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0xff);//中断映射使能
	}
	else if (gsensor_ret == NEW_GSENSOR)
	{
		da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, LOW_SENSITIVITY);//设置传感器精度+-16g
		da380_i2c_write(DEVICE_ADDR, TAP_THS, 0x02);//点击中断阈值配置
		da380_i2c_write(DEVICE_ADDR, TAP_DUR, 0x10);//点击持续时间
		da380_i2c_write(DEVICE_ADDR, INT_SET1, 0x30);//中断使能
		da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0x30);//中断映射使能
	}
	InitSensor();
}

void CarStopSensorMediumSensitivity() {
	da380_i2c_write(DEVICE_ADDR, INT_SET1, 0xff);//中断使能
	da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0xff);//中断映射使能
	da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, HIGH_SENSITIVITY);//设置传感器精度+-4g
	da380_i2c_write(DEVICE_ADDR, ACTIVE_THS, 0xc0);//运行检测中断阈值配置高1.5g
	InitSensor();
}

void CarStopSensorLowSensitivity() {
	da380_i2c_write(DEVICE_ADDR, INT_SET1, 0xff);//中断使能
	da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0xff);//中断映射使能
	da380_i2c_write(DEVICE_ADDR, RESOLUTION_RANGE, HIGH_SENSITIVITY);//设置传感器精度+-4g
	da380_i2c_write(DEVICE_ADDR, ACTIVE_THS, 0xff);//运行检测中断阈值配置高2g
	InitSensor();
}

void CarStopSensorShutDownSensitivity() {
	da380_i2c_write(DEVICE_ADDR, INT_SET1, 0x00);//中断使能
	da380_i2c_write(DEVICE_ADDR, INT_MAP1, 0x00);//中断映射使能
	InitSensor();
}

int CarCollisionDetectPowerOn() {
	//碰撞检测开机
	//static unsigned int Int_gsensor;
	//readl(DA380_GSENSOR_INT1, &Int_gsensor);
	//if (Int_gsensor == HIGH_POWER)
	//	XMLogI("INT1_gsensor=%#X", Int_gsensor);
	int ret = 0;
	static unsigned int interrupt_flag = 0;
	static  bool collision_status = false;
	readl(GSENSOR_INT1_REG, &interrupt_flag);

	if (interrupt_flag == 0x1000) {
		XMLogI("Power on  detect no Collision ");
		collision_status = false;
		ret = 0;
	}
	if (interrupt_flag == 0x3000 && !collision_status) {
		XMLogW("Collision make  power  open");
		collision_status = true;
		ret = 1;
	}

	return ret;
}

int CarCollisionDetect() {

	static unsigned int Int_gsensor;
    readl(DA380_GSENSOR_INT1, &Int_gsensor);
  /*  if (Int_gsensor == HIGH_POWER)
	XMLogI("************INT1_gsensor=%#X", Int_gsensor);*/

	static unsigned char interrupt_signal = 0;
	static  bool collision_flag = false;
	interrupt_signal = da380_i2c_read(DEVICE_ADDR, MOTION_FLAG);
	//if(interrupt_signal)
	//XMLogI("###########Crash  signal=%d", interrupt_signal);
	if (interrupt_signal == 0) {
		collision_flag = false;
	}
	if (interrupt_signal != 0 && !collision_flag) {
		XMLogI("Crash  signal");
		collision_flag = true;
		XM_Middleware_Periphery_Notify(XM_EVENT_COLLISION_DETECTED, "", 0);
	}
	return 0;
}

int SensitivityDegreeSet(int degree) {
	XMLogI("gsensor degree=%d", degree);
	switch (degree)
	{
	case 0:InitSensorShutDownSensitivity(); break;
	case 1:InitSensorHighSensitivity(); break;
	case 2:InitSensorMediumSensitivity(); break;
	case 3:InitSensorLowSensitivity(); break;
	case 4: InitSensorMediumSensitivity(); break;
	default:XMLogE("Set gsensor level error");
		break;
	}
	return 1;
}

int CarStopSensitivityDegreeSet(int degree) {
	switch (degree)
	{
	case 0:CarStopSensorShutDownSensitivity(); break;
	case 1:CarStopSensorHighSensitivity(); break;
	case 2:CarStopSensorMediumSensitivity(); break;
	case 3:CarStopSensorLowSensitivity(); break;
	default:XMLogE("Set car stop gsensor level error");
		break;
	}
	return 1;
}

int CarBackDetect() {
	static unsigned int car_back_signal = 0;
	static bool car_back_state = false;
	readl(ACR_BACK_DETECT, &car_back_signal);
	//XMLogI("car_back_signal=%#x", car_back_signal);
	if (car_back_signal == HIGH_POWER && car_back_state) {
		usleep(2000*10);
		readl(ACR_BACK_DETECT, &car_back_signal);
		if (car_back_signal == HIGH_POWER && car_back_state) {
			car_back_state = false;
			XMLogI("Car stop backing");
			XM_Middleware_Periphery_Notify(XM_EVENT_REVERSE_END, "", 0);
		}
	}
	if (car_back_signal == LOW_POWER&&!car_back_state) {
		usleep(2000*10);
		readl(ACR_BACK_DETECT, &car_back_signal);
		if (car_back_signal == LOW_POWER && !car_back_state) {
			car_back_state = true;
			XMLogI("Car start backing");
			XM_Middleware_Periphery_Notify(XM_EVENT_REVERSE_DETECTED, "", 0);
		}
	}
	return 0;
}

void CarStateDetect() {
	//CarStopDetect();
	CarCollisionDetect();
	//CarBackDetect();
}