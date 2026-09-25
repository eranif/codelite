#ifndef LSPNETWORKSOCKET_H
#define LSPNETWORKSOCKET_H

#include "AsyncProcess/asyncprocess.h"
#include "LSPNetwork.h"
#include "SocketAPI/clSocketClientAsync.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/ffile.h>

///===------------------------------
/// LSPNetwork based on STDIO
///===------------------------------
class ChildProcess;
class WXDLLIMPEXP_SDK LSPNetworkSTDIO : public LSPNetwork
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
    void Close() override;
    /**
     * @brief open the LSP process. Throws clException on error
     */
    void Open(const LSPStartupInfo& info) override;
    virtual void Send(const std::string& data);
    bool IsConnected() const override;

    LSPNetworkSTDIO();
    ~LSPNetworkSTDIO() override;
};

#endif // LSPNETWORKSOCKET_H
