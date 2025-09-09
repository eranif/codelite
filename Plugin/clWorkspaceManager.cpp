#include "clWorkspaceManager.h"

#include "FileManager.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

#include <algorithm>

clWorkspaceManager::clWorkspaceManager()
    : m_workspace(NULL)
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clWorkspaceManager::OnWorkspaceClosed, this);
}

clWorkspaceManager::~clWorkspaceManager()
{
    for (IWorkspace*& workspace : m_workspaces) {
        wxDELETE(workspace);
    }
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clWorkspaceManager::OnWorkspaceClosed, this);
}

clWorkspaceManager& clWorkspaceManager::Get()
{
    static clWorkspaceManager mgr;
    return mgr;
}

void clWorkspaceManager::RegisterWorkspace(IWorkspace* workspace) { m_workspaces.push_back(workspace); }

void clWorkspaceManager::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    SetWorkspace(NULL);
}

wxArrayString clWorkspaceManager::GetAllWorkspaces() const
{
    wxArrayString all;
    for (IWorkspace* workspace : m_workspaces) {
        all.Add(workspace->GetWorkspaceType());
    }
    return all;
}

wxArrayString clWorkspaceManager::GetUnifiedFilesMask() const
{
    wxArrayString all;
    for (IWorkspace* workspace : m_workspaces) {
        all.Add(workspace->GetFilesMask());
    }
    return all;
}

/// ---------------------------------------------------------------------------
/// ---------------------------------------------------------------------------

IEditor* LocalWorkspaceCommon::OpenFileInEditor(const wxString& filepath, bool createIfMissing)
{
    wxFileName local_file{filepath};
    if (!local_file.FileExists() && !createIfMissing) {
        return nullptr;
    }
    return clGetManager()->CreateOrOpenLocalFile(local_file.GetFullPath());
}

IEditor* LocalWorkspaceCommon::CreateOrOpenSettingFile(const wxString& filename)
{
    return OpenFileInEditor(FileManager::GetSettingFileFullPath(filename), true);
}
