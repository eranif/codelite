#ifndef LSPNETWORKSOCKET_H
#define LSPNETWORKSOCKET_H

#include "AsyncProcess/asyncprocess.h"
#include "LSPNetwork.h"
#include "SocketAPI/clSocketClientAsync.h"
#include "cl_command_event.h"

#include <wx/ffile.h>

///===------------------------------
/// LSPNetwork based on STDIO
///===------------------------------
class ChildProcess;
class LSPNetworkSTDIO : public LSPNetwork
{
protected:
    ChildProcess* m_server = nullptr;

protected:
    void DoStartLocalProcess();
    void BindEvents();

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
