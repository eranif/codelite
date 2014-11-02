#include "WordCompletionThread.h"
#include "wordcompletion.h"

WordCompletionThread::WordCompletionThread(WordCompletionPlugin* plugin)
    : m_plugin(plugin)
{
}

WordCompletionThread::~WordCompletionThread() {}
void WordCompletionThread::ProcessRequest(ThreadRequest* request)
{
    WordCompletionThread::Request* req = dynamic_cast<WordCompletionThread::Request*>(request);
    CHECK_PTR_RET(req);
    
    wxString buffer = req->buffer;
    
}
