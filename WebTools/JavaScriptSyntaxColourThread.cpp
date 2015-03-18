#include "JavaScriptSyntaxColourThread.h"
#include "macros.h"
#include "JavaScriptFunctionsLocator.h"
#include "CxxPreProcessor.h"

JavaScriptSyntaxColourThread::JavaScriptSyntaxColourThread() {}

JavaScriptSyntaxColourThread::~JavaScriptSyntaxColourThread() {}

void JavaScriptSyntaxColourThread::ProcessRequest(ThreadRequest* request)
{
    JavaScriptSyntaxColourThread::Request* req = dynamic_cast<JavaScriptSyntaxColourThread::Request*>(request);
    CHECK_PTR_RET(req);
    
    CxxPreProcessor pp;
    pp.SetMaxDepth(20);
    JavaScriptFunctionsLocator collector(&pp, req->filename);
    collector.Parse();
}

void JavaScriptSyntaxColourThread::QueueFile(const wxString& filename)
{
    JavaScriptSyntaxColourThread::Request* req = new JavaScriptSyntaxColourThread::Request();
    req->filename = filename;
    Add(req);
}
