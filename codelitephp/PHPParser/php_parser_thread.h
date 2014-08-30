#ifndef PHPPARSERTHREAD_H
#define PHPPARSERTHREAD_H

#include <worker_thread.h>
#include "php_storage.h"

class PHPParserThreadRequest : public ThreadRequest
{
public:
    bool          bIsFullRetag;
    wxArrayString files;
    wxString      workspaceFile;

public:
    PHPParserThreadRequest() : bIsFullRetag(false) {}
    virtual ~PHPParserThreadRequest() {}

};

// A progress class used by the parser thread to report its parsing
// progress
class PHPParserThreadProgressCB : public wxEvtHandler
{
public:
    PHPParserThreadProgressCB() {}
    virtual ~PHPParserThreadProgressCB() {}

    virtual void OnProgress(size_t currentIndex, size_t total) {
        wxUnusedVar(currentIndex);
        wxUnusedVar(total);
    }
    
    virtual void OnCompleted() {}
};

class PHPParserThread : public WorkerThread
{
    static PHPParserThread* ms_instance;
    PHPParserThreadProgressCB* m_progress;

public:
    static PHPParserThread* Instance();
    static void Release();

private:
    PHPParserThread();
    virtual ~PHPParserThread();
    void ParseFiles( PHPParserThreadRequest* request, PHPStorage& myStorage );

public:
    virtual void ProcessRequest(ThreadRequest* request);
    /**
     * @brief set progress bar. The progress is owned by this class
     */
    void SetProgress(PHPParserThreadProgressCB* progress) {
        this->m_progress = progress;
    }
};

#endif // PHPPARSERTHREAD_H
