#ifndef __GPS_H_
#define __GPS_H_
#include "lvgl.h"
extern char write_start;
extern char write_startback;
extern void get_gps_data(const char *enc_file);
extern void get_gps_back_data(const char *enc_file);
extern void GPS_Player();
struct gps_time
{
    int day;
    int month;
    int year;
    int hour;
    int min;
    int sec;
    int msec;
};

struct rmc//推荐最小定位信息
{
    unsigned int have;           // 此语句是否存在标志
    char state;                  // 定位状态, V:未定位，A:定位
    struct gps_time time;        // UTC原始时间(0时区)
    float lat;                  // 纬度
    char lat_direct;             // 纬度方向 S N
    float lon;                  // 经度
    char lon_direct;             // 经度方向 E W
    double speed;                // 对地航速,海里/小时
    double speed_direct;         // 对地航向，相当于二维罗盘
    double magnetic_decl;        // 磁偏角
    double magnetic_decl_direct; // 磁偏角方向
};

struct gsv//可见卫星信息
{
    unsigned int have; // 此语句是否存在标志
    int total_index;   // 本次gsv语句总数目
    int cur_index;     // 当前gsv语句序号
    int total_num;     // 可见卫星总数
    int PRN[20];       // 卫星PRN码编号
    int Elev[20];      // 卫星仰角
    int Azim[20];      // 卫星方位角
    int SNR[20];       // 信噪比
    int available_num;     // 可用卫星总数，可见卫星总数-信噪比为0的
};

struct gsa//当前卫星信息
{
    unsigned int have; // 此语句是否存在标志
    char pospt_mode;   // 定位模式, M:手动，A:自动
    char pospt_type;   // 定位类型, 1:无定位信息，2:二维定位，3:三维定位
    int PRN[20];       // 第1信道~第12信道使用的卫星PRN码编号
    double HDOP;       // 水平精度因子
    double VDOP;       // 垂直精度因子
};

struct gll//地理定位信息
{
    unsigned int have; // 此语句是否存在标志
    double lat;                  // 纬度
    char lat_direct;             // 纬度方向 S N
    double lon;                  // 经度
    char lon_direct;             // 经度方向 E W
    double time;                // 定位时间 hhmmss.sss
    char state;                  // 数据状态, V:未定位，A:定位
};

struct vtg//地面速度信息
{
    unsigned int have; // 此语句是否存在标志
    double speed_direct;         // 对地航向，相当于二维罗盘
    char speed_direct_rs;        //真北参考系，数据为'T'
    double magnetic_decl;        // 磁偏角
    char magnetic_decl_rs;        // 磁北参考系，数据为'M'
    double speed;                // 对地航速,海里/小时
    char speed_unit;            //速度单位 N:节(即海里/小时)
    double speed1;                // 水平运动速度
    char speed1_unit;            //速度单位 K:Km/h
};

struct gga//GPS定位信息
{
    unsigned int have; // 此语句是否存在标志
    struct gps_time time;        // UTC原始时间(0时区)
    double lat;                  // 纬度
    char lat_direct;             // 纬度方向 S N
    double lon;                  // 经度
    char lon_direct;             // 经度方向 E W
    char state;                  // 定位质量指示, 0:未定位；1:无差分，SPS模式，定位有效；2:有差分，SPS模式，定位有效；3:PPS模式，定位有效
    char use_num;                // 使用卫星数量
    double Hori_accu;            //水平精度
    double height;              //海平面高度
    char height_unit;           //高度单位，一般为M(米)
    double height1;              //大地椭球面相对于海平面高度
    char height_unit1;           //高度单位，一般为M(米)
    int DRT;                     //差分修订时间，即从最近一次接收到差分信号开始的秒数，若不是差分定位，此项为空
    int DRT_ID;                  //差分参考基站ID号    
};


struct gps_info
{
    struct rmc GNRMC;
    struct rmc GPRMC;
    struct gsv GPGSV;
    struct gsv BDGSV;
    struct gga GPGGA;
    struct gsa GPGSA;
    

    int PRN[40];       // 卫星PRN码编号
    int Elev[40];      // 卫星仰角
    int Azim[40];      // 卫星方位角
    int SNR[40];       // 信噪比
    double speed;      // 速度,Km/h
    struct gps_time time; 
};



/*
 * 时区格式
 */
#define GMT_1200W 11200 // GMT-12:00
#define GMT_1100W 11100
#define GMT_1000W 11000
#define GMT_0900W 10900
#define GMT_0800W 10800
#define GMT_0700W 10700
#define GMT_0600W 10600 // GMT-06:00
#define GMT_0500W 10500
#define GMT_0400W 10400
#define GMT_0300W 10300
#define GMT_0200W 10200
#define GMT_0100W 10100
#define GMT_0000W 0 // GMT
#define GMT_0000E 0 // GMT
#define GMT_0100E 100
#define GMT_0200E 200
#define GMT_0300E 300
#define GMT_0330E 330
#define GMT_0400E 400
#define GMT_0430E 430
#define GMT_0500E 500
#define GMT_0530E 530
#define GMT_0545E 545
#define GMT_0600E 600 // GMT+06:00
#define GMT_0630E 630 // GMT+06:30
#define GMT_0700E 700
#define GMT_0800E 800 // GMT+08:00
#define GMT_0900E 900
#define GMT_0930E 930
#define GMT_1000E 1000
#define GMT_1100E 1100
#define GMT_1200E 1200 // GMT+12:00
#define GMT_1300E 1300 // GMT+13:00


extern unsigned char gps_online_flag;//gps定位标志 1:已定位
extern unsigned char gps_insert_flag;//gps插入标志 1:已插入
extern struct gps_info gps;

int gps_data_rmc(char *gps_data_frame,struct rmc *_rmc_);
int gps_data_gsv(char *gps_data_frame,struct gsv *_gsv_);
int gps_data_gga(char *gps_data_frame,struct gga *_gga_);
int gps_data_gsa(char *gps_data_frame,struct gsa *_gsa_);

int gps_upgrade_sys_time(struct gps_time gpstime, int scr_zoon,int tar_zoon);//时区变换;
int gps_snr_sort();//信噪比处理

void UartGpsThreadStart();
void gps_create_ui(int tmp);
void update_hardware_time(struct gps_time time1);
extern void update_gps_infomation_ui(lv_timer_t* timer);
#endif
