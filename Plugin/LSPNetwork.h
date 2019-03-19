#ifndef LSPNETWORK_H
#define LSPNETWORK_H

#include <wx/event.h>
#include "codelite_exports.h"
#include "cl_command_event.h"
#include <wx/string.h>
#include <wx/arrstr.h>
#include <macros.h>
#include "LSPStartupInfo.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_NET_DATA_READY, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_NET_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_NET_CONNECTED, clCommandEvent);

enum class eNetworkType {
    kStdio,
    kTcpIP,
};

class WXDLLIMPEXP_SDK LSPNetwork : public wxEvtHandler
{
protected:
    LSPStartupInfo m_startupInfo;

public:
    typedef wxSharedPtr<LSPNetwork> Ptr_t;

    LSPNetwork& SetStartupInfo(const LSPStartupInfo& startupInfo)
    {
        this->m_startupInfo = startupInfo;
        return *this;
    }
    const LSPStartupInfo& GetStartupInfo() const { return m_startupInfo; }

public:
    LSPNetwork();
    virtual ~LSPNetwork();

    //===-----------------------------------
    // Interface
    //===-----------------------------------

    /**
     * @brief send data to the LSP server
     */
    virtual void Send(const std::string& data) = 0;

    /**
     * @brief close the connection to the LSP server
     */
    virtual void Close() = 0;

    /**
     * @brief open connection to the LSP server
     */
    virtual void Open(const LSPStartupInfo& info) = 0;

    /**
     * @brief are we connected to the LSP server?
     */
    virtual bool IsConnected() const = 0;
};

#endif // LSPNETWORK_H
