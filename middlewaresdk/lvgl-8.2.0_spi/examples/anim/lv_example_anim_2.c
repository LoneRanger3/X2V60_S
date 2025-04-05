#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SWITCH


static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

/**
 * Create a playback animation
 */
void lv_example_anim_2(void)
{

    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 300, 300);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_delay(&a, 0);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_repeat_delay(&a, 0);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a, anim_size_cb);
    lv_anim_start(&a);
    lv_anim_set_exec_cb(&a, anim_x_cb);
    lv_anim_set_values(&a, 10, 700);
    lv_anim_start(&a);


	obj = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj, LV_ALIGN_RIGHT_MID, 10, 0);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, obj);
    lv_anim_set_values(&b, 200, 200);
    lv_anim_set_time(&b, 1000);
    lv_anim_set_playback_delay(&b, 0);
    lv_anim_set_playback_time(&b, 1000);
    lv_anim_set_repeat_delay(&b, 0);
    lv_anim_set_repeat_count(&b, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&b, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&b, anim_size_cb);
    lv_anim_start(&b);
    lv_anim_set_exec_cb(&b, anim_x_cb);
    lv_anim_set_values(&b, 10, -800);
    lv_anim_start(&b);

	obj = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj, LV_ALIGN_TOP_MID, 10, 0);
	
	lv_anim_t c;
    lv_anim_init(&c);
    lv_anim_set_var(&c, obj);
    lv_anim_set_values(&c, 100, 200);
    lv_anim_set_time(&c, 1000);
    lv_anim_set_playback_delay(&c, 0);
    lv_anim_set_playback_time(&c, 1000);
    lv_anim_set_repeat_delay(&c, 0);
    lv_anim_set_repeat_count(&c, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&c, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&c, anim_size_cb);
    lv_anim_start(&c);
    lv_anim_set_exec_cb(&c, anim_y_cb);
    lv_anim_set_values(&c, 10, 500);
    lv_anim_start(&c);
}

#endif
