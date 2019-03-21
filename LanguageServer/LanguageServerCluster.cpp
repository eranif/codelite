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
#include "cl_standard_paths.h"
#include "clWorkspaceManager.h"

LanguageServerCluster::LanguageServerCluster()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerCluster::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &LanguageServerCluster::OnWorkspaceOpen, this);

    Bind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Bind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Bind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Bind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
}

LanguageServerCluster::~LanguageServerCluster()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerCluster::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &LanguageServerCluster::OnWorkspaceOpen, this);
    Unbind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Unbind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Unbind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Unbind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
}

void LanguageServerCluster::Reload()
{
    StopAll();

    // If we are not enabled, stop here
    if(!LanguageServerConfig::Get().IsEnabled()) { return; }

    StartAll();
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

    // Manage the browser (BACK and FORWARD) ourself
    BrowseRecord from;
    IEditor* oldEditor = clGetManager()->GetActiveEditor();
    if(oldEditor) { from = oldEditor->CreateBrowseRecord(); }
    IEditor* editor = clGetManager()->OpenFile(fn.GetFullPath(), "", wxNOT_FOUND, OF_None);
    if(editor) {
        editor->SelectRange(location.GetRange());
        if(oldEditor) { NavMgr::Get()->AddJump(from, editor->CreateBrowseRecord()); }
    }
}

void LanguageServerCluster::OnLSPInitialized(LSPEvent& event) { wxUnusedVar(event); }

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
    server->Stop();

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
        LanguageServerProtocol::Ptr_t lsp(new LanguageServerProtocol(entry.GetName(), entry.GetNetType(), this));
        lsp->SetPriority(entry.GetPriority());
        wxString lspCommand = entry.GetExepath();
        ::WrapWithQuotes(lspCommand);

        if(!entry.GetArgs().IsEmpty()) { lspCommand << " " << entry.GetArgs(); }

        wxString helperCommand;

        if(entry.GetNetType() == eNetworkType::kStdio) {
            helperCommand = LanguageServerConfig::Get().GetNodejs();
            if(helperCommand.IsEmpty() || !wxFileName::FileExists(helperCommand)) {
                // TODO : prompt the user to install NodeJS
                return;
            }

            ::WrapWithQuotes(helperCommand);
            wxFileName fnScriptPath(clStandardPaths::Get().GetBinFolder(), "codelite-lsp-helper");
            wxString scriptPath = fnScriptPath.GetFullPath();
            ::WrapWithQuotes(scriptPath);
            helperCommand << " " << scriptPath;
        } else {
            helperCommand = entry.GetConnectionString();
        }

        size_t flags = 0;
        if(entry.IsShowConsole()) { flags |= LSPStartupInfo::kShowConsole; }
        wxString rootDir;
        if(clWorkspaceManager::Get().GetWorkspace()) {
            rootDir = clWorkspaceManager::Get().GetWorkspace()->GetFileName().GetPath();
        }
        clDEBUG() << "Starting lsp:";
        clDEBUG() << "helperCommand / connection string:" << helperCommand;
        clDEBUG() << "lspCommand:" << lspCommand;
        clDEBUG() << "entry.GetWorkingDirectory():" << entry.GetWorkingDirectory();
        clDEBUG() << "rootDir:" << rootDir;
        clDEBUG() << "entry.GetLanguages():" << entry.GetLanguages();

        lsp->Start(helperCommand, lspCommand, entry.GetWorkingDirectory(), rootDir, entry.GetLanguages(), flags);
        m_servers.insert({ entry.GetName(), lsp });
    }
}

void LanguageServerCluster::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    this->StopAll();
}

void LanguageServerCluster::OnWorkspaceOpen(wxCommandEvent& event)
{
    event.Skip();
    this->Reload();

    // Now that the workspace is loaded, parse the active file
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    LanguageServerProtocol::Ptr_t lsp = GetServerForFile(editor->GetFileName());
    if(lsp) { lsp->OpenEditor(editor); }
}

void LanguageServerCluster::StopAll()
{
    for(const std::unordered_map<wxString, LanguageServerProtocol::Ptr_t>::value_type& vt : m_servers) {
        // stop all current processes
        LanguageServerProtocol::Ptr_t server = vt.second;
        server.reset(nullptr);
    }
    m_servers.clear();
}

void LanguageServerCluster::StartAll()
{
    // create a new list
    const LanguageServerEntry::Map_t& servers = LanguageServerConfig::Get().GetServers();
    for(const LanguageServerEntry::Map_t::value_type& vt : servers) {
        const LanguageServerEntry& entry = vt.second;
        StartServer(entry);
    }
}
