#ifndef __GLOBAL_PAGE_H__
#define __GLOBAL_PAGE_H__

#include "PageMain.h"
#include "PageUsb.h"
#define lv_font_all &lv_font_montserrat_30

class GlobalPage
{
public:
	static GlobalPage* Instance();
	static void Uninstance();

	PageMain* page_main() { return &page_main_; }
	PageUsb* page_usb() { return &page_usb_; }
private:
	GlobalPage();
	~GlobalPage();
private:
	static GlobalPage* instance_;

	PageMain page_main_;
	PageUsb page_usb_;
};

#endif // !__GLOBAL_PAGE_H__
