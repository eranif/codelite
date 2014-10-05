#ifndef PHPPARSERTHREAD_H
#define PHPPARSERTHREAD_H

#include <worker_thread.h>


class PHPParserThreadRequest : public ThreadRequest
{
public:
    enum ePHPParserThreadRequestType {
        kParseWorkspaceFilesFull,
        kParseWorkspaceFilesQuick,
        kParseSingleFile,
    };
    
    ePHPParserThreadRequestType requestType;
    wxArrayString files;
    wxString workspaceFile;
    wxString file;
    wxArrayString frameworksPaths;
    
public:
    PHPParserThreadRequest(ePHPParserThreadRequestType type)
        : requestType(type)
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
    void ParseFile(PHPParserThreadRequest* request);

public:
    virtual void ProcessRequest(ThreadRequest* request);
};

#endif // PHPPARSERTHREAD_H
