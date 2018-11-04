#ifndef NODESTDINTHREAD_H
#define NODESTDINTHREAD_H

#include <clJoinableThread.h>
#include <cl_command_event.h>
#include <codelite_exports.h>

class NodeStdinThread : public clJoinableThread
{
    wxEvtHandler* m_owner = nullptr;

public:
    NodeStdinThread(wxEvtHandler* owner);
    virtual ~NodeStdinThread();

    void* Entry();
    void Write(const wxString& command);
};
wxDECLARE_EVENT(wxEVT_STDIN_MESSAGE, clCommandEvent);
wxDECLARE_EVENT(wxEVT_STDIN_EXIT, clCommandEvent);
#endif // NODESTDINTHREAD_H
