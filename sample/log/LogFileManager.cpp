#include "LogFileManager.h"
#ifndef WIN32
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#endif
#include <string.h>
#include "GUI/global_data.h"
const int kMaxLogFileLen = 1024000;

LogFileManager* LogFileManager::instance_ = 0;
LogFileManager* LogFileManager::Instance()
{
    if (0 == instance_) {
		instance_ = new LogFileManager();
	}
	return instance_;
}

void LogFileManager::Uninstance()
{
    if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

LogFileManager::LogFileManager() : log_file_(-1), enable_(false),log_data_len(0),log_data_total_len(0),
enable_write_sd(false),receive_cache_statu(false),get_log_statu(false)
{
	memset(log_data,0,LOG_DATA_CACHE);
	memset(log_data_sd,0,LOG_DATA_CACHE);
}

LogFileManager::~LogFileManager()
{
}

int LogFileManager::Enable(bool enable)
{
	enable_ = enable;
	printf("LogFileManager::Enable, enable=%d\n", enable);
	return 0;
}

int LogFileManager::Enable_Write_Sd(bool enable)
{
	enable_write_sd = enable;
	printf("LogFileManager::Enable_Write_Sd, enable=%d\n", enable);
	return 0;
}

int LogFileManager::Write(char* log_str)
{
#ifndef WIN32
	if (!enable_) {
		return -1;
	}
	
	if (log_file_ == -1) {
		char file_name[256] = {0};
		struct timeval tv = { 0 };
		gettimeofday(&tv, 0);  //获取相对于1970到现在的秒数
		tm nowTime;
		localtime_r(&tv.tv_sec, &nowTime);
		sprintf(file_name, "%s/%04d%02d%02d_%02d%02d%02d_log.txt", kTFCardPath,
			nowTime.tm_year + 1900, nowTime.tm_mon + 1, nowTime.tm_mday, 
			nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
		log_file_ = ::open(file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		printf("create log file, name=[%s], ret=%d\n", file_name, log_file_);
	}

	if (log_file_ != -1) {
		int ret = write(log_file_, log_str, strlen(log_str));
		if (ret <= 0) {
			close(log_file_);
			log_file_ = -1;
			printf("fprintf error, may be sdcard removed\n");
			return -1;
		}
		//立即刷新
		::fsync(log_file_);

		//检查长度是否超出1MB
		struct stat statistics;
		if (fstat(log_file_, &statistics) == 0) {
			if ((unsigned int)statistics.st_size >= kMaxLogFileLen) {
				close(log_file_);
				log_file_ = -1;
				printf("close log file, len=[%d]\n", (unsigned int)statistics.st_size);
			}
		} else {
			close(log_file_);
			log_file_ = -1;
		} 
	} 
#endif
	return 0;
}


int LogFileManager::Write_sd()
{
#ifndef WIN32
	if (!enable_) {
		return -1;
	}

	if(!enable_write_sd){
		return log_data_len;
	}
	int size = 0;

	if(!log_data_len){
		return -1;
	}

	if(get_log_statu){
		while(get_log_statu){
			usleep(1*1000);
			XMLogW("get_log_statu\n");
		}
	}

	receive_cache_statu = true;
	memcpy(log_data_sd,log_data,log_data_len);
	memset(log_data,0,log_data_len);
	size = log_data_len;
	log_data_len = 0;
	receive_cache_statu = false;

	if (log_file_ == -1) {
		char file_name[256] = {0};
		struct timeval tv = { 0 };
		gettimeofday(&tv, 0);  //获取相对于1970到现在的秒数
		tm nowTime;
		localtime_r(&tv.tv_sec, &nowTime);
		sprintf(file_name, "%s/%04d%02d%02d_%02d%02d%02d_log.txt", kTFCardPath,
			nowTime.tm_year + 1900, nowTime.tm_mon + 1, nowTime.tm_mday, 
			nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
		log_file_ = open(file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		XMLogW("create log file, name=[%s], ret=%d, len=%d\n", file_name, log_file_,size);
	}

	if (log_file_ != -1) {
		int ret = write(log_file_, log_data_sd, size);
		if (ret <= 0) {
			close(log_file_);
			log_file_ = -1;
			XMLogW("fprintf error, may be sdcard removed\n");
			return -1;
		}
		
		//立即刷新
		fsync(log_file_);

		if (log_data_total_len >= kMaxLogFileLen) {
			close(log_file_);
			log_file_ = -1;
			XMLogW("close log file, len=[%d]\n", log_data_total_len);
			log_data_total_len = 0;			
		}
	} 
#endif
	return 0;
}



int LogFileManager::get_log_data(char* log_str,int log_len, int64_t user)//此接口中不能使用 XMLog 这种接口打印数据
{
#ifndef WIN32
	if(receive_cache_statu){
		while(receive_cache_statu){
			usleep(1*1000);
			printf("receive_cache_statu\n");
		}
	}
	get_log_statu = true;
	if((log_data_len+log_len) > LOG_DATA_CACHE){
		printf("log data memset:%d\n",log_len);
		log_data_len = 0;
		memset(log_data,0,LOG_DATA_CACHE);
	}
	memcpy(log_data+log_data_len,log_str,log_len);
	log_data_len += log_len;
	log_data_total_len += log_len;
	get_log_statu = false;
#endif // !WIN32
	return 0;
}

void write_log_sd(lv_timer_t* timer)
{
	LogFileManager::Instance()->Write_sd();
}
void LogFileManager::write_log_sd_set(uint8_t enable_time, uint8_t enable_write)
{
#ifndef WIN32
	static lv_timer_t* log_timer_ = NULL;
	if(enable_write == 1){
		LogFileManager::Instance()->Enable_Write_Sd(true);
	}else if(enable_write == 0){
		LogFileManager::Instance()->Enable_Write_Sd(false);
	}else if(enable_write == 100){
		LogFileManager::Instance()->Enable_Write_Sd(false);
		if(log_file_ != -1){
			close(log_file_);
			log_file_ = -1;
		}
		log_data_total_len = 0;		
	}
	if(enable_time == 1){
		FILE *file=NULL;
		if(!log_timer_){
			file = fopen("/mnt/tfcard/syslog.bin","r");
			if(file){
				XMLogW("have syslog.bin file,start write log to sd\n");
				fclose(file);
				if(!log_timer_){
					log_timer_ = lv_timer_create(write_log_sd,1000,NULL);	
				}
			}else{
				XMLogW("no have syslog.bin file\n");
				LogFileManager::Instance()->Enable_Write_Sd(false);
			}
		}
	}else if(enable_time == 0){
		if(log_timer_){
			lv_timer_del(log_timer_);
			log_timer_ = NULL;
		}
	}
#endif
}
