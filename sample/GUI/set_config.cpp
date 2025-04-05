#include "set_config.h"
#include "Log.h"
#include "global_data.h"
#include "cJSON.h"
#include <algorithm>
#include <string>
using namespace std;

static string path;
static char line_right[256];
#ifdef WIN32
#define language_path "D:\\resource\\"
#else
#define language_path "/mnt/custom/Strings/"
#endif

char* GetParsedString(const char* left_string) {
	FILE* fp = fopen(path.c_str(), "r");
	if (fp == NULL) {
		XMLogE("fopen error");
		return (char*)left_string;
	}

	int str_len = strlen(left_string);
	int max_len = 256 > (str_len * 2 + 2) ? 256 : (str_len * 2 + 2);
	char read_line[max_len];
	while (fgets(read_line, max_len, fp)) {
		for (int i = max_len; i >= 0; i--) {
			if (read_line[i] == '\n') {
				read_line[i] = '\0';
			}
		}

		string s(read_line);
		string::iterator t = find(s.begin(), s.end(), '=');
		if (t != s.end()) {
			string left_str = s.substr(0, t - s.begin());
			string right_str = s.substr(t - s.begin() + 1, s.end() - t);
			if (strcmp(left_str.c_str(), left_string) == 0) {		
				memset(line_right, 0, sizeof(line_right));
				strcpy(line_right, right_str.c_str());
				fclose(fp);
				return line_right;
			}
		}
	}
	fclose(fp);
	XMLogW("%s not found", left_string);
	return (char*)left_string;
}


int AlarmRegionCoorSw(float coor, float length, bool flag)
{
	if (flag) {
		return (coor / length) * 8192;
	}
	else {
		return (coor / 8192) * length;
	} 
}

void set_language(int language)
{
	if(language == English)
		path = language_path"English";
	else if(language == SimpChinese)
		path = language_path"SimpChinese";
	else if(language == TradChinese)
		path = language_path"TradChinese";
}

const int kMaxConfigFileLen = 32*1024;
#ifdef WIN32
const char* kConfigFileName = "D:\\resource\\car_cfg.json";
#else
const char* kConfigFileName = "/mnt/mtd/Config/car_cfg.json";
#endif
CarConfig::CarConfig() : json_obj_(NULL)
{

}

CarConfig::~CarConfig()
{
	if (json_obj_) {
		cJSON_Delete(json_obj_);
	}
}

int CarConfig::ReadConfigFromFile()
{
	FILE* fp = fopen(kConfigFileName, "r+");
	if (!fp) {
		return SetDefaultConfig();
	}

	char buffer[kMaxConfigFileLen] = {0};
	int len = fread(buffer, 1, kMaxConfigFileLen, fp);
	fclose(fp);
	if (len >= kMaxConfigFileLen || len <= 0) {
		XMLogE("read config file error, name=[%s], len=[%d]", kConfigFileName, len);
		return SetDefaultConfig();
	}

	json_obj_ = cJSON_Parse(buffer);
	if (!json_obj_) {
		XMLogE("parse config file error, name=[%s]", kConfigFileName);
		return SetDefaultConfig();
	}
	XMLogI("read config file , name=[%s], len=[%d]", kConfigFileName, len);
	return 0;
}

int CarConfig::SaveToFile(cJSON* json_obj)
{
	char* str_root = cJSON_Print(json_obj);
	FILE* fp = fopen(kConfigFileName, "w+");
	if (!fp) {
		free(str_root);
		XMLogE("write config file error, file name=[%s], value=%s", kConfigFileName, str_root);
		return -1;
	}
	fwrite(str_root, 1, strlen(str_root), fp);
	//XMLogI("save config file , value=[%s]", str_root);
	fclose(fp);
	free(str_root);

	return 0;
}

int CarConfig::SetDefaultConfig()
{
	int count = sizeof(CFG_ALL_OPERATION_UNITS)/sizeof(XM_CONFIG_UNIT);
	cJSON *root = cJSON_CreateObject();
	for (int i = 0; i < count; i++) {
		XM_CONFIG_UNIT config_unit = CFG_ALL_OPERATION_UNITS[i];
		if (config_unit.value_type == CFG_Operation_Value_Int) {
			cJSON_AddNumberToObject(root,  config_unit.opr_str, config_unit.default_value.int_value);
		} else if (config_unit.value_type == CFG_Operation_Value_Bool) {
			cJSON_AddBoolToObject(root,  config_unit.opr_str, config_unit.default_value.bool_value);
		} else if (config_unit.value_type == CFG_Operation_Value_Float) {
			cJSON_AddNumberToObject(root,  config_unit.opr_str, config_unit.default_value.float_value);
		}
	}
	
	int ret = SaveToFile(root);
	if (ret < 0) {
		return -1;
	}

	XMLogI("set config to default value");

	if (json_obj_) {
		cJSON_Delete(json_obj_);
	}
	json_obj_ = root;
	return 0;
}

int CarConfig::GetValue(const XM_CONFIG_OPERATION& cfg_opr, XM_CONFIG_VALUE& out_value)
{
	if (!json_obj_) {
		return -1;
	}

	XM_CONFIG_UNIT config_unit = CFG_ALL_OPERATION_UNITS[cfg_opr];
	cJSON* param = cJSON_GetObjectItem(json_obj_, config_unit.opr_str);
	if (!param) {
		XMLogE("can't find this opration, opr=%s, may be it is added lately, should delete the old config file", config_unit.opr_str);
		return -1;
	}

	if (config_unit.value_type == CFG_Operation_Value_Int) {
		out_value.int_value = param->valueint;
	} else if (config_unit.value_type == CFG_Operation_Value_Bool) {
		out_value.bool_value = cJSON_IsTrue(param) ? true : false;
	} else if (config_unit.value_type == CFG_Operation_Value_Float) {
		out_value.int_value = param->valuedouble;
	}

	return 0;
}

int CarConfig::SetValue(const XM_CONFIG_OPERATION& cfg_opr, const XM_CONFIG_VALUE& in_value)
{
	if (!json_obj_) {
		return -1;
	}

	XM_CONFIG_UNIT config_unit = CFG_ALL_OPERATION_UNITS[cfg_opr];
	if (config_unit.value_type == CFG_Operation_Value_Int) {
		cJSON_ReplaceItemInObject(json_obj_,  config_unit.opr_str, cJSON_CreateNumber(in_value.int_value));
	} else if (config_unit.value_type == CFG_Operation_Value_Bool) {
		cJSON_ReplaceItemInObject(json_obj_,  config_unit.opr_str, cJSON_CreateBool(in_value.bool_value));
	} else if (config_unit.value_type == CFG_Operation_Value_Float) {
		cJSON_ReplaceItemInObject(json_obj_,  config_unit.opr_str, cJSON_CreateNumber(in_value.float_value));
	}

	SaveToFile(json_obj_);
	return 0;
}

int CarConfig::SetMultiValue(XM_CONFIG_OPERATION cfg_oprs[], XM_CONFIG_VALUE in_values[], int count)
{
	if (!json_obj_) {
		return -1;
	}

	for (int i = 0; i < count; i++) {
		XM_CONFIG_UNIT config_unit = CFG_ALL_OPERATION_UNITS[cfg_oprs[i]];
		if (config_unit.value_type == CFG_Operation_Value_Int) {
			cJSON_ReplaceItemInObject(json_obj_,  config_unit.opr_str, cJSON_CreateNumber(in_values[i].int_value));
		} else if (config_unit.value_type == CFG_Operation_Value_Bool) {
			cJSON_ReplaceItemInObject(json_obj_,  config_unit.opr_str, cJSON_CreateBool(in_values[i].bool_value));
		} else if (config_unit.value_type == CFG_Operation_Value_Float) {
			cJSON_ReplaceItemInObject(json_obj_,  config_unit.opr_str, cJSON_CreateNumber(in_values[i].float_value));
		}
	}
	SaveToFile(json_obj_);
	return 0;
}

