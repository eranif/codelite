#ifndef CSNETWORKTHREAD_H
#define CSNETWORKTHREAD_H

#include "SocketAPI/clSocketBase.h"
#include "cl_command_event.h"
#include "csConfig.h"
#include "csJoinableThread.h"

wxDECLARE_EVENT(wxEVT_SOCKET_CONNECTION_READY, clCommandEvent);
wxDECLARE_EVENT(wxEVT_SOCKET_SERVER_ERROR, clCommandEvent);

class csNetworkThread : public csJoinableThread
{
public:
    typedef clSocketBase* clSocketBasePtr_t;

protected:
    const csConfig& m_config;

protected:
    void* Entry();

public:
    csNetworkThread(wxEvtHandler* manager, const csConfig& config);
    virtual ~csNetworkThread();
};

#endif // CSNETWORKTHREAD_H
