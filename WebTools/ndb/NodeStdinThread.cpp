#include "NodeStdinThread.h"
#include <wx/crt.h>

wxDEFINE_EVENT(wxEVT_STDIN_MESSAGE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_STDIN_EXIT, clCommandEvent);
NodeStdinThread::NodeStdinThread(wxEvtHandler* owner)
    : m_owner(owner)
{
}

NodeStdinThread::~NodeStdinThread() {}

void* NodeStdinThread::Entry()
{
    while(!TestDestroy()) {
        char command[1024];
        memset(command, 0, sizeof(command));
        if(fgets(command, sizeof(command), stdin)) {
            wxString buffer;
            buffer << command;
            if(buffer.Contains(".exit")) { break; }
            clCommandEvent evt(wxEVT_STDIN_MESSAGE);
            evt.SetString(command);
            m_owner->AddPendingEvent(evt);
        }
    }
    clCommandEvent evt_exit(wxEVT_STDIN_EXIT);
    m_owner->AddPendingEvent(evt_exit);
    return NULL;
}

void NodeStdinThread::Write(const wxString& command) { wxFprintf(stdin, "%s\n", command); }
