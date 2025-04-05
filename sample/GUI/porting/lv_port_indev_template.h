
/**
 * @file lv_port_indev_templ.h
 *
 */

 /*Copy this file as "lv_port_indev.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_INDEV_TEMPL_H
#define LV_PORT_INDEV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct MOUSE_DATA
{
  /// ˳±똳Ԓא3¸򰴼񗵌¬£¬׃1±°´¼񐂣¬׃0±°´¼簡£ԫº離 MOUSE_XXX
  /// Рԫµõ½¶Փ¦°´¼񗵌¬¡£خ¸ࠎ»¶Փ¦´¥ľǁ±떾£¬׃1±Ϊ´¥ľǁ˽¾ݣ¬׃0
  /// ±˳±닽¾ݡ£ ´¥ľǁ°´Ђµ¯ǰ¶¯ط´¦mΪس¼
  unsigned char  key;

  /// xظ±놫ӆ£¬ֽ˽вԒ£¬¸º˽вس£¬ȡֵ[-127,128]¡£˳±늹ԃ¡£
  signed char	 x;

  /// yظ±놫ӆ£¬ֽ˽вʏ£¬¸º˽вЂ£¬ȡֵ[-127,128]¡£˳±늹ԃ¡£
  signed char	 y;

  /// ¹󃗆«ӆ£¬ֽ˽вº󣬸º˽вǰ£¬ȡֵ[-127,128]¡£˳±늹ԃ¡£
  signed char	 z;

  /// ¾󓹗򪣬ֽ·½ввԒ¡£´¥ľǁʹԃ¡£
  unsigned short  ax;

  /// ¾󓺗򪣬ֽ·½ввЂ¡£´¥ľǁʹԃ¡£
  unsigned short  ay;  
} MOUSE_DATA;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
int MouseCreate(void);
int MouseGetData_NonBlock(MOUSE_DATA *pData);
int MouseGetData(MOUSE_DATA* pData);
void* mouse_proc(void* args);

int LibXmDvr_Input_getGDICallInterface(void);
int LibXmDvr_Input_init(void);
int LibXmDvr_Input_getData(MOUSE_DATA* pData);
int LibXmdvr_TouchScreen_enable(int enable);
void* touch_proc(void* args);

void lv_port_indev_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_INDEV_TEMPL_H*/

#endif /*Disable/Enable content*/
