#ifndef JAVASCRIPTSYNTAXCOLOURTHREAD_H
#define JAVASCRIPTSYNTAXCOLOURTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class JavaScriptSyntaxColourThread : public WorkerThread
{
public:
    struct Request : public ThreadRequest
    {
        wxString filename;
        Request() {}
    };

public:
    JavaScriptSyntaxColourThread();
    virtual ~JavaScriptSyntaxColourThread();

public:
    virtual void ProcessRequest(ThreadRequest* request);
    void QueueFile(const wxString& filename);
};

#endif // JAVASCRIPTSYNTAXCOLOURTHREAD_H
