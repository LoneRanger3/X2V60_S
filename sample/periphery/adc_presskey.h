#ifndef ADC_PRESSKEY_H_
#define ADC_PRESSKEY_H_
#include "xm_middleware_def.h"

//typedef enum {
//	KEY_MODE_OR_UP=1,
//	KEY_DOWN_OR_OK,
//	KEY_MENU_OR_LOCK,
//	KEY_NONE,
//}KEY_VALUE_E;

typedef enum {
	KEY_MENU = 1,
	KEY_UP,
	KEY_DOWN,
	KEY_OK,
	KEY_NONE,
}KEY_VALUE_E;


#define count_max 3

void key_init();
void PressKeyQuery();
int getKeyValue();
void SetKeyValue(int key);
int shutDown(ShutDownMode shutdownmode);
int UsbAccGsensorCheckShutDown(ShutDownMode ShutType);

#endif//end ADC_PRESSKEY_H_