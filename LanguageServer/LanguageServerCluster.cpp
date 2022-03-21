#include "LanguageServerCluster.h"

#include "CompileCommandsGenerator.h"
#include "LSP/LSPEvent.h"
#include "LSPOutlineViewDlg.h"
#include "LanguageServerConfig.h"
#include "PathConverterDefault.hpp"
#include "StringUtils.h"
#include "clEditorBar.h"
#include "clSFTPEvent.h"
#include "clSelectSymbolDialog.h"
#include "clWorkspaceManager.h"
#include "cl_calltip.h"
#include "cl_standard_paths.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "languageserver.h"
#include "macromanager.h"
#include "macros.h"
#include "wxCodeCompletionBoxManager.h"

#include <algorithm>
#include <thread>
#include <unordered_set>
#include <wx/arrstr.h>
#include <wx/stc/stc.h>

namespace
{
struct SymbolClientData : public wxClientData {
    SymbolClientData(const LSP::Location& loc) { location = loc; }
    LSP::Location location;
};
} // namespace

LanguageServerCluster::LanguageServerCluster(LanguageServerPlugin* plugin)
    : m_plugin(plugin)
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerCluster::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &LanguageServerCluster::OnWorkspaceOpen, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_CLOSED, &LanguageServerCluster::OnEditorClosed, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerCluster::OnActiveEditorChanged, this);

    EventNotifier::Get()->Bind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED,
                               &LanguageServerCluster::OnCompileCommandsGenerated, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &LanguageServerCluster::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_OPEN_RESOURCE, &LanguageServerCluster::OnOpenResource, this);

    Bind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Bind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Bind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Bind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
    Bind(wxEVT_LSP_INITIALIZED, &LanguageServerCluster::OnLSPInitialized, this);
    Bind(wxEVT_LSP_METHOD_NOT_FOUND, &LanguageServerCluster::OnMethodNotFound, this);
    Bind(wxEVT_LSP_SIGNATURE_HELP, &LanguageServerCluster::OnSignatureHelp, this);
    Bind(wxEVT_LSP_HOVER, &LanguageServerCluster::OnHover, this);
    Bind(wxEVT_LSP_SET_DIAGNOSTICS, &LanguageServerCluster::OnSetDiagnostics, this);
    Bind(wxEVT_LSP_CLEAR_DIAGNOSTICS, &LanguageServerCluster::OnClearDiagnostics, this);
    Bind(wxEVT_LSP_SHOW_QUICK_OUTLINE_DLG, &LanguageServerCluster::OnShowQuickOutlineDlg, this);
    Bind(wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE, &LanguageServerCluster::OnQuickOutlineView, this);
    Bind(wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, &LanguageServerCluster::OnOulineViewSymbols, this);
    Bind(wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT, &LanguageServerCluster::OnDocumentSymbolsForHighlight, this);
    Bind(wxEVT_LSP_SEMANTICS, &LanguageServerCluster::OnSemanticTokens, this);
    Bind(wxEVT_LSP_LOGMESSAGE, &LanguageServerCluster::OnLogMessage, this);
}

LanguageServerCluster::~LanguageServerCluster()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerCluster::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &LanguageServerCluster::OnWorkspaceOpen, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CLOSED, &LanguageServerCluster::OnEditorClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerCluster::OnActiveEditorChanged, this);

    EventNotifier::Get()->Unbind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED,
                                 &LanguageServerCluster::OnCompileCommandsGenerated, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_ENDED, &LanguageServerCluster::OnBuildEnded, this);

    EventNotifier::Get()->Unbind(wxEVT_CMD_OPEN_RESOURCE, &LanguageServerCluster::OnOpenResource, this);

    Unbind(wxEVT_LSP_SHOW_QUICK_OUTLINE_DLG, &LanguageServerCluster::OnShowQuickOutlineDlg, this);
    Unbind(wxEVT_LSP_DEFINITION, &LanguageServerCluster::OnSymbolFound, this);
    Unbind(wxEVT_LSP_COMPLETION_READY, &LanguageServerCluster::OnCompletionReady, this);
    Unbind(wxEVT_LSP_REPARSE_NEEDED, &LanguageServerCluster::OnReparseNeeded, this);
    Unbind(wxEVT_LSP_RESTART_NEEDED, &LanguageServerCluster::OnRestartNeeded, this);
    Unbind(wxEVT_LSP_INITIALIZED, &LanguageServerCluster::OnLSPInitialized, this);
    Unbind(wxEVT_LSP_METHOD_NOT_FOUND, &LanguageServerCluster::OnMethodNotFound, this);
    Unbind(wxEVT_LSP_SIGNATURE_HELP, &LanguageServerCluster::OnSignatureHelp, this);
    Unbind(wxEVT_LSP_HOVER, &LanguageServerCluster::OnHover, this);
    Unbind(wxEVT_LSP_SET_DIAGNOSTICS, &LanguageServerCluster::OnSetDiagnostics, this);
    Unbind(wxEVT_LSP_CLEAR_DIAGNOSTICS, &LanguageServerCluster::OnClearDiagnostics, this);
    Unbind(wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE, &LanguageServerCluster::OnQuickOutlineView, this);
    Unbind(wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, &LanguageServerCluster::OnOulineViewSymbols, this);
    Unbind(wxEVT_LSP_SEMANTICS, &LanguageServerCluster::OnSemanticTokens, this);
    Unbind(wxEVT_LSP_LOGMESSAGE, &LanguageServerCluster::OnLogMessage, this);
    Unbind(wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT, &LanguageServerCluster::OnDocumentSymbolsForHighlight, this);
    if(m_quick_outline_dlg) {
        m_quick_outline_dlg->Destroy();
        m_quick_outline_dlg = nullptr;
    }
}

void LanguageServerCluster::Reload(const std::unordered_set<wxString>& languages)
{
    StopAll(languages);

    // If we are not enabled, stop here
    if(!LanguageServerConfig::Get().IsEnabled()) {
        return;
    }

    StartAll(languages);
}

LanguageServerProtocol::Ptr_t LanguageServerCluster::GetServerForEditor(IEditor* editor)
{
    LanguageServerProtocol::Ptr_t bestServer(nullptr);
    for(const auto& vt : m_servers) {
        const auto& thisServer = vt.second;
        if(bestServer && thisServer->GetPriority() <= bestServer->GetPriority()) {
            // this LSP has lower priority, skip
            continue;
        }
        if(thisServer->CanHandle(editor)) {
            bestServer = thisServer;
        }
    }

    return bestServer;
}

void LanguageServerCluster::OnSymbolFound(LSPEvent& event)
{
    // if we have more than one location - prompt the user
    if(event.GetLocations().empty()) {
        return;
    }

    // for now, use the first location
    LSP::Location location;
    if(event.GetLocations().size() > 1) {
        // multiple matches
        clSelectSymbolDialogEntry::List_t entries;
        entries.reserve(event.GetLocations().size());
        for(const auto& loc : event.GetLocations()) {
            entries.emplace_back();
            auto& entry = entries.back();
            entry.name = loc.GetPattern();
            entry.clientData = new SymbolClientData(loc);
        }

        clSelectSymbolDialog dlg(nullptr, entries);
        if(dlg.ShowModal() != wxID_OK) {
            return;
        }
        location = static_cast<SymbolClientData*>(dlg.GetSelection())->location;
    } else {
        location = event.GetLocations()[0];
    }

    // let someone else try and open this file first, as it might be a remote file
    LSPEvent open_event(wxEVT_LSP_OPEN_FILE);
    open_event.SetLocation(location);
    open_event.SetFileName(location.GetPath());
    open_event.SetLineNumber(location.GetRange().GetStart().GetLine());
    if(EventNotifier::Get()->ProcessEvent(open_event)) {
        return;
    }

    wxFileName fn(location.GetPath());
    clDEBUG() << "LSP: Opening file:" << fn << "(" << location.GetRange().GetStart().GetLine() << ":"
              << location.GetRange().GetStart().GetCharacter() << ")";

    // Manage the browser (BACK and FORWARD) ourself
    BrowseRecord from;
    IEditor* oldEditor = clGetManager()->GetActiveEditor();
    if(oldEditor) {
        from = oldEditor->CreateBrowseRecord();
    }

    auto cb = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        // try selecting using a location first
        if(!editor->SelectLocation(location)) {
            // try the range
            editor->SelectRange(location.GetRange());
        }
        NavMgr::Get()->StoreCurrentLocation(from, editor->CreateBrowseRecord());
    };
    clGetManager()->OpenFileAndAsyncExecute(fn.GetFullPath(), std::move(cb));
}

void LanguageServerCluster::OnLSPInitialized(LSPEvent& event)
{
    // Now that the workspace is loaded, parse the active file
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    LanguageServerProtocol::Ptr_t lsp = GetServerByName(event.GetServerName());
    if(lsp) {
        lsp->OpenEditor(editor);
    }
}

void LanguageServerCluster::OnSignatureHelp(LSPEvent& event)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    // Signature help results are ready, display them in the editor
    const LSP::SignatureHelp& sighelp = event.GetSignatureHelp();

    TagEntryPtrVector_t tags;
    LSPSignatureHelpToTagEntries(tags, sighelp);

    if(tags.empty()) {
        return;
    }
    editor->ShowCalltip(new clCallTip(tags));
}

void LanguageServerCluster::OnHover(LSPEvent& event)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    const LSP::Hover& hover = event.GetHover();
    const LSP::MarkupContent& contents = hover.GetContents();
    const LSP::Range& range = hover.GetRange();

    if(contents.GetValue().IsEmpty()) {
        // No available tooltip
        return;
    }

    // Sanity check for range (if available)
    if(range.IsOk()) {
        // Is the mouse pointer still under the hovered position?
        int pos = editor->GetPosAtMousePointer();
        if(pos == wxNOT_FOUND) {
            return;
        }
        LSP::Position cur(editor->LineFromPos(pos), editor->GetColumnInChars(pos));
        if(cur < range.GetStart() || cur > range.GetEnd()) {
            // The hover is no longer valid
            return;
        }
    }

    if(contents.GetKind() == "markdown") {
        editor->ShowTooltip(contents.GetValue());
    } else {
        wxString fixedTip = contents.GetValue();
        StringUtils::DisableMarkdownStyling(fixedTip);
        editor->ShowTooltip(fixedTip);
    }
}

void LanguageServerCluster::OnMethodNotFound(LSPEvent& event) { wxUnusedVar(event); }

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
    if(!server) {
        return;
    }

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    server->CloseEditor(editor);
    server->OpenEditor(editor);
}

void LanguageServerCluster::OnSemanticTokens(LSPEvent& event)
{
    LanguageServerProtocol::Ptr_t server = GetServerByName(event.GetServerName());
    CHECK_PTR_RET(server);

    clDEBUG() << "Processing semantic tokens from server:" << server->GetName() << "file:" << event.GetFileName()
              << endl;
    IEditor* editor = clGetManager()->FindEditor(event.GetFileName());
    if(!editor) {
        clDEBUG() << "Could not locate file:" << event.GetFileName() << endl;
        // maybe the given path is on a remote machine, try again, using the remote path this time
        IEditor::List_t all_editors;
        clGetManager()->GetAllEditors(all_editors);
        for(auto e : all_editors) {
            if(e->IsRemoteFile() && (e->GetRemotePath() == event.GetFileName())) {
                editor = e;
                clDEBUG() << "Found remote file:" << e->GetRemotePath() << endl;
                break;
            }
        }
    }
    CHECK_PTR_RET(editor);
    clDEBUG() << "Found the editor!" << endl;
    const auto& semanticTokens = event.GetSemanticTokens();

    wxStringSet_t variables_tokens = { "variable", "parameter", "typeParameter", "property" };
    wxStringSet_t classes_tokens = { "class", "enum", "namespace", "type", "struct", "trait", "interface" };
    wxStringSet_t method_tokens = { "function", "method" };

    wxStringSet_t classes_set;
    wxStringSet_t variables_set;
    wxStringSet_t methods_set;

    wxString classes_str;
    wxString variabls_str;
    wxString method_str;

    clDEBUG() << "Going over" << semanticTokens.size() << "tokens" << endl;
    for(const auto& token : semanticTokens) {
        // is this an interesting token?
        wxString token_type = server->GetSemanticToken(token.token_type);
        bool is_class = classes_tokens.count(token_type) > 0;
        bool is_variable = variables_tokens.count(token_type) > 0;
        bool is_method = method_tokens.count(token_type) > 0;

        // read its name
        int start_pos = editor->GetCtrl()->PositionFromLine(token.line) + token.column;
        int end_pos = start_pos + token.length;
        wxString token_name = editor->GetTextRange(start_pos, end_pos);
        if(!is_class && !is_variable && !is_method) {
            continue;
        }

        if(is_class && classes_set.count(token_name) == 0) {
            classes_set.insert(token_name);
            classes_str << token_name << " ";
        } else if(is_variable && variables_set.count(token_name) == 0) {
            variables_set.insert(token_name);
            variabls_str << token_name << " ";
        } else if(is_method && methods_set.count(token_name) == 0) {
            methods_set.insert(token_type);
            method_str << token_name << " ";
        }
    }
    clDEBUG() << "Done" << endl;

    clDEBUG() << "Calling editor->SetSemanticTokens" << endl;
    editor->SetSemanticTokens(classes_str, variabls_str, method_str, wxEmptyString);
    clDEBUG() << "Success" << endl;
}

void LanguageServerCluster::OnRestartNeeded(LSPEvent& event)
{
    // Don't keep restarting a crashing LSP
    // We consider a "crashing LSP" as:
    // 1. Crashed more than 10 times
    // 2. The interval between crashes is less than 1 minute

    clDEBUG() << "LSP:" << event.GetServerName() << "needs to be restarted" << clEndl;
    auto iter = m_restartCounters.find(event.GetServerName());
    if(iter == m_restartCounters.end()) {
        iter = m_restartCounters.insert({ event.GetServerName(), {} }).first;
    }

    time_t curtime = time(nullptr);
    CrashInfo& crash_info = iter->second;
    if((curtime - crash_info.last_crash) >= 60) {
        // if the last crash occured over 1 min ago, reset the crash counters
        crash_info.times = 0;
    }

    crash_info.times++;              // increase the restart counter
    crash_info.last_crash = curtime; // remember when the crash occured
    if(crash_info.times > 10) {
        clWARNING() << "Too many restart failures for LSP:" << event.GetServerName() << ". Will not restart it again"
                    << clEndl;
        return;
    }
    RestartServer(event.GetServerName());
}

LanguageServerProtocol::Ptr_t LanguageServerCluster::GetServerByName(const wxString& name)
{
    if(m_servers.count(name) == 0) {
        return LanguageServerProtocol::Ptr_t(nullptr);
    }
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
        if(!server) {
            return;
        }
        clDEBUG() << "Restarting LSP server:" << name << clEndl;
        server->Stop();

        // Remove the old instance
        m_servers.erase(name);
    }

    // Create new instance
    if(LanguageServerConfig::Get().GetServers().count(name) == 0) {
        return;
    }
    const LanguageServerEntry& entry = LanguageServerConfig::Get().GetServers().at(name);
    StartServer(entry);
}

void LanguageServerCluster::StartServer(const LanguageServerEntry& entry)
{
    if(!entry.IsEnabled()) {
        clDEBUG1() << "LSP" << entry.GetName() << "is not enabled" << clEndl;
        return;
    }

    clDEBUG1() << "Connecting to LSP server:" << entry.GetName();

    if(!entry.IsValid()) {
        clWARNING() << "LSP Server" << entry.GetName()
                    << "is not valid and it will not be started (one of the specified paths do not "
                       "exist)";
        LanguageServerConfig::Get().GetServers()[entry.GetName()].SetEnabled(false);
        LanguageServerConfig::Get().Save();
        return;
    }

    if(m_servers.count(entry.GetName())) {
        clDEBUG() << "LSP" << entry.GetName() << "is already running" << endl;
        return;
    }

    LanguageServerProtocol::Ptr_t lsp(new LanguageServerProtocol(entry.GetName(), entry.GetNetType(), this));
    lsp->SetPriority(entry.GetPriority());
    lsp->SetDisaplayDiagnostics(entry.IsDisaplayDiagnostics());

    if(lsp->GetName() == "ctagsd") {
        // set startup callback
        auto cb = [=]() {
            if(!clWorkspaceManager::Get().IsWorkspaceOpened()) {
                return;
            }

            wxFileName fn(clWorkspaceManager::Get().GetWorkspace()->GetFileName().GetPath(), wxEmptyString);
            fn.AppendDir(".ctagsd");
            fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

            wxFileName settings_json(fn.GetPath(), "ctagsd.json");
            {
                if(!settings_json.FileExists()) {
                    // create an empty json object file
                    FileUtils::WriteFileContent(settings_json, "{}");
                } else {
                    // the file exists, ensure its a valid json
                    JSON root(settings_json);
                    if(!root.isOk()) {
                        FileUtils::WriteFileContent(settings_json, "{}");
                    }
                }
            }

            JSON root(settings_json);
            JSONItem json = root.toElement();

            if(json.hasNamedObject("limit_results")) {
                json.removeProperty("limit_results");
                clDEBUG() << "ctagsd: found limit_results -> removing it" << endl;
            }

            // update the entries
            if(json.hasNamedObject("codelite_indexer")) {
                json.removeProperty("codelite_indexer");
                clDEBUG() << "ctagsd: found codelite_indexer -> removing it" << endl;
            }

            json.addProperty("codelite_indexer", clStandardPaths::Get().GetBinaryFullPath("codelite-ctags"));
            json.addProperty("limit_results", TagsManagerST::Get()->GetCtagsOptions().GetCcNumberOfDisplayItems());
            root.save(settings_json);
            clDEBUG() << "ctagsd: writing new file:" << settings_json << endl;

            // create the file_list.txt file
            wxFileName file_list(fn.GetPath(), "file_list.txt");
            wxArrayString files;
            clWorkspaceManager::Get().GetWorkspace()->GetWorkspaceFiles(files);

            wxString file_list_content;
            for(const auto& filepath : files) {
                file_list_content << filepath << "\n";
            }
            FileUtils::WriteFileContent(file_list, file_list_content);
        };
        lsp->SetStartedCallback(std::move(cb));
    }

    wxString command = entry.GetCommand();

    wxString project;
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        project = clCxxWorkspaceST::Get()->GetActiveProjectName();
    }
    command = MacroManager::Instance()->Expand(command, clGetManager(), project);
    wxArrayString lspCommand;
    lspCommand = StringUtils::BuildArgv(command);

    wxString rootDir;
    if(clWorkspaceManager::Get().GetWorkspace() && entry.IsAutoRestart() && !entry.IsRemoteLSP()) {
        // we have an opened workspace. if the workspace has a root directory, let's use it
        // otherwise, default to the LSP value
        wxString path = clWorkspaceManager::Get().GetWorkspace()->GetFileName().GetPath();
        rootDir = path.empty() ? entry.GetWorkingDirectory() : path;
    } else {
        // Remote LSP will set the *remote* working directory in the configuration
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
    if(entry.IsAutoRestart()) {
        flags |= LSPStartupInfo::kAutoStart;
    }

    if(entry.IsRemoteLSP()) {
        flags |= LSPStartupInfo::kRemoteLSP;
    }

    LSPStartupInfo startup_info;
    startup_info.SetConnectioString(entry.GetConnectionString());
    startup_info.SetLspServerCommand(lspCommand);
    startup_info.SetFlags(flags);
    startup_info.SetWorkingDirectory(entry.GetWorkingDirectory());
    startup_info.SetAccountName(entry.GetSshAccount());
    lsp->Start(startup_info, entry.GetEnv(), entry.GetInitOptions(), rootDir, entry.GetLanguages());
    m_servers.insert({ entry.GetName(), lsp });
}

void LanguageServerCluster::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    clDEBUG() << "LSP: workspace CLOSED event" << endl;
    LanguageServerProtocol::workspace_file_type = FileExtManager::TypeOther;
    this->StopAll();
    m_symbols_to_file_cache.clear();
}

void LanguageServerCluster::OnWorkspaceOpen(clWorkspaceEvent& event)
{
    event.Skip();
    clDEBUG() << "LSP: workspace OPEN event" << endl;
    this->Reload();
    m_symbols_to_file_cache.clear();
    DiscoverWorkspaceType();
}

void LanguageServerCluster::SetWorkspaceType(FileExtManager::FileType type)
{
    LanguageServerProtocol::workspace_file_type = type;
    clDEBUG() << "*** LSP: workspace type is set:" << LanguageServerProtocol::workspace_file_type << "***" << endl;
}

void LanguageServerCluster::StopAll(const std::unordered_set<wxString>& languages)
{
    clDEBUG() << "LSP: Stopping all servers" << endl;
    if(languages.empty()) {
        // stop all
        for(const std::unordered_map<wxString, LanguageServerProtocol::Ptr_t>::value_type& vt : m_servers) {
            // stop all current processes
            LanguageServerProtocol::Ptr_t server = vt.second;
            server.reset(nullptr);
        }
        m_servers.clear();
    } else {
        for(const auto& lang : languages) {
            clDEBUG() << "Stopping server for language:" << lang << endl;
            auto server = GetServerForLanguage(lang);
            if(server) {
                // this will stop and remove the server from the list
                StopServer(server->GetName());
            }
        }
    }
    clDEBUG() << "LSP: Success" << endl;

    // Clear all markers
    ClearAllDiagnostics();
}

void LanguageServerCluster::StartAll(const std::unordered_set<wxString>& languages)
{
    // create a new list
    ClearAllDiagnostics();

    clDEBUG() << "LSP: Staring all servers..." << endl;
    if(languages.empty()) {
        const LanguageServerEntry::Map_t& servers = LanguageServerConfig::Get().GetServers();
        for(const LanguageServerEntry::Map_t::value_type& vt : servers) {
            const LanguageServerEntry& entry = vt.second;
            StartServer(entry);
        }
    } else {
        for(const wxString& lang : languages) {
            const LanguageServerEntry::Map_t& servers = LanguageServerConfig::Get().GetServers();
            for(const auto& vt : servers) {
                // start all the LSPs for the current language
                if(vt.second.IsEnabled() && vt.second.GetLanguages().Index(lang) != wxNOT_FOUND) {
                    StartServer(vt.second);
                }
            }
        }
    }
    clDEBUG() << "LSP: Success" << endl;
}

void LanguageServerCluster::LSPSignatureHelpToTagEntries(TagEntryPtrVector_t& tags, const LSP::SignatureHelp& sighelp)
{
    if(sighelp.GetSignatures().empty()) {
        return;
    }
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
        tag->SetTypename(returnValue);
        tag->SetKind("function");
        tag->SetFlags(TagEntry::Tag_No_Signature_Format);
        tags.push_back(tag);
    }
}

void LanguageServerCluster::OnCompileCommandsGenerated(clCommandEvent& event)
{
    event.Skip();
    clGetManager()->SetStatusMessage(_("Restarting Language Servers..."));
    this->Reload({ "c", "cpp" }); // restart c/c++ the servers
    clGetManager()->SetStatusMessage(_("Ready"));
}

void LanguageServerCluster::OnOulineViewSymbols(LSPEvent& event)
{
    event.Skip();
    // we use it for outline + editor bar
    if(m_symbols_to_file_cache.count(event.GetFileName())) {
        m_symbols_to_file_cache.erase(event.GetFileName());
    }
    m_symbols_to_file_cache.insert({ event.GetFileName(), event.GetSymbolsInformation() });
    clDEBUG() << "LSP: cached symbols for file" << event.GetFileName() << endl;
    UpdateNavigationBar();
}

void LanguageServerCluster::OnShowQuickOutlineDlg(LSPEvent& event)
{
    wxUnusedVar(event);
    if(m_quick_outline_dlg == nullptr) {
        m_quick_outline_dlg = new LSPOutlineViewDlg(EventNotifier::Get()->TopFrame());
    }
    if(!m_quick_outline_dlg->IsShown()) {
        m_quick_outline_dlg->Show();
    }
    m_quick_outline_dlg->SetSymbols({});
}

void LanguageServerCluster::OnQuickOutlineView(LSPEvent& event)
{
    if(m_quick_outline_dlg && m_quick_outline_dlg->IsShown()) {
        m_quick_outline_dlg->SetSymbols(event.GetSymbolsInformation());
    }
}

void LanguageServerCluster::OnSetDiagnostics(LSPEvent& event)
{
    event.Skip();
    IEditor* editor = FindEditor(event);
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
    IEditor* editor = FindEditor(event);
    if(editor) {
        editor->DelAllCompilerMarkers();
    }
}

void LanguageServerCluster::ClearAllDiagnostics()
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for(IEditor* editor : editors) {
        editor->DelAllCompilerMarkers();
    }
}

void LanguageServerCluster::ClearRestartCounters() { m_restartCounters.clear(); }

void LanguageServerCluster::OnBuildEnded(clBuildEvent& event) { event.Skip(); }

void LanguageServerCluster::StopServer(const wxString& name)
{
    LanguageServerProtocol::Ptr_t server = GetServerByName(name);
    if(!server) {
        return;
    }

    clDEBUG() << "Stopping LSP server:" << name << clEndl;
    server->Stop();
    // Remove the old instance
    m_servers.erase(name);
}

void LanguageServerCluster::DeleteServer(const wxString& name)
{
    StopServer(name); // stop any server that goes by that name
    // Delete it's configuration entry
    LanguageServerConfig::Get().RemoveServer(name);
    LanguageServerConfig::Get().Save();
}

void LanguageServerCluster::StartServer(const wxString& name)
{
    auto entry = LanguageServerConfig::Get().GetServer(name);
    if(!entry.IsValid()) {
        return;
    }
    StartServer(entry);
}

wxString LanguageServerCluster::GetEditorFilePath(IEditor* editor) const
{
    if(editor->IsRemoteFile()) {
        return editor->GetRemotePath();
    } else {
        return editor->GetFileName().GetFullPath();
    }
}

IEditor* LanguageServerCluster::FindEditor(const LSPEvent& event) const
{
    return FindEditor(event.GetLocation().GetPath());
}

IEditor* LanguageServerCluster::FindEditor(const wxString& path) const
{
    IEditor::List_t all_editors;
    clGetManager()->GetAllEditors(all_editors);
    for(IEditor* editor : all_editors) {
        if(editor->GetFileName().GetFullPath() == path || editor->GetRemotePath() == path) {
            return editor;
        }
    }
    return nullptr;
}

LanguageServerProtocol::Ptr_t LanguageServerCluster::GetServerForLanguage(const wxString& lang)
{
    LanguageServerProtocol::Ptr_t bestServer(nullptr);
    for(const auto& vt : m_servers) {
        const auto& thisServer = vt.second;
        if(bestServer && thisServer->GetPriority() <= bestServer->GetPriority()) {
            // this LSP has lower priority, skip
            continue;
        }
        if(thisServer->IsRunning() && thisServer->IsLanguageSupported(lang)) {
            bestServer = thisServer;
        }
    }

    if(bestServer) {
        clDEBUG() << "Using server" << bestServer->GetName() << "for language" << lang << endl;
    }
    return bestServer;
}

void LanguageServerCluster::OnLogMessage(LSPEvent& event)
{
    event.Skip();
    m_plugin->LogMessage(event.GetServerName(), event.GetMessage(), event.GetLogMessageSeverity());
}

void LanguageServerCluster::OnDocumentSymbolsForHighlight(LSPEvent& event)
{
    clDEBUG() << "LanguageServerCluster::OnDocumentSymbolsForHighlight called for file:" << event.GetFileName() << endl;
    IEditor* editor = FindEditor(event.GetFileName());
    CHECK_PTR_RET(editor);

    const auto& symbols = event.GetSymbolsInformation();
    wxString classes, variables, methods, others;
    for(const auto& si : symbols) {
        switch(si.GetKind()) {
        case LSP::kSK_Module:
        case LSP::kSK_Namespace:
        case LSP::kSK_Package:
        case LSP::kSK_Class:
        case LSP::kSK_Enum:
        case LSP::kSK_Interface:
        case LSP::kSK_Struct:
        case LSP::kSK_Object:
            classes << si.GetName() << " ";
            break;
        case LSP::kSK_Method:
        case LSP::kSK_Function:
            methods << si.GetName() << " ";
            break;
        case LSP::kSK_TypeParameter:
            others << si.GetName() << " ";
            break;
        case LSP::kSK_EnumMember:
        case LSP::kSK_Property:
        case LSP::kSK_Field:
        case LSP::kSK_Variable:
        case LSP::kSK_Constant:
            variables << si.GetName() << " ";
        default:
            break;
        }
    }
    clDEBUG() << "Setting semantic highlight (using DocumentSymbolsRequest):" << endl;
    clDEBUG() << "Classes  :" << classes << endl;
    clDEBUG() << "Variables:" << variables << endl;
    clDEBUG() << "Methods:" << methods << endl;
    editor->SetSemanticTokens(classes, variables, methods, others);
}

void LanguageServerCluster::OnOpenResource(wxCommandEvent& event)
{
    event.Skip();
    DiscoverWorkspaceType();
}

void LanguageServerCluster::DiscoverWorkspaceType()
{
    if(LanguageServerProtocol::workspace_file_type != FileExtManager::TypeOther) {
        return;
    }

    wxArrayString files;
    clWorkspaceManager::Get().GetWorkspace()->GetWorkspaceFiles(files);
    if(files.empty()) {
        clWARNING() << "Workspace contains no files" << endl;
        return;
    }

    LanguageServerProtocol::workspace_file_type = FileExtManager::TypeOther;
    std::thread thr(
        [=](const wxArrayString& files, wxEvtHandler* owner) {
            // create a table for all the known file types
            int table[FileExtManager::TypeLast + 1];
            memset(table, 0, sizeof(table));

            // holds the index to the biggest file type found so far
            int cur_max = wxNOT_FOUND;
            FileExtManager::FileType cur_type = FileExtManager::TypeOther;

            std::unordered_set<int> acceptable_file_types = {
                FileExtManager::TypeSQL,       FileExtManager::TypeShellScript, FileExtManager::TypeSourceC,
                FileExtManager::TypeSourceCpp, FileExtManager::TypeCSS,         FileExtManager::TypeHeader,
                FileExtManager::TypeHtml,      FileExtManager::TypeJS,          FileExtManager::TypeJava,
                FileExtManager::TypeLua,       FileExtManager::TypeRuby,        FileExtManager::TypeRust,
                FileExtManager::TypePython,
            };
            for(const wxString& file : files) {
                auto file_type = FileExtManager::GetType(file);
                if(file_type != FileExtManager::TypeOther &&
                   acceptable_file_types.count(file_type)) { // TypeOther is -1
                    table[file_type]++;
                    if(table[file_type] > cur_max) {
                        cur_type = file_type;
                        // update the current max
                        cur_max = table[file_type];
                    }
                }
            }
            owner->CallAfter(&LanguageServerCluster::SetWorkspaceType, cur_type);
        },
        files, this);
    thr.detach();
}

void LanguageServerCluster::OnEditorClosed(clCommandEvent& event)
{
    event.Skip();
    // clear the cache for the closed file
    m_symbols_to_file_cache.erase(event.GetFileName());
}

void LanguageServerCluster::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    UpdateNavigationBar();
}

void LanguageServerCluster::UpdateNavigationBar()
{
    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    wxString fullpath = editor->IsRemoteFile() ? editor->GetRemotePath() : editor->GetFileName().GetFullPath();
    if(m_symbols_to_file_cache.count(fullpath) == 0) {
        return;
    }

    auto symbols = m_symbols_to_file_cache.find(fullpath)->second;

    // prepare list of scopes and send them to the navigation bar
    clEditorBar::ScopeEntry::vec_t scopes;
    scopes.reserve(symbols.size());

    for(const LSP::SymbolInformation& symbol : symbols) {
        // only collect methdos
        if(symbol.GetKind() != LSP::kSK_Function && symbol.GetKind() != LSP::kSK_Method &&
           symbol.GetKind() != LSP::kSK_Constructor)
            continue;
        clEditorBar::ScopeEntry scope_entry;
        const LSP::Location& location = symbol.GetLocation();
        scope_entry.line_number = location.GetRange().GetStart().GetLine();

        wxString display_string;
        if(!symbol.GetContainerName().empty()) {
            display_string << symbol.GetContainerName() << ".";
        }

        wxString short_name = symbol.GetName();
        short_name = short_name.BeforeFirst('(');
        short_name += "()";
        display_string << short_name;

        scope_entry.display_string.swap(display_string);
        scopes.push_back(scope_entry);
    }
    clGetManager()->GetNavigationBar()->SetScopes(fullpath, scopes);
}
