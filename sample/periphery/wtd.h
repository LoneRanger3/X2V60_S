#ifndef WDT_H_
#define WDT_H_

#ifdef  _cplusplus
#if _cplusplus
extern "C"
{
#endif
#endif //  _cplusplus

	int WdtCreate(void);
	int WdtSetTimeout(unsigned int timeout);
	int WdtDestory(void);
	int WdtFeed(void);

#ifdef _cplusplus
#if _cplusplus
}
#endif
#endif // _cplusplus

#endif // !WDT_H_
