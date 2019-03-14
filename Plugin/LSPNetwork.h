#ifndef LSPNETWORK_H
#define LSPNETWORK_H

#include <wx/event.h>
#include "codelite_exports.h"
#include "cl_command_event.h"
#include <wx/string.h>
#include <wx/arrstr.h>
#include <macros.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_NET_DATA_READY, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_NET_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_NET_CONNECTED, clCommandEvent);

class WXDLLIMPEXP_SDK LSPNetwork : public wxEvtHandler
{
public:
    struct StartupInfo {
    protected:
        wxString m_helperCommand;
        wxString m_lspServerCommand;

    public:
        void SetHelperCommand(const wxString& helperCommand) { this->m_helperCommand = helperCommand; }
        const wxString& GetHelperCommand() const { return m_helperCommand; }

        void SetLspServerCommand(const wxString& lspServerCommand) { this->m_lspServerCommand = lspServerCommand; }
        const wxString& GetLspServerCommand() const { return m_lspServerCommand; }

    public:
        StartupInfo() {}

        StartupInfo(const wxString& helperCommand, const wxString& lspServerCommand)
            : m_helperCommand(helperCommand)
            , m_lspServerCommand(lspServerCommand)
        {
        }
    };

protected:
    LSPNetwork::StartupInfo m_startupInfo;

public:
    typedef wxSharedPtr<LSPNetwork> Ptr_t;

    LSPNetwork& SetStartupInfo(const LSPNetwork::StartupInfo& startupInfo)
    {
        this->m_startupInfo = startupInfo;
        return *this;
    }
    const LSPNetwork::StartupInfo& GetStartupInfo() const { return m_startupInfo; }

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
    virtual void Open(const LSPNetwork::StartupInfo& info) = 0;

    /**
     * @brief are we connected to the LSP server?
     */
    virtual bool IsConnected() const = 0;
};

#endif // LSPNETWORK_H
