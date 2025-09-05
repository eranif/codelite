#include "clWorkspaceManager.h"

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

IEditor* LocalWorkspaceCommon::CreateOrOpenFile(const wxString& filepath)
{
    return clGetManager()->CreateOrOpenLocalFile(filepath);
}

IEditor* LocalWorkspaceCommon::CreateOrOpenSettingFile(const wxString& filename)
{
    return CreateOrOpenFile(GetSettingFileFullPath(filename));
}

wxString LocalWorkspaceCommon::GetSettingFileFullPath(const wxString& filename) const
{
    wxFileName fullpath{ GetFileName() };
    fullpath.AppendDir(".codelite");
    fullpath.SetFullName(filename);
    return fullpath.GetFullPath();
}

std::optional<wxString> LocalWorkspaceCommon::ReadSettingFile(const wxString& filename) const
{
    wxString fullpath = GetSettingFileFullPath(filename);
    wxString content;
    if (!FileUtils::ReadFileContent(fullpath, content)) {
        return std::nullopt;
    }
    return content;
}