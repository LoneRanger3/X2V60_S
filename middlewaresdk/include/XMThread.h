#ifndef XM_THREAD_H
#define XM_THREAD_H

#include <stdint.h>
#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#else
#include <sys/time.h>
#include <sys/times.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <signal.h>
#include <pthread.h>

#endif

#ifndef WIN32
#define HANDLE			void*
#define WINAPI
#define INFINITE		0xFFFFFFFF
typedef void* (WINAPI *LPTHREAD_START_ROUTINE)(void* lpThreadParameter);
#endif

class COSThread
{
public:
	COSThread();
	virtual ~COSThread();

#ifdef WIN32
	HANDLE		thread_;
#else
	pthread_t	thread_;
#endif
	bool		created_;
};

#define OS_THREAD		COSThread
int	CreateThreadEx(OS_THREAD &thread, LPTHREAD_START_ROUTINE lpStartAddress, void* lpParameter);
void CloseThreadEx(OS_THREAD &thread);
uint32_t WaitForSingleObjectEx(OS_THREAD &thread, uint32_t dwMilliseconds);

#endif