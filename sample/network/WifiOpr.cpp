#include "WifiOpr.h"
#include <sstream> 
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "Log.h"
#include "xm_middleware_api.h"

using namespace std;

int GetMacParam(WIFI_MAC_PARAM_S* param)
{
#ifndef WIN32
	memset(param, 0, sizeof(WIFI_MAC_PARAM_S));
	
	ifstream infile;
	infile.open("/mnt/mtd/Config/hostapd.conf", ios::in);
	if (!infile) {
		XMLogE("/mnt/mtd/Config/hostapd.conf open failed!");
		return -1;
	}
	string temp;
	string ssid;
	string password;
	string param_flag;
	while (getline(infile, temp, '\n')) {
		param_flag = temp.substr(0, strlen("ssid"));
		XMLogW("param_flag:%s", param_flag.c_str());
		if (strcmp(param_flag.c_str(), "ssid") == 0) {
			ssid = temp.substr(5);
		}

		param_flag = temp.substr(0, strlen("wpa_passphrase"));
		XMLogW("param_flag:%s", param_flag.c_str());
		if (strcmp(param_flag.c_str(), "wpa_passphrase") == 0) {
			password = temp.substr(15);
		}
	}
	infile.close();

	XMSDK_NET_ATTR_S pstNetAttr;
	int result = XM_Middleware_WIFI_GetEthAttr("eth2", &pstNetAttr);
	if (result < 0) {
		XMLogE("wifi_mac_param error!");
		return -1;
	}

	memcpy(param->mac, pstNetAttr.mac, strlen(pstNetAttr.mac));
	memcpy(param->ssid, ssid.c_str(), strlen(ssid.c_str()));
	memcpy(param->password, password.c_str(), strlen(password.c_str()));
#endif
	return 0;
}

int CreateWifiParamFile(WIFI_PARAM_S* param)
{
	XMLogI("CreateWifiParamFile");
#ifndef WIN32
	FILE* fp = fopen("/mnt/mtd/Config/hostapd.conf", "r");
	if (fp == NULL) {
		system("cp /usr/sbin/hostapd.conf /mnt/mtd/Config/");
		system("cp /usr/sbin/hostapd_5G.conf /mnt/mtd/Config/");
		ChangeWifiParam(param, 3);
		XMLogI("CreateWifiParamFile, write ok");
	}

	if (fp) {
		fclose(fp);
	}
#endif

	return 0;
}

int ChangeWifiParam(WIFI_PARAM_S* wifi_param, int level)
{
	XMLogI("ChangeWifiParam");
	if (!wifi_param) {
		XMLogE("wifi_param error!");
		return -1;
	}

	ifstream infile;
	infile.open("/mnt/mtd/Config/hostapd.conf", ios::in);
	if (!infile) {
		XMLogE("/mnt/mtd/Config/hostapd.conf open failed!");
		return -1;
	}
	string temp;
	string ssid;
	string password;
	string param_flag;
	while (getline(infile, temp, '\n')) {
		param_flag = temp.substr(0, strlen("ssid"));
		XMLogW("param_flag:%s", param_flag.c_str());
		if (strcmp(param_flag.c_str(), "ssid") == 0) {
			ssid = temp.substr(5);
		}

		param_flag = temp.substr(0, strlen("wpa_passphrase"));
		XMLogW("param_flag:%s", param_flag.c_str());
		if (strcmp(param_flag.c_str(), "wpa_passphrase") == 0) {
			password = temp.substr(15);
		}
	}
	infile.close();

	XMLogI("Wifi name:%s, password:%s", wifi_param->name, wifi_param->password);
	char buf[1024] = { 0 };
	FILE* fp = fopen("/mnt/mtd/Config/hostapd.conf", "w");
	if (fp) {
		if (1 == level) {
			ssid = wifi_param->name;
		}
		else if (2 == level) {
			password = wifi_param->password;
		}
		else {
			ssid = wifi_param->name;
			password = wifi_param->password;
		}

		memset(buf, 0, sizeof(buf));
		sprintf(buf, "ssid=%s\n", ssid.c_str());
		fputs(buf, fp);
		
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "wpa_passphrase=%s\n", password.c_str());
		fputs(buf, fp);

		memset(buf, 0, sizeof(buf));
		#if 1
		strcpy(buf, "interface=eth2\ndriver=nl80211\nctrl_interface=/var/run/hostapd\nchannel=6\nieee80211n=1\nhw_mode=g\nht_capab=[SHORT-GI-20][SHORT-GI-40][HT40+][HT40-]\nignore_broadcast_ssid=0\nwpa=2\nrsn_pairwise=CCMP\nmax_num_sta=1\n");
		#else
		strcpy(buf, "interface=eth2\ndriver=nl80211\nctrl_interface=/var/run/hostapd\nchannel=6\nieee80211n=1\nhw_mode=g\nignore_broadcast_ssid=0\nwpa=2\nrsn_pairwise=CCMP\nmax_num_sta=1\n");
		#endif
		fputs(buf, fp);

		fclose(fp);
	}
	else {
		XMLogE("/mnt/mtd/Config/hostapd.conf open failed!");
		return -1;
	}

	fp = fopen("/mnt/mtd/Config/hostapd_5G.conf", "w");
	if (fp) {
		if (1 == level) {
			ssid = wifi_param->name;
		}
		else if (2 == level) {
			password = wifi_param->password;
		}
		else {
			ssid = wifi_param->name;
			password = wifi_param->password;
		}

		memset(buf, 0, sizeof(buf));
		sprintf(buf, "ssid=%s\n", ssid.c_str());
		fputs(buf, fp);
		
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "wpa_passphrase=%s\n", password.c_str());
		fputs(buf, fp);

		memset(buf, 0, sizeof(buf));
		#if 1
		strcpy(buf, "interface=eth2\nctrl_interface=/var/run/hostapd\nchannel=36\nwpa=2\nbeacon_int=100\ndriver=nl80211\nhw_mode=a\nht_capab=[SHORT-GI-20][SHORT-GI-40][HT40+][HT40-]\nwme_enabled=1\nht_capab=[SHORT-GI-20][SHORT-GI-40][HT40+][HT40-]\nwpa_key_mgmt=WPA-PSK\nrsn_pairwise=CCMP\nieee80211n=1\nmax_num_sta=1\nwpa_group_rekey=86400\ncountry_code=CN\n");
		#else
		strcpy(buf, "interface=eth2\nctrl_interface=/var/run/hostapd\nchannel=36\nwpa=2\nbeacon_int=100\ndriver=nl80211\nhw_mode=a\nwme_enabled=1\nht_capab=[SHORT-GI-20][SHORT-GI-40][HT40+][HT40-]\nwpa_key_mgmt=WPA-PSK\nrsn_pairwise=CCMP\nieee80211n=1\nmax_num_sta=1\nwpa_group_rekey=86400\ncountry_code=CN\n");
		#endif
		fputs(buf, fp);

		fclose(fp);
	}
	else {
		XMLogE("/mnt/mtd/Config/hostapd_5G.conf open failed!");
		return -1;
	}

	return 0;
}

int WifiEnableOpr(bool enable)
{
	XMLogI("WifiEnable, enable=%d", (int)enable);
#ifndef WIN32
	if (enable) {
		XM_Middleware_SafeSystem("/usr/sbin/hostapd /mnt/mtd/Config/hostapd.conf -B -ddd");
		XM_Middleware_SafeSystem("/sbin/udhcpd -fS /usr/sbin/udhcpd.conf &");
	}
	else {
		XM_Middleware_SafeSystem("killall -7 hostapd");
		XM_Middleware_SafeSystem("killall -7 udhcpd");
	}
#endif
	return 0;
}
