#include "CompileCommandsGenerator.h"
#include "LanguageServerCluster.h"
#include "LanguageServerConfig.h"
#include "PathConverterDefault.hpp"
#include "StringUtils.h"
#include "clWorkspaceManager.h"
#include "cl_calltip.h"
#include "cl_standard_paths.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macromanager.h"
#include "wxCodeCompletionBoxManager.h"
#include <algorithm>
#include <wx/stc/stc.h>

LanguageServerCluster::LanguageServerCluster()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerCluster::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &LanguageServerCluster::OnWorkspaceOpen, this);
    EventNotifier::Get()->Bind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED,
                               &LanguageServerCluster::OnCompileCommandsGenerated, this);

    Bind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Bind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Bind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Bind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
    Bind(wxEVT_LSP_INITIALIZED, &LanguageServerCluster::OnLSPInitialized, this);
    Bind(wxEVT_LSP_METHOD_NOT_FOUND, &LanguageServerCluster::OnMethodNotFound, this);
    Bind(wxEVT_LSP_SIGNATURE_HELP, &LanguageServerCluster::OnSignatureHelp, this);
    Bind(wxEVT_LSP_SET_DIAGNOSTICS, &LanguageServerCluster::OnSetDiagnostics, this);
    Bind(wxEVT_LSP_CLEAR_DIAGNOSTICS, &LanguageServerCluster::OnClearDiagnostics, this);
}

LanguageServerCluster::~LanguageServerCluster()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerCluster::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &LanguageServerCluster::OnWorkspaceOpen, this);
    EventNotifier::Get()->Unbind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED,
                                 &LanguageServerCluster::OnCompileCommandsGenerated, this);
    Unbind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Unbind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Unbind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Unbind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
    Unbind(wxEVT_LSP_INITIALIZED, &LanguageServerCluster::OnLSPInitialized, this);
    Unbind(wxEVT_LSP_METHOD_NOT_FOUND, &LanguageServerCluster::OnMethodNotFound, this);
    Unbind(wxEVT_LSP_SIGNATURE_HELP, &LanguageServerCluster::OnSignatureHelp, this);
    Unbind(wxEVT_LSP_SET_DIAGNOSTICS, &LanguageServerCluster::OnSetDiagnostics, this);
    Unbind(wxEVT_LSP_CLEAR_DIAGNOSTICS, &LanguageServerCluster::OnClearDiagnostics, this);
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

void LanguageServerCluster::OnLSPInitialized(LSPEvent& event)
{
    wxUnusedVar(event); // Now that the workspace is loaded, parse the active file
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    LanguageServerProtocol::Ptr_t lsp = GetServerForFile(editor->GetFileName());
    if(lsp) { lsp->OpenEditor(editor); }
}

void LanguageServerCluster::OnSignatureHelp(LSPEvent& event)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    // Signature help results are ready, display them in the editor
    const LSP::SignatureHelp& sighelp = event.GetSignatureHelp();

    TagEntryPtrVector_t tags;
    LSPSignatureHelpToTagEntries(tags, sighelp);

    if(tags.empty()) { return; }
    editor->ShowCalltip(new clCallTip(tags));
}

void LanguageServerCluster::OnMethodNotFound(LSPEvent& event)
{
    LanguageServerEntry& entry = LanguageServerConfig::Get().GetServer(event.GetServerName());
    if(entry.IsValid()) {
        entry.AddUnImplementedMethod(event.GetString());
        LanguageServerConfig::Get().Save();
    }
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
    {
        // Incase a server terminated, remove it from the list of servers
        // We do this in an inner block because 'server' (line below) will have
        // ref-count of 2 to make sure it is destroyed (i.e. unregister itself from
        // the service provider manager) the ref count needs to get to 0
        // Hence the inner block
        LanguageServerProtocol::Ptr_t server = GetServerByName(name);
        if(!server) { return; }
        clDEBUG() << "Restarting LSP server:" << name;
        server->Stop();

        // Remove the old instance
        m_servers.erase(name);
    }

    // Create new instance
    if(LanguageServerConfig::Get().GetServers().count(name) == 0) { return; }
    const LanguageServerEntry& entry = LanguageServerConfig::Get().GetServers().at(name);
    StartServer(entry);
}

void LanguageServerCluster::StartServer(const LanguageServerEntry& entry)
{
    if(entry.IsEnabled()) {
        clDEBUG() << "Connecting to LSP server:" << entry.GetName();

        if(!entry.IsValid()) {
            clWARNING() << "LSP Server" << entry.GetName()
                        << "is not valid and it will not be started (one of the specified paths do not "
                           "exist)";
            LanguageServerConfig::Get().GetServers()[entry.GetName()].SetEnabled(false);
            LanguageServerConfig::Get().Save();
            return;
        }

        IPathConverter::Ptr_t pathConverter(new PathConverterDefault());
        LanguageServerProtocol::Ptr_t lsp(
            new LanguageServerProtocol(entry.GetName(), entry.GetNetType(), this, pathConverter));
        lsp->SetPriority(entry.GetPriority());
        lsp->SetDisaplayDiagnostics(entry.IsDisaplayDiagnostics());
        lsp->SetUnimplementedMethods(entry.GetUnimplementedMethods());

        wxString command = entry.GetCommand();

        wxString project;
        if(clCxxWorkspaceST::Get()->IsOpen()) { project = clCxxWorkspaceST::Get()->GetActiveProjectName(); }
        command = MacroManager::Instance()->Expand(command, clGetManager(), project);
        wxArrayString lspCommand;
        lspCommand = StringUtils::BuildArgv(command);

        wxString rootDir;
        if(clWorkspaceManager::Get().GetWorkspace() && entry.IsAutoRestart()) {
            rootDir = clWorkspaceManager::Get().GetWorkspace()->GetFileName().GetPath();
        } else {
            rootDir = entry.GetWorkingDirectory();
        }

        clDEBUG() << "Starting lsp:";
        clDEBUG() << "Connection string:" << entry.GetConnectionString();
        if(entry.IsAutoRestart()) {
            clDEBUG() << "lspCommand:" << lspCommand;
            clDEBUG() << "entry.GetWorkingDirectory():" << entry.GetWorkingDirectory();
        }
        clDEBUG() << "rootDir:" << rootDir;
        clDEBUG() << "entry.GetLanguages():" << entry.GetLanguages();

        size_t flags = 0;
        if(entry.IsAutoRestart()) { flags |= LSPStartupInfo::kAutoStart; }

        lsp->Start(lspCommand, entry.GetConnectionString(), entry.GetWorkingDirectory(), rootDir, entry.GetLanguages(),
                   flags);
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
}

void LanguageServerCluster::StopAll()
{
    for(const std::unordered_map<wxString, LanguageServerProtocol::Ptr_t>::value_type& vt : m_servers) {
        // stop all current processes
        LanguageServerProtocol::Ptr_t server = vt.second;
        server.reset(nullptr);
    }
    m_servers.clear();

    // Clear all markers
    ClearAllDiagnostics();
}

void LanguageServerCluster::StartAll()
{
    // create a new list
    ClearAllDiagnostics();

    const LanguageServerEntry::Map_t& servers = LanguageServerConfig::Get().GetServers();
    for(const LanguageServerEntry::Map_t::value_type& vt : servers) {
        const LanguageServerEntry& entry = vt.second;
        StartServer(entry);
    }
}

void LanguageServerCluster::LSPSignatureHelpToTagEntries(TagEntryPtrVector_t& tags, const LSP::SignatureHelp& sighelp)
{
    if(sighelp.GetSignatures().empty()) { return; }
    const LSP::SignatureInformation::Vec_t& sigs = sighelp.GetSignatures();
    for(const LSP::SignatureInformation& si : sigs) {
        TagEntryPtr tag(new TagEntry());
        wxString sig = si.GetLabel().BeforeFirst('-');
        sig.Trim().Trim(false);
        wxString returnValue = si.GetLabel().AfterFirst('-');
        if(!returnValue.IsEmpty()) {
            returnValue.Remove(0, 1); // remove ">"
            returnValue.Trim().Trim(false);
        }

        tag->SetSignature(sig);
        tag->SetReturnValue(returnValue);
        tag->SetKind("function");
        tag->SetFlags(TagEntry::Tag_No_Signature_Format);
        tags.push_back(tag);
    }
}

void LanguageServerCluster::OnCompileCommandsGenerated(clCommandEvent& event)
{
    event.Skip();
    clGetManager()->SetStatusMessage(_("Restarting Language Servers..."));
    this->Reload(); // restart the servers
    clGetManager()->SetStatusMessage(_("Ready"));
}

void LanguageServerCluster::OnSetDiagnostics(LSPEvent& event)
{
    event.Skip();
    wxString uri = event.GetLocation().GetUri();
    wxFileName fn(uri);
    IEditor* editor = clGetManager()->FindEditor(fn.GetFullPath());
    if(editor) {
        editor->DelAllCompilerMarkers();
        for(const LSP::Diagnostic& d : event.GetDiagnostics()) {
            // LSP uses 1 based line numbers
            editor->SetErrorMarker(d.GetRange().GetStart().GetLine(), d.GetMessage());
        }
    }
}

void LanguageServerCluster::OnClearDiagnostics(LSPEvent& event)
{
    event.Skip();
    wxString uri = event.GetLocation().GetUri();
    wxFileName fn(uri);
    IEditor* editor = clGetManager()->FindEditor(fn.GetFullPath());
    if(editor) { editor->DelAllCompilerMarkers(); }
}

void LanguageServerCluster::ClearAllDiagnostics()
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for(IEditor* editor : editors) {
        editor->DelAllCompilerMarkers();
    }
}
