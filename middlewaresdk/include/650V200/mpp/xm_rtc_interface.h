/*
* xm_rtc_interface.h
*
*	Created on: 2021年11月10日
*		Author: mdp
*/

#ifndef _XM_RTC_INTERFACE_H_
#define _XM_RTC_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define RTC_MAJOR_VERSION               (0)             /* 哭声检测算法主版本号 */
#define RTC_MINOR_VERSION               (1)             /* 哭声检测算法次版本号 */
#define RTC_REVERSION                   (1)             /* 哭声检测算法修改版本号 */
#define RTC_BUILD                       (0)             /* 哭声检测算法编译版本号(暂不用) */


 
 int XM_RTC_INIT(int fps, void **ppvHandle);
 
 int XM_RTC_WORK(short* indata, short* outsign, void **ppvHandle);
 
 
 
#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif /* _IA_GMM_INTERFACE_H_ */