#ifndef LSPNETWORKSOCKETCLIENT_H
#define LSPNETWORKSOCKETCLIENT_H

#include "LSPNetwork.h" // Base class: LSPNetwork
#include "SocketAPI/clSocketClientAsync.h"
#include "asyncprocess.h"
#include "wx/process.h"

class LSPNetworkSocketClient : public LSPNetwork
{
protected:
    clAsyncSocket::Ptr_t m_socket;
    IProcess* m_lspServer = nullptr;
    int m_pid = wxNOT_FOUND;

protected:
    void OnSocketConnected(clCommandEvent& event);
    void OnSocketConnectionLost(clCommandEvent& event);
    void OnSocketConnectionError(clCommandEvent& event);
    void OnSocketError(clCommandEvent& event);
    void OnSocketData(clCommandEvent& event);

public:
    LSPNetworkSocketClient();
    virtual ~LSPNetworkSocketClient();

public:
    virtual void Close();
    virtual bool IsConnected() const;
    virtual void Open(const LSPStartupInfo& info);
    virtual void Send(const std::string& data);
};

#endif // LSPNETWORKSOCKETCLIENT_H
