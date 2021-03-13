#include "RemoteWorkspace.hpp"
#include "RemotySwitchToWorkspaceDlg.h"
#include "RemotyWorkspaceView.hpp"
#include "clFileSystemWorkspace.hpp"
#include "clSFTPManager.hpp"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "shell_command.h"
#include <wx/msgdlg.h>
#include <wx/uri.h>

#define CHECK_EVENT(e)     \
    {                      \
        if(!IsOpened()) {  \
            e.Skip();      \
            return;        \
        }                  \
        event.Skip(false); \
    }

RemoteWorkspace::RemoteWorkspace()
{
    SetWorkspaceType(WORKSPACE_TYPE_NAME);
    Initialise();
}

RemoteWorkspace::RemoteWorkspace(bool dummy)
{
    wxUnusedVar(dummy);
    SetWorkspaceType(WORKSPACE_TYPE_NAME);
}

RemoteWorkspace::~RemoteWorkspace() { UnbindEvents(); }
wxFileName RemoteWorkspace::GetFileName() const { return {}; }
wxString RemoteWorkspace::GetFilesMask() const { return clFileSystemWorkspace::Get().GetFilesMask(); }

wxFileName RemoteWorkspace::GetProjectFileName(const wxString& projectName) const
{
    wxUnusedVar(projectName);
    return wxFileName();
}

void RemoteWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    wxUnusedVar(projectName);
    wxUnusedVar(files);
}

wxString RemoteWorkspace::GetProjectFromFile(const wxFileName& filename) const { return wxEmptyString; }

void RemoteWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    // TODO :: implement this
    wxUnusedVar(files);
}

wxArrayString RemoteWorkspace::GetWorkspaceProjects() const { return {}; }

bool RemoteWorkspace::IsBuildSupported() const { return true; }

bool RemoteWorkspace::IsProjectSupported() const { return false; }

void RemoteWorkspace::BindEvents()
{
    if(m_eventsConnected) {
        return;
    }

    EventNotifier::Get()->Bind(wxEVT_SWITCHING_TO_WORKSPACE, &RemoteWorkspace::OnOpenWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &RemoteWorkspace::OnCloseWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &RemoteWorkspace::OnBuildStarting, this);
    EventNotifier::Get()->Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &RemoteWorkspace::OnIsBuildInProgress, this);
    EventNotifier::Get()->Bind(wxEVT_STOP_BUILD, &RemoteWorkspace::OnStopBuild, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_CUSTOM_TARGETS_MENU_SHOWING, &RemoteWorkspace::OnCustomTargetMenu, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RemoteWorkspace::OnBuildHotspotClicked, this);

    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &RemoteWorkspace::OnBuildProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &RemoteWorkspace::OnBuildProcessOutput, this);
}

void RemoteWorkspace::UnbindEvents()
{
    if(!m_eventsConnected) {
        return;
    }
    EventNotifier::Get()->Unbind(wxEVT_SWITCHING_TO_WORKSPACE, &RemoteWorkspace::OnOpenWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &RemoteWorkspace::OnCloseWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &RemoteWorkspace::OnBuildStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &RemoteWorkspace::OnIsBuildInProgress, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RemoteWorkspace::OnBuildHotspotClicked, this);

    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &RemoteWorkspace::OnBuildProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &RemoteWorkspace::OnBuildProcessOutput, this);
    m_eventsConnected = false;
}

void RemoteWorkspace::OnOpenWorkspace(clCommandEvent& event)
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

    // Close any opened workspace
    auto frame = EventNotifier::Get()->TopFrame();
    wxCommandEvent eventCloseWsp(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    eventCloseWsp.SetEventObject(frame);
    frame->GetEventHandler()->ProcessEvent(eventCloseWsp);

    // parse the remote file path
    wxString remote_path = dlg.GetPath();

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
    auto localFile = clSFTPManager::Get().Download(path, accounts[0].GetAccountName());
    if(!localFile.IsOk()) {
        ::wxMessageBox(_("Failed to download remote workspace file!"), "CodeLite", wxICON_ERROR | wxCENTER);
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

    path.Replace("\\", "/");
    wxString workspacePath = path.BeforeLast('/');
    if(workspacePath.empty()) {
        ::wxMessageBox(_("Invalid empty remote path provided"), "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }
    m_view->OpenWorkspace(workspacePath, m_account.GetAccountName());

    // Notify CodeLite that this workspace is opened
    clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    clWorkspaceManager::Get().SetWorkspace(this);

    // Notify that the a new workspace is loaded
    wxCommandEvent open_event(wxEVT_WORKSPACE_LOADED);
    EventNotifier::Get()->AddPendingEvent(open_event);
}

void RemoteWorkspace::OnCloseWorkspace(clCommandEvent& event)
{
    event.Skip();
    DoClose(true);
}

void RemoteWorkspace::Initialise()
{
    if(m_eventsConnected) {
        return;
    }
    BindEvents();
    m_view = new RemotyWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook(), this);
    clGetManager()->GetWorkspaceView()->AddPage(m_view, WORKSPACE_TYPE_NAME);
}

bool RemoteWorkspace::IsOpened() const { return !m_account.GetAccountName().empty(); }

void RemoteWorkspace::DoClose(bool notify)
{
    m_view->CloseWorkspace();
    m_settings.Save(m_localWorkspaceFile, m_localUserWorkspaceFile);
    m_settings.Clear();

    m_account = {};
    m_remoteWorkspaceFile.clear();
    m_localWorkspaceFile.clear();
    m_localUserWorkspaceFile.clear();
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

void RemoteWorkspace::SaveSettings()
{
    if(m_remoteWorkspaceFile.empty() || m_localWorkspaceFile.empty() || m_account.GetAccountName().empty()) {
        return;
    }

    wxBusyCursor bc;
    m_settings.Save(m_localWorkspaceFile, m_localUserWorkspaceFile);
    clSFTPManager::Get().SaveFile(m_localWorkspaceFile, m_remoteWorkspaceFile, m_account.GetAccountName());
}

void RemoteWorkspace::OnBuildStarting(clBuildEvent& event)
{
    event.Skip();
    if(IsOpened()) {
        event.Skip(false);
        // before we start the build, save all modified files
        clGetManager()->SaveAll(false);
        DoBuild(event.GetKind());
    }
}

wxString RemoteWorkspace::GetTargetCommand(const wxString& target) const
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

void RemoteWorkspace::DoBuild(const wxString& target)
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

    // Build the environment to use
    const wxString& envstr = conf->GetEnvironment();
    auto envmap = FileUtils::CreateEnvironment(envstr);

    // Creae a build script
    wxString buildScript;
    buildScript << "/tmp/" << m_account.GetUsername() << "_codelite-build.sh";
    wxString buildScriptContent;

    buildScriptContent << "#!/bin/bash -e\n"; // stop at first error
    buildScriptContent << "# prepare the environment variables\n";
    for(auto& vt : envmap) {
        buildScriptContent << "export " << vt.first << "=" << ::WrapWithQuotes(vt.second) << "\n";
    }
    buildScriptContent << "\n";

    wxString working_dir = m_remoteWorkspaceFile.BeforeLast('/');
    buildScriptContent << "cd " << working_dir << "\n";
    buildScriptContent << cmd << "\n";

    // upload the test script
    if(!clSFTPManager::Get().WriteFile(buildScriptContent, buildScript, m_account.GetAccountName())) {
        ::wxMessageBox(_("Failed to write build script to the remote machine!"), "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }

    vector<wxString> args = { "/bin/bash", "-f", buildScript };
    m_buildProcess = ::CreateAsyncProcess(this, args, IProcessCreateDefault | IProcessCreateSSH, wxEmptyString, nullptr,
                                          m_account.GetAccountName());
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

void RemoteWorkspace::OnBuildProcessTerminated(clProcessEvent& event)
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

void RemoteWorkspace::OnBuildProcessOutput(clProcessEvent& event)
{
    if(event.GetProcess() == m_buildProcess) {
        DoPrintBuildMessage(event.GetOutput());
    }
}

void RemoteWorkspace::DoPrintBuildMessage(const wxString& message)
{
    clCommandEvent e(wxEVT_SHELL_COMMAND_ADDLINE);
    e.SetString(message);
    EventNotifier::Get()->AddPendingEvent(e);
}

void RemoteWorkspace::OnIsBuildInProgress(clBuildEvent& event)
{
    CHECK_EVENT(event);
    event.SetIsRunning(m_buildProcess != nullptr);
}

void RemoteWorkspace::OnStopBuild(clBuildEvent& event)
{
    CHECK_EVENT(event);
    if(!m_buildProcess) {
        return;
    }
    m_buildProcess->Terminate();
}

void RemoteWorkspace::OnCustomTargetMenu(clContextMenuEvent& event)
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
                this->CallAfter(&RemoteWorkspace::DoBuild, iter->second);
            },
            menuId);
    }
}

void RemoteWorkspace::OnBuildHotspotClicked(clBuildEvent& event)
{
    CHECK_EVENT(event);
    const wxString& filename = event.GetFileName();
    int line_number = event.GetLineNumber();

    auto editor = clSFTPManager::Get().OpenFile(filename, m_account.GetAccountName());
    if(editor) {
        editor->CenterLine(line_number);
    }
}
