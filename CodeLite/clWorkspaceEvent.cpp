#include "clWorkspaceEvent.hpp"

clWorkspaceEvent::clWorkspaceEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clWorkspaceEvent::clWorkspaceEvent(const clWorkspaceEvent& event) { *this = event; }

clWorkspaceEvent& clWorkspaceEvent::operator=(const clWorkspaceEvent& src)
{
    clCommandEvent::operator=(src);
    m_isRemote = src.m_isRemote;
    m_remoteAccount = src.m_remoteAccount;
    m_codeliteRemotePath = src.m_codeliteRemotePath;
    m_workspaceType = src.m_workspaceType;
    return *this;
}

clWorkspaceEvent::~clWorkspaceEvent() {}
