#ifndef LSPNETWORKSOCKETCLIENT_H
#define LSPNETWORKSOCKETCLIENT_H

#include "AsyncProcess/asyncprocess.h"
#include "LSPNetwork.h" // Base class: LSPNetwork
#include "SocketAPI/clSocketClientAsync.h"
#include "codelite_exports.h"

#include <wx/process.h>

class WXDLLIMPEXP_SDK LSPNetworkSocketClient : public LSPNetwork
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
    LSPNetworkSocketClient() = default;
    ~LSPNetworkSocketClient() override;

public:
    void Close() override;
    bool IsConnected() const override;
    void Open(const LSPStartupInfo& info) override;
    virtual void Send(const std::string& data);
};

#endif // LSPNETWORKSOCKETCLIENT_H
