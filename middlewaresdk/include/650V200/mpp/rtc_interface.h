

#ifndef _RTC_INTERFACE_H_
#define _RTC_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

//回声消除参数设置结构体
typedef struct
{
	short UserMode;                       // 用户模式 0：自动模式 1：用户模式
	short CngMode;                        // 舒适噪声模式  0：关闭 1：开启
	short NearSilenceEnergyMode;          //近端信号强制清零的能量阈值
	short EchoFreq;                       //语音处理频段1
	short MixedFreq;                      //语音处理频段2
	short ERLEBand[6];                    //ERLE(回波损耗增益)保护频段数组参数
	short ERLE[7];                        //ERLE(回声损耗增益)频段保护值数组，配合参数ERLBand[6]参数使用
	short SpeechProtectFreqLow;           // 近端低频保护区域频点低频参数
	short SpeechProtectFreqHigh;          // 近端低频保护区域频点高频参数
	short LatencyEstimation;              // 延迟估计模式
	short EchoMode;                       //回声残留模式
	short msInSndCardBuf;                 //声卡延迟
} AEC_CONFIG_X;   

 // 语音降噪参数设置结构体
typedef struct
{
	short UserMode;                     // 用户模式 0:自动模式 1:用户模式
	short DenoiseLevel;                 // 降噪程度配置参数
	short NoiseGate;                    // 噪声门限配置参数
}ANS_CONFIG_X;
 
// 语音自动增益模式
typedef struct
{
	short UserMode;                    // 用户模式 0：自动模式 1：用户模式 
	short AgcMode;                     // 增益模式
	short TargetLevelDb;               // 目标电平
	short CompressionGaindB;           // 最大增益
    short LimiterEnable;               // 压限器开关
	short HighPassFilter;              // 高通滤波器

} AGC_CONFIG_X; 

// 回声消除函数  farend：远端信号  nearend：近端信号 output：输出信号
int AEC_Create(void** aecInst);                                             // 创建
int AEC_Init(void* aecInst);                                                // 初始化
int AEC_set_config(void * aecInst, AEC_CONFIG_X config);                    // 参数配置
int AEC_WORK(void* aecInst, short* farend, short* nearend, short* output);  // 工作  
int AEC_Free(void* aecInst);                                                // 释放
 
// 语音降噪函数 indata：输入信号  outsign：输出信号
int ANS_Create(void **ppvHandle);                              // 创建
int ANS_Init(void *ppvHandle);                                 // 初始化
int ANS_set_config(void * ppvHandle, ANS_CONFIG_X config);     // 参数配置
int ANS_WORK(short* indata, short* outsign, void *ppvHandle);  // 工作
int ANS_Free(void *ansInst);                                   // 释放
 
 
// 语音增益函数  inNear: 输入信号   out：输出信号
 int AGC_Create(void **agcInst);                            // 创建
 int AGC_Init(void *agcInst);                               // 初始化
 int AGC_set_config(void* agcInst, AGC_CONFIG_X config);    // 参数配置
 int AGC_WORK(void* agcInst, const short* inNear,
                      const short* inNear_H,
                      short samples,
                      short* out,
                      short* out_H,
                      int inMicLevel,
                      int* outMicLevel,
                      short echo,
                      unsigned char* saturationWarning);    // 工作
 int AGC_Free(void *agcInst);                               // 释放
 
  // 打印版本号
 void librtc_version_info(char *info, int size);
 
#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif /* _IA_GMM_INTERFACE_H_ */
