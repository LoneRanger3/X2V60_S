#ifndef CAR_STATE_DETECT_H_
#define CAR_STATE_DETECT_H_

typedef unsigned short u16;
#define REG_WIDTH 1
#define DATA_WIDTH 1
#define REG_NUM  1
//DA380传感器寄存器
#define DEVICE_ADDR   0x4E 

#define CONFIGURATION 0x00
#define CHIPID        0x01
#define ACC_X_LSB     0x02
#define ACC_X_MSB     0x03
#define ACC_Y_LSB     0x04
#define ACC_Y_MSB     0x05
#define ACC_Z_LSB     0x06
#define ACC_Z_MSB     0x07
#define MOTION_FLAG   0x09
#define NEWDATA_FLAG  0x0A
#define TAP_ACTIVE_STATUS 0x0B
#define ORIENT_STATUS 0x0C
#define RESOLUTION_RANGE 0x0F
#define ODR_AXIS      0x10
#define MODE_BW       0x11
#define SWAP_POLARITY 0x12
#define INT_SET1      0x16
#define INT_SET2      0x17
#define INT_MAP1      0x19
#define INT_MAP2      0x1A
#define INT_CONFIG    0x20
#define INT_LTACH     0x21
#define FREEFALL_DUR  0x22
#define FREEFALL_THS  0x23
#define FREEFALL_HYST 0x24
#define ACTIVE_DUR    0x27
#define ACTIVE_THS    0x28
#define TAP_DUR       0x2A
#define TAP_THS       0x2B
#define ORIENT_HYST   0x2C

#define IIC_REUSE  0x84
#define DEVICE_DEFAULT_ADDRESS 0x13

#define HIGH_SENSITIVITY 0x61
#define MEDIUM_SENSITIVITY 0x62
#define LOW_SENSITIVITY 0x63
#define GSENSOR_INT1_REG 0x20000f08
#define GSENSOR_INT1_SIGNAL 1

typedef enum _GsensorLevel
{
	CLOSE_SENSITIVITY =0,
	HIGH=1,
	MEDIUM=2,
	LOW=3,
}GsensorLevel;

void InitSensor();
void CarStateDetect();
int CarStopDetect();
int CarCollisionDetectPowerOn();
int CarBackDetect();
void InitSensorHighSensitivity();
void InitSensorMediumSensitivity();
void InitSensorLowSensitivity();
int SensitivityDegreeSet(int degree);
int CarStopSensitivityDegreeSet(int degree);
int GsensorMode(int mode);
//int GsensorCompatible();
#endif // !CAR_STATE_DETECT_H_
