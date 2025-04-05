/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "lvgl.h"
#include "global_data.h"
#include "cr_lcd.h"
#ifndef WIN32
#include <sys/time.h>
#endif

/*********************
 *      DEFINES
 *********************/
#define CHECK_RET(express,name) \
		do{ \
			XM_S32 Ret; \
			Ret = express; \
			if (XM_SUCCESS != Ret) \
			{ \
				printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret); \
				return; \
			} \
		}while(0)
			
/**********************
 *      TYPEDEFS
 **********************/
typedef struct SendStreamThreadParam{
	SIZE_S stImageSize;
	PIXEL_FORMAT_E enPixFormat;
	XM_CHAR cFileName[128];
	XM_S32 s32MilliSec;
	VO_LAYER VoLayer;
	XM_S32 s32ChnCnt;
}ThreadParam;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);


/**********************
 *      MACROS
 **********************/

lv_disp_drv_t* lv_port_disp_init()
{
	static lv_disp_draw_buf_t draw_buf_dsc_3;
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_VER_RES * MY_DISP_HOR_RES);   /*Initialize the display buffer*/
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/
    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

	disp_drv.physical_hor_res = MY_DISP_HOR_RES;
	disp_drv.physical_ver_res = MY_DISP_VER_RES;

	disp_drv.screen_transp    = LV_COLOR_SCREEN_TRANSP;
	disp_drv.dpi              = LV_DPI_DEF;
	disp_drv.color_chroma_key = LV_COLOR_CHROMA_KEY;
    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;
    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_3;
	disp_drv.rotated = 1;
	disp_drv.sw_rotate = 1;
    /*Required for Example 3)*/
	disp_drv.full_refresh = 0;
    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);

	return &disp_drv;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	//因放关机画面的时候刷新屏幕会覆盖关机画面，所以停止刷新屏幕
	if (GlobalData::Instance()->stop_screen_refresh_) {
		lv_disp_flush_ready(disp_drv);
		return;
	}

    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    //int32_t x;
    int32_t y;
	//static struct timeval start, end;
	//gettimeofday(&end, NULL);
	//printf(" time %lu us\n", (unsigned long)(end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);
	//gettimeofday(&start, NULL);
	if(area->x2<0||area->y2<0||area->x1>disp_drv->hor_res-1||area->y1 >disp_drv->ver_res-1)
	{        
		lv_disp_flush_ready(disp_drv);
		return;    
	}
#if 0
	uint16_t w = lv_area_get_width(area);
	for (y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
		lcd_data_s lcd_data = {0};
		lcd_data.x = area->x1;
		lcd_data.y = y;
		lcd_data.width = w;
		lcd_data.height = 1;
		lcd_data.data = (uint8_t*)color_p;
		lcd_data.size = w * 2;
		LIBCR_LCD_Display(&lcd_data);
		color_p += w;
	}
#endif	
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
