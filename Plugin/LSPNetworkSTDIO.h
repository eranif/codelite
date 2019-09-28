#ifndef LSPNETWORKSOCKET_H
#define LSPNETWORKSOCKET_H

#include "LSPNetwork.h"
#include "cl_command_event.h"
#include "SocketAPI/clSocketClientAsync.h"
#include <wx/process.h>

///===------------------------------
/// LSPNetwork based on sockets
///===------------------------------

class ChildProcess;
class LSPNetworkSTDIO : public LSPNetwork
{
protected:
    clAsyncSocket::Ptr_t m_socket;
    ChildProcess* m_server = nullptr;

protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    
public:
    virtual void Close();
    /**
     * @brief open the LSP process. Throws clException on error
     */
    virtual void Open(const LSPStartupInfo& info);
    virtual void Send(const std::string& data);
    virtual bool IsConnected() const;

    LSPNetworkSTDIO();
    virtual ~LSPNetworkSTDIO();
};

#endif // LSPNETWORKSOCKET_H
