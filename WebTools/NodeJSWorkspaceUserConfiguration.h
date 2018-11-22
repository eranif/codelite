//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSWorkspaceUserConfiguration.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef NODEJSWORKSPACEUSERCONFIGURATION_H
#define NODEJSWORKSPACEUSERCONFIGURATION_H

#include "NodeJSDebuggerBreakpoint.h"
#include <wx/filename.h>
#include <wx/arrstr.h>

class NodeJSWorkspaceUser
{
    NodeJSBreakpoint::Vec_t m_breakpoints;
    wxString m_workspacePath;
    wxString m_scriptToExecute;
    int m_debuggerPort;
    wxString m_debuggerHost;
    wxArrayString m_commandLineArgs;
    wxString m_workingDirectory;

protected:
    wxFileName GetFileName() const;

public:
    NodeJSWorkspaceUser(const wxString& workspacePath);
    virtual ~NodeJSWorkspaceUser();

    NodeJSWorkspaceUser& Load();
    NodeJSWorkspaceUser& Save();
    NodeJSWorkspaceUser& SetBreakpoints(const NodeJSBreakpoint::Vec_t& breakpoints)
    {
        this->m_breakpoints = breakpoints;
        return *this;
    }
    const NodeJSBreakpoint::Vec_t& GetBreakpoints() const { return m_breakpoints; }
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
    NodeJSWorkspaceUser& SetWorkingDirectory(const wxString& workingDirectory)
    {
        this->m_workingDirectory = workingDirectory;
        return *this;
    }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
};

#endif // NODEJSWORKSPACEUSERCONFIGURATION_H
