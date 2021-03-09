#include "RemoteWorkspace.hpp"
#include "clFileSystemWorkspace.hpp"

RemoteWorkspace::RemoteWorkspace()
{
    SetWorkspaceType(WORKSPACE_TYPE_NAME);
    BindEvents();
}

RemoteWorkspace::RemoteWorkspace(bool dummy) { SetWorkspaceType(WORKSPACE_TYPE_NAME); }

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
    m_eventsConnected = true; // TODO : bind CodeLite events here
}

void RemoteWorkspace::UnbindEvents()
{
    if(!m_eventsConnected) {
        return;
    }
    // TODO :: Unbind the events
    m_eventsConnected = false;
}
