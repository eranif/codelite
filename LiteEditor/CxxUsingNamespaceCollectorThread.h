#ifndef CXXUSINGNAMESPACECOLLECTORTHREAD_H
#define CXXUSINGNAMESPACECOLLECTORTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class CxxUsingNamespaceCollectorThread : public WorkerThread
{
public:
    struct Request : public ThreadRequest {
        wxString filename;
        wxArrayString includePaths;
        Request() {}
    };

public:
    CxxUsingNamespaceCollectorThread();
    virtual ~CxxUsingNamespaceCollectorThread();

    virtual void ProcessRequest(ThreadRequest* request);
public:
    void QueueFile(const wxString &filename, const wxArrayString &searchPaths);
};

#endif // CXXUSINGNAMESPACECOLLECTORTHREAD_H
