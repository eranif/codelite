#include "JSParserThread.h"
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/arrstr.h>
#include "fileutils.h"
#include "JSCodeCompletion.h"
#include <wx/stopwatch.h>

JSParserThread::JSParserThread(JSCodeCompletion* jsCC)
    : m_jsCC(jsCC)
{
}

JSParserThread::~JSParserThread() {}

void JSParserThread::ParseFiles(const wxString& folder)
{
    if(!wxFileName::DirExists(folder)) return;
    Request* r = new Request();
    r->path = folder;
    Add(r);
}

void JSParserThread::ProcessRequest(ThreadRequest* request)
{
    Request* r = dynamic_cast<Request*>(request);
    if(!r) return;
    
    wxStopWatch sw;
    
    sw.Start();
    wxArrayString files;
    wxDir::GetAllFiles(r->path, &files, "*.js");
}
