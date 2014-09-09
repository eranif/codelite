#ifndef CXXPREPROCESSORTHREAD_H
#define CXXPREPROCESSORTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class CxxPreProcessorThread : public WorkerThread
{
public:
    struct Request : public ThreadRequest
    {
        wxString filename;
        wxArrayString definitions;
        wxArrayString includePaths;

        Request()
        {
        }
    };

public:
    CxxPreProcessorThread();
    virtual ~CxxPreProcessorThread();

public:
    virtual void ProcessRequest(ThreadRequest* request);

    void QueueFile(const wxString& filename, const wxArrayString& definitions, const wxArrayString& includePaths);
};

#endif // CXXPREPROCESSORTHREAD_H
