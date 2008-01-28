#ifndef __colourthread__
#define __colourthread__

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
#endif // __colourthread__
