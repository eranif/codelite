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
#include "wxCodeCompletionBoxManager.h"

LanguageServerCluster::LanguageServerCluster()
{
    EventNotifier::Get()->Bind(wxEVT_CC_FIND_SYMBOL, &LanguageServerCluster::OnFindSymbold, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE, &LanguageServerCluster::OnCodeComplete, this);
    Bind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Bind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Bind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Bind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
}

LanguageServerCluster::~LanguageServerCluster()
{
    EventNotifier::Get()->Unbind(wxEVT_CC_FIND_SYMBOL, &LanguageServerCluster::OnFindSymbold, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE, &LanguageServerCluster::OnCodeComplete, this);
    Unbind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Unbind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Unbind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Unbind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
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
        StartServer(entry);
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
        server->FindDefinition(editor);
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

void LanguageServerCluster::OnLSPInitialized(LSPEvent& event) { wxUnusedVar(event); }

void LanguageServerCluster::OnCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);
    if(event.GetTriggerKind() == LSP::CompletionItem::kTriggerKindInvoked) { return; }

    LanguageServerProtocol::Ptr_t lsp = GetServerForFile(editor->GetFileName());
    if(!lsp) { return; }
    event.Skip(false);
    lsp->CodeComplete(editor);
}

void LanguageServerCluster::OnCompletionReady(LSPEvent& event)
{
    const LSP::CompletionItem::Vec_t& items = event.GetCompletions();

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    wxCodeCompletionBoxManager::Get().ShowCompletionBox(clGetManager()->GetActiveEditor()->GetCtrl(), items);
}

void LanguageServerCluster::OnReparseNeeded(LSPEvent& event)
{
    LanguageServerProtocol::Ptr_t server = GetServerByName(event.GetServerName());
    if(!server) { return; }

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    server->CloseEditor(editor);
    server->OpenEditor(editor);
}

void LanguageServerCluster::OnRestartNeeded(LSPEvent& event) { RestartServer(event.GetServerName()); }

LanguageServerProtocol::Ptr_t LanguageServerCluster::GetServerByName(const wxString& name)
{
    if(m_servers.count(name) == 0) { return LanguageServerProtocol::Ptr_t(nullptr); }
    return m_servers[name];
}

void LanguageServerCluster::RestartServer(const wxString& name)
{
    LanguageServerProtocol::Ptr_t server = GetServerByName(name);
    if(!server) { return; }
    clDEBUG() << "Restarting LSP server:" << name;
    server->Stop(true);

    // Remove the old instance
    m_servers.erase(name);

    // Create new instance
    if(LanguageServerConfig::Get().GetServers().count(name) == 0) { return; }
    const LanguageServerEntry& entry = LanguageServerConfig::Get().GetServers().at(name);
    StartServer(entry);
}

void LanguageServerCluster::StartServer(const LanguageServerEntry& entry)
{
    if(entry.IsEnabled()) {
        LanguageServerProtocol::Ptr_t lsp(new LanguageServerProtocol(entry.GetName(), this));
        wxArrayString argv;
        argv.Add(entry.GetExepath());
        if(!entry.GetArgs().IsEmpty()) {
            wxArrayString args = ::wxStringTokenize(entry.GetArgs(), " ", wxTOKEN_STRTOK);
            argv.insert(argv.end(), args.begin(), args.end());
        }
        lsp->Start(argv, entry.GetWorkingDirectory(), entry.GetLanguages());
        m_servers.insert({ entry.GetName(), lsp });
        lsp->Bind(wxEVT_LSP_INITIALIZED, &LanguageServerCluster::OnLSPInitialized, this);
    }
}
