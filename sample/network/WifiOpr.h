#ifndef WIFI_OPR_H_
#define WIFI_OPR_H_

#include "xm_middleware_def.h"

int GetMacParam(WIFI_MAC_PARAM_S* param);
int CreateWifiParamFile(WIFI_PARAM_S* param);
int ChangeWifiParam(WIFI_PARAM_S* param, int level);
int WifiEnableOpr(bool enable);

#endif