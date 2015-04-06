#ifndef JAVASCRIPTSYNTAXCOLOURTHREAD_H
#define JAVASCRIPTSYNTAXCOLOURTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class WebTools;
class JavaScriptSyntaxColourThread : public WorkerThread
{
private:
    WebTools* m_plugin;

public:
    struct Request : public ThreadRequest {
        wxString filename;
        wxString content;
        Request() {}
    };

    struct Reply {
        wxString filename;
        wxString functions;
        wxString properties;
    };

public:
    JavaScriptSyntaxColourThread(WebTools* plugin);
    virtual ~JavaScriptSyntaxColourThread();

public:
    virtual void ProcessRequest(ThreadRequest* request);
    void QueueFile(const wxString& filename);
    void QueueBuffer(const wxString& filename, const wxString& content);
};

#endif // JAVASCRIPTSYNTAXCOLOURTHREAD_H
