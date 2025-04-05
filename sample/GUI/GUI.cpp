#include "GUI.h"
#include "Log.h"

GUI::GUI():selector_(LV_PART_MAIN | LV_STATE_DEFAULT)
{
	
}

GUI::~GUI()
{

}

int GUI::AddStyle(lv_obj_t* obj, lv_style_t* style, lv_style_selector_t selector)
{
	if (!obj || !style) {
		XMLogE("AddStyle error");
		return -1;
	}

	lv_obj_add_style(obj, style, selector);
	return 0;
}

lv_obj_t* GUI::CreatePage(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, lv_style_t* style)
{
	if (!parent || w < 0 || h < 0) {
		XMLogE("CreatePage error");
		return NULL;
	}
	
	lv_obj_t* page = lv_obj_create(parent);
	lv_obj_set_size(page, w, h);
	if (style) {
		lv_obj_add_style(page, style, selector_);
	}
	
	//默认去掉页面的滚动条和填充
	lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_all(page, 0, selector_);

	return page;
}

lv_obj_t* GUI::CreatePage(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t bg_color,
	lv_coord_t radius, lv_coord_t border_width, const lv_font_t* font, lv_color_t text_color)
{

	lv_obj_t* page = lv_obj_create(parent);
	lv_obj_set_size(page, width, height);
	lv_obj_set_style_bg_color(page, bg_color, selector_);
	lv_obj_set_style_radius(page, radius, selector_);
	lv_obj_set_style_border_width(page, border_width, selector_);
	lv_obj_set_style_text_font(page, font, selector_);
	lv_obj_set_style_text_color(page, text_color, selector_);
	lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_all(page, 0, 0);

	return page;
}

lv_obj_t* GUI::CreateButton(lv_obj_t* parent, lv_coord_t w, lv_coord_t h,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data,
	lv_style_t* style)
{
	if (!parent || w < 0 || h < 0) {
		XMLogE("CreateButton error");
		return NULL;
	}

	lv_obj_t* button = lv_btn_create(parent);
	lv_obj_set_size(button, w, h);

	if (event_cb) {
		lv_obj_add_event_cb(button, event_cb, event_code, user_data);
	}

	if (style) {
		lv_obj_add_style(button, style, selector_);
	}

	//因为按钮的阴影比较难看，所以默认去掉
	lv_obj_set_style_shadow_width(button, 0, selector_);

	return button;
}

lv_obj_t* GUI::CreateImgBtn(lv_obj_t* parent,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data,
	const void* src_mid, const void* src_left, const void* src_right, lv_imgbtn_state_t state)
{
	if (!parent || !src_mid) {
		XMLogE("CreateImgBtn error");
		return NULL;
	}

	lv_obj_t* imgbtn = lv_imgbtn_create(parent);
	lv_imgbtn_set_src(imgbtn, state, src_left, src_mid, src_right);

	if (event_cb) {
		lv_obj_add_event_cb(imgbtn, event_cb, event_code, user_data);
	}

	//默认点击效果
	lv_obj_set_style_img_recolor_opa(imgbtn, LV_OPA_30, LV_STATE_PRESSED);

	return imgbtn;
}

lv_obj_t* GUI::CreateLabel(lv_obj_t* parent, const char* text, lv_coord_t width, lv_text_align_t text_align)
{
	if (!parent || !text) {
		XMLogE("CreateLabel error");
		return NULL;
	}

	lv_obj_t* label = lv_label_create(parent);
	lv_label_set_text(label, text);
	if (width > 0) {
		lv_obj_set_width(label, width);
		lv_obj_set_style_text_align(label, text_align, selector_);
	}

	return label;
}

lv_obj_t* GUI::CreateImage(lv_obj_t* parent, const void* img_path)
{
	if (!parent || !img_path) {
		XMLogE("CreateImage error");
		return NULL;
	}

	lv_obj_t* img = lv_img_create(parent);
	lv_img_set_src(img, img_path);

	return img;
}

lv_obj_t* GUI::CreateSwitch(lv_obj_t* parent, lv_coord_t w, lv_coord_t h,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data)
{
	if (!parent || w < 0 || h < 0) {
		XMLogE("CreateSwitch error");
		return NULL;
	}

	lv_obj_t* sw = lv_switch_create(parent);
	lv_obj_set_size(sw, w, h);
	if (event_cb) {
		lv_obj_add_event_cb(sw, event_cb, event_code, user_data);
	}
	
	return sw;
}

lv_obj_t* GUI::CreateRoller(lv_obj_t* parent, lv_coord_t w,
	const char* options, lv_roller_mode_t roller_mode, uint8_t row_cnt,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data)
{
	if (!parent || w < 0 || !options) {
		XMLogE("CreateRoller error");
		return NULL;
	}

	lv_obj_t* roller = lv_roller_create(parent);
	lv_obj_set_width(roller, w);
	lv_roller_set_options(roller, options, roller_mode);
	lv_roller_set_visible_row_count(roller, row_cnt);
	if (event_cb) {
		lv_obj_add_event_cb(roller, event_cb, event_code, user_data);
	}
	
	return roller;
}

lv_obj_t* GUI::CreateLine(lv_obj_t* parent, lv_coord_t w, const lv_point_t* points, uint16_t point_num,
	lv_color_t color, bool round)
{
	if (!parent || w < 0 || !points || point_num < 0) {
		XMLogE("CreateLine error");
		return NULL;
	}

	lv_obj_t* line = lv_line_create(parent);
	lv_line_set_points(line, points, point_num);
	lv_obj_set_style_line_width(line, w, selector_);
	lv_obj_set_style_line_color(line, color, selector_);
	lv_obj_set_style_line_rounded(line, round, selector_);
	
	return line;
}

lv_obj_t* GUI::CreateCheckBox(lv_obj_t* parent, const char* text,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data)
{
	if (!parent || !text) {
		XMLogE("CreateCheckBox error");
		return NULL;
	}

	lv_obj_t* cb = lv_checkbox_create(parent);
	lv_checkbox_set_text(cb, text);
	if (event_cb) {
		lv_obj_add_event_cb(cb, event_cb, event_code, user_data);
	}

	return cb;
}

lv_obj_t* GUI::AddListBtn(lv_obj_t* list, const char* txt,
	const void* left_icon, const void* right_icon, lv_coord_t h)
{
	if (!list) {
		XMLogE("AddListBtn error");
		return NULL;
	}

	LV_LOG_INFO("begin");
	lv_obj_t* obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
	lv_obj_class_init_obj(obj);

	if (h > 0) {
		lv_obj_set_size(obj, LV_PCT(100), h);
	}
	else {
		lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
	}
	
	lv_obj_t* left_img = NULL;
#if LV_USE_IMG == 1
	if (left_icon) {
		left_img = lv_img_create(obj);
		lv_img_set_src(left_img, left_icon);
		lv_obj_align(left_img, LV_ALIGN_LEFT_MID, 0, 0);
	}

	if (right_icon) {
		lv_obj_t* right_img = lv_img_create(obj);
		lv_img_set_src(right_img, right_icon);
		lv_obj_align(right_img, LV_ALIGN_RIGHT_MID, 0, 0);
	}
#endif

	if (txt) {
		lv_obj_t* label = lv_label_create(obj);
		lv_label_set_text(label, txt);
		if (left_img) {
			lv_obj_align_to(label, left_img, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
		}
		else {
			lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
		}
	}

	return obj;
}

int GUI::SetListStyle(lv_obj_t* list, lv_coord_t w, lv_coord_t h,
	lv_style_t* list_style, lv_style_t* btn_style, lv_style_t* label_style)
{
	if (!list || w < 0 || h < 0) {
		XMLogE("SetListStyle error");
		return -1;
	}

	lv_obj_set_size(list, w, h);
	if (list_style) {
		lv_obj_add_style(list, list_style, selector_);
	}

	for (int i = 0; i < lv_obj_get_child_cnt(list); i++)
	{
		lv_obj_t* item = lv_obj_get_child(list, i);
		if (btn_style && lv_obj_check_type(item, &lv_list_btn_class)) {
			AddStyle(item, btn_style, selector_);
		}

		if (label_style && lv_obj_check_type(item, &lv_list_text_class)) {
			AddStyle(item, label_style, selector_);
		}
	}

	return 0;
}

lv_obj_t* GUI::CreateSlider(lv_obj_t* parent, lv_coord_t w, lv_coord_t h,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data,
	int32_t min, int32_t max)
{
	if (!parent || w < 0 || h < 0) {
		XMLogE("CreateSlider error");
		return NULL;
	}

	lv_obj_t* slider = lv_slider_create(parent);
	lv_obj_set_size(slider, w, h);
	lv_slider_set_range(slider, min, max);
	if (event_cb) {
		lv_obj_add_event_cb(slider, event_cb, event_code, user_data);
	}
	
	return slider;
}

lv_obj_t* CreateDropdown(lv_obj_t* parent, const char* options,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data,
	lv_dir_t dir, const void* symbol)
{
	if (!parent || !options) {
		XMLogE("CreateDropdown error");
		return NULL;
	}

	lv_obj_t* dd = lv_dropdown_create(parent);
	lv_dropdown_set_options(dd, options);
	lv_dropdown_set_dir(dd, dir);
	lv_dropdown_set_symbol(dd, symbol);
	if (event_cb) {
		lv_obj_add_event_cb(dd, event_cb, event_code, user_data);
	}

	return dd;
}