#ifndef PHPPARSERTHREAD_H
#define PHPPARSERTHREAD_H

#include <worker_thread.h>


class PHPParserThreadRequest : public ThreadRequest
{
public:
    bool bIsFullRetag;
    wxArrayString files;
    wxString workspaceFile;

public:
    PHPParserThreadRequest()
        : bIsFullRetag(false)
    {
    }
    virtual ~PHPParserThreadRequest() {}
};

class PHPParserThread : public WorkerThread
{
    static PHPParserThread* ms_instance;

public:
    static PHPParserThread* Instance();
    static void Release();

private:
    PHPParserThread();
    virtual ~PHPParserThread();
    void ParseFiles(PHPParserThreadRequest* request);

public:
    virtual void ProcessRequest(ThreadRequest* request);
};

#endif // PHPPARSERTHREAD_H
