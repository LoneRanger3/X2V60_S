#pragma once
#include "XMThread.h"
#include "HttpDef.h"

class HttpUnit
{
public:
	HttpUnit();
	~HttpUnit();

	int Init(XMMsgInfo& msg_info);
	int Clean();
	int Run();
	bool thread_exit() { return thread_exit_; };

private:
	XMMsgInfo msg_info_;
	bool thread_exit_;
	OS_THREAD send_thread_;
};

