#ifndef __colourthread__
#define __colourthread__

#include "worker_thread.h"

class colourthread : public WorkerThread {

	static colourthread* ms_instance;

public:
	static colourthread* Instance();
	static void Release();

private:
	colourthread();
	~colourthread();

public:
	virtual void ProcessRequest(ThreadRequest *request);
};
#endif // __colourthread__
