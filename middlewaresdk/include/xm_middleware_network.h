#pragma once

#include <string>
#include <stdint.h>
#include "xm_middleware_def.h"
#include "HttpDef.h"

//http
int XM_Middleware_Network_StartServer(int signal_port, int media_port);
int XM_Middleware_Network_StopServer();
int XM_Middleware_Network_SetDataCallback(XM_Data_CallBack cb);
//int XM_Middleware_Network_SetEventCallback(XM_Event_CallBack cb);
int XM_Middleware_Network_SendBuffer(int engineId, int connId, uint8_t type, char * data, int contentLen);
int XM_Middleware_Network_SendTcpBuffer(int engineId, uint8_t type, char* data, int len);
int XM_Middleware_Network_SendFrame(int engineId, int connId, char* data, int len);
int XM_Middleware_Network_SendOnly(int engineId, int connId);
int XM_Middleware_Network_CloseThreadCallback(XM_CloseThread_CallBack cb);
int XM_Middleware_Network_CloseThread(int connId);
int XM_Middleware_Network_CloseAllThread();
int XM_Middleware_Network_CloseHttpUnit(void);
int XM_Middleware_Network_CloseClient(int engineId, int connId);

//xmip
int XM_Middleware_Network_XMIP_StartServer(int signal_port, int media_port);
int XM_Middleware_Network_XMIP_StopServer();
int XM_Middleware_Network_XMIP_SetDataCallback(XM_Data_CallBack cb);
int XM_Middleware_Network_XMIP_SendBuffer(int engineId, int connId, uint8_t type, char* data, int contentLen);
int XM_Middleware_Network_XMIP_SendFrame(int engineId, int connId, char* data, int len, bool video_frame, bool key_frame, uint32_t pts, int id);
int XM_Middleware_Network_XMIP_SetFrameList(int count);
int XM_Middleware_Network_XMIP_ClearFrameList();

//客户端
void XM_Middleware_Network_Client_Startup();
int XM_Middleware_Network_Client_StopClient();
int XM_Middleware_Network_Client_StartClient(char *ip, int signal_port, int media_port);
int XM_Middleware_Network_Client_SendData(uint8_t type, char* buf, int len);
int XM_Middleware_Network_Client_SetEventCallback(XM_Event_CallBack cb);
int XM_Middleware_Network_Client_SetDataCallback(XM_Data_CallBack cb);