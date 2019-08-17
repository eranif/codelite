#ifndef CSJOINABLETHREAD_H
#define CSJOINABLETHREAD_H

#include "csJoinableThread.h"
#include <cl_command_event.h>
#include <wx/event.h>

wxDECLARE_EVENT(wxEVT_THREAD_GOING_DOWN, clCommandEvent);
class csJoinableThread : public wxThread
{
protected:
    wxEvtHandler* m_manager;

protected:
    void Stop();
    void NotifyGoingDown();

public:
    csJoinableThread(wxEvtHandler* manager);
    virtual ~csJoinableThread();

    /**
     * Start the thread as joinable thread.
     * This call must be called from the context of other thread (e.g. main thread)
     */
    void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);
};

#endif // CSJOINABLETHREAD_H
