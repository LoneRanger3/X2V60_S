/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_template.h"
#include <sys/time.h>
#include <string>
#include <deque>
#include <unistd.h>
#include "lvgl.h"
#include "TimeUtil.h"
#include "Log.h"
#include "XMThread.h"
#include "xm_middleware_api.h"
#include "global_page.h"
#include "global_data.h"
#include "periphery/PeripheryManager.h"
#include "mpp/MppMdl.h"
/*********************
 *      DEFINES
 *********************/

#define lv_input_dev_touchpad 0
#define lv_input_dev_mouse 0
#define lv_input_dev_keypad 1
#define lv_input_dev_encoder 0
#define lv_input_dev_button 0



/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if lv_input_dev_touchpad
static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
#endif
#if lv_input_dev_mouse
static void mouse_init(void);
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);
#endif
#if lv_input_dev_keypad
static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);
#endif
#if lv_input_dev_encoder
static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);
#endif
#if lv_input_dev_button
static void button_init(void);
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/
#if lv_input_dev_touchpad
lv_indev_t * indev_touchpad;
#endif
#if lv_input_dev_mouse
lv_indev_t * indev_mouse;
#endif
#if lv_input_dev_keypad
lv_indev_t * indev_keypad;
#endif
#if lv_input_dev_encoder
lv_indev_t * indev_encoder;
#endif
#if lv_input_dev_button
lv_indev_t * indev_button;
#endif
#if lv_input_dev_encoder
static int32_t encoder_diff;
static lv_indev_state_t encoder_state;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    /*------------------
     * Touchpad
     * -----------------*/
#if lv_input_dev_touchpad
    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    static lv_indev_drv_t touchpad_drv;
    lv_indev_drv_init(&touchpad_drv);
    touchpad_drv.type = LV_INDEV_TYPE_POINTER;
    touchpad_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&touchpad_drv);
#endif
    /*------------------
     * Mouse
     * -----------------*/
#if lv_input_dev_mouse
    /*Initialize your mouse if you have*/
    mouse_init();

    /*Register a mouse input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
    indev_mouse = lv_indev_drv_register(&indev_drv);

    /*Set cursor. For simplicity set a HOME symbol now.*/
    lv_obj_t * mouse_cursor = lv_label_create(lv_scr_act());
    lv_label_set_text(mouse_cursor, LV_SYMBOL_HOME);
    lv_obj_set_style_text_color(mouse_cursor, lv_color_white(), 0);
    lv_indev_set_cursor(indev_mouse, mouse_cursor);
#endif
    /*------------------
     * Keypad
     * -----------------*/
#if lv_input_dev_keypad
    /*Initialize your keypad or keyboard if you have*/
   // keypad_init();

    /*Register a keypad input device*/
    static lv_indev_drv_t keypad_drv;
    lv_indev_drv_init(&keypad_drv);
    keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&keypad_drv);

    GlobalData::Instance()->group = lv_group_create();
    lv_group_set_default(GlobalData::Instance()->group);
    lv_indev_set_group(indev_keypad, GlobalData::Instance()->group);
    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/
#endif
    /*------------------
     * Encoder
     * -----------------*/
#if lv_input_dev_encoder
    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/
#endif
    /*------------------
     * Button
     * -----------------*/
#if lv_input_dev_button
    /*Initialize your button if you have*/
    button_init();

    /*Register a button input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_BUTTON;
    indev_drv.read_cb = button_read;
    indev_button = lv_indev_drv_register(&indev_drv);

    /*Assign buttons to points on the screen*/
    static const lv_point_t btn_points[2] = {
            {10, 10},   /*Button 0 -> x:10; y:10*/
            {40, 100},  /*Button 1 -> x:40; y:100*/
    };
    lv_indev_set_button_points(indev_button, btn_points);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/
#if lv_input_dev_touchpad
/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
	//MouseCreate();
    if (!LibXmDvr_Input_getGDICallInterface()) {
        XMLogW("There are touch screen");
        return;
    }
    LibXmDvr_Input_init();
    LibXmdvr_TouchScreen_enable(1);

    OS_THREAD touchpad;
    CreateThreadEx(touchpad, (LPTHREAD_START_ROUTINE)touch_proc, NULL);
}

static std::deque<MOUSE_DATA> touch_data_deque;
static std::mutex touch_mutex;
static MOUSE_DATA pData;
static bool released = true;
static bool rest_screen = false;
void* touch_proc(void* args)
{
    MOUSE_DATA data;
    while (1) {
        LibXmDvr_Input_getData(&data);
        touch_mutex.lock();
        if (touch_data_deque.size() > 8) {
            int size = touch_data_deque.size(), have_rel = 0;
            MOUSE_DATA data1;
            for (int i = 0;i < size;i++) {
                if (touch_data_deque.at(i).key == 0x80) {
                    data1 = touch_data_deque.at(i);
                    have_rel = 1;
                }
            }
            touch_data_deque.clear();

            if (have_rel) {
                touch_data_deque.push_back(data1);
            }
        }
        touch_data_deque.push_back(data);
        touch_mutex.unlock();
        
        pData = data;

        //XMLogI("========touch_proc(%d, %d), key = %x=========", pData.ax, pData.ay, pData.key);
    }
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    //XMLogI("input dev period");
    MOUSE_DATA touch_data;
    touch_mutex.lock();
    if (touch_data_deque.size() > 0) {
        touch_data = touch_data_deque.front();
        touch_data_deque.pop_front();
    }
    else {
        touch_data = pData;
    }
    touch_mutex.unlock();

    data->point.x = 360 - touch_data.ay + 60;
    data->point.y = touch_data.ax;

    if (touch_data.key == 0x81)//按下
    {
        //XMLogI("========touchpad_read(%d, %d)=========", data->point.x, data->point.y);
        if (!rest_screen) {
            rest_screen = GlobalPage::Instance()->page_main()->rest_screen_;
        }
        lv_event_send(lv_scr_act(), (lv_event_code_t)GlobalPage::Instance()->page_main()->MY_EVENT_SCREEN_CLICKED, NULL);
        if (rest_screen) return;
        data->state = LV_INDEV_STATE_PR;

        if (released && GlobalData::Instance()->key_tone_) {
            std::string sound_file = kAudioPath;
            sound_file += "dianji_16k.pcm";
            MppMdl::Instance()->PlaySound(sound_file.c_str());
            released = false;
        }
    }
    else if (touch_data.key == 0x80)//弹起
    {
        if (rest_screen) {
            rest_screen = false;
            return;
        }
        data->state = LV_INDEV_STATE_REL;
        released = true;

        if (GlobalPage::Instance()->page_set()->set_page_) {
            if (!lv_obj_has_flag(GlobalPage::Instance()->page_set()->sys_set_list_, LV_OBJ_FLAG_HIDDEN)) {
                lv_obj_add_flag(GlobalPage::Instance()->page_set()->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(GlobalPage::Instance()->page_set()->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
}
#endif
/*------------------
 * Mouse
 * -----------------*/
#if lv_input_dev_mouse
/*Initialize your mouse*/
static void mouse_init(void)
{
    LibXmDvr_Input_init();

    OS_THREAD mouse;
    CreateThreadEx(mouse, (LPTHREAD_START_ROUTINE)mouse_proc, NULL);
}

static MOUSE_DATA pData;
static int pre_x = 500;
static int pre_y = 500;
void* mouse_proc(void* args)
{
    int ret = -1;
    while (1) {
        ret = MouseGetData(&pData);
        if (ret == 0) {
            pre_x += pData.x;
            pre_y -= pData.y;
        }

        if (pre_x > 1024) pre_x = 1024;
        if (pre_x < 0) pre_x = 0;
        if (pre_y > 600) pre_y = 600;
        if (pre_y < 0) pre_y = 0;
    }
}

/*Will be called by the library to read the mouse*/
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    data->point.x = pre_x;
    data->point.y = pre_y;
    //XMLogI("-------------mouse:(%d, %d), %x", data->point.x, data->point.y, pData.key);

    if (pData.key == 0x1)//按下
    {
        data->state = LV_INDEV_STATE_PR;
    }
    else if (pData.key == 0x0)//弹起
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

/*Return true is the mouse button is pressed*/
static bool mouse_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}
#endif
/*------------------
 * Keypad
 * -----------------*/
#if lv_input_dev_keypad
extern bool g_app_connect;
/*Initialize your keypad*/
static void keypad_init(void)
{
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    //mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;
        XMLogI("act_key = %d", act_key);

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
        case KEYMAP_DOWN:
            act_key = LV_KEY_NEXT;
            break;
        case KEYMAP_UP:
            act_key = LV_KEY_PREV;
            break;
        case KEYMAP_OK:
            act_key = LV_KEY_ENTER;
            break;
        }
        last_key = act_key;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
}

extern int g_sd_status;
static bool rest_screen1 = false;
#if 1//X2V60_S_DEBUG1
bool g_wifi_man_oper = false;
#endif
/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    int key = PeripheryManager::Instance()->GetKeyValue();
    if (key == KEYMAP_MENU_LONG)
        key = KEYMAP_MODE;
    if (key == -1) return 0;
    XMLogI("GetKeyValue key = %d", key);

    //升级时停止按键的作用
    if (GlobalData::Instance()->upgrading_) return 0;

 //连接APP失能按键
    if (g_app_connect) return 0;
    

    //按键音
    int ret = 0;
    if (GlobalData::Instance()->key_tone_) {
        if (ret == 0) {
            std::string sound_file = kAudioPath;
            sound_file += "dianji_16k.pcm";
            MppMdl::Instance()->PlaySound(sound_file.c_str());
        }
    }

    if(key ==KEYMAP_DEF){
      XM_Middleware_Periphery_Notify(XM_EVENT_CFG_RESET, "", 0);
      return 0;
    }
    if(key ==KEYMAP_WIFI){
      if (GlobalPage::Instance()->page_main()->wifi_prepared_) {
	  	
          GlobalPage::Instance()->page_main()->WifiEnable(!GlobalPage::Instance()->page_main()->wifi_enable_);
		  #if 1//X2V60_S_DEBUG1
		  g_wifi_man_oper = true;
		  if(GlobalPage::Instance()->page_main()->wifi_enable_){
		  	//
    		std::string sound_file = kAudioPath;
    		sound_file += "WiFi_enable.pcm";
    		MppMdl::Instance()->PlaySound(sound_file.c_str());
		  }
		  #endif
       }
          return 0; 
    }
    if (key == KEYMAP_OK) {
        if(GlobalData::Instance()->recording_ == true){
            GlobalPage::Instance()->page_main()->CloseRecord();
        }else{
           GlobalPage::Instance()->page_main()->StartRecord(); 
        }
         return 0; 
    }

    //usb模式页面操作
    // if (GlobalPage::Instance()->page_usb()->usb_page_) {
    //     return key;
    // }

    // //锁住当前文件
    // if (key == KEYMAP_LOCK) {
    //     GlobalPage::Instance()->page_main()->LockCurrentFile();
    // }
    return 0;
}
#endif
/*------------------
 * Encoder
 * -----------------*/
#if lv_input_dev_encoder
/*Initialize your keypad*/
static void encoder_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    data->enc_diff = encoder_diff;
    data->state = encoder_state;
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void)
{
    /*Your code comes here*/

    encoder_diff += 0;
    encoder_state = LV_INDEV_STATE_REL;
}
#endif
/*------------------
 * Button
 * -----------------*/
#if lv_input_dev_button
/*Initialize your buttons*/
static void button_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for(i = 0; i < 2; i++) {
        /*Return the pressed button's ID*/
        if(button_is_pressed(i)) {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    /*Your code comes here*/

    return false;
}
#endif
#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
