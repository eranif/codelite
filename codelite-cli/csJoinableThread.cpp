#include "csJoinableThread.h"

wxDEFINE_EVENT(wxEVT_THREAD_GOING_DOWN, clCommandEvent);

csJoinableThread::csJoinableThread(wxEvtHandler* manager)
    : wxThread(wxTHREAD_JOINABLE)
    , m_manager(manager)
{
}

csJoinableThread::~csJoinableThread() { Stop(); }

void csJoinableThread::Stop()
{
    // Notify the thread to exit and
    // wait for it
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);

    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void csJoinableThread::Start(int priority)
{
    Create();
    SetPriority(priority);
    Run();
}

void csJoinableThread::NotifyGoingDown()
{
    clCommandEvent event(wxEVT_THREAD_GOING_DOWN);
    event.SetClientData(this);
    m_manager->AddPendingEvent(event);
}
