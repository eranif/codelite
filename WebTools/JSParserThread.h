#ifndef JSPARSERTHREAD_H
#define JSPARSERTHREAD_H

#include "worker_thread.h"
#include <wx/string.h>
#include "JSLookUpTable.h"

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
        JSLookUpTable* lookup;
        Reply()
            : lookup(NULL)
        {
        }
        ~Reply()
        {
        }
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
