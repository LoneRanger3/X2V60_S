#include "MediaFormat.h"

#ifndef WIN32

#include "fvideo.h"

const MEDIA_FORMATE_S kMediaFormate[256] =
{
	[VIDEO_FORMAT_UNKNOW] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 1280,
		.u32ViCapHeight = 720,
		.u32ViDestWidth = 1280,
		.u32ViDestHeight = 720,
	},
	[VIDEO_FORMAT_CVBS] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 3840,
		.u32ViCapHeight = 240,
		.u32ViDestWidth = 960,
		.u32ViDestHeight = 240,
	},
	[VIDEO_FORMAT_AHDH] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 1920,
		.u32ViCapHeight = 1080,
		.u32ViDestWidth = 1920,
		.u32ViDestHeight = 1080,
	},
	[VIDEO_FORMAT_CVIH] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 1920,
		.u32ViCapHeight = 1080,
		.u32ViDestWidth = 1920,
		.u32ViDestHeight = 1080,
	},
	[VIDEO_FORMAT_TVIH] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 1920,
		.u32ViCapHeight = 1080,
		.u32ViDestWidth = 1920,
		.u32ViDestHeight = 1080,
	},

	[VIDEO_FORMAT_AHD5M_NRT] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 2592,
		.u32ViCapHeight = 1944,
		.u32ViDestWidth = 2592 / 2,
		.u32ViDestHeight = 1944,
	},
	[VIDEO_FORMAT_AHDM_25] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 2560,
		.u32ViCapHeight = 720,
		.u32ViDestWidth = 1280,
		.u32ViDestHeight = 720,
	},

	[VIDEO_FORMAT_AHD5M] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 2592,
		.u32ViCapHeight = 1944,
		.u32ViDestWidth = 2592 / 2,
		.u32ViDestHeight = 1944 / 2,
	},
	[VIDEO_FORMAT_TVI8M_15] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 3840,
		.u32ViCapHeight = 2160,
		.u32ViDestWidth = 1920,
		.u32ViDestHeight = 1080,
	},
	[VIDEO_FORMAT_AHD8M_15P] = {
		.u32ViCapX = 0,
		.u32ViCapY = 0,
		.u32ViCapWidth = 3840 / 2,
		.u32ViCapHeight = 2160,
		.u32ViDestWidth = 1920 / 2,
		.u32ViDestHeight = 1080,
	},
};

#endif //#ifndef WIN32