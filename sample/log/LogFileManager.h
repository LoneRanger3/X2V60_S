#ifndef FILE_LOG_MANAGER_H
#define FILE_LOG_MANAGER_H

#include <stdio.h>
#include <stdint.h>
#define LOG_DATA_CACHE (16*1024)

class LogFileManager
{
private:
    /* data */
public:
    static LogFileManager* Instance();
	static void Uninstance();

	int Enable(bool enable);
    int Enable_Write_Sd(bool enable);
	int Write(char* log_str);
    int Write_sd();
    int get_log_data(char* log_str,int log_len, int64_t user);
    void write_log_sd_set(uint8_t enable_time, uint8_t enable_write);
private:
	int log_file_;
    int log_data_len;
    int log_data_total_len;
    char log_data[LOG_DATA_CACHE];
    char log_data_sd[LOG_DATA_CACHE];
	bool enable_;
    bool enable_write_sd;//写卡开关
    bool receive_cache_statu;
    bool get_log_statu;
    LogFileManager(/* args */);
    ~LogFileManager();
    static LogFileManager* instance_;
};

#endif
