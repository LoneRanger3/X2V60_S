#include "../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

static void slider_event_cb(lv_event_t * e);
static lv_obj_t * slider_label;

/**
 * A default slider with a label displaying the current value
 */
 static void set_angle(void * obj, int32_t v)
{
    lv_arc_set_value(obj, v);
}
void lv_example_slider_1(void)
{
	 /*Create an Arc*/
	lv_obj_t * arc = lv_arc_create(lv_scr_act());
	lv_arc_set_rotation(arc, 270);
	lv_arc_set_bg_angles(arc, 0, 360);
	lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
	lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
	lv_obj_align(arc, LV_ALIGN_LEFT_MID, 0, 0);

	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, arc);
	lv_anim_set_exec_cb(&a, set_angle);
	lv_anim_set_time(&a, 1000);
	lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);    /*Just for the demo*/
	lv_anim_set_repeat_delay(&a, 500);
	lv_anim_set_values(&a, 0, 100);
	lv_anim_start(&a);
	
    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_align(slider, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    /*Create a label below the slider*/
    slider_label = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label, "0%");
	lv_bar_set_value(slider,20,LV_ANIM_ON);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

#endif
