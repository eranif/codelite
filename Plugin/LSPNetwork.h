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
        wxString m_rootUri;
        wxString m_command;
        wxArrayString m_languages;

    public:
        void SetCommand(const wxString& command) { this->m_command = command; }
        void SetLanguages(const wxArrayString& languages) { this->m_languages = languages; }
        void SetRootUri(const wxString& rootUri) { this->m_rootUri = rootUri; }
        const wxString& GetCommand() const { return m_command; }
        const wxArrayString& GetLanguages() const { return m_languages; }
        const wxString& GetRootUri() const { return m_rootUri; }

    public:
        StartupInfo() {}
        
        StartupInfo(const wxString& rootUri, const wxString& command, const wxStringSet_t& langsSet)
            : m_rootUri(rootUri)
            , m_command(command)
        {
            for(const wxString& lang : langsSet) {
                m_languages.Add(lang);
            }
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
