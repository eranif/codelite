#ifndef JSPARSERTHREAD_H
#define JSPARSERTHREAD_H

#include "worker_thread.h"
#include <wx/string.h>

class JSCodeCompletion;
class JSParserThread : public WorkerThread
{
private:
    JSCodeCompletion* m_jsCC;

public:
    struct Request : public ThreadRequest {
        wxString path;
    };

    struct Reply {
        int num_files;
        long total_time;
        Reply()
            : num_files(0)
            , total_time(0)
        {
        }
        ~Reply() {}
    };

public:
    void ProcessRequest(ThreadRequest* request);

    JSParserThread(JSCodeCompletion* jsCC);
    virtual ~JSParserThread();

    /**
     * @brief parse all JS files at a given folder
     */
    void ParseFiles(const wxString& folder);
};

#endif // JSPARSERTHREAD_H
