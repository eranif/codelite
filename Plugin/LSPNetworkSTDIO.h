#ifndef LSPNETWORKSOCKET_H
#define LSPNETWORKSOCKET_H

#include "LSPNetwork.h"
#include "cl_command_event.h"
#include "SocketAPI/clSocketClientAsync.h"
#include <wx/process.h>

///===------------------------------
/// LSPNetwork based on sockets
///===------------------------------

class LSPNetworkSTDIO : public LSPNetwork
{
protected:
    clAsyncSocket::Ptr_t m_socket;
    wxProcess* m_lspServer = nullptr;

protected:
    void OnSocketConnected(clCommandEvent& event);
    void OnSocketServerReady(clCommandEvent& event);
    void OnSocketConnectionLost(clCommandEvent& event);
    void OnSocketConnectionError(clCommandEvent& event);
    void OnSocketError(clCommandEvent& event);
    void OnSocketData(clCommandEvent& event);
    void OnProcessTerminated(wxProcessEvent& event);

public:
    virtual void Close();
    virtual void Open(const LSPStartupInfo& info);
    virtual void Send(const std::string& data);
    virtual bool IsConnected() const;

    LSPNetworkSTDIO();
    virtual ~LSPNetworkSTDIO();
};

#endif // LSPNETWORKSOCKET_H
