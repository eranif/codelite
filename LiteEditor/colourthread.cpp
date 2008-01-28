#include "precompiled_header.h"
#include "colourthread.h"
#include "colourrequest.h"

#if 0
ColourThread* ColourThread::ms_instance = 0;

ColourThread::ColourThread()
{
}

ColourThread::~ColourThread()
{
}

ColourThread* ColourThread::Instance()
{
	if(ms_instance == 0){
		ms_instance = new ColourThread();
	}
	return ms_instance;
}

void ColourThread::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

void ColourThread::ProcessRequest(ThreadRequest *request)
{
}
#endif
