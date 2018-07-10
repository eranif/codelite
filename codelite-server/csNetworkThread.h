#ifndef CSNETWORKTHREAD_H
#define CSNETWORKTHREAD_H

#include "worker_thread.h"
#include "csNetworkRequest.h"

class csNetworkThread : public WorkerThread
{
public:
    csNetworkThread();
    virtual ~csNetworkThread();
    
    void ProcessRequest(csNetworkRequest* request);
};

#endif // CSNETWORKTHREAD_H
