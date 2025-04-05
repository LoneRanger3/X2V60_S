#pragma once
#include "xm_type.h"

typedef struct MEDIA_FORMATE
{
	XM_U32 u32ViCapX;
	XM_U32 u32ViCapY;
	XM_U32 u32ViCapWidth;
	XM_U32 u32ViCapHeight;
	XM_U32 u32ViDestWidth;
	XM_U32 u32ViDestHeight;
}MEDIA_FORMATE_S;

extern const MEDIA_FORMATE_S kMediaFormate[256];
