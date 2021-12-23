#include "clWorkspaceManager.h"

#include "codelite_events.h"
#include "event_notifier.h"

#include <algorithm>

clWorkspaceManager::clWorkspaceManager()
    : m_workspace(NULL)
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clWorkspaceManager::OnWorkspaceClosed, this);
}

clWorkspaceManager::~clWorkspaceManager()
{
    std::for_each(m_workspaces.begin(), m_workspaces.end(), [&](IWorkspace* workspace) { wxDELETE(workspace); });
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
    std::for_each(m_workspaces.begin(), m_workspaces.end(),
                  [&](IWorkspace* workspace) { all.Add(workspace->GetWorkspaceType()); });
    return all;
}

wxArrayString clWorkspaceManager::GetUnifiedFilesMask() const
{
    wxArrayString all;
    std::for_each(m_workspaces.begin(), m_workspaces.end(),
                  [&](IWorkspace* workspace) { all.Add(workspace->GetFilesMask()); });
    return all;
}
