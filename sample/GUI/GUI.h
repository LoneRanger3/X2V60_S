#ifndef __GUI_H__
#define __GUI_H__

#include "lvgl.h"

class GUI
{
public:
	GUI();
	~GUI();

	int AddStyle(lv_obj_t* obj, lv_style_t* style, lv_style_selector_t selector = LV_PART_MAIN | LV_STATE_DEFAULT);
	
	lv_obj_t* AddListBtn(lv_obj_t* list, const char* txt = NULL,
		const void* left_icon = NULL, const void* right_icon = NULL, lv_coord_t h = 0);
	int SetListStyle(lv_obj_t* list, lv_coord_t w, lv_coord_t h,
		lv_style_t* list_style = NULL, lv_style_t* btn_style = NULL, lv_style_t* label_style = NULL);

	//以下是创建组件
	lv_obj_t* CreatePage(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, lv_style_t* style = NULL);
	lv_obj_t* CreatePage(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t bg_color, lv_coord_t radius,
		lv_coord_t border_width, const lv_font_t* font, lv_color_t text_color);

	lv_obj_t* CreateButton(lv_obj_t* parent, lv_coord_t w, lv_coord_t h,
		lv_event_cb_t event_cb = NULL, lv_event_code_t event_code = LV_EVENT_ALL, void* user_data = NULL,
		lv_style_t* style = NULL);

	lv_obj_t* CreateImgBtn(lv_obj_t* parent,
		lv_event_cb_t event_cb = NULL, lv_event_code_t event_code = LV_EVENT_ALL, void* user_data = NULL,
		const void* src_mid = NULL, const void* src_left = NULL, const void* src_right = NULL,
		lv_imgbtn_state_t state = LV_IMGBTN_STATE_RELEASED);

	lv_obj_t* CreateLabel(lv_obj_t* parent, const char* text,
		lv_coord_t width = 0, lv_text_align_t text_align = LV_TEXT_ALIGN_CENTER);

	lv_obj_t* CreateImage(lv_obj_t* parent, const void* img_path);

	lv_obj_t* CreateSwitch(lv_obj_t* parent, lv_coord_t w, lv_coord_t h,
		lv_event_cb_t event_cb = NULL, lv_event_code_t event_code = LV_EVENT_ALL, void* user_data = NULL);

	lv_obj_t* CreateRoller(lv_obj_t* parent, lv_coord_t w,
		const char* options, lv_roller_mode_t roller_mode = LV_ROLLER_MODE_NORMAL, uint8_t row_cnt = 4,
		lv_event_cb_t event_cb = NULL, lv_event_code_t event_code = LV_EVENT_ALL, void* user_data = NULL);

	lv_obj_t* CreateLine(lv_obj_t* parent, lv_coord_t w, const lv_point_t* points, uint16_t point_num,
		lv_color_t color, bool round = true);

	lv_obj_t* CreateCheckBox(lv_obj_t* parent, const char* text,
		lv_event_cb_t event_cb = NULL, lv_event_code_t event_code = LV_EVENT_ALL, void* user_data = NULL);

	lv_obj_t* CreateSlider(lv_obj_t* parent, lv_coord_t w, lv_coord_t h,
		lv_event_cb_t event_cb = NULL, lv_event_code_t event_code = LV_EVENT_ALL, void* user_data = NULL,
		int32_t min = 0, int32_t max = 100);

	lv_obj_t* CreateDropdown(lv_obj_t* parent, const char* options,
		lv_event_cb_t event_cb = NULL, lv_event_code_t event_code = LV_EVENT_ALL, void* user_data = NULL,
		lv_dir_t dir = LV_DIR_BOTTOM, const void* symbol = LV_SYMBOL_DOWN);
private:
	lv_style_selector_t selector_;
};


#endif // !__GUI_H__
