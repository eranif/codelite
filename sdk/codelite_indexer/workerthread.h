#ifndef __workerthread__
#define __workerthread__

#include "ethread.h"
#include "equeue.h"
#include "network/named_pipe.h"

class WorkerThread : public eThread {
	eQueue<clNamedPipe*> *m_queue;

public:
	WorkerThread(eQueue<clNamedPipe*> *queue);
	~WorkerThread();

public:
	virtual void start();
};
#endif // __workerthread__
