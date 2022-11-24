#include "RemotyWorkspace.hpp"

#include "JSON.h"
#include "Platform.hpp"
#include "RemotyConfig.hpp"
#include "RemotyNewWorkspaceDlg.h"
#include "RemotySwitchToWorkspaceDlg.h"
#include "RemotyWorkspaceView.hpp"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "clCodeLiteRemoteProcess.hpp"
#include "clConsoleBase.h"
#include "clFileSystemWorkspace.hpp"
#include "clRemoteFindDialog.h"
#include "clSFTPManager.hpp"
#include "clTempFile.hpp"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "debuggermanager.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "open_resource_dialog.h"
#include "processreaderthread.h"
#include "sample_codelite_remote_json.cpp"
#include "shell_command.h"
#include "wxStringHash.h"

#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/tokenzr.h>
#include <wx/uri.h>
#include <wx/utils.h>

#define CHECK_EVENT(e)     \
    {                      \
        if(!IsOpened()) {  \
            e.Skip();      \
            return;        \
        }                  \
        event.Skip(false); \
    }

namespace
{
const char* CONTEXT_FINDER = "finder";
const char* CONTEXT_BUILDER = "builder";
constexpr int MAX_LOAD_WORKSPACE_RETRIES = 3;
} // namespace

RemotyWorkspace::RemotyWorkspace()
{
    SetWorkspaceType(WORKSPACE_TYPE_NAME);
    Initialise();
}

RemotyWorkspace::RemotyWorkspace(bool dummy)
{
    wxUnusedVar(dummy);
    SetWorkspaceType(WORKSPACE_TYPE_NAME);
}

RemotyWorkspace::~RemotyWorkspace() { UnbindEvents(); }

wxString RemotyWorkspace::GetFileName() const { return m_remoteWorkspaceFile; }
wxString RemotyWorkspace::GetDir() const { return GetFileName().BeforeLast('/'); }

wxString RemotyWorkspace::GetFilesMask() const { return clFileSystemWorkspace::Get().GetFilesMask(); }

wxFileName RemotyWorkspace::GetProjectFileName(const wxString& projectName) const
{
    wxUnusedVar(projectName);
    return wxFileName();
}

void RemotyWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    wxUnusedVar(projectName);
    wxUnusedVar(files);
}

wxString RemotyWorkspace::GetProjectFromFile(const wxFileName& filename) const { return wxEmptyString; }

void RemotyWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    files.clear();
    files.reserve(m_workspaceFiles.size());
    for(const wxString& file : m_workspaceFiles) {
        files.Add(file);
    }
}

wxArrayString RemotyWorkspace::GetWorkspaceProjects() const { return {}; }

bool RemotyWorkspace::IsBuildSupported() const { return true; }

bool RemotyWorkspace::IsProjectSupported() const { return false; }

void RemotyWorkspace::BindEvents()
{
    if(m_eventsConnected) {
        return;
    }

    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &RemotyWorkspace::OnDebugStarting, this);
    EventNotifier::Get()->Bind(wxEVT_SWITCHING_TO_WORKSPACE, &RemotyWorkspace::OnOpenWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_RELOAD_WORKSPACE, &RemotyWorkspace::OnReloadWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &RemotyWorkspace::OnCloseWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &RemotyWorkspace::OnBuildStarting, this);
    EventNotifier::Get()->Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &RemotyWorkspace::OnIsBuildInProgress, this);
    EventNotifier::Get()->Bind(wxEVT_STOP_BUILD, &RemotyWorkspace::OnStopBuild, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_CUSTOM_TARGETS_MENU_SHOWING, &RemotyWorkspace::OnCustomTargetMenu, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RemotyWorkspace::OnBuildHotspotClicked, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &RemotyWorkspace::OnNewWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &RemotyWorkspace::OnDebugEnded, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &RemotyWorkspace::OnRun, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &RemotyWorkspace::OnStop, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_IS_PROGRAM_RUNNING, &RemotyWorkspace::OnIsProgramRunning, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_FIND_MATCHING_PAIR, &RemotyWorkspace::OnFindSwapped, this);
    Bind(wxEVT_TERMINAL_EXIT, &RemotyWorkspace::OnExecProcessTerminated, this);
    EventNotifier::Get()->Bind(wxEVT_OPEN_RESOURCE_FILE_SELECTED, &RemotyWorkspace::OnOpenResourceFile, this);
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, &RemotyWorkspace::OnShutdown, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &RemotyWorkspace::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_OPEN_FILE, &RemotyWorkspace::OnLSPOpenFile, this);
    EventNotifier::Get()->Bind(wxEVT_DOWNLOAD_FILE, &RemotyWorkspace::OnDownloadFile, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_STOP_SEARCH, &RemotyWorkspace::OnStopFindInFiles, this);

    EventNotifier::Get()->Bind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &RemotyWorkspace::OnSftpSaveSuccess, this);
    EventNotifier::Get()->Bind(wxEVT_SFTP_ASYNC_SAVE_ERROR, &RemotyWorkspace::OnSftpSaveError, this);
    // codelite-remote events

    // finder
    m_codeliteRemoteFinder.Bind(wxEVT_CODELITE_REMOTE_RESTARTED, &RemotyWorkspace::OnCodeLiteRemoteTerminated, this);
    m_codeliteRemoteFinder.Bind(wxEVT_CODELITE_REMOTE_FIND_RESULTS, &RemotyWorkspace::OnCodeLiteRemoteFindProgress,
                                this);
    m_codeliteRemoteFinder.Bind(wxEVT_CODELITE_REMOTE_FIND_RESULTS_DONE, &RemotyWorkspace::OnCodeLiteRemoteFindDone,
                                this);
    m_codeliteRemoteFinder.Bind(wxEVT_CODELITE_REMOTE_LIST_FILES, &RemotyWorkspace::OnCodeLiteRemoteListFilesProgress,
                                this);
    m_codeliteRemoteFinder.Bind(wxEVT_CODELITE_REMOTE_LIST_FILES_DONE, &RemotyWorkspace::OnCodeLiteRemoteListFilesDone,
                                this);

    // builder
    m_codeliteRemoteBuilder.Bind(wxEVT_CODELITE_REMOTE_EXEC_OUTPUT, &RemotyWorkspace::OnCodeLiteRemoteBuildOutput,
                                 this);
    m_codeliteRemoteBuilder.Bind(wxEVT_CODELITE_REMOTE_EXEC_DONE, &RemotyWorkspace::OnCodeLiteRemoteBuildOutputDone,
                                 this);
    m_codeliteRemoteBuilder.Bind(wxEVT_CODELITE_REMOTE_RESTARTED, &RemotyWorkspace::OnCodeLiteRemoteTerminated, this);
}

void RemotyWorkspace::UnbindEvents()
{
    if(!m_eventsConnected) {
        return;
    }
    EventNotifier::Get()->Unbind(wxEVT_SWITCHING_TO_WORKSPACE, &RemotyWorkspace::OnOpenWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &RemotyWorkspace::OnCloseWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_RELOAD_WORKSPACE, &RemotyWorkspace::OnReloadWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &RemotyWorkspace::OnBuildStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &RemotyWorkspace::OnIsBuildInProgress, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RemotyWorkspace::OnBuildHotspotClicked, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &RemotyWorkspace::OnNewWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &RemotyWorkspace::OnDebugStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &RemotyWorkspace::OnDebugEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &RemotyWorkspace::OnRun, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &RemotyWorkspace::OnStop, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_FIND_MATCHING_PAIR, &RemotyWorkspace::OnFindSwapped, this);
    EventNotifier::Get()->Unbind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &RemotyWorkspace::OnSftpSaveSuccess, this);
    EventNotifier::Get()->Unbind(wxEVT_SFTP_ASYNC_SAVE_ERROR, &RemotyWorkspace::OnSftpSaveError, this);

    Unbind(wxEVT_TERMINAL_EXIT, &RemotyWorkspace::OnExecProcessTerminated, this);
    EventNotifier::Get()->Unbind(wxEVT_OPEN_RESOURCE_FILE_SELECTED, &RemotyWorkspace::OnOpenResourceFile, this);
    EventNotifier::Get()->Unbind(wxEVT_GOING_DOWN, &RemotyWorkspace::OnShutdown, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &RemotyWorkspace::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_OPEN_FILE, &RemotyWorkspace::OnLSPOpenFile, this);
    EventNotifier::Get()->Unbind(wxEVT_DOWNLOAD_FILE, &RemotyWorkspace::OnDownloadFile, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_STOP_SEARCH, &RemotyWorkspace::OnStopFindInFiles, this);
    // codelite-remote events

    // finder
    m_codeliteRemoteFinder.Unbind(wxEVT_CODELITE_REMOTE_FIND_RESULTS, &RemotyWorkspace::OnCodeLiteRemoteFindProgress,
                                  this);
    m_codeliteRemoteFinder.Unbind(wxEVT_CODELITE_REMOTE_FIND_RESULTS_DONE, &RemotyWorkspace::OnCodeLiteRemoteFindDone,
                                  this);
    m_codeliteRemoteFinder.Unbind(wxEVT_CODELITE_REMOTE_RESTARTED, &RemotyWorkspace::OnCodeLiteRemoteTerminated, this);
    m_codeliteRemoteFinder.Unbind(wxEVT_CODELITE_REMOTE_LIST_FILES, &RemotyWorkspace::OnCodeLiteRemoteListFilesProgress,
                                  this);
    m_codeliteRemoteFinder.Unbind(wxEVT_CODELITE_REMOTE_LIST_FILES_DONE,
                                  &RemotyWorkspace::OnCodeLiteRemoteListFilesDone, this);

    // builder
    m_codeliteRemoteBuilder.Unbind(wxEVT_CODELITE_REMOTE_EXEC_OUTPUT, &RemotyWorkspace::OnCodeLiteRemoteBuildOutput,
                                   this);
    m_codeliteRemoteBuilder.Unbind(wxEVT_CODELITE_REMOTE_EXEC_DONE, &RemotyWorkspace::OnCodeLiteRemoteBuildOutputDone,
                                   this);
    m_codeliteRemoteBuilder.Unbind(wxEVT_CODELITE_REMOTE_RESTARTED, &RemotyWorkspace::OnCodeLiteRemoteTerminated, this);
    m_eventsConnected = false;
}

void RemotyWorkspace::OnCodeLiteRemoteTerminated(clCommandEvent& event)
{
    if(event.GetEventObject() == &m_codeliteRemoteBuilder) {
        clWARNING() << "codelite-remote (builder) terminated" << endl;
        m_buildInProgress = false;
    }
}

void RemotyWorkspace::OnOpenWorkspace(clCommandEvent& event)
{
    RemotySwitchToWorkspaceDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    if(!dlg.IsRemote()) {
        event.Skip();
        event.SetFileName(dlg.GetPath());

    } else {
        DoOpen(dlg.GetPath(), dlg.GetAccount());
    }
}

void RemotyWorkspace::OnCloseWorkspace(clCommandEvent& event)
{
    event.Skip();
    DoClose(true);
}

void RemotyWorkspace::Initialise()
{
    if(m_eventsConnected) {
        return;
    }
    BindEvents();
    m_view = new RemotyWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook(), this);
    clGetManager()->GetWorkspaceView()->AddPage(m_view, WORKSPACE_TYPE_NAME);
}

bool RemotyWorkspace::IsOpened() const { return !m_account.GetAccountName().empty(); }

void RemotyWorkspace::DoClose(bool notify)
{
    m_listLspOutput.clear();
    if(!IsOpened()) {
        clDEBUG() << "Remoty: DoClose() -> not opened..." << endl;
        return;
    }

    m_view->CloseWorkspace();
    m_settings.Save(m_localWorkspaceFile, m_localUserWorkspaceFile);
    m_settings.Clear();

    m_account = {};
    m_remoteWorkspaceFile.clear();
    m_localWorkspaceFile.clear();
    m_localUserWorkspaceFile.clear();

    m_codeliteRemoteBuilder.Stop();
    m_codeliteRemoteFinder.Stop();

    // and restart all the lsp_metadata_arr
    clLanguageServerEvent restart_event(wxEVT_LSP_RESTART_ALL);
    EventNotifier::Get()->AddPendingEvent(restart_event);

    if(notify) {
        // notify codelite to close all opened files
        wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
        eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

        // Notify workspace closed event
        clWorkspaceEvent event_closed(wxEVT_WORKSPACE_CLOSED);
        EventNotifier::Get()->ProcessEvent(event_closed);
    }
}

void RemotyWorkspace::SaveSettings()
{
    if(m_remoteWorkspaceFile.empty() || m_localWorkspaceFile.empty() || m_account.GetAccountName().empty()) {
        return;
    }

    wxBusyCursor bc;
    m_settings.Save(m_localWorkspaceFile, m_localUserWorkspaceFile);
    clSFTPManager::Get().AsyncSaveFile(m_localWorkspaceFile, m_remoteWorkspaceFile, m_account.GetAccountName());
}

void RemotyWorkspace::OnBuildStarting(clBuildEvent& event)
{
    event.Skip();
    if(IsOpened()) {
        event.Skip(false);
        // before we start the build, save all modified files
        clGetManager()->SaveAll(false);
        BuildTarget(event.GetKind());
    }
}

wxString RemotyWorkspace::GetTargetCommand(const wxString& target) const
{
    if(!m_settings.GetSelectedConfig()) {
        return wxEmptyString;
    }
    const auto& M = m_settings.GetSelectedConfig()->GetBuildTargets();
    if(M.count(target)) {
        wxString cmd = M.find(target)->second;
        return cmd;
    }
    return wxEmptyString;
}

void RemotyWorkspace::BuildTarget(const wxString& target)
{
    wxBusyCursor bc;
    auto conf = m_settings.GetSelectedConfig();
    if(!conf) {
        ::wxMessageBox(_("You should have at least one workspace configuration.\n0 found\nOpen the project "
                         "settings and add one"),
                       "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }

    wxString cmd = GetTargetCommand(target);
    if(cmd.IsEmpty()) {
        ::wxMessageBox(_("Don't know how to run '") + target + "'", "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }

    auto envlist = FileUtils::CreateEnvironment(conf->GetEnvironment());
    wxString working_dir = GetRemoteWorkingDir();
    m_codeliteRemoteBuilder.Exec(cmd, working_dir, envlist);
    m_buildInProgress = true;

    // notify about starting build process.
    // we pass the selected compiler in the event
    clBuildEvent e(wxEVT_BUILD_PROCESS_STARTED);
    e.SetToolchain(conf->GetCompiler());
    EventNotifier::Get()->AddPendingEvent(e);

    // Notify about build process started
    clBuildEvent eventStart(wxEVT_BUILD_STARTED);
    EventNotifier::Get()->AddPendingEvent(eventStart);
}

void RemotyWorkspace::DoPrintBuildMessage(const wxString& message)
{
    clBuildEvent e(wxEVT_BUILD_PROCESS_ADDLINE);
    e.SetString(message);
    EventNotifier::Get()->AddPendingEvent(e);
}

void RemotyWorkspace::OnIsBuildInProgress(clBuildEvent& event)
{
    CHECK_EVENT(event);
    event.SetIsRunning(m_codeliteRemoteBuilder.IsRunning() && m_buildInProgress);
}

void RemotyWorkspace::OnStopBuild(clBuildEvent& event)
{
    CHECK_EVENT(event);
    RestartCodeLiteRemote(&m_codeliteRemoteBuilder, m_codeliteRemoteBuilder.GetContext(), true);
    m_buildInProgress = false;

    clBuildEvent eventStopped(wxEVT_BUILD_ENDED);
    EventNotifier::Get()->AddPendingEvent(eventStopped);
}

void RemotyWorkspace::OnCustomTargetMenu(clContextMenuEvent& event)
{
    CHECK_EVENT(event);
    CHECK_PTR_RET(m_settings.GetSelectedConfig());

    wxMenu* menu = event.GetMenu();
    wxArrayString arrTargets;
    const auto& targets = m_settings.GetSelectedConfig()->GetBuildTargets();

    std::unordered_map<int, wxString> M;
    for(const auto& vt : targets) {
        const wxString& name = vt.first;
        int menuId = wxXmlResource::GetXRCID(vt.first);
        M.insert({ menuId, name });
        menu->Append(menuId, name, name, wxITEM_NORMAL);
        menu->Bind(
            wxEVT_MENU,
            [=](wxCommandEvent& e) {
                auto iter = M.find(e.GetId());
                if(iter == M.end()) {
                    return;
                }
                this->CallAfter(&RemotyWorkspace::BuildTarget, iter->second);
            },
            menuId);
    }
}

void RemotyWorkspace::OnBuildHotspotClicked(clBuildEvent& event)
{
    CHECK_EVENT(event);
    wxString filename = event.GetFileName();
    int line_number = event.GetLineNumber();

    clDEBUG() << "Remoty: attempting to open file:" << filename << endl;

    wxFileName fn(filename);
    if(fn.IsRelative(wxPATH_UNIX)) {
        // attempt to make it absolute
        fn.MakeAbsolute(GetRemoteWorkingDir(), wxPATH_UNIX);
        filename = fn.GetFullPath(wxPATH_UNIX);
        clDEBUG() << "Remoty: file is relative, converting to fullpath:" << filename << endl;
    }

    // download the file
    wxBusyCursor bc;
    clGetManager()->GetStatusBar()->SetStatusText(_("Downloading file: ") + filename);
    auto editor = clSFTPManager::Get().OpenFile(filename, m_account.GetAccountName());
    if(editor) {
        // the compiler report line numbers from 1
        // clEditor start counting lines from 0
        // hence the -1
        editor->CenterLine(line_number - 1);
    }
}

void RemotyWorkspace::OnNewWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(event.GetString() == GetWorkspaceType()) {
        event.Skip(false);
        // Prompt the user for folder and name
        RemotyNewWorkspaceDlg dlg(EventNotifier::Get()->TopFrame());
        if(dlg.ShowModal() != wxID_OK) {
            return;
        }

        // create the workspace file
        wxString name;
        wxString remote_path;
        wxString account;
        dlg.GetData(name, remote_path, account);

        // Create the file
        clTempFile tmpfile;
        clFileSystemWorkspaceSettings s;
        s.Save(tmpfile.GetFileName());

        remote_path << "/" << name << ".workspace";
        // upload this file to the remote location
        clDEBUG() << "Writing file: [" << account << "]" << tmpfile.GetFullPath() << "->" << remote_path << endl;
        if(!clSFTPManager::Get().AwaitSaveFile(tmpfile.GetFullPath(), remote_path, account)) {
            ::wxMessageBox(wxString() << _("Failed to create new workspace file:\n") << remote_path, "CodeLite",
                           wxOK | wxICON_ERROR);
            return;
        }

        auto acc = SSHAccountInfo::LoadAccount(account);
        // add this file to the list of recently opened workspaces
        RemotyConfig config;
        RemoteWorkspaceInfo wi{ account, remote_path };
        config.UpdateRecentWorkspaces(wi);
        DoOpen(remote_path, account);
    }
}

void RemotyWorkspace::DoOpen(const wxString& file_path, const wxString& account)
{
    // Close any opened workspace
    auto frame = EventNotifier::Get()->TopFrame();
    wxCommandEvent eventCloseWsp(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    eventCloseWsp.SetEventObject(frame);
    frame->GetEventHandler()->ProcessEvent(eventCloseWsp);

    // close any previously opened workspace
    DoClose(true);

    // Load the account
    auto ssh_account = SSHAccountInfo::LoadAccount(account);

    if(ssh_account.GetAccountName().empty()) {
        ::wxMessageBox(_("Could not find a matching SSH account to load the workspace!"), "CodeLite",
                       wxICON_ERROR | wxCENTER);
        return;
    }

    wxBusyCursor bc;
    wxFileName localFile;
    for(size_t i = 0; i < MAX_LOAD_WORKSPACE_RETRIES; ++i) {
        // first: attempt to download the workspace file and store it locally
        localFile = clSFTPManager::Get().Download(file_path, account);
        if(localFile.IsOk()) {
            break;
        }

        // check if we can do another retry
        if((i + 1) >= MAX_LOAD_WORKSPACE_RETRIES) {
            // retries exhausted
            ::wxMessageBox(_("Failed to download remote workspace file!\n") + clSFTPManager::Get().GetLastError(),
                           "CodeLite", wxICON_ERROR | wxCENTER);
            return;
        }
        clGetManager()->SetStatusMessage(_("Retrying to load workspace..."));
    }

    wxFileName userSettings{ clStandardPaths::Get().GetUserDataDir(), localFile.GetFullName() };
    userSettings.AppendDir("Remoty");
    userSettings.AppendDir("LocalWorkspaces");
    userSettings.Mkdir(wxPATH_MKDIR_FULL);
    clDEBUG() << "User workspace file is set:" << userSettings << endl;
    if(!m_settings.Load(localFile, userSettings)) {
        ::wxMessageBox(_("Failed to load workspace file: ") + m_localWorkspaceFile, "CodeLite",
                       wxICON_ERROR | wxCENTER);
        return;
    }

    m_account = ssh_account;
    m_remoteWorkspaceFile = file_path;
    m_localWorkspaceFile = localFile.GetFullPath();
    m_localUserWorkspaceFile = userSettings.GetFullPath();

    wxString fixed_path = file_path;
    fixed_path.Replace("\\", "/");
    wxString workspacePath = GetRemoteWorkingDir();
    if(workspacePath.empty()) {
        ::wxMessageBox(_("Invalid empty remote path provided"), "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }
    m_view->OpenWorkspace(workspacePath, m_account.GetAccountName());

    // Notify CodeLite that this workspace is opened
    clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    clWorkspaceManager::Get().SetWorkspace(this);

    // wrap the command in ssh
    wxString ssh_exe;
    EnvSetter setter;
    if(!ThePlatform->Which("ssh", &ssh_exe)) {
        ::wxMessageBox(
            _("Could not locate ssh executable in your PATH!\nUpdate your PATH from 'settings -> environment "
              "variables' to a location that contains your 'ssh' executable"),
            "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
    }

    RestartCodeLiteRemote(&m_codeliteRemoteBuilder, CONTEXT_BUILDER);
    RestartCodeLiteRemote(&m_codeliteRemoteFinder, CONTEXT_FINDER);
    ScanForWorkspaceFiles();

    // Notify that the a new workspace is loaded
    clWorkspaceEvent open_event(wxEVT_WORKSPACE_LOADED);
    open_event.SetIsRemote(true);
    open_event.SetFileName(m_remoteWorkspaceFile);
    open_event.SetString(m_remoteWorkspaceFile);
    open_event.SetRemoteAccount(m_account.GetAccountName());
    open_event.SetWorkspaceType(GetWorkspaceType());
    EventNotifier::Get()->AddPendingEvent(open_event);

    // update the remote workspace list
    RemotyConfig config;
    RemoteWorkspaceInfo wi{ m_account.GetAccountName(), m_remoteWorkspaceFile };
    config.UpdateRecentWorkspaces(wi);

    CallAfter(&RemotyWorkspace::RestoreSession);
}

void RemotyWorkspace::OnDebugStarting(clDebugEvent& event)
{
    EnvSetter env;
    CHECK_EVENT(event);
    auto conf = m_settings.GetSelectedConfig();
    CHECK_PTR_RET(conf);

    DebuggerMgr::Get().SetActiveDebugger(conf->GetDebugger());
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(!dbgr) {
        return;
    }

    // if already running, skip this
    // the default behaviour is to "continue"
    if(dbgr->IsRunning()) {
        event.Skip();
        return;
    }

    // Update the debugger information
    DebuggerInformation dinfo = dbgr->GetDebuggerInformation();
    dinfo.breakAtWinMain = true;
    dbgr->SetDebuggerInformation(dinfo);
    dbgr->SetIsRemoteDebugging(false);

    // Setup the debug session
    wxString exe, args, wd;
    GetExecutable(exe, args, wd);

    // Start the debugger
    DebugSessionInfo sesstion_info;
    clDebuggerBreakpoint::Vec_t bpList;

    const wxString& user_debugger = conf->GetDebuggerPath();

    sesstion_info.debuggerPath = user_debugger.empty() ? "gdb" : user_debugger;
    sesstion_info.init_file_content = conf->GetDebuggerCommands();

    sesstion_info.exeName = exe;
    sesstion_info.cwd = wd;
    clGetManager()->GetBreakpoints(bpList);
    sesstion_info.bpList = bpList;
    sesstion_info.isSSHDebugging = true;
    sesstion_info.sshAccountName = m_account.GetAccountName();

    // open new terminal on the remote host
    m_remote_terminal.reset(new clRemoteTerminal(m_account));
    if(!m_remote_terminal->Start()) {
        ::wxMessageBox(_("Failed to start remote terminal process. Do you have `ssh` client installed and in PATH?"),
                       "CodeLite", wxICON_ERROR | wxOK | wxOK_DEFAULT);
        return;
    }

    // wait for the tty
    wxBusyCursor bc;
    size_t count = 100; // 10 seconds
    wxString tty;

    clDEBUG() << "Waiting for tty..." << endl;
    while(--count) {
        tty = m_remote_terminal->ReadTty();
        if(tty.empty()) {
            wxMilliSleep(100);
        } else {
            break;
        }
    }

    clDEBUG() << "Using remote tty:" << tty << endl;

    // convert the envlist into map
    auto envlist = FileUtils::CreateEnvironment(conf->GetEnvironment());
    wxStringMap_t envmap;
    envmap.reserve(envlist.size());
    envmap.insert(envlist.begin(), envlist.end());

    // override the gdb executable with the one provided with the GDB environment variable
    if(envmap.count("GDB")) {
        const wxString& gdbpath = envmap["GDB"];
        sesstion_info.debuggerPath = gdbpath;
    }
    clDEBUG() << "Using gdb:" << sesstion_info.debuggerPath << endl;

    sesstion_info.ttyName = tty;
    sesstion_info.enablePrettyPrinting = true;

    clDEBUG() << "Starting gdb:" << sesstion_info.debuggerPath << endl;
    if(!dbgr->Start(sesstion_info, &envlist)) {
        // message box about this and cancel the debugge session
        ::wxMessageBox(_("Failed to start debugger!"), "CodeLite", wxICON_ERROR | wxOK | wxOK_DEFAULT);
        clDebugEvent eventStarted(wxEVT_DEBUG_ENDED);
        EventNotifier::Get()->ProcessEvent(eventStarted);
    }

    // Notify that debug session started
    // this will ensure that the debug layout is loaded
    clDebugEvent eventStarted(wxEVT_DEBUG_STARTED);
    eventStarted.SetClientData(&sesstion_info);
    EventNotifier::Get()->ProcessEvent(eventStarted);

    // Now run the debuggee
    dbgr->Run(args, "");
}

void RemotyWorkspace::GetExecutable(wxString& exe, wxString& args, wxString& wd)
{
    auto conf = m_settings.GetSelectedConfig();
    CHECK_PTR_RET(conf);

    exe = conf->GetExecutable();
    args = conf->GetArgs();
    wd = conf->GetWorkingDirectory().IsEmpty() ? GetDir() : conf->GetWorkingDirectory();
}

IProcess* RemotyWorkspace::DoRunSSHProcess(const wxString& scriptContent, bool sync)
{
    wxString path = UploadScript(scriptContent);
    std::vector<wxString> args = { "/bin/bash", path };
    size_t flags = IProcessCreateDefault | IProcessCreateSSH;
    if(sync) {
        flags |= IProcessCreateSync;
    }
    return ::CreateAsyncProcess(this, args, flags, wxEmptyString, nullptr, m_account.GetAccountName());
}

wxString RemotyWorkspace::GetRemoteWorkingDir() const { return m_remoteWorkspaceFile.BeforeLast('/'); }

void RemotyWorkspace::OnDebugEnded(clDebugEvent& event)
{
    event.Skip();
    m_remote_terminal.reset();
}

void RemotyWorkspace::OnRun(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    auto conf = m_settings.GetSelectedConfig();
    CHECK_PTR_RET(conf);

    wxString command;
    command = conf->GetExecutable();
    if(command.empty()) {
        ::wxMessageBox(_("Please specify an executable to run"), "CodeLite", wxICON_ERROR | wxOK | wxOK_DEFAULT);
        return;
    }

    // the main command to run
    ::WrapWithQuotes(command);

    wxString args = conf->GetArgs();
    args.Replace("\r", wxEmptyString);
    args.Replace("\n", " ");
    auto args_arr = StringUtils::BuildArgv(args);

    // append the args
    for(auto& arg : args_arr) {
        arg.Trim().Trim(false);
        // wrap with quotes if required
        ::WrapWithQuotes(arg);
        command << " " << arg;
    }

    wxString envString = CreateEnvScriptContent();
    wxString wd = conf->GetWorkingDirectory();
    wd.Trim().Trim(false);

    if(wd.empty()) {
        wd = GetRemoteWorkingDir();
    }

    wxString scriptContent;
    scriptContent << envString << "\n";
    scriptContent << "cd " << wd << "\n";

    command.Trim().Trim(false);
    scriptContent << command << "\n";
    scriptContent << "exit $?";

    wxString script_path = UploadScript(scriptContent);

    // build the command
    wxString cmd_args;
    cmd_args << m_account.GetUsername() << "@" << m_account.GetHost() << " -p " << m_account.GetPort() << " '";
    cmd_args << "/bin/bash " << script_path << "'";

    // start the process
    auto terminal = clConsoleBase::GetTerminal();
    terminal->SetCommand("ssh", cmd_args);
    terminal->SetTerminalNeeded(true);
    terminal->SetWaitWhenDone(true);
    terminal->SetSink(this);
    terminal->Start();
    m_execPID = terminal->GetPid();
}

void RemotyWorkspace::OnStop(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    if(m_execPID != wxNOT_FOUND) {
        ::clKill(m_execPID, wxSIGTERM, true, false);
        m_execPID = wxNOT_FOUND;
    }
}

wxString RemotyWorkspace::CreateEnvScriptContent() const
{
    auto conf = m_settings.GetSelectedConfig();
    CHECK_PTR_RET_EMPTY_STRING(conf);

    const wxString& envstr = conf->GetEnvironment();
    auto envmap = FileUtils::CreateEnvironment(envstr);

    wxString content;
    content << "# prepare the environment variables\n";
    for(auto& vt : envmap) {
        content << "export " << vt.first << "=" << ::WrapWithQuotes(vt.second) << "\n";
    }
    return content;
}

wxString RemotyWorkspace::UploadScript(const wxString& content, const wxString& script_path) const
{
    wxString script_content;
    script_content << "#!/bin/bash -e\n"; // stop at first error
    script_content << content;

    wxString default_path;
    default_path << "/tmp/codelite-remoty." << clGetUserName() << ".sh";

    wxString path = default_path;
    if(!script_path.empty()) {
        path = script_path;
    }

    if(!clSFTPManager::Get().AwaitWriteFile(script_content, path, m_account.GetAccountName())) {
        ::wxMessageBox(_("Failed to write remote script on the remote machine!"), "CodeLite", wxICON_ERROR | wxCENTER);
        return wxEmptyString;
    }
    return path;
}

void RemotyWorkspace::OnIsProgramRunning(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    event.SetAnswer(m_execPID != wxNOT_FOUND);
}

void RemotyWorkspace::OnExecProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    m_execPID = wxNOT_FOUND;
}

void RemotyWorkspace::OnFindSwapped(clFileSystemEvent& event)
{
    auto editor = clGetManager()->GetActiveEditor();
    if(!editor) {
        event.Skip();
        return;
    }
    if(!editor->IsRemoteFile()) {
        event.Skip();
        return;
    }
    CHECK_EVENT(event);
    std::vector<wxString> exts;

    // replace the file extension
    auto type = FileExtManager::GetTypeFromExtension(editor->GetFileName().GetFullName());
    if(type == FileExtManager::TypeSourceC || type == FileExtManager::TypeSourceCpp) {
        // try to find a header file
        exts.push_back("h");
        exts.push_back("hpp");
        exts.push_back("hxx");
        exts.push_back("h++");

    } else {
        // try to find a implementation file
        exts.push_back("cpp");
        exts.push_back("cxx");
        exts.push_back("cc");
        exts.push_back("c++");
        exts.push_back("c");
    }

    wxString remote_path = editor->GetRemotePath();
    for(const auto& other_ext : exts) {
        remote_path = remote_path.BeforeLast('.');
        remote_path << "." << other_ext;
        if(clSFTPManager::Get().IsFileExists(remote_path, m_account)) {
            // open this file
            auto other_editor = clSFTPManager::Get().OpenFile(remote_path, m_account);
            event.SetPath(other_editor->GetFileName().GetFullPath());
        }
    }
}

void RemotyWorkspace::RestartCodeLiteRemote(clCodeLiteRemoteProcess* proc, const wxString& context, bool restart)
{
    CHECK_PTR_RET(proc);

    // if running and restart is true, restart codelite-remote
    if(proc->IsRunning() && restart) {
        clDEBUG() << "Stopping codelite-remote..." << endl;
        proc->Stop();
    }

    // make sure we are not running
    if(proc->IsRunning()) {
        clDEBUG() << "codelite-remote is already running" << endl;
        return;
    }

    clDEBUG() << "Starting codelite-remote...(" << context << ") ..." << endl;

    // upload codelite-remote script to the workspace folder
    clSFTPManager::Get().NewFolder(GetRemoteWorkingDir() + "/.codelite", m_account);

    wxString codelite_remote_script;
    codelite_remote_script << GetRemoteWorkingDir() << "/.codelite/codelite-remote";
    clDEBUG() << "Calling proc->StartInteractive(..," << codelite_remote_script << ",..)" << endl;
    proc->StartInteractive(m_account, codelite_remote_script, context);
    clDEBUG() << "Starting codelite-remote...(" << context << ") ... done" << endl;
}

void RemotyWorkspace::OnCodeLiteRemoteListFilesProgress(clCommandEvent& event)
{
    m_workspaceFiles.reserve(event.GetStrings().size() + m_workspaceFiles.size());
    m_workspaceFiles.insert(m_workspaceFiles.end(), event.GetStrings().begin(), event.GetStrings().end());
}

void RemotyWorkspace::OnCodeLiteRemoteListFilesDone(clCommandEvent& event)
{
    wxString message;
    message << _("Remote file system scan completed. Found: ") << m_workspaceFiles.size() << _(" files");
    clGetManager()->SetStatusMessage(message);

    // notify that scan is completed
    clDEBUG() << "Sending wxEVT_WORKSPACE_FILES_SCANNED event..." << endl;
    clWorkspaceEvent event_scan{ wxEVT_WORKSPACE_FILES_SCANNED };
    EventNotifier::Get()->ProcessEvent(event_scan);
}

void RemotyWorkspace::ScanForWorkspaceFiles()
{
    wxString root_dir = GetRemoteWorkingDir();
    wxString file_extensions = GetSettings().GetSelectedConfig()->GetFileExtensions();
    file_extensions.Replace("*", "");

    auto files_exts = ::wxStringTokenize(file_extensions, ";,", wxTOKEN_STRTOK);
    std::unordered_set<wxString> S{ files_exts.begin(), files_exts.end() };

    // common file extensions
    S.insert(".txt");
    S.insert(".toml");
    S.insert("Rakefile");

    file_extensions.clear();
    for(const auto& s : S) {
        file_extensions << s << ";";
    }
    m_workspaceFiles.clear();

    // use the finder codelite-remote
    m_codeliteRemoteFinder.ListFiles(root_dir, file_extensions);
}

void RemotyWorkspace::OnOpenResourceFile(clCommandEvent& event)
{
    CHECK_EVENT(event);

    // our event
    auto editor = clSFTPManager::Get().OpenFile(event.GetFileName(), m_account);
    if(editor) {
        editor->GetCtrl()->GotoLine(event.GetLineNumber());
    }
}

void RemotyWorkspace::OnShutdown(clCommandEvent& event)
{
    event.Skip();
    DoClose(false);
}

void RemotyWorkspace::OnInitDone(wxCommandEvent& event) { event.Skip(); }

void RemotyWorkspace::FindInFiles(const wxString& root_dir, const wxString& file_extensions, const wxString& find_what,
                                  bool whole_word, bool icase)
{
    m_remoteFinder.SetCodeLiteRemote(&m_codeliteRemoteFinder);
    wxString search_folder = root_dir;
    if(search_folder == "<Workspace Folder>") {
        search_folder = GetRemoteWorkingDir();
    }
    m_remoteFinder.Search(search_folder, find_what, file_extensions, whole_word, icase);
}

void RemotyWorkspace::OnCodeLiteRemoteFindProgress(clFindInFilesEvent& event)
{
    m_remoteFinder.ProcessSearchOutput(event, false);
}

void RemotyWorkspace::OnCodeLiteRemoteFindDone(clFindInFilesEvent& event)
{
    m_remoteFinder.ProcessSearchOutput(event, true);
}

void RemotyWorkspace::OnCodeLiteRemoteBuildOutput(clProcessEvent& event)
{
    DoProcessBuildOutput(event.GetOutput(), false);
}

void RemotyWorkspace::OnCodeLiteRemoteBuildOutputDone(clProcessEvent& event)
{
    DoProcessBuildOutput(event.GetOutput(), true);
    m_buildInProgress = false;
}

void RemotyWorkspace::DoProcessBuildOutput(const wxString& output, bool is_completed)
{
    if(!output.empty()) {
        DoPrintBuildMessage(output);
    }

    if(is_completed) {
        clBuildEvent e(wxEVT_BUILD_PROCESS_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);

        // Notify about build process started
        clBuildEvent eventStopped(wxEVT_BUILD_ENDED);
        EventNotifier::Get()->AddPendingEvent(eventStopped);
    }
}

void RemotyWorkspace::OnLSPOpenFile(LSPEvent& event)
{
    CHECK_EVENT(event);
    auto editor = clSFTPManager::Get().OpenFile(event.GetFileName(), m_account);
    if(!editor) {
        event.Skip();
        return;
    }
    editor->SelectRange(event.GetLocation().GetRange());
}

wxString RemotyWorkspace::GetName() const { return wxFileName(m_localWorkspaceFile).GetName(); }

bool LSPParams::IsOk() const { return !this->command.empty() && !this->lsp_name.empty(); }

void LSPParams::From(const JSONItem& json)
{
    this->lsp_name = "Remoty." + json["name"].toString();
    this->command = json["command"].toString();
    this->working_directory = json["working_directory"].toString();

    wxArrayString langs_arr = json["languages"].toArrayString();
    this->languages.reserve(langs_arr.size());
    this->languages.insert(languages.end(), langs_arr.begin(), langs_arr.end());
    this->priority = json["priority"].toSize_t(80);

    int env_list_size = json["env"].arraySize();
    for(int i = 0; i < env_list_size; ++i) {
        auto env_entry = json["env"][i];
        wxString name = env_entry["name"].toString();
        wxString value = env_entry["value"].toString();
        if(!name.empty()) {
            this->env.push_back({ name, value });
        }
    }
}

void RemotyWorkspace::SetProjectActive(const wxString& name) { wxUnusedVar(name); }

void RemotyWorkspace::OnDownloadFile(clCommandEvent& event)
{
    clDEBUG() << "Downloading file:" << event.GetFileName() << "using account:" << m_account.GetName() << endl;
    CHECK_EVENT(event);
    auto editor = clSFTPManager::Get().OpenFile(event.GetFileName(), m_account);
    if(editor) {
        // update the event with the local file's fullpath
        event.SetFileName(editor->GetFileName().GetFullPath());
        event.Skip(false);
    }
}

void RemotyWorkspace::OpenWorkspace(const wxString& path, const wxString& account) { DoOpen(path, account); }

void RemotyWorkspace::OnReloadWorkspace(clCommandEvent& event)
{
    CHECK_EVENT(event);
    wxString filepath = GetRemoteWorkspaceFile();
    wxString account_name = GetAccount().GetAccountName();
    CallAfter(&RemotyWorkspace::OpenWorkspace, filepath, account_name);
}

IEditor* RemotyWorkspace::OpenFile(const wxString& remote_file_path)
{
    if(!IsOpened())
        return nullptr;
    return clSFTPManager::Get().OpenFile(remote_file_path, m_account);
}

void RemotyWorkspace::OpenAndEditCodeLiteRemoteJson()
{
    wxString remote_file_path = GetRemoteWorkingDir();
    remote_file_path << "/.codelite/codelite-remote.json";
    IEditor* editor = OpenFile(remote_file_path);
    if(editor) {
        return;
    }
    // Could not find the file, prompt the user
    if(wxMessageBox(_("Could not find codelite-remote.json file\nWould you like to create one?"), "CodeLite",
                    wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTRE) != wxYES) {
        return;
    }

    wxString remote_file_dir = GetRemoteWorkingDir() + "/.codelite";
    if(!clSFTPManager::Get().NewFolder(remote_file_dir, m_account)) {
        wxMessageBox(_("Failed to create directory: ") + remote_file_dir, "CodeLite", wxICON_ERROR | wxOK);
        return;
    }

    // create a new file
    if(!clSFTPManager::Get().NewFile(remote_file_path, m_account)) {
        wxMessageBox(_("Failed to create file: ") + remote_file_path, "CodeLite", wxICON_ERROR | wxOK);
        return;
    }

    editor = OpenFile(remote_file_path);
    if(!editor) {
        wxMessageBox(_("Failed to open file: ") + remote_file_path, "CodeLite", wxICON_ERROR | wxOK);
        return;
    }
    editor->SetEditorText(DEFAULT_CODELITE_REMOTE_JSON);
    editor->SetActive();
}

void RemotyWorkspace::OnStopFindInFiles(clFindInFilesEvent& event)
{
    event.Skip();
    if(IsOpened() && m_codeliteRemoteFinder.IsRunning()) {
        RestartCodeLiteRemote(&m_codeliteRemoteFinder, CONTEXT_FINDER, true);
        // send event notifying that the search has been cancelled
        m_remoteFinder.NotifySearchCancelled();
    }
}

void RemotyWorkspace::RestoreSession()
{
    clCommandEvent event_loading{ wxEVT_SESSION_LOADING };
    EventNotifier::Get()->AddPendingEvent(event_loading);

    // TODO:
    // Do the actual session loading here

    clCommandEvent event_loaded{ wxEVT_SESSION_LOADED };
    EventNotifier::Get()->AddPendingEvent(event_loaded);
}

wxString RemotyWorkspace::GetDebuggerName() const
{
    if(m_settings.GetSelectedConfig()) {
        return m_settings.GetSelectedConfig()->GetDebugger();
    } else {
        return wxEmptyString;
    }
}

void RemotyWorkspace::OnSftpSaveError(clCommandEvent& event)
{
    event.Skip();
    if(!IsOpened()) {
        return;
    }
    ::wxMessageBox(_("Failed to save file: ") + event.GetFileName(), "CodeLite", wxICON_WARNING | wxOK | wxCENTRE);
}

void RemotyWorkspace::OnSftpSaveSuccess(clCommandEvent& event)
{
    event.Skip();
    if(!IsOpened()) {
        return;
    }
    clGetManager()->SetStatusMessage(_("Remote file: ") + event.GetFileName() + _(" successfully saved"));
}

wxString RemotyWorkspace::GetSshAccount() const { return GetAccount().GetAccountName(); }
