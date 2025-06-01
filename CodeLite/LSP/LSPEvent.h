#ifndef LSPEVENT_H
#define LSPEVENT_H

#include "LSP/CompletionItem.h"
#include "LSP/basic_types.h"
#include "LSP/json_rpc_params.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <vector>

#define LSP_LOG_ERROR 1
#define LSP_LOG_WARNING 2
#define LSP_LOG_INFO 3
#define LSP_LOG_LOG 4

class WXDLLIMPEXP_CL LSPEvent : public clCommandEvent
{
public:
    // https://microsoft.github.io/language-server-protocol/specifications/specification-3-17/#messageType

protected:
    LSP::Location m_location;
    wxString m_serverName;
    LSP::CompletionItem::Vec_t m_completions;
    LSP::SignatureHelp m_signatureHelp;
    LSP::Hover m_hover;
    std::vector<LSP::Diagnostic> m_diagnostics;
    std::vector<LSP::SymbolInformation> m_symbolsInformation;
    std::vector<LSP::SemanticTokenRange> m_semanticTokens;
    std::vector<LSP::Location> m_locations;                             // used by wxEVT_LSP_REFERENCES
    std::vector<LSP::Command> m_commands;                               // used by wxEVT_LSP_CODE_ACTIONS
    std::unordered_map<wxString, std::vector<LSP::TextEdit>> m_changes; // list of changes per file
    int m_logMessageSeverity = LSP_LOG_INFO;
    LSP::CompletionItem::eTriggerKind m_triggerKind =
        LSP::CompletionItem::kTriggerKindInvoked; // CC response is due to 24x7 cc

public:
    LSPEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    LSPEvent(const LSPEvent&) = default;
    LSPEvent& operator=(const LSPEvent& ) = delete;
    ~LSPEvent() override = default;

    void SetTriggerKind(LSP::CompletionItem::eTriggerKind triggerKind) { this->m_triggerKind = triggerKind; }
    LSP::CompletionItem::eTriggerKind GetTriggerKind() const { return m_triggerKind; }

    void SetLogMessageSeverity(int sev) { m_logMessageSeverity = sev; }
    int GetLogMessageSeverity() const { return m_logMessageSeverity; }

    void SetMessage(const wxString& message) { SetString(message); }
    wxString GetMessage() const { return GetString(); }

    void SetSemanticTokens(const std::vector<LSP::SemanticTokenRange>& semanticTokens)
    {
        this->m_semanticTokens = semanticTokens;
    }
    const std::vector<LSP::SemanticTokenRange>& GetSemanticTokens() const { return m_semanticTokens; }

    LSPEvent& SetLocation(const LSP::Location& location)
    {
        this->m_location = location;
        return *this;
    }
    const LSP::Location& GetLocation() const { return m_location; }
    LSP::Location& GetLocation() { return m_location; }
    wxEvent* Clone() const override { return new LSPEvent(*this); }
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
    LSPEvent& SetHover(const LSP::Hover& hover)
    {
        this->m_hover = hover;
        return *this;
    }
    const LSP::Hover& GetHover() const { return m_hover; }
    LSPEvent& SetDiagnostics(const std::vector<LSP::Diagnostic>& diagnostics)
    {
        this->m_diagnostics = diagnostics;
        return *this;
    }
    const std::vector<LSP::Diagnostic>& GetDiagnostics() const { return m_diagnostics; }
    void SetSymbolsInformation(const std::vector<LSP::SymbolInformation>& symbolsInformation)
    {
        this->m_symbolsInformation = symbolsInformation;
    }
    const std::vector<LSP::SymbolInformation>& GetSymbolsInformation() const { return m_symbolsInformation; }
    std::vector<LSP::SymbolInformation>& GetSymbolsInformation() { return m_symbolsInformation; }
    void SetLocations(const std::vector<LSP::Location>& locations) { this->m_locations = locations; }
    const std::vector<LSP::Location>& GetLocations() const { return m_locations; }
    std::vector<LSP::Location>& GetLocations() { return m_locations; }
    void SetCommands(const std::vector<LSP::Command>& commands) { this->m_commands = commands; }
    const std::vector<LSP::Command>& GetCommands() const { return m_commands; }
    std::vector<LSP::Command>& GetCommands() { return m_commands; }
    void SetChanges(const std::unordered_map<wxString, std::vector<LSP::TextEdit>>& changes)
    {
        this->m_changes = changes;
    }
    const std::unordered_map<wxString, std::vector<LSP::TextEdit>>& GetChanges() const { return m_changes; }
};

using LSPEventFunction = void (wxEvtHandler::*)(LSPEvent&);
#define LSPEventHandler(func) wxEVENT_HANDLER_CAST(LSPEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_DEFINITION, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_INITIALIZED, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_COMPLETION_READY, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE,
                         LSPEvent); // fired twice: m_owner && EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_RESTART_NEEDED, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_REPARSE_NEEDED, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_METHOD_NOT_FOUND, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_SIGNATURE_HELP, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_HOVER, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_SET_DIAGNOSTICS, LSPEvent);   // EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_CLEAR_DIAGNOSTICS, LSPEvent); // EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_OPEN_FILE, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_SEMANTICS, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_LOGMESSAGE, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_WORKSPACE_SYMBOLS, LSPEvent);        // EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_SYMBOL_DECLARATION_FOUND, LSPEvent); // EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_SHOW_QUICK_OUTLINE_DLG, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_REFERENCES, LSPEvent);            // EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_REFERENCES_INPROGRESS, LSPEvent); // EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_CODE_ACTIONS, LSPEvent);          // EventNotifier
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_EDIT_FILES, LSPEvent);

#endif // LSPEVENT_H
