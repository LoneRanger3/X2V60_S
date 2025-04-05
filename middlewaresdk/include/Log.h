#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "xm_middleware_def.h"

void SetGlobalLogLevel(XMLogLevel level);
void XMLog(XMLogLevel level, const char* str_file, const char* str_fun, int line, const char* msg, ...);

#define XMLogD(...)  XMLog(XM_LOG_VERBOSE, __FILE__,__FUNCTION__, __LINE__, __VA_ARGS__)
#define XMLogI(...)  XMLog(XM_LOG_INFO, __FILE__,__FUNCTION__, __LINE__, __VA_ARGS__)
#define XMLogW(...)  XMLog(XM_LOG_WARNING, __FILE__,__FUNCTION__, __LINE__, __VA_ARGS__)
#define XMLogE(...)  XMLog(XM_LOG_ERROR, __FILE__,__FUNCTION__, __LINE__, __VA_ARGS__)

