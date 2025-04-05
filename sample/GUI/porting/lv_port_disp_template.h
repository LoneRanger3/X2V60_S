/**
 * @file lv_port_disp_templ.h
 *
 */

 /*Copy this file as "lv_port_disp.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_DISP_TEMPL_H
#define LV_PORT_DISP_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#endif
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vo.h"
#include "mpi_vpss.h"
#include "mpi_vdec.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_disp_drv_t* lv_port_disp_init();
/**********************
 *      MACROS
 **********************/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_TEMPL_H*/

#endif /*Disable/Enable content*/
