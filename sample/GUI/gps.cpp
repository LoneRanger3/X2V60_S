#include "gps.h"
#include "string.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Log.h"
#include "io.h"
#include "../periphery/uart.h"
#include "XMThread.h"
#include <time.h>
#include <unistd.h>
#include "global_data.h"
#include "global_page.h"
#include <string>
#include "osd_user.h"
#include "SunAlgorithm.h"
#include "TimeUtil.h"
#if GPS_EN
//SD卡中放入GPS.BIN则显示GPS相关信息:20组信噪比数据、经纬度、速度、UTC时间、定位状态
#define SIX_APP 1// 1--匹配6侦探
#define COMPAT_PC// 兼容PC工具
#define BUF_DATA_LEN_MAX       4096
//#define SIMUlonION_GPS //模拟GPS数据
#define GPS_DATA_NUM 600//需要大于或等于循环录像最长时长

static unsigned char gps_data_buf[BUF_DATA_LEN_MAX] ={0};
static int gps_data_len =0;
unsigned char gps_online_flag = 0;//gps定位标志 1:已定位
unsigned char gps_insert_flag = 0;//gps插入标志 1:已插入
unsigned char gps_online_flag_last = 2;
unsigned char gps_insert_flag_last = 2;
static char gps_last_data_buf[BUF_DATA_LEN_MAX]={0};
static int gps_last_data_len=0;
struct gps_info gps={0};
 static int gxbd_flag=5;
 static int gxbd_rmc=5;

char header[20];
char nCount[4];
char chInfomation[128]={0};
unsigned char chOutput[GPS_DATA_NUM][132]={0};
unsigned char chOutputback[GPS_DATA_NUM][132]={0};

#ifdef COMPAT_PC
char chInfomation_pc[128]={0};
unsigned char chOutput_pc[GPS_DATA_NUM][132]={0};
unsigned char chOutputback_pc[GPS_DATA_NUM][132]={0};
#endif

#if 1//X2V60_S_DEBUG1
unsigned char clean_gps_osd_flag = 0;//清除gps水印标志 1:清除
static int last_clean_gps_osd_flag=-1;
unsigned char gps_online_old_flag = 0;//gps定位标志 1:已定位
#endif

int gps_count=0;
int gps_countback=0;
char write_start=0;
char write_startback=0;
int gps_second = 0;
float flat_decode;  //解密后的纬度
float flon_decode;  //解密后的经度
extern int g_sd_status;
void GPS_Player()
{
    static bool gps_player_write_flag=true;
    if (gps_player_write_flag && g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
      FILE* fp = fopen("/mnt/tfcard/GPS_Player_Download.txt", "r");
        if (fp == NULL) {
            fp = fopen("/mnt/tfcard/GPS_Player_Download.txt", "w+");
            if (!fp) {
                perror("fopen");
                return;
            }

            char buf[512] = { 0 };
            sprintf(buf, "Thanks for choosing our product.\n\n\
            The product you have purchased is competabe with GPS track function, you can trace the exact GPS location in your video.\n\n\
            To trace the GPS log, you will need to play the video file with customized GPS player, enter our offical website to get it,\n\
            Or you can enter the following website to get it.\n\n\
            For Windows :https://pcgps.ce2ce.com/hit/hitplayer.html");
            fputs(buf, fp);
        }
         fclose(fp);  
         gps_player_write_flag=false;
    }else {
        gps_player_write_flag=true;
    }
}

// 判断语句是否完整有效
static bool CheckGpsValid(char* rawGps)
{
    bool bCheck = false;
    unsigned char s_verify;
    unsigned char tempValid;
    char* pstr = NULL;
    if(!rawGps){
        XMLogW("----CheckGpsValid str null");
    }

    if(strlen(rawGps)<10){
        XMLogW("----CheckGpsValid str loss:%s",rawGps);
    }

    char* s_start = strstr(rawGps, "$");
    char* s_end = strstr(rawGps, "*");

    if((s_start==NULL) || (s_end==NULL)){//插拔GPS时会有一些异常的数据，如此处不做判断就会有概率导致空指针从而导致设备卡死重启
        //XMLogW("----CheckGpsValid str err:%s",rawGps);
        return false;
    }
    if(s_end - s_start < 3){
        XMLogW("----CheckGpsValid str abnormal:%s",rawGps);
        return false;        
    }

    s_verify = *(s_end + 1);

    if (s_verify >= 'A') s_verify = s_verify - 'A' + 10;
    else s_verify = s_verify = s_verify - '0';

    tempValid = *(s_end + 2);

    if (tempValid >= 'A') tempValid = tempValid - 'A' + 10;
    else tempValid = tempValid = tempValid - '0';
    s_verify *= 16;
    s_verify += tempValid;
    unsigned char xor_verify = *(s_start + 1);

    for (pstr = s_start + 2; pstr < s_end; pstr++) {
        //xor the key information
        xor_verify ^= *pstr;
    }

    if(xor_verify == s_verify) {
       bCheck = true;
    }

    return bCheck;
}
//int gps_data_tp11(char *rawGps,float gx,float gy,float gz)
int gps_data_tp11(char *rawGps)
{
    char *pDataCheck = NULL;
    memset(chInfomation,0,128);
    if (strstr(rawGps, "GXBDT"))
    {
        pDataCheck = strstr(rawGps, "GXBDT");
        pDataCheck = strstr(pDataCheck, ",");
        pDataCheck ++;
        // 是否定位成功
        if (pDataCheck && strstr(rawGps, ",A,") && (strlen(pDataCheck)) < 120)
        {
            //成功
            pDataCheck = strstr(pDataCheck, ",");
            pDataCheck ++ ;
            char *pEnd = strstr(pDataCheck, "*");
            if (pEnd) {
                memset(chInfomation,0,sizeof(chInfomation));
                memcpy(chInfomation, pDataCheck, pEnd - pDataCheck);
               // sprintf(&chInfomation[strlen(chInfomation)],",%.02f,%.02f,%.02f",gx, gy, gz);
                if(write_start)
                {
                    MakeEncryptDataBlock(gps_second, (char*)chInfomation, strlen(chInfomation),chOutput[gps_count]);
                    gps_count ++;
                }
                if(write_startback)
                {
                    MakeEncryptDataBlock(gps_second,chInfomation ,strlen(chInfomation),chOutputback[gps_countback]);
                    gps_countback++;
                }
            }
        }
    }
    return 0;
}

char nema_comma_num(char *buf,int *table,int *num)
{
    int len=0,i=0,j=0,k=0,err=0;
    len = strlen(buf);
//    printf("======nema_comma_num:%d,%s\n",len,buf);
    for(i=0,j=0;i<len;i++){
        if((buf[i])==','){
            table[j] = i;
//            printf("*********nema_comma_num:%d,%d\n",i,_table_[j]);
            j++;
        }
    }
    *num = j;
//    for(i=0;i<j;i++){
//        printf(">>>>%d,%d,%d\n",j,i,table[i]);
//    }
    return j;
}


using namespace std;
#ifdef LVGL_SIM_VISUAL_STUDIO
const char* DevGpsFileName = _SYSTEM_ROOT_PATH_"sample/custom/tfcard/GPS_MONI.txt";
#else
const char* DevGpsFileName = "/mnt/tfcard/GPS_MONI.txt";
#endif
extern int g_sd_status;
int get_gps_moni_data(unsigned char *data,int get_len)
{
    static FILE* gps_file = NULL;
    static const char* gps_start = "$GPGGA,";
    static const char* gps_end = "$GNZDA,";
    static int first = 0;
    unsigned char gps_data[BUF_DATA_LEN_MAX] = { 0 };
    int copy_len = 0;
    static int64_t t1 = 0;
    static int64_t t2 = 0;
    static int64_t t1_t2 = 0;
    if(g_sd_status != XM_SD_NORMAL){
        return -3;
    }
    t2 = GetTickTime();
    t1_t2 = t2 - t1;
    if (t1_t2 < 900) {//1秒来一次
        return -2;
    }
    t1 = t2;

    if (first) {
        if (!gps_file) {
            return -1;
        }
    }
    first = 1;
    if (!gps_file) {
        gps_file = fopen(DevGpsFileName, "r");
    }
    if (gps_file == NULL) {
        return -2;
    }
    int max_len = 256;
    char read_line[300] = { 0 };
    int statr = 0, read_line_end = 0;
    while (fgets(read_line, max_len, gps_file)) {//获取一行数据
        if(!strncmp((const char *)read_line, gps_start, 6)){ //开始头
            statr = 1;
        }
        if (!strncmp((const char *)read_line, gps_end, 6)) {//结束头
            statr = 2;
        }
        if (statr == 1) {
            memcpy(gps_data+ copy_len, read_line,strlen(read_line));
            copy_len += strlen(read_line);
        }
        else if (statr == 2) {
            memcpy(gps_data + copy_len, read_line, strlen(read_line));
            copy_len += strlen(read_line);
            memcpy(data, gps_data, copy_len);
            return copy_len;
        }
    }
    fclose(gps_file);
    gps_file = NULL;
    gps_file = fopen(DevGpsFileName, "r");
    return -3;
}

int get_gps_total_len = 0;
//int uart_gps_hander_thread()
void uart_gps_hander_thread(lv_timer_t* timer)
{
	int len = 0 , len1 = 0;
    static int cnt = 0;
	unsigned char data[BUF_DATA_LEN_MAX] ={0};
    static int insert_cnt = 0;
    if(timer){
        lv_timer_pause(timer);
    }    
  
	if (1) {   //1
		//usleep(200*1000);
        #ifdef SIMUlonION_GPS
        len = get_gps_moni_data(data,2048);
        #else
        len = Uart1RecvData(data,2048);
        #endif

		if(len>0){
            len1 = strlen((const char*)data);
            get_gps_total_len += len1;
            insert_cnt = 0;
            if((gps_data_len+len1) > BUF_DATA_LEN_MAX){
                memset(gps_data_buf,0,gps_data_len);
                gps_data_len  = 0;
            }
			memcpy(gps_data_buf+gps_data_len,data,len1);
            gps_data_len += len1;
            // if(len != len1){
            //     XMLogE("=========%d: %d, %d, [%s]\n",len,gps_data_len,len1,data);
            // }
            //extern void network_sys_log_data_savecash(unsigned char *data, int len, int set);//除指令之外的数据需要保存发送给网络则对set进行设置
	        //network_sys_log_data_savecash(data,len,2);
            // XMLogI("=========%d: %d, [%s]\n",len,strlen((const char*)data),data);
            // memset(data,0,2048);
        }else{
            //XMLogI("=========%d: %d\n",len,insert_cnt);
            insert_cnt++;
            if(insert_cnt>=5*2){
                insert_cnt = 0;
                memset(&gps, 0, sizeof(struct gps_info));
                if(gps_data_len){
                    memset(gps_data_buf,0,gps_data_len);
                }
                if(gps_last_data_len){
                    memset(gps_last_data_buf,0,gps_last_data_len);
                }
                gps_online_flag = 0;//gps定位标志 1:已定位
                if(gps_insert_flag){
                   clean_gps_osd_data(0);
                   gps_insert_flag = 0;//gps插入标志 1:已插入
                }

                gps_online_flag_last = 2;
                gps_insert_flag_last = 2;
                gps_data_len = 0;
                gps_last_data_len = 0;
                cnt = 0;
                if(timer){
                    lv_timer_resume(timer);
                }
                return;
            }
        }
        int temp=0;
        char gps_soce_data_buf[BUF_DATA_LEN_MAX]={0};
        int i=0,j=0,max_index=0;
        int temp1=0,temp2=0,temp3=0;
        char table[32][100]={0};
        cnt++;
        if(gps_data_len && cnt>=1){
            cnt = 0;
            if(gps_last_data_len && ((gps_last_data_len+gps_data_len)<BUF_DATA_LEN_MAX)){
                memcpy(gps_soce_data_buf,gps_last_data_buf,gps_last_data_len);//上一次不全的语句拼接完整
                memcpy(gps_soce_data_buf+gps_last_data_len,gps_data_buf,gps_data_len);
                memset(gps_last_data_buf,0,gps_last_data_len);
            }else{
                gps_last_data_len = 0;
                 memcpy(gps_soce_data_buf,gps_data_buf,gps_data_len);
            }
            memset(gps_data_buf,0,gps_data_len);
            gps_data_len  =0;

            len=strlen(gps_soce_data_buf);
        	//XMLogI("========start:%d, [%s]\n",len,gps_soce_data_buf);
            for(temp1=0,i=0,j=0;i<len;i++){
                if(gps_soce_data_buf[i]=='\n'){
                    temp2=i;
                     if(temp2-temp1 >= 100){
                        memcpy(data,gps_soce_data_buf+temp1,temp2-temp1);
                        data[temp2-temp1]=0;
                        //XMLogE("=====err2====%d,%d,%d,%d,%d,%s\n",len,gps_data_len, j, temp2,temp2-temp1,data);
                        temp1=i;
                        continue;
                    }
                    memcpy(table[j],gps_soce_data_buf+temp1,temp2-temp1);
        			//XMLogI("=========%d,%s\n",j,table[j]);
                    temp1=i+1;
                    j++;
                    max_index = j;
                    if(j>=32){
                        break;
                    }
                }
                if(gps_soce_data_buf[i]=='\0'){
                    break;
                }
            }

            gps_last_data_len = len-temp1;
            if(gps_last_data_len){
                memcpy(gps_last_data_buf,gps_soce_data_buf+temp1,gps_last_data_len);//若最后一次语句不全则保存
                 gps_last_data_buf[gps_last_data_len] = '\0';
            }


            //continue;

            for(i=0;i<max_index;i++){

            //判断是否gps数据完整与合法
            if(CheckGpsValid(table[i]) == false )
            {
                //如果非法
                continue ;
            }else {
                gps_insert_flag = 1;
             }
                 //兼容多种类型，避免生产问题
                if(gxbd_flag==5){
                   if(strstr(table[i],"GXBDT5")){
                        gxbd_flag=11;
                        gxbd_rmc = 5;
                        XMLogI("====GXBDT5====");
                        SunSetEncType(11);
                        //读取协议数据
                    }
                    else if(strstr(table[i],"GXBDT11")){
                        gxbd_flag=11;
                        gxbd_rmc = 6;
                        XMLogI("====GXBDT11====");
                        SunSetEncType(11);
                         //读取协议数据
                    }
                    else if(strstr(table[i],"GXBD,")){
                        gxbd_flag=6;
                        gxbd_rmc = 6;
                        XMLogI("====GXBD 6====");
                        SunSetEncType(6);
                    }
                }

                switch(table[i][2]){
                    case 'D'://$BD 北斗卫星导航系统（BDS）单系统定位模式
                        switch(table[i][5]){
                            case 'V':
                                if(table[i][4] == 'S'){//$BDGSV
                                    gps_data_gsv(table[i],&gps.BDGSV);
                                    // for(i=0;i<4;i++){
                                    //     XMLogI("====bdgsv:%d,%d,%d,%d,%d\n",gps.BDGSV.total_num,gps.BDGSV.cur_index,i,gps.BDGSV.PRN[i+(gps.BDGSV.cur_index-1)*4],gps.BDGSV.SNR[i+(gps.BDGSV.cur_index-1)*4]);
                                    // }
                                }
                                break;
                            case 'C':
                                break;
                            case 'A':
                                break;
                            default:
                                break;
                        }
                        break;
                    case 'N'://$GN 全球卫星导航系统（GNSS）多系统组合定位模式
                        switch(table[i][5]){
                            case 'V':
                                break;
                            case 'C':
                                if(table[i][4] == 'M'){//$GNRMC
                                    gps_data_rmc(table[i],&gps.GNRMC);
                                    if(gxbd_flag != 11)
                                    {
                                        if(gps_second >= 0) gps_second ++;
                                    }
                                    // XMLogI(">>>>>gps_data_gnrmc:\ntime:%04d/%02d/%02d %02d:%02d:%02d\nstate=%c\nspeed=%0.2f m/s\nlat:%c %06f\nlon:%c %06f\n",
                                    //     gps.GNRMC.time.year,gps.GNRMC.time.month,gps.GNRMC.time.day,
                                    //     gps.GNRMC.time.hour,gps.GNRMC.time.min,gps.GNRMC.time.sec,
                                    //     gps.GNRMC.state,
                                    //     gps.GNRMC.speed*0.514,
                                    //     gps.GNRMC.lat_direct,gps.GNRMC.lat,
                                    //     gps.GNRMC.lon_direct,gps.GNRMC.lon
                                    // ); 
                                }
                                break;
                            case 'A':
                                break;
                            default:
                                break;
                        }
                        break;
                    case 'P'://$GP 全球定位系统（GPS）单系统定位模式
                        switch(table[i][5]){
                            case 'V':
                                if(table[i][4] == 'S'){//$GPGSV
                                    gps_data_gsv(table[i],&gps.GPGSV);
                                    // for(i=0;i<4;i++){
                                    //     XMLogI("====gpgsv:%d,%d,%d,%d,%d\n",gps.GPGSV.total_num,gps.GPGSV.cur_index,i,gps.GPGSV.PRN[i+(gps.GPGSV.cur_index-1)*4],gps.GPGSV.SNR[i+(gps.GPGSV.cur_index-1)*4]);
                                    // }
                                }
                                break;
                            case 'C':
                                if(table[i][4] == 'M'){//$GPRMC
                                    gps_data_rmc(table[i],&gps.GPRMC);
                                    // XMLogI(">>>>>gps_data_gprmc:\ntime:%04d/%02d/%02d %02d:%02d:%02d\nstate=%c\nspeed=%0.2f m/s\nlat:%c %06f\nlon:%c %06f\n",
                                    //     gps.GPRMC.time.year,gps.GPRMC.time.month,gps.GPRMC.time.day,
                                    //     gps.GPRMC.time.hour,gps.GPRMC.time.min,gps.GPRMC.time.sec,
                                    //     gps.GPRMC.state,
                                    //     gps.GPRMC.speed*0.514,
                                    //     gps.GPRMC.lat_direct,gps.GPRMC.lat,
                                    //     gps.GPRMC.lon_direct,gps.GPRMC.lon
                                    // );                                    
                                }                            
                                break;
                            case 'A':
                                if(table[i][4] == 'S'){//$GPGSA
                                   gps_data_gsa(table[i], &gps.GPGSA);
                                }else{
                                    gps_data_gga(table[i], &gps.GPGGA);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case 'L'://$GL 全球卫星导航系统（GlatASS）单系统定位模式
                        break;
                    case 'A'://$GA 伽利略系统（Galileo）单系统定位模式
                        break;
                    case 'X'://
                        if(gxbd_flag == 11)
                        {
                            gps_data_tp11(table[i]);
                            if(gps_second >= 0) gps_second ++;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        gps_snr_sort();//信噪比数据处理
        // if(gps.GPRMC.have){
        //     gps.speed = gps.GPRMC.speed * 1.852;
        // }else if(gps.GNRMC.have){
        //     gps.speed = gps.GNRMC.speed * 1.852;
        // }
        if(timer){
            lv_timer_resume(timer);
        }
    }else{
        if(gps_insert_flag){
            clean_gps_osd_data(0);
            gps_insert_flag = 0;//gps插入标志 1:已插入
        }
}
	return;
}


void* gps_hander_thread(void* args)
{
	int64_t t1 = 0;
	int64_t t2 = 0;
	int64_t t1_t2 = 0;
    int num = 0;
    while(1){
        num ++;
        if(num%2 == 0){
            t1 = GetTickTime();
            uart_gps_hander_thread(NULL);
            t2 = GetTickTime();
            t1_t2 = t2 - t1;
            if(t1_t2 > 40){
                XMLogW("gps analy over 40ms:%lld\n",t1_t2);
            }
        }
        usleep(100*1000);
    }
}

void UartGpsThreadStart()
{
    memset(&gps, 0, sizeof(struct gps_info));
    UartInit(1);

    OS_THREAD uart_gps;
    CreateThreadEx(uart_gps, (LPTHREAD_START_ROUTINE)gps_hander_thread, NULL);

    //lv_timer_create(uart_gps_hander_thread, 500, NULL);
}


int64_t getCurrentDateTimeSec(int Y,int M, int D,int h,int m,int s) //将当前时间转换为1970年1月1日至今的秒数
{
    int64_t i = 0;
    int64_t sec = 0;
    int64_t days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    for (i = 1970; i < Y; i++)
    {
        if ((((i % 4) == 0) && (i % 100) != 0) || (i % 400 == 0))
            sec += 366 * 24 * 60 * 60;
        else
            sec += 365 * 24 * 60 * 60;
    }
    for (i = 1; i < M; i++)
    {
        if (i == 2 && ((((Y % 4) == 0) && (Y % 100) != 0) || (Y % 400 == 0)))
        {
            sec += 29 * 24 * 60 * 60;
        }
        else {
            sec += days[i] * 24 * 60 * 60;
        }
    }
    sec += (D - 1) * 24 * 60 * 60;
    sec += h * 60 * 60 + m * 60 + s;
    sec = sec - (8 * 60 * 60);
    return sec;
}

static char day[2][13]={{31,31,28,31,30,31,30,31,31,30,31,30,31},
               {31,31,29,31,30,31,30,31,31,30,31,30,31}};
/*
gpstime:传入时间
scr_zoon:传入时间对应的时区
tar_zoon:最终结果的时区
*/
int gps_upgrade_sys_time(struct gps_time gpstime, int scr_zoon,int tar_zoon)//时区变换
{
    void *fd;
    int Zone_parm;
    int Zone_Base;
    int parm1;
    int flag1=0;
    int Zone_base_hour = 0;
    int Zone_base_min = 0;
    struct gps_time time1;
    parm1 = tar_zoon;
    Zone_Base = scr_zoon;//gpstime时间对应的时区

//    gps_printf("====================== Zone1=%d ===sys_time:%4d-%2d-%2d %2d:%2d:%2d\n",Zone_Base,time.year,time.month,time.day,time.hour,time.min,time.sec);

    if(parm1>=10000){
        parm1 = parm1%10000;
        parm1 = 0-parm1;
    }
    Zone_base_hour = parm1/100;
    Zone_base_min = parm1%100;

    if(Zone_Base>=10000){
        Zone_Base = Zone_Base%10000;
        Zone_Base = 0-Zone_Base;
    }

    Zone_base_min = Zone_base_min-(Zone_Base%100);
    gpstime.min=gpstime.min+Zone_base_min;
    if(gpstime.min<0){
        gpstime.min +=60;
        gpstime.hour -=1;
    }else if(gpstime.min>59){
        gpstime.min -=60;
        gpstime.hour +=1;
    }

    Zone_base_hour = Zone_base_hour-(Zone_Base/100);
    gpstime.hour=gpstime.hour+Zone_base_hour;
    if((gpstime.year%100!=0 && gpstime.year%4==0)||(gpstime.year%400==0)){
        flag1=1;
    }
    if(gpstime.hour<0){
        gpstime.hour +=24;
        gpstime.day -=1;
        if(gpstime.day<1){
            gpstime.day +=day[flag1][gpstime.month-1];
            gpstime.month -=1;
            if(gpstime.month<1){
                gpstime.month +=12;
                gpstime.year -=1;
            }
        }
    }else if(gpstime.hour>24){
        gpstime.hour -=24;
        gpstime.day +=1;
        if(gpstime.day>day[flag1][gpstime.month]){
            gpstime.day -=day[flag1][gpstime.month];
            gpstime.month +=1;
            if(gpstime.month>12){
                gpstime.month -=12;
                gpstime.year +=1;
            }
        }
    }

    gps.time.year = gpstime.year;
    gps.time.month = gpstime.month;
    gps.time.day = gpstime.day;
    gps.time.hour = gpstime.hour;
    gps.time.min = gpstime.min;
    gps.time.sec = gpstime.sec;

    int64_t value1;
    int64_t value2;
    value1=getCurrentDateTimeSec(gps.time.year,gps.time.month,gps.time.day,gps.time.hour,gps.time.min,gps.time.sec);

    //选择当前时间
	time_t t = time(NULL);
	struct tm* current_time = localtime(&t);
    value2=getCurrentDateTimeSec(current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday,
    current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    //XMLogW("gps upgrade sys_time hand:%lld,%lld",value1,value2);
    if(value1-value2>3 || value1-value2<-3){
        return 1;
    }
    return 0;
}

void update_hardware_time(struct gps_time time1)
{
    //修改系统时间
    char buf1[128] = { 0 };
    sprintf(buf1, "date -s \"%d-%d-%d %d:%d:%d\"", time1.year, time1.month,
        time1.day, time1.hour, time1.min, time1.sec);
    XMLogI("gps change time command string:%s", buf1);
#ifndef SIMUlonION_GPS
    int ret = system(buf1);
    XMLogI("date -s ret = %d", ret);
    if (ret != 0) {
        XMLogE("date -s ret = %d", ret);
        return;
    }
    //同步硬件时间
    std::string a("/sbin/hwclock -w");
    ret = system(a.c_str());
    XMLogI("/sbin/hwclock -w ret = %d", ret);
    if (ret != 0) {
        XMLogE("/sbin/hwclock -w ret = %d", ret);
        return;
    }
#endif

}




void get_current_time(char* str);
int gps_snr_sort()//信噪比数据处理
{
    #define LEN 40
    int i=0,j=0,max_index=0;
    int temp = 0;
    int _PRN_[LEN]={0};
    int _SNR_[LEN]={0};
    int _Elev_[LEN]={0};      // 卫星仰角
    int _Azim_[LEN]={0};      // 卫星方位角
    int data_len_ = 0;
    if(gps.GPGSV.have || gps.BDGSV.have){
        for (i=0;i<LEN/2;i++){
            if(gps.GPGSV.SNR[i]){
                _SNR_[data_len_] = gps.GPGSV.SNR[i];
                _PRN_[data_len_] = gps.GPGSV.PRN[i];
                _Elev_[data_len_] = gps.GPGSV.Elev[i];
                _Azim_[data_len_] = gps.GPGSV.Azim[i];
                data_len_ ++;
            }
        }
        for (i=LEN/2;i<LEN;i++){
            if(gps.GPGSV.SNR[i]){
                _SNR_[data_len_] = gps.BDGSV.SNR[i-LEN/2];
                _PRN_[data_len_] = gps.BDGSV.PRN[i-LEN/2];
                _Elev_[data_len_] = gps.BDGSV.Elev[i-LEN/2];
                _Azim_[data_len_] = gps.BDGSV.Azim[i-LEN/2];
                data_len_ ++;
            }
        }
    }else if(gps.GPGSV.have && !gps.BDGSV.have){
        for (i=0;i<LEN/2;i++){
            if(gps.GPGSV.SNR[i]){
                _SNR_[data_len_] = gps.GPGSV.SNR[i];
                _PRN_[data_len_] = gps.GPGSV.PRN[i];
                _Elev_[data_len_] = gps.GPGSV.Elev[i];
                _Azim_[data_len_] = gps.GPGSV.Azim[i];
                data_len_ ++;
            }
        }
    }else if(!gps.GPGSV.have && gps.BDGSV.have){
        for (i=0;i<LEN/2;i++){
            if(gps.GPGSV.SNR[i]){
                _SNR_[data_len_] = gps.BDGSV.SNR[i];
                _PRN_[data_len_] = gps.BDGSV.PRN[i];
                _Elev_[data_len_] = gps.BDGSV.Elev[i];
                _Azim_[data_len_] = gps.BDGSV.Azim[i];
                data_len_ ++;
            }
        }
    }else{
        data_len_ = 0;
    }
	
#if 1//从大到小排序,若不需要排序则写0
    int m=0;
    for( i = 0; i < data_len_-1; i++)
    {
        m = i;  //从剩下的到LEN-1个数据中选择最小数据的下标
        for( j = i+1; j < LEN; j++) //从剩余数据中不断获取更大的数据
            if(_SNR_[j] > _SNR_[m])         //如果还有更大的数据
                m = j;    //获取它的数组下标（位置）
        if(m != i)//数据不一样才需要互换位置
        {
            temp = _SNR_[i];
            _SNR_[i] = _SNR_[m];
            _SNR_[m] = temp;
            temp = _PRN_[i];
            _PRN_[i] = _PRN_[m];
            _PRN_[m] = temp;
            temp = _Elev_[i];
            _Elev_[i] = _Elev_[m];
            _Elev_[m] = temp;
            temp = _Azim_[i];
            _Azim_[i] = _Azim_[m];
            _Azim_[m] = temp;
        }
    }
#endif

#if 1//选择前n个有效数据保存，若不需要则写0
	static int wr_count = 0;
	static bool need_wr = false;

	wr_count++;
	if(wr_count > 10){
		
		need_wr = true;
	}
	
    if(data_len_){
    	FILE* fp = NULL;
		int gps_len = 0;
        for (j=0,i=0;i<LEN;i++){
            if(!_SNR_[i]){
                continue;
            }
            gps.SNR[j] = _SNR_[i];
            gps.PRN[j] = _PRN_[i];
            gps.Elev[j] = _Elev_[i];
            gps.Azim[j] = _Azim_[i];
            j++;

			#if 0
			static int snr_count = 0;
			char snr_str[25] = {0};
			char file_name[64] = {0};
			
			if(/*gps.PRN[i]>0 && gps.SNR[i]>0 &&*/ i <= 10 && need_wr){
				
				memset(snr_str, 0, 25);

			    sprintf(snr_str, "[%03d]%02d ", gps.PRN[i], gps.SNR[i]);
			    sprintf(file_name, "/mnt/tfcard/gps_snr.txt");
				snr_count++;
				
				//XMLogW("[gps_snr_sort] 1 , i = (%d,%d) ,snr_str = %s \r\n", i, snr_count, snr_str);
				
    			FILE* fp = fopen(file_name, "at+");
    			
    			if (fp) {
					if(gps.PRN[i]>0 && gps.SNR[i]>0){
						
    				    fwrite(snr_str, 1, 8, fp);
					}
				
					if(i == 10 || snr_count == 11){
						
					    memset(snr_str, 0, 25);
						get_current_time(snr_str);
						
    				    fwrite(snr_str, 1, 21, fp);
					
    				    fwrite("\n", 1, 1, fp);
						snr_count = 0;
						wr_count = 0;
		                need_wr = false;
						
					}
    				fclose(fp);
    			}
			}
			#endif
			
        }
		
        for (i=j;i<LEN;i++){
            gps.SNR[i] = 0;
            gps.PRN[i] = 0;
            gps.Elev[i] = 0;
            gps.Azim[i] = 0;
        }
    }
    return 0;
#endif

    if(data_len_){
        for (i=0;i<40;i++){
            gps.SNR[i] = _SNR_[i];
            gps.PRN[i] = _PRN_[i];
            gps.Elev[i] = _Elev_[i];
            gps.Azim[i] = _Azim_[i];
        }
    }
//        putchar('\n');
//        for(i=0;i<16;i++){
//            printf(">>%03d, %03d\n",gps.SNR[i],gps.PRN[i]);
//        }
//        putchar('\n');
    return 0;
}
int gps_data_gga(char *gps_data_frame,struct gga*_gga_)
{
    int table[32]={0},err=0,num=0,i=0;
    char temp=0;
    char *p1;
    char *data=gps_data_frame;
    char str[16]={0};
    p1=strstr(data,"GGA");
    if(!p1){
        XMLogE("======GGA err:%s\n",data);
        return 1;
    }
    //XMLogI("======gga:%s\n",data);
    temp = nema_comma_num(data,table,&num);
    if(!temp){
        return 2;
    }else{
//        for(i=0;i<num;i++){
//            XMLogI("==%d,%d,%d\n",num,i,table[i]);
//        }
    }
    _gga_->have ++ ;
       if(table[9]-table[8]>3){
        memset(str,0,16);
        memcpy(str,data+table[8]+1,table[9]-table[8]-1);
      //  XMLogI("%s",str);
        _gga_->height = atof(str);//海拔高度
    }

    //XMLogI("gps_data_gga:\n _gga_->height=%f\n", _gga_->height);
    return 0;
}

extern int osd_time_ofs_y;
extern int osd_data_init(void);
#include "mpp/MppMdl.h"
#include "DemoDef.h"

int gps_data_rmc(char *gps_data_frame,struct rmc *_rmc_)
{
    int table[32]={0},err=0,num=0,i=0;
    char temp=0;
    char *p1;
    char *data=gps_data_frame;
    char str[16]={0};
    static char time_updata_flag=0;
    int du=0;
    p1=strstr(data,"RMC");
    if(!p1){
        XMLogE("======rmc err:%s\n",data);
        return 1;
    }
    //XMLogI("======rmc:%s\n",data);
    temp = nema_comma_num(data,table,&num);
    if(!temp){
        return 2;
    }else{
//        for(i=0;i<num;i++){
//            XMLogI("==%d,%d,%d\n",num,i,table[i]);
//        }
    }
    _rmc_->have ++ ;
    if(table[1]-table[0]>4){
        memset(str,0,16);
        memcpy(str,data+table[0]+1,table[1]-table[0]-1);
        double time = atof(str);//UTC时间
        int _time_ = (int)time;
        _rmc_->time.hour=(int)_time_/10000;
        _rmc_->time.min=(int)(_time_/100)%100;
        _rmc_->time.sec=(int)_time_%100;
    }

    _rmc_->state = *(data+table[1]+1);//定位状态
    if(_rmc_->state == 'A'){
        gps_online_flag = 1;
		
#if 1//X2V60_S_DEBUG1
	clean_gps_osd_flag = 1;

    #if 1//OSD_SHOW_ADJUST
	if(gps_online_old_flag != gps_online_flag){
		
    	if(gps_online_flag){
			
			gps_online_old_flag = gps_online_flag;
        	osd_time_ofs_y = 100;
			MppMdl::Instance()->EnableOsdTime(4,1, 128, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight + osd_time_ofs_y); //APP时间水印
    	}
	}
    #endif
#endif
    }else{
    
        gps_online_flag = 0;
		gps_online_old_flag = 0;
		
#if 1//X2V60_S_DEBUG1
		if(last_clean_gps_osd_flag!=clean_gps_osd_flag){
			
			if(clean_gps_osd_flag){
				
			    clean_gps_osd_flag = 0;
    			last_clean_gps_osd_flag=clean_gps_osd_flag;
				
                #if 1//OSD_SHOW_ADJUST
				osd_time_ofs_y = 450;
			    clean_gps_osd_data(0);
				MppMdl::Instance()->EnableOsdTime(4,1, 128, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight + osd_time_ofs_y); //APP时间水印
				#else
			    clean_gps_osd_data(0);
                #endif
				
			}
		}
#endif
    }

    if(table[3]-table[2]>4){
        _rmc_->lon_direct = 'S';
        memset(str,0,16);
        memcpy(str,data+table[2]+1,table[3]-table[2]-1);
        _rmc_->lon = atof(str);//纬度
        du= _rmc_->lon/100;
        _rmc_->lon=du+(_rmc_->lon-du*100)/60;
        _rmc_->lon_direct = *(data+table[3]+1);//纬度方向
    }

    if(table[5]-table[4]>4){
        _rmc_->lat_direct = 'W';
        memset(str,0,16);
        memcpy(str,data+table[4]+1,table[5]-table[4]-1);
        _rmc_->lat = atof(str);//经度
        du=_rmc_->lat/100;
        _rmc_->lat=du+(_rmc_->lat-du*100)/60;
        _rmc_->lat_direct = *(data+table[5]+1);//经度方向
    }

    if(table[7]-table[6]>1){
        memset(str,0,16);
        memcpy(str,data+table[6]+1,table[7]-table[6]-1);
        _rmc_->speed = atof(str);//对地速度
        gps.speed=_rmc_->speed*1.852;
    }

    if(table[8]-table[7]>1){
        memset(str,0,16);
        memcpy(str,data+table[7]+1,table[8]-table[7]-1);
        _rmc_->speed_direct = atof(str);//速度方向
    }

    if(table[9]-table[8]>4){
        memset(str,0,16);
        memcpy(str,data+table[8]+1,table[9]-table[8]-1);
        int _time_ = atoi(str);//UTC时间
        _rmc_->time.day = _time_/10000;
        _rmc_->time.month = (_time_/100)%100;
        _rmc_->time.year = _time_%100 + 2000;
    }
#if 0

    if(time_updata_flag==0&& gps_online_flag){
        time_updata_flag=1;
        XMLogW("gps upgrade sys_time start");
        XM_CONFIG_VALUE cfg_value;
        cfg_value.int_value = 0;
        GlobalData::Instance()->car_config()->GetValue(CFG_Operation_GPS_UTC, cfg_value);
        XM_Middleware_Periphery_Notify(XM_EVENT_GPS_TIME, "", gps_upgrade_sys_time(_rmc_->time,GMT_0000E, cfg_value.int_value));
    }
#endif
    if(table[10]-table[9]>2){
        memset(str,0,16);
        memcpy(str,data+table[9]+1,table[10]-table[9]-1);
        _rmc_->magnetic_decl = atof(str);//磁偏角
    }

    if(table[11]-table[10]>2){
        memset(str,0,16);
        memcpy(str,data+table[10]+1,table[11]-table[10]-1);
        _rmc_->magnetic_decl_direct = atof(str);//磁偏角方向
    }

    // XMLogI("===gps_data_rmc:\ntime:%04d/%02d/%02d %02d:%02d:%02d\nstate=%c\nspeed=%0.2f m/s\nlat:%c %06f\nlon:%c %06f\n",
    //     _rmc_->time.year,_rmc_->time.month,_rmc_->time.day,
    //     _rmc_->time.hour,_rmc_->time.min,_rmc_->time.sec,
    //     _rmc_->state,
    //     _rmc_->speed*0.514,
    //     _rmc_->lat_direct,_rmc_->lat,
    //     _rmc_->lon_direct,_rmc_->lon
    // );

 if(gps_online_flag) {
    char osdstr[32+1]={0};
    int flat1,flat[1];
    int flon1,flon[1];
    int speed=_rmc_->speed*1.852;
    flat1=_rmc_->lat*100000;
    flon1=_rmc_->lon*100000;
    SunCoordTransformation(flon,flat,flon1,flat1);
    flat_decode=flat[0];
    flat_decode=flat_decode/100000;
    flon_decode=flon[0];
    flon_decode=flon_decode/100000;
    sprintf(osdstr, "%c %06f %c %06f %dkm/h", _rmc_->lat_direct,flat_decode,_rmc_->lon_direct, flon_decode,speed);
         // XMLogW("\nosdstr=====%s\n",osdstr);
      osd_upgrade_all_data(osdstr);      //更新速度、经纬度水印

  #if SIX_APP
  if(write_start || write_startback){
    memset(chInfomation,0,128);
   // sprintf(chInfomation,"%4d/%02d/%02d %02d:%02d:%02d %c:%.6f %c:%.6f %.1f km/h - - -  A:%.1f",_rmc_->time.year,_rmc_->time.month,_rmc_->time.day,_rmc_->time.hour,
   //       _rmc_->time.min,_rmc_->time.sec,_rmc_->lon_direct,_rmc_->lon,_rmc_->lat_direct,_rmc_->lat,_rmc_->speed,_rmc_->speed_direct);
    sprintf(chInfomation,"%4d/%02d/%02d %02d:%02d:%02d %c:%.6f %c:%.6f %.1f km/h - - -  A:%.1f",_rmc_->time.year,_rmc_->time.month,_rmc_->time.day,_rmc_->time.hour,
           _rmc_->time.min,_rmc_->time.sec,_rmc_->lon_direct,flon_decode,_rmc_->lat_direct,flat_decode,_rmc_->speed,_rmc_->speed_direct);
    #ifdef COMPAT_PC
    memset(chInfomation_pc,0,128);
    sprintf(chInfomation_pc,"%4d/%02d/%02d %02d:%02d:%02d %c:%.6f %c:%.6f %.1f km/h",_rmc_->time.year,_rmc_->time.month,_rmc_->time.day,_rmc_->time.hour,
        _rmc_->time.min,_rmc_->time.sec,_rmc_->lon_direct,_rmc_->lon,_rmc_->lat_direct,_rmc_->lat,_rmc_->speed);
    #endif
    }
     if(write_start){
       chOutput[gps_count][3]=(gps_count+1)/1000000;
       chOutput[gps_count][2]=(gps_count+1)/10000%100;
       chOutput[gps_count][1]=(gps_count+1)/100%100;
       chOutput[gps_count][0]=(gps_count+1)%100;
        memcpy(chOutput[gps_count]+4,chInfomation,strlen(chInfomation));
        #ifdef COMPAT_PC
        MakeEncryptDataBlock(gps_count,chInfomation_pc ,strlen(chInfomation_pc),chOutput_pc[gps_count]);
        #endif
        gps_count++;
     }
     if(write_startback){
       chOutputback[gps_countback][3]=(gps_countback+1)/1000000;
       chOutputback[gps_countback][2]=(gps_countback+1)/10000%100;
       chOutputback[gps_countback][1]=(gps_countback+1)/100%100;
       chOutputback[gps_countback][0]=(gps_countback+1)%100;
        memcpy(chOutputback[gps_countback]+4,chInfomation,strlen(chInfomation));
        #ifdef COMPAT_PC
        MakeEncryptDataBlock(gps_countback,chInfomation_pc ,strlen(chInfomation_pc),chOutputback_pc[gps_countback]);
        #endif
        gps_countback++;
     }

 #else
    if(gxbd_flag != 11 && gps_online_flag){
        memset(chInfomation,0,128);
        sprintf(chInfomation,"%4d/%02d/%02d %02d:%02d:%02d %c:%.6f %c:%.6f %.1f km/h",_rmc_->time.year,_rmc_->time.month,_rmc_->time.day,_rmc_->time.hour,
            _rmc_->time.min,_rmc_->time.sec,_rmc_->lon_direct,_rmc_->lon,_rmc_->lat_direct,_rmc_->lat,_rmc_->speed);
        if(write_start)
        {
            MakeEncryptDataBlock(gps_count,chInfomation ,strlen(chInfomation),chOutput[gps_count]);
            gps_count ++;
        }
        if(write_startback)
        {
            MakeEncryptDataBlock(gps_countback,chInfomation ,strlen(chInfomation),chOutputback[gps_countback]);
            gps_countback++;
        }
    }
  #endif
 }
    return 0;
}


void test_gps()
{
    struct rmc _tmc_;
    static int cnt=0;
    _tmc_.time.year=2023;
    _tmc_.time.month=12;
    _tmc_.time.day=19;
    _tmc_.time.hour=18;
    _tmc_.time.min=10;
    _tmc_.time.sec=1;
    _tmc_.lon_direct='N';
    _tmc_.lon=22.525431;
    _tmc_.lat_direct='E';
    _tmc_.lat=114.030060;
    _tmc_.speed=10;
    if(write_start || write_startback){
        printf("\n<<<<<<<<<<<<<<<<test_gps,gps_count=%d\n",gps_count);
        memset(chInfomation,0,128);
        sprintf(chInfomation,"%4d/%02d/%02d %02d:%02d:%02d %c:%.6f %c:%.6f %.1f km/h - - -  A:%.1f",_tmc_.time.year,_tmc_.time.month,_tmc_.time.day,_tmc_.time.hour,
        _tmc_.time.min,_tmc_.time.sec,_tmc_.lon_direct,_tmc_.lon,_tmc_.lat_direct,_tmc_.lat,_tmc_.speed,_tmc_.speed_direct);
    }
    if(write_start){
        chOutput[gps_count][3]=(gps_count+1)/1000000;
        chOutput[gps_count][2]=(gps_count+1)/10000%100;
        chOutput[gps_count][1]=(gps_count+1)/100%100;
        chOutput[gps_count][0]=(gps_count+1)%100;
        memcpy(chOutput[gps_count]+4,chInfomation,strlen(chInfomation));
        gps_count++;
    }
    if(write_startback){
        chOutputback[gps_countback][3]=(gps_countback+1)/1000000;
        chOutputback[gps_countback][2]=(gps_countback+1)/10000%100;
        chOutputback[gps_countback][1]=(gps_countback+1)/100%100;
        chOutputback[gps_countback][0]=(gps_countback+1)%100;
        memcpy(chOutputback[gps_countback],chInfomation,strlen(chInfomation));
        gps_countback++;
    }
}

unsigned int gps_calculone(unsigned int x)//gps数据长度转小端输出
{
    unsigned int a=0,b=0,c=0,d=0;
    a=x%256;
    b=(x>>8)%256;
    c=(x>>16)%256;
    d=(x>>24);
    x=(a<<24)+(b<<16)+(c<<8)+d;
    return x;
}

void MakeHeader_(char* data,int count)
{
    sprintf(data,"LCAIGPSINF");
    *(data+19)=count/1000000;
    *(data+18)=count/10000%100;
    *(data+17)=count/100%100;
    *(data+16)=count%100;
}

void get_gps_data(const char *enc_file)//gps数据写入程序 前路
{
    XMLogW("get_gps_data  start :%d",gps_count);
    if(gps_count>1){
        char skip[4]={'s','k','i','p'};
        #ifdef COMPAT_PC
        char gps_data_end[4]={'#','#','#','#'};
        char gps_data_end_two[4]={'&','&','&','&'};
        #else
        char gps_data_end[4]={'&','&','&','&'};
        #endif
        gps_second = -1;
        FILE *fgps;
        unsigned int gps_array[1]={0};
        fgps=fopen(enc_file,"ab");
        if(!fgps) XMLogI("~~~~~~~~file err~~~~~~~~");
        else XMLogI("~~~~~~~~file open~~~~~~~~");
    // fseek(fgps,0,SEEK_END);
        unsigned int  gps_length=36+132*gps_count;
        XMLogI("~~~~~~~|~~~~~~%08x~~~~~~|~~~~~~",gps_count);
        gps_array[0]=gps_calculone(gps_length);
        gps_length=0;
        XMLogI("~~~~~~~|~~~~~~%08x~~~~~~|~~~~~~",gps_array[0]);

    #ifdef COMPAT_PC
        fwrite(gps_array, 1, 4, fgps);
        fwrite(skip,1,4,fgps);
        memset(header,0,20);
        MakeHeader(header,gps_count);
        XMLogI("~~~~~~~~~~%02x~~~~~~~~~",header);
        fwrite(header,1,20,fgps);
        for(int i=0;i<gps_count;i++){
            fwrite(chOutput_pc[i],1,sizeof(chOutput_pc[i]),fgps);
        }
        fwrite(gps_data_end_two,1,4,fgps);
        fwrite(gps_array,1,4,fgps);
        memset(header,0,20);
    #endif

        fwrite(gps_array, 1, 4, fgps);
        fwrite(skip,1,4,fgps);
        #if SIX_APP
        MakeHeader_(header,gps_count);
        #else
        MakeHeader(header,gps_count);
        #endif
        XMLogI("~~~~~~~~~~%02x~~~~~~~~~",header);
        fwrite(header,1,20,fgps);
        for(int i=0;i<gps_count;i++){
            fwrite(chOutput[i],1,sizeof(chOutput[i]),fgps);
        }
        fwrite(gps_data_end,1,4,fgps);
        fwrite(gps_array,1,4,fgps);

        fclose(fgps);
        sync();
        memset(chOutput,0,sizeof(chOutput));
        memset(header,0,sizeof(header));
        gps_count=0;
        gps_second = 0;
    }
    XMLogW("get_gps_data  end :%d",gps_count);
}

void get_gps_back_data(const char *enc_file)//gps数据写入程序 后路
{
    XMLogW("get_gps_back_data  start :%d",gps_countback);
    if(gps_countback>1){
        char skip[4]={'s','k','i','p'};
        #ifdef COMPAT_PC
        char gps_data_end[4]={'#','#','#','#'};
        char gps_data_end_two[4]={'&','&','&','&'};
        #else
        char gps_data_end[4]={'&','&','&','&'};
        #endif
        gps_second = -1;
        FILE *fgps;
        unsigned int gps_array[1]={0};
        fgps=fopen(enc_file,"ab");
        if(!fgps) XMLogI("~~~~~~~~file err~~~~~~~~");
        else XMLogI("~~~~~~~~file open~~~~~~~~");
    // fseek(fgps,0,SEEK_END);
        unsigned int  gps_length=36+132*gps_countback;
        XMLogI("~~~~~~~|~~~~~~%08x~~~~~~|~~~~~~",gps_countback);
        gps_array[0]=gps_calculone(gps_length);
        gps_length=0;
        XMLogI("~~~~~~~|~~~~~~%08x~~~~~~|~~~~~~",gps_array[0]);


    #ifdef COMPAT_PC
        fwrite(gps_array, 1, 4, fgps);
        fwrite(skip,1,4,fgps);
        memset(header,0,20);
        MakeHeader(header,gps_countback);
        XMLogI("~~~~~~~~~~%02x~~~~~~~~~",header);
        fwrite(header,1,20,fgps);
        for(int i=0;i<gps_countback;i++){
        fwrite(chOutputback_pc[i],1,sizeof(chOutputback_pc[i]),fgps);}
        fwrite(gps_data_end_two,1,4,fgps);
        fwrite(gps_array,1,4,fgps);
        memset(header,0,20);
    #endif

        fwrite(gps_array, 1, 4, fgps);
        fwrite(skip,1,4,fgps);
        #if SIX_APP
        MakeHeader_(header,gps_countback);
        #else
        MakeHeader(header,gps_countback);
        #endif
        XMLogI("~~~~~~~~~~%02x~~~~~~~~~",header);
        fwrite(header,1,20,fgps);
        for(int i=0;i<gps_countback;i++){
        fwrite(chOutputback[i],1,sizeof(chOutputback[i]),fgps);}
        fwrite(gps_data_end,1,4,fgps);
        fwrite(gps_array,1,4,fgps);


        fclose(fgps);
        sync();
        memset(chOutputback,0,sizeof(chOutputback));
        memset(header,0,sizeof(header));
        gps_countback=0;
        gps_second = 0;
    }
    XMLogW("get_gps_back_data  end :%d",gps_countback);
}

int gps_data_gsv(char *gps_data_frame,struct gsv *_gsv_)
{
    int table[32]={0},err=0,num=0,i=0;
    char temp=0;
    char *p1;
    char *data=gps_data_frame;
    char str[16]={0};
    static int available_cnt = 0;
    p1=strstr(data,"GSV");
    if(!p1){
        XMLogE("======gsv err:%s\n",data);
        return 1;
    }
    if(strlen(data)<16){
        return 3;
    }
    //XMLogI("======gsv:%s\n",data);
    temp = nema_comma_num(data,table,&num);
    if(!temp){
        return 2;
    }else{
//        for(i=0;i<num;i++){
//            XMLogI("==%d,%d,%d\n",num,i,table[i]);
//        }
    }

    _gsv_->have ++;
    if(table[1]-table[0]>1){
        memset(str,0,16);
        memcpy(str,data+table[0]+1,table[1]-table[0]-1);
        _gsv_->total_index = atoi(str);//本次gsv语句总数目
    }
    if(table[2]-table[1]>1){
        memset(str,0,16);
        memcpy(str,data+table[1]+1,table[2]-table[1]-1);
        _gsv_->cur_index = atoi(str);//当前gsv语句序号
    }
    if(table[3]-table[2]>1){
        memset(str,0,16);
        memcpy(str,data+table[2]+1,table[3]-table[2]-1);
        _gsv_->total_num = atoi(str);//可见卫星总数
         if(_gsv_->cur_index==1)
           available_cnt = _gsv_->total_num;
    }
    for(i=0;i<(num-3)/4;i++){
        if((_gsv_->cur_index == 1) && (_gsv_->total_num == 1)){
            break;
        }
        if(_gsv_->total_num<=(i+(_gsv_->cur_index-1)*4)){
            break;
        }
        if(table[i*4+4]-table[i*4+3]>1){
            memset(str,0,16);
            memcpy(str,data+table[i*4+3]+1,table[i*4+4]-table[i*4+3]-1);
            _gsv_->PRN[i+(_gsv_->cur_index-1)*4] = atoi(str);//卫星PRN码编号
        }else{
            _gsv_->PRN[i+(_gsv_->cur_index-1)*4] = 0;
        }
        if(table[i*4+5]-table[i*4+4]>1){
            memset(str,0,16);
            memcpy(str,data+table[i*4+4]+1,table[i*4+5]-table[i*4+4]-1);
            _gsv_->Elev[i+(_gsv_->cur_index-1)*4] = atoi(str);//卫星仰角
        }else{
            _gsv_->Elev[i+(_gsv_->cur_index-1)*4] = 0;//卫星仰角
        }
        if(table[i*4+6]-table[i*4+5]>1){
            memset(str,0,16);
            memcpy(str,data+table[i*4+5]+1,table[i*4+6]-table[i*4+5]-1);
            _gsv_->Azim[i+(_gsv_->cur_index-1)*4] = atoi(str);//卫星方位角
        }else{
            _gsv_->Azim[i+(_gsv_->cur_index-1)*4] = 0;//卫星方位角
        }
        if(table[i*4+7]-table[i*4+6]>1){
            memset(str,0,16);
            memcpy(str,data+table[i*4+6]+1,table[i*4+7]-table[i*4+6]-1);
            _gsv_->SNR[i+(_gsv_->cur_index-1)*4] = atoi(str);//信噪比
        }else{
            _gsv_->SNR[i+(_gsv_->cur_index-1)*4] = 0;//信噪比
            available_cnt--;
        }
        //XMLogI("====gsv:%d,%d,%d,%d,%d\n",_gsv_->total_num,_gsv_->cur_index,i,_gsv_->PRN[i+(_gsv_->cur_index-1)*4],_gsv_->SNR[i+(_gsv_->cur_index-1)*4]);
    }
        if (_gsv_->cur_index == _gsv_->total_index)
        if (available_cnt < 0) _gsv_->available_num = 0;
        else  _gsv_->available_num = available_cnt;
    return 0;
}

int gps_data_gsa(char *gps_data_frame,struct gsa *_gsa_)
{

    return 0;
}

int gps_data_gll(char *gps_data_frame,struct gll *_gll_)
{

    return 0;
}

int gps_data_vtg(char *gps_data_frame,struct vtg *_vtg_)
{

    return 0;
}

#endif
