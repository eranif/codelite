#ifndef LSPNETWORKSOCKET_H
#define LSPNETWORKSOCKET_H

#include "LSPNetwork.h"
#include "cl_command_event.h"
#include "SocketAPI/clSocketClientAsync.h"

///===------------------------------
/// LSPNetwork based on sockets
///===------------------------------

class LSPNetworkSocket : public LSPNetwork
{
protected:
    clSocketClientAsync::Ptr_t m_socket;

protected:
    void OnSocketConnected(clCommandEvent& event);
    void OnSocketConnectionLost(clCommandEvent& event);
    void OnSocketConnectionError(clCommandEvent& event);
    void OnSocketError(clCommandEvent& event);
    void OnSocketData(clCommandEvent& event);

public:
    virtual void Close();
    virtual void Open(const LSPNetwork::StartupInfo& info);
    virtual void Send(const std::string& data);
    virtual bool IsConnected() const;
    
    LSPNetworkSocket();
    virtual ~LSPNetworkSocket();
};

#endif // LSPNETWORKSOCKET_H
