#ifndef WORDCOMPLETIONTHREAD_H
#define WORDCOMPLETIONTHREAD_H

#include "worker_thread.h"
#include <wx/string.h>

class WordCompletionPlugin;
class WordCompletionThread : public WorkerThread
{
public:
    struct Request {
        wxString buffer;
    };
    
protected:
    WordCompletionPlugin* m_plugin;
public:
    WordCompletionThread(WordCompletionPlugin* plugin);
    ~WordCompletionThread();
    
    virtual void ProcessRequest(ThreadRequest* request);
};

#endif // WORDCOMPLETIONTHREAD_H
