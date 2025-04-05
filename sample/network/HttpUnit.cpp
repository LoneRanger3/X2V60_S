#include "HttpUnit.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#ifndef WIN32
#include <fcntl.h>
#include <sys/prctl.h>
#endif
#include "Log.h"
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
// #include "HttpServer.h"
#include "CommDef.h"

const int kBufSize = 16384;
using namespace std;

HttpUnit::HttpUnit(): thread_exit_(false)
{
    msg_info_.engineId = -1;
    msg_info_.connId = -1;
}

HttpUnit::~HttpUnit()
{
    int engineId = msg_info_.engineId;
    int connId = msg_info_.connId;
    if (engineId >= 0 && connId >= 0) {
        XMLogW("~HttpUnit, engineId=%d, connId=%d", engineId, connId);
        XM_Middleware_Network_CloseClient(engineId, connId);

    }
    Clean();
}

void ThreadPlayFunction(void* param)
{
	static_cast<HttpUnit*>(param)->Run();
}

int HttpUnit::Init(XMMsgInfo& msg_info)
{
	msg_info_ = msg_info;
	CreateThreadEx(send_thread_, (LPTHREAD_START_ROUTINE)ThreadPlayFunction, this);
	return 0;
}

int HttpUnit::Clean()
{
    thread_exit_ = true;
    WaitForSingleObjectEx(send_thread_, INFINITE);
    CloseThreadEx(send_thread_);
	return 0;
}


int64_t FindIFrameBeginPos(FILE* fp, int pos)
{
    const int kBlkLen = 188;
    char buffer[kBlkLen];
    int64_t l_pos = pos / kBlkLen * kBlkLen;
    fseek(fp, l_pos, SEEK_SET);

    for (int i = 0; i < 10000; i++) {
        int len = fread(buffer, 1, kBlkLen, fp);
        if (len != kBlkLen) {
            XMLogE("FindIFrameBeginPos, read err, i=%d", i);
            break;
        }
        uint8_t* ptr = (uint8_t*)buffer;
        if (ptr[0] == 0x47) {
            uint32_t Code = 0xffffffff;
            for (int j = 0; j < 40; j++) {
                Code = (Code << 8) | ptr[j];
                if (Code == 0x00000140) {
                    int64_t iframe_pos = l_pos + i * kBlkLen;
                    XMLogI("pos=%d, i frame begin pos=%lld", pos, iframe_pos);
                    return iframe_pos;
                }
            }
        }
        else {
            break;
        }
    }
    XMLogE("!!!not find nearest I frame, in 10000 block");
    return pos;
}

int HttpUnit::Run()
{
#ifndef WIN32
	prctl(PR_SET_NAME, "http_unit");
#endif   
    int engineId = msg_info_.engineId;
    int connId = msg_info_.connId;
    char* msg = (char*)(msg_info_.msg.c_str());
    char* connType = (char*)(msg_info_.connType.c_str());

    bool play_thumbnail = false;
    if (strstr(msg, "getthumbnail") != NULL) {
        play_thumbnail = true;
    }

    if (play_thumbnail) {
        char* file_path = strstr(msg, "=") + 1;
        XMLogW("play thumbnail-->filename:%s", msg);
        char thumbnail_buf[MAX_PNG_FILE_LEN];
        int thumbnail_len = 0;
        int ret = XM_Middleware_Picture_GetFromRecord(XM_FILE_FORMAT_MP4, file_path, thumbnail_buf, &thumbnail_len);
        if (ret < 0 || thumbnail_len <= 0 || thumbnail_len > MAX_PNG_FILE_LEN) {
            XMLogW("XM_Middleware_Picture_GetFromRecord failed! path:%s, len:%d", file_path, thumbnail_len);
            thread_exit_ = true;
            return -1;
        }

        // static int zgf = 1000;
        // char str_file[64] = {0};
        // sprintf(str_file, "/mnt/tfcard/%d.png", zgf++);
        // FILE* fp = fopen(str_file, "wb");
        // if (fp) {
        //     fwrite(thumbnail_buf, 1, thumbnail_len, fp);
        //     fclose(fp);
        // }
        StatusCode status_code = Ok;
        ostringstream oss;
        oss << "HTTP/1.1 " << status_code << " message" << "\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Accept-Ranges: bytes\r\n";
        oss << "Content-Type: application/octet-stream\r\n";
        oss << "Content-Length: " << thumbnail_len << "\r\n";
        oss << "\r\n";

        int result = XM_Middleware_Network_SendFrame(engineId, connId, (char*)oss.str().c_str(), oss.str().length());
        unsigned int remain_len = thumbnail_len;
        XMLogW("-------connId=%d, total len=%d--------", connId, remain_len);

        char* beg_ptr = thumbnail_buf;
        chrono::milliseconds dura(5);
        while (remain_len > 0 && !thread_exit_) {
            int read_len = remain_len > kBufSize ? kBufSize : remain_len;
            int isend = XM_Middleware_Network_SendFrame(engineId, connId, beg_ptr, read_len);
            if (isend < 0) {
                XMLogI("connId=%d,isend:%d,remain_len:%d", connId, isend, remain_len);
                if (isend == -99) {
                    int cir_num = 0;
                    while (isend < 0) {
                        XMLogI("connId=%d,isend:%d,cir_num:%d", connId, isend, cir_num);
                        std::this_thread::sleep_for(dura);
                        isend = XM_Middleware_Network_SendFrame(engineId, connId, beg_ptr, read_len);
                        if (isend == -102 || cir_num > 10) {
                            thread_exit_ = true;
                            return -1;
                        }
                        cir_num++;
                    }
                }
                else {
                    thread_exit_ = true;
                    return -1;
                }
            }

            beg_ptr += read_len;
            remain_len -= read_len;
        }
    }
    else {
        XMLogW("play video-->filename:%s", msg);
        FILE* fp = fopen(msg, "rb");
        if (!fp) {
            XMLogE("%s is open failed", msg);
            thread_exit_ = true;
            return -1;
        }

        fseek(fp, 0, SEEK_END);
        int video_length = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char buf[kBufSize];
        int beg_num = 0, end_num = 0;
        char* ptmp = strstr(connType, "bytes");
        StatusCode status_code = PartialContent;
        if (ptmp == NULL) {
            beg_num = 0;
            end_num = video_length - 1;
            status_code = Ok;
        }
        else {
            string range_value(connType);
            XMLogW("range len=%d, value=%s", range_value.length(), range_value.c_str());
            int pos = range_value.find("-");
            string beg = range_value.substr(7, pos - 7);
            string end = range_value.substr(pos + 1);
            //XMLogW("brg:%s,pos:%d",beg.c_str(), pos);
            if (pos == range_value.length() - 2) {
                end_num = video_length - 1;
                beg_num = atoi(beg.c_str());
                //beg_num = FindIFrameBeginPos(fp, beg_num);
            }
            else if (pos == 7) {
                beg_num = video_length - atoi(end.c_str());
                end_num = video_length - 1;
            }
            else {
                beg_num = atoi(beg.c_str());
                end_num = atoi(end.c_str());
                // beg_num = FindIFrameBeginPos(fp, beg_num);
                // XMLogW("222, begin pos=%d", beg_num);
            } 
        }

        if (beg_num < 0 || beg_num > end_num || beg_num > video_length || end_num < 0 || end_num > video_length) {
            XMLogW("playback params error! beg_num:%d, end_num:%d, video_length:%d", beg_num, end_num, video_length);
            fclose(fp);
            thread_exit_ = true;
            return -1;
        }

        unsigned int remain_len = end_num - beg_num + 1;
        ostringstream oss;
        oss << "HTTP/1.1 " << status_code << " message" << "\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Accept-Ranges: bytes\r\n";
        oss << "Content-Type: application/octet-stream\r\n";
        if (status_code == PartialContent)
            oss << "Content-Range: bytes " << beg_num << "-" << end_num << "/" << video_length << "\r\n";
        oss << "Content-Length: " << remain_len << "\r\n";
        oss << "\r\n";
        //  XMLogI("resp_body:%s", oss.str().c_str());
        int result = XM_Middleware_Network_SendFrame(engineId, connId, (char*)oss.str().c_str(), oss.str().length());
        // ��Ҫ��need_len���ֽ�
        XMLogW("-------connId=%d, total len=%d, beg_num:%d, end_num:%d--------",
            connId, remain_len, beg_num, end_num);

        fseek(fp, beg_num, SEEK_SET);
        chrono::milliseconds dura(50);
        while (remain_len > 0 && !thread_exit_) {
            int blk_len = remain_len > kBufSize ? kBufSize : remain_len;
            int read_len = fread(buf, 1, blk_len, fp);
            if (read_len <= 0) {
                break;
            }
            int isend = XM_Middleware_Network_SendFrame(engineId, connId, buf, read_len);
            if (isend < 0) {
                // XMLogI("connId:%d,isend:%d,remain_len:%d", connId, isend, remain_len);
                if (isend == -99) {
                    int cir_num = 0;
                    while (isend < 0 && !thread_exit_) {
                        // XMLogI("connId:%d,isend:%d,cir_num:%d", connId, isend, cir_num);
                        std::this_thread::sleep_for(dura);
                        isend = XM_Middleware_Network_SendFrame(engineId, connId, buf, read_len);
                        if (isend == -102 || cir_num > 20) {
                            fclose(fp);
                            XMLogI("file is send error, connId:%d,isend:%d,cir_num:%d", connId, isend, cir_num);
                            thread_exit_ = true;
                            return -1;
                        }
                        cir_num++;
                    }
                }
                else {
                    fclose(fp);
                    XMLogI("file is send error, connId:%d,isend:%d", connId, isend);
                    thread_exit_ = true;
                    return -1;
                }
            }
            remain_len -= read_len;
        }
        fclose(fp);
    }

    XMLogW("file is send over ,connid:%d close thread", connId);
    thread_exit_ = true;
	return 0;
}
