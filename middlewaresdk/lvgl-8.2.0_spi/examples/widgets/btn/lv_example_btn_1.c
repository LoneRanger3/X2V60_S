#include "../../lv_examples.h"
#if LV_USE_BTN && LV_BUILD_EXAMPLES

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void lv_example_btn_1(void)
{
	lv_obj_t * label;


    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_size(btn1, 100, 100);
	lv_obj_set_style_shadow_width(btn1, 0, 0);

	

	static lv_style_t s;
	lv_style_init(&s);
	//lv_style_set_bg_opa(&s, LV_OPA_0);
	lv_style_set_bg_color(&s, lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_bg_img_src(&s, "A:/home/lushilong/General/resource/th.jpg");
	lv_style_set_img_opa(&s, LV_OPA_0);
	

	lv_obj_add_style(btn1, &s, 0);
	
	//lv_obj_set_style_bg_img_src(btn1, "A:/home/lushilong/General/resource/back.jpg", 0);

/*
	lv_obj_set_style_bg_opa(btn1, LV_OPA_0, 0);
	lv_obj_set_style_border_width(btn1, 0, 0);
	
	lv_obj_set_pos(btn1, 100, 100);

	
    label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
*/

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 150);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Toggle");
    lv_obj_center(label);
}

LV_IMG_DECLARE(img_cogwheel_argb);

void lv_example_btn_4()
{
	lv_obj_t* btn1 = lv_obj_create(lv_scr_act());
	//lv_obj_center(btn1);

	lv_obj_t* icon = lv_img_create(lv_scr_act());
	lv_img_set_src(icon,"A:/home/lushilong/General/resource/th.jpg");
	lv_obj_center(icon);
	lv_img_set_zoom(icon, 128);

	static lv_style_t style_normal;
	lv_style_init(&style_normal);
	lv_style_set_size(&style_normal, 160);
	lv_style_set_bg_color(&style_normal, lv_color_black());
	//lv_style_set_bg_img_src(&style_normal, "A:/home/lushilong/General/resource/th.jpg");

	static lv_style_t style_pressed;
	lv_style_init(&style_pressed);
	lv_style_set_size(&style_pressed, 200);
	//lv_style_set_bg_img_src(&style_pressed, "A:/home/lushilong/General/resource/th.jpg");

	lv_obj_add_style(btn1, &style_normal, LV_STATE_DEFAULT);
	lv_obj_add_style(btn1, &style_pressed, LV_STATE_PRESSED);
}
#endif
