#include "LSPEvent.h"

wxDEFINE_EVENT(wxEVT_LSP_DEFINITION, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_INITIALIZED, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_COMPLETION_READY, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_RESTART_NEEDED, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_REPARSE_NEEDED, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_METHOD_NOT_FOUND, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_SIGNATURE_HELP, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_HOVER, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_SET_DIAGNOSTICS, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_CLEAR_DIAGNOSTICS, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_OPEN_FILE, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_SEMANTICS, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_LOGMESSAGE, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_WORKSPACE_SYMBOLS, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_SYMBOL_DECLARATION_FOUND, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_SHOW_QUICK_OUTLINE_DLG, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_REFERENCES, LSPEvent);
wxDEFINE_EVENT(wxEVT_LSP_REFERENCES_INPROGRESS, LSPEvent);

LSPEvent::LSPEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

LSPEvent::LSPEvent(const LSPEvent& src)
    : clCommandEvent(src)
{
    *this = src;
}

LSPEvent& LSPEvent::operator=(const LSPEvent& other)
{
    clCommandEvent::operator=(other);
    m_location = other.m_location;
    m_serverName = other.m_serverName;
    m_completions = other.m_completions;
    m_signatureHelp = other.m_signatureHelp;
    m_hover = other.m_hover;
    m_diagnostics = other.m_diagnostics;
    m_symbolsInformation = other.m_symbolsInformation;
    m_semanticTokens = other.m_semanticTokens;
    m_logMessageSeverity = other.m_logMessageSeverity;
    m_locations = other.m_locations;
    return *this;
}

LSPEvent::~LSPEvent() {}
