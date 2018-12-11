#include "clJoinableThread.h"

clJoinableThread::clJoinableThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

clJoinableThread::~clJoinableThread() { Stop(); }

void clJoinableThread::Stop()
{
    // Notify the thread to exit and
    // wait for it
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);

    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void clJoinableThread::Start(int priority)
{
    Create();
    SetPriority(priority);
    Run();
}
