#ifndef CLTERNWORKERTHREAD_H
#define CLTERNWORKERTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class clTernServer;
class clTernWorkerThread : public WorkerThread
{
    clTernServer* m_ternSerer;
    int m_port;
public:
    struct Request : public ThreadRequest {
        char* jsonRequest;
        wxString filename;
        bool isFunctionTip;
    };
    
    struct Reply {
        wxString json;
        wxString filename;
        bool isFunctionTip;
    };
    
public:
    clTernWorkerThread(clTernServer* ternServer);
    virtual ~clTernWorkerThread();

public:
    virtual void ProcessRequest(ThreadRequest* request);
};

#endif // CLTERNWORKERTHREAD_H
