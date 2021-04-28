#include "RemotyNewWorkspaceDlg.h"
#include "RemotySwitchToWorkspaceDlg.h"
#include "RemotyWorkspace.hpp"
#include "RemotyWorkspaceView.hpp"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "clConsoleBase.h"
#include "clFileSystemWorkspace.hpp"
#include "clRemoteFindDialog.h"
#include "clSFTPManager.hpp"
#include "clTempFile.hpp"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "debuggermanager.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "processreaderthread.h"
#include "shell_command.h"
#include <wx/msgdlg.h>
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

RemotyWorkspace::~RemotyWorkspace()
{
    UnbindEvents();
}
wxFileName RemotyWorkspace::GetFileName() const
{
    return {};
}
wxString RemotyWorkspace::GetFilesMask() const
{
    return clFileSystemWorkspace::Get().GetFilesMask();
}

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

wxString RemotyWorkspace::GetProjectFromFile(const wxFileName& filename) const
{
    return wxEmptyString;
}

void RemotyWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    // TODO :: implement this
    wxUnusedVar(files);
}

wxArrayString RemotyWorkspace::GetWorkspaceProjects() const
{
    return {};
}

bool RemotyWorkspace::IsBuildSupported() const
{
    return true;
}

bool RemotyWorkspace::IsProjectSupported() const
{
    return false;
}

void RemotyWorkspace::BindEvents()
{
    if(m_eventsConnected) {
        return;
    }

    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &RemotyWorkspace::OnDebugStarting, this);
    EventNotifier::Get()->Bind(wxEVT_SWITCHING_TO_WORKSPACE, &RemotyWorkspace::OnOpenWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &RemotyWorkspace::OnCloseWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &RemotyWorkspace::OnBuildStarting, this);
    EventNotifier::Get()->Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &RemotyWorkspace::OnIsBuildInProgress, this);
    EventNotifier::Get()->Bind(wxEVT_STOP_BUILD, &RemotyWorkspace::OnStopBuild, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_CUSTOM_TARGETS_MENU_SHOWING, &RemotyWorkspace::OnCustomTargetMenu, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RemotyWorkspace::OnBuildHotspotClicked, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &RemotyWorkspace::OnNewWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &RemotyWorkspace::OnDebugEnded, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &RemotyWorkspace::OnBuildProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &RemotyWorkspace::OnBuildProcessOutput, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &RemotyWorkspace::OnRun, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &RemotyWorkspace::OnStop, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_IS_PROGRAM_RUNNING, &RemotyWorkspace::OnIsProgramRunning, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_FIND_MATCHING_PAIR, &RemotyWorkspace::OnFindSwapped, this);
    Bind(wxEVT_TERMINAL_EXIT, &RemotyWorkspace::OnExecProcessTerminated, this);
}

void RemotyWorkspace::UnbindEvents()
{
    if(!m_eventsConnected) {
        return;
    }
    EventNotifier::Get()->Unbind(wxEVT_SWITCHING_TO_WORKSPACE, &RemotyWorkspace::OnOpenWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &RemotyWorkspace::OnCloseWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &RemotyWorkspace::OnBuildStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &RemotyWorkspace::OnIsBuildInProgress, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RemotyWorkspace::OnBuildHotspotClicked, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &RemotyWorkspace::OnNewWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &RemotyWorkspace::OnDebugStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &RemotyWorkspace::OnDebugEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &RemotyWorkspace::OnRun, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &RemotyWorkspace::OnStop, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_FIND_MATCHING_PAIR, &RemotyWorkspace::OnFindSwapped, this);

    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &RemotyWorkspace::OnBuildProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &RemotyWorkspace::OnBuildProcessOutput, this);
    Unbind(wxEVT_TERMINAL_EXIT, &RemotyWorkspace::OnExecProcessTerminated, this);
    m_eventsConnected = false;
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
        return;
    }
    DoOpen(dlg.GetPath());
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

bool RemotyWorkspace::IsOpened() const
{
    return !m_account.GetAccountName().empty();
}

void RemotyWorkspace::DoClose(bool notify)
{
    if(!IsOpened()) {
        return;
    }

    m_view->CloseWorkspace();
    m_settings.Save(m_localWorkspaceFile, m_localUserWorkspaceFile);
    m_settings.Clear();

    m_account = {};
    m_remoteWorkspaceFile.clear();
    m_localWorkspaceFile.clear();
    m_localUserWorkspaceFile.clear();

    // remove any clangd configured by us
    clLanguageServerEvent delete_event(wxEVT_LSP_DELETE);
    delete_event.SetLspName("Remoty - clangd");
    EventNotifier::Get()->ProcessEvent(delete_event);

    // and restart all the LSPs
    clLanguageServerEvent restart_event(wxEVT_LSP_RESTART_ALL);
    EventNotifier::Get()->AddPendingEvent(restart_event);

    if(notify) {
        // notify codelite to close all opened files
        wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
        eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

        // Notify workspace closed event
        wxCommandEvent event_closed(wxEVT_WORKSPACE_CLOSED);
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
        DoBuild(event.GetKind());
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

void RemotyWorkspace::DoBuild(const wxString& target)
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

    if(m_buildProcess) {
        return;
    }

    // Creae a build script
    wxString buildScriptContent;

    buildScriptContent << CreateEnvScriptContent();
    buildScriptContent << "\n";

    wxString working_dir = GetRemoteWorkingDir();
    buildScriptContent << "cd " << working_dir << "\n";
    buildScriptContent << cmd << "\n";

    m_buildProcess = DoRunSSHProcess(buildScriptContent);
    if(!m_buildProcess) {
        clCommandEvent e(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);
    } else {
        // notify about starting build process.
        // we pass the selected compiler in the event
        clCommandEvent e(wxEVT_SHELL_COMMAND_STARTED);
        e.SetString(conf->GetCompiler());
        EventNotifier::Get()->AddPendingEvent(e);

        // Notify about build process started
        clBuildEvent eventStart(wxEVT_BUILD_STARTED);
        EventNotifier::Get()->AddPendingEvent(eventStart);
    }
}

void RemotyWorkspace::OnBuildProcessTerminated(clProcessEvent& event)
{
    if(event.GetProcess() == m_buildProcess) {
        wxDELETE(m_buildProcess);
        DoPrintBuildMessage(event.GetOutput());

        clCommandEvent e(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);

        // Notify about build process started
        clBuildEvent eventStopped(wxEVT_BUILD_ENDED);
        EventNotifier::Get()->AddPendingEvent(eventStopped);
    }
}

void RemotyWorkspace::OnBuildProcessOutput(clProcessEvent& event)
{
    if(event.GetProcess() == m_buildProcess) {
        DoPrintBuildMessage(event.GetOutput());
    }
}

void RemotyWorkspace::DoPrintBuildMessage(const wxString& message)
{
    clCommandEvent e(wxEVT_SHELL_COMMAND_ADDLINE);
    e.SetString(message);
    EventNotifier::Get()->AddPendingEvent(e);
}

void RemotyWorkspace::OnIsBuildInProgress(clBuildEvent& event)
{
    CHECK_EVENT(event);
    event.SetIsRunning(m_buildProcess != nullptr);
}

void RemotyWorkspace::OnStopBuild(clBuildEvent& event)
{
    CHECK_EVENT(event);
    if(!m_buildProcess) {
        return;
    }
    m_buildProcess->Terminate();
}

void RemotyWorkspace::OnCustomTargetMenu(clContextMenuEvent& event)
{
    CHECK_EVENT(event);
    CHECK_PTR_RET(m_settings.GetSelectedConfig());

    wxMenu* menu = event.GetMenu();
    wxArrayString arrTargets;
    const auto& targets = m_settings.GetSelectedConfig()->GetBuildTargets();

    unordered_map<int, wxString> M;
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
                this->CallAfter(&RemotyWorkspace::DoBuild, iter->second);
            },
            menuId);
    }
}

void RemotyWorkspace::OnBuildHotspotClicked(clBuildEvent& event)
{
    CHECK_EVENT(event);
    const wxString& filename = event.GetFileName();
    int line_number = event.GetLineNumber();

    auto editor = clSFTPManager::Get().OpenFile(filename, m_account.GetAccountName());
    if(editor) {
        editor->CenterLine(line_number);
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
        // Build the workspcae URI and open it
        wxString uri;
        uri << "ssh://" << acc.GetUsername() << "@" << acc.GetHost() << ":" << acc.GetPort() << ":" << remote_path;

        // add this file to the list of recently opened workspaces
        auto recentRemoteWorkspaces = clConfig::Get().Read("remoty/recent_workspaces", wxArrayString());
        if(recentRemoteWorkspaces.Index(uri) == wxNOT_FOUND) {
            recentRemoteWorkspaces.Add(uri);
            clConfig::Get().Write("remoty/recent_workspaces", recentRemoteWorkspaces);
        }
        DoOpen(uri);
    }
}

void RemotyWorkspace::DoOpen(const wxString& workspaceFileURI)
{
    // Close any opened workspace
    auto frame = EventNotifier::Get()->TopFrame();
    wxCommandEvent eventCloseWsp(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    eventCloseWsp.SetEventObject(frame);
    frame->GetEventHandler()->ProcessEvent(eventCloseWsp);

    // close any previously opened workspace
    DoClose(true);

    // parse the remote file path
    wxString remote_path = workspaceFileURI;

    wxString path, scheme, user_name, remote_server, port;
    FileUtils::ParseURI(remote_path, path, scheme, user_name, remote_server, port);

    long nPort = 22;
    port.ToCLong(&nPort);

    // Load the account
    auto accounts = SSHAccountInfo::Load([&](const SSHAccountInfo& acc) -> bool {
        clDEBUG1() << "Checking account:" << acc.GetAccountName() << endl;
        clDEBUG1() << "user_name:" << user_name << "vs" << acc.GetUsername() << endl;
        clDEBUG1() << "remote_server:" << remote_server << "vs" << acc.GetHost() << endl;
        clDEBUG1() << "nPort:" << nPort << "vs" << acc.GetPort() << endl;
        return acc.GetUsername() == user_name && acc.GetPort() == nPort && acc.GetHost() == remote_server;
    });

    if(accounts.empty()) {
        ::wxMessageBox(_("Could not find a matching SSH account to load the workspace!"), "CodeLite",
                       wxICON_ERROR | wxCENTER);
        return;
    }

    wxBusyCursor bc;
    // first: attempt to download the workspace file and store it locally
    const auto& account = accounts[0];
    auto localFile = clSFTPManager::Get().Download(path, account.GetAccountName());
    if(!localFile.IsOk()) {
        ::wxMessageBox(_("Failed to download remote workspace file!\n") + clSFTPManager::Get().GetLastError(),
                       "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }

    wxFileName userSettings(clStandardPaths::Get().GetUserDataDir(), localFile.GetFullName());
    userSettings.AppendDir("Remoty");
    userSettings.AppendDir("LocalWorkspaces");
    userSettings.Mkdir(wxPATH_MKDIR_FULL);
    clDEBUG() << "User workspace file is set:" << userSettings << endl;
    if(!m_settings.Load(localFile, userSettings)) {
        ::wxMessageBox(_("Failed to load workspace file: ") + m_localWorkspaceFile, "CodeLite",
                       wxICON_ERROR | wxCENTER);
        return;
    }

    m_account = accounts[0];
    m_remoteWorkspaceFile = path;
    m_localWorkspaceFile = localFile.GetFullPath();
    m_localUserWorkspaceFile = userSettings.GetFullPath();

    // If the user has .clang-format file, download it as well and place it next to the root download folder
    wxString remoteClangFormatFile = path;
    remoteClangFormatFile = remoteClangFormatFile.BeforeLast('/');
    remoteClangFormatFile << "/.clang-format";

    // Construct the local file path
    wxFileName localClangFormatFile = clSFTP::GetLocalFileName(account, remoteClangFormatFile, true);
    bool hasClangFormatFile = clSFTPManager::Get().IsFileExists(remoteClangFormatFile, account);
    if(hasClangFormatFile) {
        localClangFormatFile = clSFTPManager::Get().Download(remoteClangFormatFile, account.GetAccountName(),
                                                             localClangFormatFile.GetFullPath());
        if(localClangFormatFile.IsOk() && localClangFormatFile.FileExists()) {
            clGetManager()->SetStatusMessage(_("Downloaded .clang-format file"));
        } else {
            ::wxMessageBox(_("Failed to download file: ") + remoteClangFormatFile + "\n" +
                               clSFTPManager::Get().GetLastError(),
                           "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
        }
    }

    path.Replace("\\", "/");
    wxString workspacePath = GetRemoteWorkingDir();
    if(workspacePath.empty()) {
        ::wxMessageBox(_("Invalid empty remote path provided"), "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }
    m_view->OpenWorkspace(workspacePath, m_account.GetAccountName());

    // Notify CodeLite that this workspace is opened
    clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    clWorkspaceManager::Get().SetWorkspace(this);

    // Configure LSP for this workspace
    ConfigureClangd();

    // Notify that the a new workspace is loaded
    wxCommandEvent open_event(wxEVT_WORKSPACE_LOADED);
    EventNotifier::Get()->AddPendingEvent(open_event);
}

void RemotyWorkspace::OnDebugStarting(clDebugEvent& event)
{
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
    DebugSessionInfo startup_info;
    clDebuggerBreakpoint::Vec_t bpList;
    startup_info.debuggerPath = "gdb"; // just assume we have it in the path :)
    startup_info.exeName = exe;
    startup_info.cwd = wd;
    startup_info.cmds = ::wxStringTokenize(dinfo.startupCommands, "\r\n", wxTOKEN_STRTOK);
    clGetManager()->GetBreakpoints(bpList);
    startup_info.bpList = bpList;
    startup_info.isSSHDebugging = true;
    startup_info.sshAccountName = m_account.GetAccountName();

    // open new terminal on the remote host
    m_remote_terminal.reset(new clRemoteTerminal(m_account));
    m_remote_terminal->Start();

    // wait for the tty
    wxBusyCursor bc;
    size_t count = 100; // 10 seconds
    wxString tty;
    while(--count) {
        tty = m_remote_terminal->ReadTty();
        if(tty.empty()) {
            wxMilliSleep(100);
        } else {
            break;
        }
    }
    clDEBUG() << "Using remote tty:" << tty << endl;

    startup_info.ttyName = tty;
    startup_info.enablePrettyPrinting = true;
    dbgr->Start(startup_info);

    // Notify that debug session started
    // this will ensure that the debug layout is loaded
    clDebugEvent eventStarted(wxEVT_DEBUG_STARTED);
    eventStarted.SetClientData(&startup_info);
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
    wd = conf->GetWorkingDirectory().IsEmpty() ? GetFileName().GetPath() : conf->GetWorkingDirectory();
}

void RemotyWorkspace::ConfigureClangd()
{
    // create a detection script on the remote host
    wxString clangd_exe;
    wxString script_output;
    {
        wxString command;
        command << "clangd=$(ls -vr /usr/bin/clangd*|head -1)\n";
        command << "echo $clangd\n";
        IProcess::Ptr_t p(DoRunSSHProcess(command, true));
        p->WaitForTerminate(script_output);
    }

    // parse the output
    clDEBUG() << "ConfigureClangd:" << script_output << endl;
    auto lines = ::wxStringTokenize(script_output, "\r\n", wxTOKEN_STRTOK);
    for(auto& line : lines) {
        line.Trim().Trim(false);
        if(line.StartsWith("/usr/bin/clangd")) {
            clangd_exe.swap(line);
            break;
        }
    }

    if(clangd_exe.empty()) {
        clDEBUG() << "No clangd was found on remote host" << endl;
        return;
    }

    clDEBUG() << "Found clangd:" << clangd_exe << endl;

    // Notify LSP to Configure this LSP for us
    wxArrayString langs;
    langs.Add("c");
    langs.Add("cpp");

    clLanguageServerEvent configure_event(wxEVT_LSP_CONFIGURE);
    configure_event.SetLspName("Remoty - clangd");
    configure_event.SetLanguages(langs);

    // the command: we need to set it to the workspace folder
    wxString clangd_cmd;
    clangd_cmd << "cd " << GetRemoteWorkingDir() << " && " << clangd_exe << " -limit-results=500";
    configure_event.SetLspCommand(clangd_cmd);
    configure_event.SetFlags(clLanguageServerEvent::kEnabled | clLanguageServerEvent::kDisaplyDiags |
                             clLanguageServerEvent::kSSHEnabled);
    configure_event.SetSshAccount(m_account.GetAccountName());
    configure_event.SetConnectionString("stdio");
    configure_event.SetPriority(150);
    EventNotifier::Get()->ProcessEvent(configure_event);

    clLanguageServerEvent restart_event(wxEVT_LSP_RESTART);
    restart_event.SetLspName("Remoty - clangd");
    EventNotifier::Get()->AddPendingEvent(restart_event);
}

IProcess* RemotyWorkspace::DoRunSSHProcess(const wxString& scriptContent, bool sync)
{
    wxString path = UploadScript(scriptContent);
    vector<wxString> args = { "/bin/bash", path };
    size_t flags = IProcessCreateDefault | IProcessCreateSSH;
    if(sync) {
        flags |= IProcessCreateSync;
    }
    return ::CreateAsyncProcess(this, args, flags, wxEmptyString, nullptr, m_account.GetAccountName());
}

wxString RemotyWorkspace::GetRemoteWorkingDir() const
{
    return m_remoteWorkspaceFile.BeforeLast('/');
}

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
    ::WrapWithQuotes(command);
    command << " ";
    auto argsArr = ::wxStringTokenize(conf->GetArgs(), "\n\r", wxTOKEN_STRTOK);
    if(argsArr.size() == 1) {
        command << conf->GetArgs();
    } else {
        // one line per arg
        for(auto& line : argsArr) {
            line.Trim().Trim(false);
            ::WrapWithQuotes(line);
            command << line << " ";
        }
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
    scriptContent << ::WrapWithQuotes(command) << "\n";
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

    const wxString& ext = editor->GetFileName().GetExt();
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

void RemotyWorkspace::DoScanFiles()
{
}
