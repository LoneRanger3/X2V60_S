/*************************************************************************
 * Copyright (C) 2023 Xmsilicon Tech. Co., Ltd.
 * @File Name: cr_lcd.h
 * @Description: 
 * @Author: songliuyang
 * @Created Time: 2023.01.28
 * @Modification: 
 ************************************************************************/

#ifndef __CR_LCD_H__
#define __CR_LCD_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct _lcd_data 
{
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	unsigned char *data;
	unsigned int size;
} lcd_data_s;

typedef enum
{
	HSD015,
	HSD096,
	JT60559,
	ZXYQ028KG002,
	HS028CS12T18N,
}spi_lcd_type_e;

typedef struct lcd_param 
{
	int init_flag;
	int spi_cs;
	int lcd_pwd;
	int lcd_rst;
	int lcd_width;
	int lcd_height;
	spi_lcd_type_e type;
}lcd_param_s;



/******************************************************************************
 *	函数: LIBCR_LCD_Open
 *	描述: 打开LCD
 *	参数:	lcd_param_s
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int LIBCR_LCD_Open(lcd_param_s *pstLcdPara);



/******************************************************************************
 *	函数: LIBCR_LCD_Close
 *	描述: 关闭LCD
 *	参数:	无
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int LIBCR_LCD_Close(void);


/******************************************************************************
 *	函数: LIBCR_LCD_Display
 *	描述: LCD显示接口
 *	参数:	lcd_data	要显示的数据
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int LIBCR_LCD_Display(lcd_data_s *lcd_data);


/******************************************************************************
 *	函数: LIBCR_LCD_FillColor
 *	描述: LCD全屏显示同一颜色
 *	参数:	color    RGB565 颜色值
 *	返回值: 成功返回0, 失败返回-1
 *****************************************************************************/
int LIBCR_LCD_FillColor(unsigned short color);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CR_LCD_H__ */
