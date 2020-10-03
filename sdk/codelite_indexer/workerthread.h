#ifndef __workerthread__
#define __workerthread__

#include "equeue.h"
#include "ethread.h"
#include "network/named_pipe.h"

// ---------------------------------------------
// parsing thread
// ---------------------------------------------

class WorkerThread : public eThread
{
    eQueue<clNamedPipe*>* m_queue;

public:
    WorkerThread(eQueue<clNamedPipe*>* queue);
    ~WorkerThread();

public:
    virtual void start();
};

// ---------------------------------------------
// is alive thread
// ---------------------------------------------
class IsAliveThread : public eThread
{
    int m_pid;
    std::string m_socket;

public:
    IsAliveThread(int pid, const std::string& socketName)
        : m_pid(pid)
        , m_socket(socketName)
    {
    }
    ~IsAliveThread() {}

public:
    virtual void start();
};
#endif // __workerthread__
