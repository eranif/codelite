//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : IWorkspace.h
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

#ifndef IWORKSPACE_H
#define IWORKSPACE_H

#include "asyncprocess.h"

#include <list>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/string.h>

/**
 * @class IWorkspace
 * @brief an interface representing the workspace concept of CodeLite
 * Each plugin that wants to implement a workspace must implement this interface
 */
class IWorkspace : public wxEvtHandler
{
protected:
    wxString m_workspaceType;

public:
    typedef std::list<IWorkspace*> List_t;

public:
    IWorkspace() {}
    virtual ~IWorkspace() {}

    /**
     * @brief return the workspace name
     */
    virtual wxString GetName() const = 0;

    /**
     * @brief return the workspace file full path
     */
    virtual wxString GetFileName() const = 0;

    /**
     * @brief return the workspace directory
     */
    virtual wxString GetDir() const = 0;

    /**
     * @brief set the workspace type
     * For example: "C++ Workspace", "PHP Workspace" etc
     */
    void SetWorkspaceType(const wxString& type) { this->m_workspaceType = type; }

    /**
     * @brief return the workspace name
     */
    const wxString& GetWorkspaceType() const { return m_workspaceType; }

    /**
     * @brief return the currently selected debugger. i.e. if the user hit F7
     * this debugger will be activated
     */
    virtual wxString GetDebuggerName() const = 0;

    /**
     * @brief is this workspace support the build concept?
     * e.g. for C++ workspace, the answer is 'true', for PHP workspace, this will be 'false'
     */
    virtual bool IsBuildSupported() const = 0;

    /**
     * @brief is this workspace support the project concept?
     */
    virtual bool IsProjectSupported() const = 0;

    /**
     * @brief return the file masking for this workspace
     */
    virtual wxString GetFilesMask() const = 0;

    /**
     * @brief return the project name of a file.
     * If the workspace does not support projects, return an empty string
     * If the we could not match a project for the given filename, return empty string
     */
    virtual wxString GetProjectFromFile(const wxFileName& filename) const = 0;

    /**
     * @brief return a list of files for this workspace
     * @param files [output] list of files in absolute path
     */
    virtual void GetWorkspaceFiles(wxArrayString& files) const = 0;
    /**
     * @brief return list of files belonged to the prokect. If the workspace does not support
     * projects, do not modify 'files'
     * @param projectName the project name
     * @param files [output] list of files in absolute path
     */
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const = 0;

    /**
     * @brief return the active project name. For workspace that does not support
     * projects, return an empty string
     */
    virtual wxString GetActiveProjectName() const = 0;

    /**
     * @brief set the active project
     */
    virtual void SetProjectActive(const wxString& project) = 0;

    /**
     * @brief return the underlying file for a given project name
     */
    virtual wxFileName GetProjectFileName(const wxString& projectName) const = 0;

    /**
     * @brief return list of projects for this workspace
     */
    virtual wxArrayString GetWorkspaceProjects() const = 0;

    /**
     * @brief returm true if this workspace is a remote one
     */
    virtual bool IsRemote() const { return false; }
    /**
     * @brief return the ssh account used by this workspace
     */
    virtual wxString GetSshAccount() const { return ""; }

    /**
     * @brief return the environment for the workspace
     */
    virtual clEnvList_t GetEnvironment() const { return {}; }
};

#endif // IWORKSPACE_H
