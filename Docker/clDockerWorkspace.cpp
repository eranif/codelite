#include "clDockerWorkspace.h"
#include "NewDockerWorkspaceDlg.h"
#include "clDockerWorkspaceView.h"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "docker.h"
#include "event_notifier.h"
#include "globals.h"
#include "tags_options_data.h"
#include <imanager.h>
#include <wx/msgdlg.h>

#define CHECK_EVENT(e) \
    e.Skip();          \
    if(!IsOpen()) {    \
        return;        \
    }                  \
    e.Skip(false);

clDockerWorkspace::clDockerWorkspace(bool bindEvents, Docker* plugin, clDockerDriver::Ptr_t driver)
    : m_bindEvents(bindEvents)
    , m_driver(driver)
    , m_plugin(plugin)
{
    SetWorkspaceType("Docker");
    if(m_bindEvents) {
        EventNotifier::Get()->Bind(wxEVT_CMD_OPEN_WORKSPACE, &clDockerWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &clDockerWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clDockerWorkspace::OnNewWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_SAVE_SESSION_NEEDED, &clDockerWorkspace::OnSaveSession, this);
        EventNotifier::Get()->Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clDockerWorkspace::OnIsBuildInProgress, this);
        EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &clDockerWorkspace::OnBuildStarting, this);
        EventNotifier::Get()->Bind(wxEVT_STOP_BUILD, &clDockerWorkspace::OnStopBuild, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &clDockerWorkspace::OnRun, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &clDockerWorkspace::OnStop, this);
        m_view = new clDockerWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook());
        clGetManager()->GetWorkspaceView()->AddPage(m_view, GetWorkspaceType());
    }
}

clDockerWorkspace::~clDockerWorkspace()
{
    if(m_bindEvents) {
        EventNotifier::Get()->Unbind(wxEVT_CMD_OPEN_WORKSPACE, &clDockerWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &clDockerWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clDockerWorkspace::OnNewWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_SAVE_SESSION_NEEDED, &clDockerWorkspace::OnSaveSession, this);
        EventNotifier::Get()->Unbind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clDockerWorkspace::OnIsBuildInProgress, this);
        EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &clDockerWorkspace::OnBuildStarting, this);
        EventNotifier::Get()->Unbind(wxEVT_STOP_BUILD, &clDockerWorkspace::OnStopBuild, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &clDockerWorkspace::OnRun, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &clDockerWorkspace::OnStop, this);
    }
}

wxString clDockerWorkspace::GetActiveProjectName() const { return ""; }

wxFileName clDockerWorkspace::GetFileName() const { return m_filename; }

wxString clDockerWorkspace::GetFilesMask() const { return "Dockerfile;docker-compose.yml;*.txt"; }

wxFileName clDockerWorkspace::GetProjectFileName(const wxString& projectName) const { return wxFileName(); }

void clDockerWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    wxUnusedVar(projectName);
}

wxString clDockerWorkspace::GetProjectFromFile(const wxFileName& filename) const
{
    wxUnusedVar(filename);
    return "";
}

void clDockerWorkspace::GetWorkspaceFiles(wxArrayString& files) const { wxUnusedVar(files); }

wxArrayString clDockerWorkspace::GetWorkspaceProjects() const { return wxArrayString(); }

bool clDockerWorkspace::IsBuildSupported() const { return true; }

bool clDockerWorkspace::IsProjectSupported() const { return false; }

static clDockerWorkspace* g_workspace = nullptr;

clDockerWorkspace* clDockerWorkspace::Get() { return g_workspace; }

void clDockerWorkspace::Initialise(Docker* plugin)
{
    if(!g_workspace) {
        g_workspace = new clDockerWorkspace(true, plugin, plugin->GetDriver());
    }
}

void clDockerWorkspace::Shutdown() { wxDELETE(g_workspace); }

void clDockerWorkspace::OnOpenWorkspace(clCommandEvent& event)
{
    event.Skip();

    // Close any opened workspace
    auto frame = EventNotifier::Get()->TopFrame();
    wxCommandEvent eventCloseWsp(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    eventCloseWsp.SetEventObject(frame);
    frame->GetEventHandler()->ProcessEvent(eventCloseWsp);

    // load the current workspace
    wxFileName workspaceFile(event.GetFileName());

    // Test that this is our workspace
    clDockerWorkspaceSettings conf;
    conf.Load(workspaceFile);
    if(!conf.IsOk()) {
        return;
    }

    // This is a Docker workspace, stop event processing by calling
    // event.Skip(false)
    event.Skip(false);

    if(IsOpen()) {
        Close();
    }
    Open(workspaceFile);
}

void clDockerWorkspace::OnCloseWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        Close();
    }
}

void clDockerWorkspace::Open(const wxFileName& path)
{
    m_filename = path;
    m_settings.Load(m_filename);
    m_isOpen = m_settings.Load(m_filename).IsOk();
    if(!IsOpen()) {
        m_filename.Clear();
        m_settings.Clear();
        GetView()->Clear();
    } else {

        //===------------------------------------------
        // Finalize the workspace open process:
        //===------------------------------------------

        // Notify codelite that NodeJS workspace is opened
        clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
        clWorkspaceManager::Get().SetWorkspace(this);

        // Keep the old clang state before we disable it
        const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
        m_clangOldFlag = (options.GetClangOptions() & CC_CLANG_ENABLED);

        clGetManager()->EnableClangCodeCompletion(false);

        // Notify that the a new workspace is loaded
        clWorkspaceEvent open_event(wxEVT_WORKSPACE_LOADED);
        open_event.SetFileName(m_filename.GetFullPath());
        open_event.SetString(m_filename.GetFullPath());
        open_event.SetWorkspaceType(GetWorkspaceType());
        EventNotifier::Get()->AddPendingEvent(open_event);

        // and finally, request codelite to keep this workspace in the recently opened workspace list
        clGetManager()->AddWorkspaceToRecentlyUsedList(m_filename);

        // Load the workspace session (if any)
        CallAfter(&clDockerWorkspace::RestoreSession);
    }
}

void clDockerWorkspace::Close()
{
    if(IsOpen()) {
        // Store the session
        clGetManager()->StoreWorkspaceSession(m_filename);

        // disable clang for NodeJS
        clGetManager()->EnableClangCodeCompletion(m_clangOldFlag);

        // Clear the UI
        GetView()->Clear();

        // notify codelite to close all opened files
        wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
        eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

        // Notify workspace closed event
        clWorkspaceEvent event_workspace_closed(wxEVT_WORKSPACE_CLOSED);
        EventNotifier::Get()->ProcessEvent(event_workspace_closed);

        m_filename.Clear();
        m_settings.Clear();
        m_isOpen = false;
    }
}

bool clDockerWorkspace::IsOpen() const { return m_isOpen; }

void clDockerWorkspace::OnNewWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(event.GetString() == GetWorkspaceType()) {
        event.Skip(false);

        // Create a new NodeJS workspace
        NewDockerWorkspaceDlg dlg(EventNotifier::Get()->TopFrame());
        if(dlg.ShowModal() != wxID_OK)
            return;

        wxFileName workspaceFile = dlg.GetWorkspaceFile();
        if(!workspaceFile.GetDirCount()) {
            ::wxMessageBox(_("Can not create workspace in the root folder"), _("New Workspace"),
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        // Ensure that the path the workspace exists
        workspaceFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        if(!Create(workspaceFile)) {
            ::wxMessageBox(_("Failed to create workspace\nWorkspace already exists"), _("New Workspace"),
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
        Open(workspaceFile);
    }
}

bool clDockerWorkspace::Create(const wxFileName& filename)
{
    // Already exists
    if(filename.FileExists()) {
        return false;
    }
    return m_settings.Save(filename).Load(filename).IsOk();
}

void clDockerWorkspace::RestoreSession()
{
    if(IsOpen()) {
        clGetManager()->LoadWorkspaceSession(m_filename);
    }
}

void clDockerWorkspace::OnSaveSession(clCommandEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        clGetManager()->StoreWorkspaceSession(m_filename);
    }
}

void clDockerWorkspace::OnIsBuildInProgress(clBuildEvent& event)
{
    CHECK_EVENT(event);
    event.SetIsRunning(m_driver->IsRunning());
}

void clDockerWorkspace::OnBuildStarting(clBuildEvent& event)
{
    CHECK_EVENT(event);
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    if(editor->GetFileName().GetFullName() == "Dockerfile") {
        if(event.GetKind() == "build") {
            BuildDockerfile(editor->GetFileName());
        }
    }
}

void clDockerWorkspace::OnStopBuild(clBuildEvent& event)
{
    CHECK_EVENT(event);
    if(m_driver->IsRunning()) {
        m_driver->Stop();
    }
}

void clDockerWorkspace::OnRun(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    if(editor->GetFileName().GetFullName() == "Dockerfile") {
        RunDockerfile(editor->GetFileName());
    }
}

void clDockerWorkspace::OnStop(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    if(m_driver->IsRunning()) {
        m_driver->Stop();
    }
}

void clDockerWorkspace::BuildDockerfile(const wxFileName& dockerfile) { m_driver->Build(dockerfile, m_settings); }

void clDockerWorkspace::RunDockerfile(const wxFileName& dockerfile) { m_driver->Run(dockerfile, m_settings); }

void clDockerWorkspace::BuildDockerCompose(const wxFileName& docker_compose)
{
    m_driver->Build(docker_compose, m_settings);
}

void clDockerWorkspace::RunDockerCompose(const wxFileName& docker_compose)
{
    m_driver->Run(docker_compose, m_settings);
}

void clDockerWorkspace::SetProjectActive(const wxString& project) { wxUnusedVar(project); }
