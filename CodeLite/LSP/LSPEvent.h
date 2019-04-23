#ifndef LSPEVENT_H
#define LSPEVENT_H

#include "cl_command_event.h"
#include "LSP/basic_types.h"
#include "codelite_exports.h"
#include "LSP/CompletionItem.h"
#include <vector>

class WXDLLIMPEXP_CL LSPEvent : public clCommandEvent
{
    LSP::Location m_location;
    wxString m_serverName;
    LSP::CompletionItem::Vec_t m_completions;
    LSP::SignatureHelp m_signatureHelp;
    std::vector<LSP::Diagnostic> m_diagnostics;

public:
    LSPEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    LSPEvent(const LSPEvent& src);
    LSPEvent& operator=(const LSPEvent& other);
    virtual ~LSPEvent();
    LSPEvent& SetLocation(const LSP::Location& location)
    {
        this->m_location = location;
        return *this;
    }
    const LSP::Location& GetLocation() const { return m_location; }
    LSP::Location& GetLocation() { return m_location; }
    wxEvent* Clone() const { return new LSPEvent(*this); }
    LSPEvent& SetServerName(const wxString& serverName)
    {
        this->m_serverName = serverName;
        return *this;
    }
    const wxString& GetServerName() const { return m_serverName; }
    LSPEvent& SetCompletions(const LSP::CompletionItem::Vec_t& completions)
    {
        this->m_completions = completions;
        return *this;
    }
    const LSP::CompletionItem::Vec_t& GetCompletions() const { return m_completions; }
    LSPEvent& SetSignatureHelp(const LSP::SignatureHelp& signatureHelp)
    {
        this->m_signatureHelp = signatureHelp;
        return *this;
    }
    const LSP::SignatureHelp& GetSignatureHelp() const { return m_signatureHelp; }
    LSPEvent& SetDiagnostics(const std::vector<LSP::Diagnostic>& diagnostics)
    {
        this->m_diagnostics = diagnostics;
        return *this;
    }
    const std::vector<LSP::Diagnostic>& GetDiagnostics() const { return m_diagnostics; }
};

typedef void (wxEvtHandler::*LSPEventFunction)(LSPEvent&);
#define LSPEventHandler(func) wxEVENT_HANDLER_CAST(LSPEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_DEFINITION, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_INITIALIZED, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_COMPLETION_READY, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_RESTART_NEEDED, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_REPARSE_NEEDED, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_METHOD_NOT_FOUND, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_SIGNATURE_HELP, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_SET_DIAGNOSTICS, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_CLEAR_DIAGNOSTICS, LSPEvent);

#endif // LSPEVENT_H
