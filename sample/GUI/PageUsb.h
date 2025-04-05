#ifndef __PAGEUSB_H__
#define __PAGEUSB_H__
#include "xm_middleware_def.h"

class PageUsb
{
public:
    PageUsb();
    ~PageUsb();

    void OpenPage();
    int SetUsbFunc(USB_MODE_E mode);

    int OnRealPlay(int engineId, int connId, int channel, int stream, int id);
    int OffRealPlay(int engineId, int connId, int channel, int stream, int id);
    void OnNetworkMsg(int engineId, int connId, uint8_t type, char * msg, int contentLen);
    void OnMWRealTimeStreamCallBack(int channel, int stream, XM_MW_Media_Frame* media_frame, int64_t user);
    void Exit();
private:
    void OpenMemoryModePage(USB_MODE_E mode);
    static void SelectUsbFuncEvent(lv_event_t* e);

public:
    lv_obj_t* usb_page_; //3个图标的页面
    lv_obj_t* memory_mode_page_; //单个大图标的页面
    USB_MODE_E user_selected_usb_mode_;
    bool realplay_;
    int engineId_;
    int connId_;
    int frame_id_;
    int channel_;
    int64_t first_frame_time_;
    
private:
    USB_MODE_E usb_mode_;
    lv_obj_t* mode_img_[3];
    lv_obj_t* mode_label_[3];
};

#endif // !__PAGEUSB_H__