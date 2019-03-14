#ifndef LSPNETWORKPROCESS_H
#define LSPNETWORKPROCESS_H

#include <wx/event.h>
#include "LSPNetwork.h"
class LSPNetworkProcess : public LSPNetwork
{
    wxProcess* m_process = nullptr;

protected:
    void OnProcessTerminated(wxProcessEvent& event);

public:
    virtual void Close();
    virtual bool IsConnected() const;
    virtual void Open(const LSPNetwork::StartupInfo& info);
    virtual void Send(const std::string& data);

    LSPNetworkProcess();
    ~LSPNetworkProcess();
};

#endif // LSPNETWORKPROCESS_H
