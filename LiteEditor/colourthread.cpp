#include "colourthread.h"
colourthread* colourthread::ms_instance = 0;

colourthread::colourthread()
{
}

colourthread::~colourthread()
{
}

colourthread* colourthread::Instance()
{
	if(ms_instance == 0){
		ms_instance = new colourthread();
	}
	return ms_instance;
}

void colourthread::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

void colourthread::ProcessRequest(ThreadRequest *request)
{
}
