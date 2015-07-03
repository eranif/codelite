#ifndef NODEJSWORKSPACEUSERCONFIGURATION_H
#define NODEJSWORKSPACEUSERCONFIGURATION_H

#include "NodeJSDebuggerBreakpoint.h"
#include <wx/filename.h>
#include <wx/arrstr.h>

class NodeJSWorkspaceUser
{
    NodeJSBreakpoint::List_t m_breakpoints;
    wxString m_workspacePath;
    wxString m_scriptToExecute;
    int m_debuggerPort;
    wxString m_debuggerHost;
    wxArrayString m_commandLineArgs;

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
    NodeJSWorkspaceUser& SetDebuggerHost(const wxString& debuggerHost)
    {
        this->m_debuggerHost = debuggerHost;
        return *this;
    }
    NodeJSWorkspaceUser& SetDebuggerPort(int debuggerPort)
    {
        this->m_debuggerPort = debuggerPort;
        return *this;
    }
    NodeJSWorkspaceUser& SetScriptToExecute(const wxString& scriptToExecute)
    {
        this->m_scriptToExecute = scriptToExecute;
        return *this;
    }
    NodeJSWorkspaceUser& SetWorkspacePath(const wxString& workspacePath)
    {
        this->m_workspacePath = workspacePath;
        return *this;
    }
    NodeJSWorkspaceUser& SetCommandLineArgs(const wxArrayString& commandLineArgs)
    {
        this->m_commandLineArgs = commandLineArgs;
        return *this;
    }
    const wxArrayString& GetCommandLineArgs() const { return m_commandLineArgs; }
    const wxString& GetDebuggerHost() const { return m_debuggerHost; }
    int GetDebuggerPort() const { return m_debuggerPort; }
    const wxString& GetScriptToExecute() const { return m_scriptToExecute; }
    const wxString& GetWorkspacePath() const { return m_workspacePath; }
};

#endif // NODEJSWORKSPACEUSERCONFIGURATION_H
