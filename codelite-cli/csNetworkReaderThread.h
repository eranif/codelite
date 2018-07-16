#ifndef CSNETWORKREADERTHREAD_H
#define CSNETWORKREADERTHREAD_H

#include "csCommandHandlerManager.h"
#include "csJoinableThread.h"
#include <cl_command_event.h>
#include <wx/event.h>

wxDECLARE_EVENT(wxEVT_SOCKET_READ_ERROR, clCommandEvent);

class clSocketBase;
class csNetworkReaderThread : public csJoinableThread
{
    clSocketBase* m_conn;
    csCommandHandlerManager m_handlers;

public:
    csNetworkReaderThread(wxEvtHandler* manager, clSocketBase* conn);
    virtual ~csNetworkReaderThread();
    clSocketBase* GetConnection() { return m_conn; }

protected:
    void* Entry();
    void ProcessCommand(const wxString& str);
};

#endif // CSNETWORKREADERTHREAD_H
