
#ifndef _SUN_PUBLIC_H_
#define _SUN_PUBLIC_H_

#define _DATABLOCK_MAX_ 132
//智阳
//获取当前版本号
// extern "C" int   SunVersion(char *version);
// //设置加密类型目前默认为2 不用调用
// extern "C" int   SunSetEncType(int i);
// //获取加密类型
// extern "C" int   SunGetEncType(void);
// //加密数据
// extern "C" int   SunEncrypt(unsigned char *chInput,int inputlength,unsigned char *chOutput,int *outlength);
// //生产数据头
// extern "C" int   MakeHeader(char *output,int count);
// //生成一条非加密数据
// extern "C" int   MakeRawDataBlock(int nframe,char *chInfomation ,int chInfolength,unsigned char *chOutput);
// //生成一条加密数据
// extern "C" int   MakeEncryptDataBlock(int nframe,char *chInfomation ,int chInfolength,unsigned char *chOutput);
// //解码gps数据 codectype目前只支持5或者6
// extern "C" void  sunGpsGXDecode(int codectype,float *outflon, float *outfLat,float fLon, float fLat);
// //解码gps数据 codectype目前只支持5或者6
// extern "C" int   SunCoordTransformation(int codectype,int *OutLong, int *OutLat,int InLong, int InLat);
// //收到数据后，返回0，验证成功，返回非0验证失败
// extern "C" int   sunCheckDataValid(char * reciveData);



//新高成

//设置加密类型目前默认为2 不用调用
extern "C" int   SunSetEncType(int i);
//获取加密类型
extern "C" int   SunGetEncType();

//生产数据头
extern "C" int  MakeHeader(char *output,int count);
//生成一条加密数据
extern "C" int  MakeEncryptDataBlock(int nframe,char *chInfomation ,int chInfolength,unsigned char *chOutput);
//
extern "C" int  SunCoordTransformation(int *OutLong, int *OutLat,int InLong, int InLat);
#endif
