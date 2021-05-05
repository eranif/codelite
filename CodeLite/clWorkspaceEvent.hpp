#ifndef CLWORKSPACEEVENT_HPP
#define CLWORKSPACEEVENT_HPP

#include "cl_command_event.h"

class WXDLLIMPEXP_CL clWorkspaceEvent : public clCommandEvent
{
    bool m_isRemote = false;
    wxString m_remoteAccount;
    wxString m_codeliteRemotePath;
    wxString m_workspaceType;

public:
    clWorkspaceEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clWorkspaceEvent(const clWorkspaceEvent& event);
    clWorkspaceEvent& operator=(const clWorkspaceEvent& src);
    ~clWorkspaceEvent();
    wxEvent* Clone() const override { return new clWorkspaceEvent(*this); }

    // accessors
    void SetIsRemote(bool isRemote) { this->m_isRemote = isRemote; }
    void SetRemoteAccount(const wxString& remoteAccount) { this->m_remoteAccount = remoteAccount; }
    bool IsRemote() const { return m_isRemote; }
    const wxString& GetRemoteAccount() const { return m_remoteAccount; }
    void SetCodeliteRemotePath(const wxString& codeliteRemotePath) { this->m_codeliteRemotePath = codeliteRemotePath; }
    const wxString& GetCodeliteRemotePath() const { return m_codeliteRemotePath; }
    void SetWorkspaceType(const wxString& workspaceType) { this->m_workspaceType = workspaceType; }
    const wxString& GetWorkspaceType() const { return m_workspaceType; }
};
typedef void (wxEvtHandler::*clWorkspaceEventFunction)(clWorkspaceEvent&);
#define clWorkspaceEventHandler(func) wxEVENT_HANDLER_CAST(clWorkspaceEventFunction, func)
#endif // CLWORKSPACEEVENT_HPP
