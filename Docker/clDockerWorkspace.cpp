#include "NewDockerWorkspaceDlg.h"
#include "clDockerWorkspace.h"
#include "clDockerWorkspaceView.h"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include "globals.h"
#include "tags_options_data.h"
#include <imanager.h>
#include <wx/msgdlg.h>

clDockerWorkspace::clDockerWorkspace(bool bindEvents)
    : m_bindEvents(bindEvents)
{
    SetWorkspaceType("Docker");
    if(m_bindEvents) {
        EventNotifier::Get()->Bind(wxEVT_CMD_OPEN_WORKSPACE, &clDockerWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &clDockerWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clDockerWorkspace::OnNewWorkspace, this);

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

clDockerWorkspace* clDockerWorkspace::Get()
{
    static clDockerWorkspace workspace(true);
    return &workspace;
}

void clDockerWorkspace::OnOpenWorkspace(clCommandEvent& event)
{
    event.Skip();
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

    // Check if this is a PHP workspace
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
        wxCommandEvent event(wxEVT_WORKSPACE_LOADED);
        event.SetString(m_filename.GetFullPath());
        EventNotifier::Get()->AddPendingEvent(event);

        // and finally, request codelite to keep this workspace in the recently opened workspace list
        clGetManager()->AddWorkspaceToRecentlyUsedList(m_filename);
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

        // Notify workspace closed event
        wxCommandEvent event(wxEVT_WORKSPACE_CLOSED);
        EventNotifier::Get()->ProcessEvent(event);

        // notify codelite to close the currently opened workspace
        wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
        eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

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
        if(dlg.ShowModal() != wxID_OK) return;

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
