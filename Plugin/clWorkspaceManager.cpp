#include "clWorkspaceManager.h"
#include <algorithm>

clWorkspaceManager::clWorkspaceManager()
    : m_workspace(NULL)
{
}

clWorkspaceManager::~clWorkspaceManager()
{
    std::for_each(m_workspaces.begin(), m_workspaces.end(), [&](IWorkspace* workspace) { wxDELETE(workspace); });
}

clWorkspaceManager& clWorkspaceManager::Get()
{
    static clWorkspaceManager mgr;
    return mgr;
}

void clWorkspaceManager::RegisterWorkspace(IWorkspace* workspace) { m_workspaces.push_back(workspace); }
