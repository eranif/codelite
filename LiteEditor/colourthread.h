#ifndef __colourthread__
#define __colourthread__
#if 0
#include "worker_thread.h"

class ColourThread : public WorkerThread {

	static ColourThread* ms_instance;

public:
	static ColourThread* Instance();
	static void Release();

private:
	ColourThread();
	~ColourThread();

public:
	virtual void ProcessRequest(ThreadRequest *request);
};
#endif 
#endif // __colourthread__

