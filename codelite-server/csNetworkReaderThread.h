#ifndef CSNETWORKREADERTHREAD_H
#define CSNETWORKREADERTHREAD_H

#include "csJoinableThread.h"
#include <cl_command_event.h>
#include <wx/event.h>

wxDECLARE_EVENT(wxEVT_SOCKET_READ_ERROR, clCommandEvent);

class clSocketBase;
class csNetworkReaderThread : public csJoinableThread
{
    clSocketBase* m_conn;

public:
    csNetworkReaderThread(wxEvtHandler* manager, clSocketBase* conn);
    virtual ~csNetworkReaderThread();
    clSocketBase* GetConnection() { return m_conn; }

protected:
    void* Entry();
    bool ProcessCommand(const wxString& message);
};

#endif // CSNETWORKREADERTHREAD_H
