#include "LanguageServerCluster.h"
#include "LanguageServerConfig.h"
#include <algorithm>
#include "codelite_events.h"
#include "event_notifier.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "globals.h"
#include "imanager.h"
#include "file_logger.h"

LanguageServerCluster::LanguageServerCluster()
{
    EventNotifier::Get()->Bind(wxEVT_CC_FIND_SYMBOL, &LanguageServerCluster::OnFindSymbold, this);
    Bind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
}

LanguageServerCluster::~LanguageServerCluster()
{
    EventNotifier::Get()->Unbind(wxEVT_CC_FIND_SYMBOL, &LanguageServerCluster::OnFindSymbold, this);
    Unbind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
}

void LanguageServerCluster::Reload()
{
    for(const std::unordered_map<wxString, LanguageServerProtocol::Ptr_t>::value_type& vt : m_servers) {
        // stop all current processes
        LanguageServerProtocol::Ptr_t server = vt.second;
        if(server->IsRunning()) { server->Stop(true); }
    }
    m_servers.clear();

    // If we are not enabled, stop here
    if(!LanguageServerConfig::Get().IsEnabled()) { return; }

    // create a new list
    const LanguageServerEntry::Map_t& servers = LanguageServerConfig::Get().GetServers();
    for(const LanguageServerEntry::Map_t::value_type& vt : servers) {
        const LanguageServerEntry& entry = vt.second;
        if(entry.IsEnabled()) {
            LanguageServerProtocol::Ptr_t lsp(new LanguageServerProtocol(entry.GetName(), this));
            wxString command = entry.GetExepath();
            ::WrapWithQuotes(command);
            if(!entry.GetArgs().IsEmpty()) { command << " " << entry.GetArgs(); }
            lsp->Start(command, entry.GetWorkingDirectory(), entry.GetLanguages());
            m_servers.insert({ entry.GetName(), lsp });
            lsp->Bind(wxEVT_LSP_INITIALIZED, &LanguageServerCluster::OnLSPInitialized, this);
        }
    }
}

void LanguageServerCluster::OnFindSymbold(clCodeCompletionEvent& event)
{
    event.Skip();
    if(m_servers.empty()) return;
    if(!LanguageServerConfig::Get().IsEnabled()) { return; }

    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);

    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    int col = ctrl->GetColumn(ctrl->GetCurrentPos());
    int line = ctrl->LineFromPosition(ctrl->GetCurrentPos());
    LanguageServerProtocol::Ptr_t server = GetServerForFile(editor->GetFileName());
    if(server) {
        // this event is ours to handle
        event.Skip(false);
        server->FindDefinition(editor->GetFileName(), line, col);
    }
}

LanguageServerProtocol::Ptr_t LanguageServerCluster::GetServerForFile(const wxFileName& filename)
{
    std::unordered_map<wxString, LanguageServerProtocol::Ptr_t>::iterator iter =
        std::find_if(m_servers.begin(), m_servers.end(),
                     [&](const std::unordered_map<wxString, LanguageServerProtocol::Ptr_t>::value_type& vt) {
                         return vt.second->CanHandle(filename);
                     });

    if(iter == m_servers.end()) { return LanguageServerProtocol::Ptr_t(nullptr); }
    return iter->second;
}

void LanguageServerCluster::OnSymbolFound(LSPEvent& event)
{
    const LSP::Location& location = event.GetLocation();
    wxFileName fn(location.GetUri());
    clDEBUG() << "LSP: Opening file:" << fn << "(" << location.GetRange().GetStart().GetLine() << ":"
              << location.GetRange().GetStart().GetCharacter() << ")";
    IEditor* editor = clGetManager()->OpenFile(fn.GetFullPath(), "", wxNOT_FOUND);
    if(editor) { editor->SelectRange(location.GetRange()); }
}

void LanguageServerCluster::OnLSPInitialized(LSPEvent& event)
{
    // TODO :: locate all open files and send them to be processed by the LSP
}
