#ifndef NODEJSWORKSPACEUSERCONFIGURATION_H
#define NODEJSWORKSPACEUSERCONFIGURATION_H

#include "NodeJSDebuggerBreakpoint.h"
#include <wx/filename.h>

class NodeJSWorkspaceUser
{
    NodeJSBreakpoint::List_t m_breakpoints;
    wxString m_workspacePath;

protected:
    wxFileName GetFileName() const;

public:
    NodeJSWorkspaceUser(const wxString& workspacePath);
    virtual ~NodeJSWorkspaceUser();

    NodeJSWorkspaceUser& Load();
    NodeJSWorkspaceUser& Save();
    NodeJSWorkspaceUser& SetBreakpoints(const NodeJSBreakpoint::List_t& breakpoints)
    {
        this->m_breakpoints = breakpoints;
        return *this;
    }
    const NodeJSBreakpoint::List_t& GetBreakpoints() const { return m_breakpoints; }
};

#endif // NODEJSWORKSPACEUSERCONFIGURATION_H
