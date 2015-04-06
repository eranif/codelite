#include "JavaScriptSyntaxColourThread.h"
#include "macros.h"
#include "JavaScriptFunctionsLocator.h"
#include "CxxPreProcessor.h"
#include "webtools.h"

JavaScriptSyntaxColourThread::JavaScriptSyntaxColourThread(WebTools* plugin)
    : m_plugin(plugin)
{
}

JavaScriptSyntaxColourThread::~JavaScriptSyntaxColourThread() {}

void JavaScriptSyntaxColourThread::ProcessRequest(ThreadRequest* request)
{
    JavaScriptSyntaxColourThread::Request* req = dynamic_cast<JavaScriptSyntaxColourThread::Request*>(request);
    CHECK_PTR_RET(req);

    JavaScriptFunctionsLocator collector(req->filename, req->content);
    collector.Parse();
    
    JavaScriptSyntaxColourThread::Reply reply;
    reply.filename = req->filename;
    reply.functions = collector.GetFunctionsString();
    reply.properties = collector.GetPropertiesString();
    
    m_plugin->CallAfter(&WebTools::ColourJavaScript, reply);
}

void JavaScriptSyntaxColourThread::QueueFile(const wxString& filename)
{
    JavaScriptSyntaxColourThread::Request* req = new JavaScriptSyntaxColourThread::Request();
    req->filename = filename;
    Add(req);
}

void JavaScriptSyntaxColourThread::QueueBuffer(const wxString& filename, const wxString& content)
{
    JavaScriptSyntaxColourThread::Request* req = new JavaScriptSyntaxColourThread::Request();
    req->filename = filename;
    req->content = content;
    Add(req);
}
