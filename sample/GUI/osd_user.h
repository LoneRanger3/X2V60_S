#ifndef __OSD_USER_H__
#define __OSD_USER_H__
#include "set_config.h"
#include <stdio.h>

int osd_data_init(void);
#if GPS_EN
int osd_upgrade_all_data(char* str);
int clean_gps_osd_data(char tmp);
#endif
#endif