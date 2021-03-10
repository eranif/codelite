#include "RemoteWorkspace.hpp"
#include "RemotySwitchToWorkspaceDlg.h"
#include "RemotyWorkspaceView.hpp"
#include "clFileSystemWorkspace.hpp"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include <wx/msgdlg.h>
#include <wx/uri.h>

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
}

void RemoteWorkspace::UnbindEvents()
{
    if(!m_eventsConnected) {
        return;
    }
    EventNotifier::Get()->Unbind(wxEVT_SWITCHING_TO_WORKSPACE, &RemoteWorkspace::OnOpenWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &RemoteWorkspace::OnCloseWorkspace, this);
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

    // parse the remote file path
    wxString remote_path = dlg.GetPath();
    wxString remote_file_path;

    remote_file_path = remote_path.AfterLast(':');
    remote_path = remote_path.BeforeLast(':');

    wxURI uri(remote_path);
    const wxString& remote_server = uri.GetServer();
    const wxString& user_name = uri.GetUser();
    const wxString& port = uri.GetPort();

    long nPort = 22;
    port.ToCLong(&nPort);

    // Load the account
    auto accounts = SSHAccountInfo::Load([&](const SSHAccountInfo& acc) -> bool {
        return acc.GetUsername() == user_name && acc.GetPort() == nPort && acc.GetHost() == remote_server;
    });

    if(accounts.empty()) {
        ::wxMessageBox(_("Could not find a matching SSH account to load the workspace!"), "CodeLite",
                       wxICON_ERROR | wxCENTER);
        return;
    }

    m_account = accounts[0];
    remote_file_path.Replace("\\", "/");
    wxString workspacePath = remote_file_path.BeforeLast('/');
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
    m_view->CloseWorkspace();
    m_account = {};

    // notify codelite to close all opened files
    wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
    eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

    // Notify workspace closed event
    wxCommandEvent event_closed(wxEVT_WORKSPACE_CLOSED);
    EventNotifier::Get()->ProcessEvent(event_closed);
}

void RemoteWorkspace::Initialise()
{
    if(m_eventsConnected) {
        return;
    }
    BindEvents();
    m_view = new RemotyWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook());
    clGetManager()->GetWorkspaceView()->AddPage(m_view, WORKSPACE_TYPE_NAME);
}

bool RemoteWorkspace::IsOpened() const { return !m_account.GetAccountName().empty(); }
