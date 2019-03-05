//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_workspace.h
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

#ifndef PHPWORKSPACE_H
#define PHPWORKSPACE_H

#include <map>
#include <vector>
#include <set>
#include "phpexecutor.h"
#include "JSON.h"
#include <wx/filename.h>
#include "PHPWrokspaceStorageInterface.h"
#include <macros.h>
#include "php_project_settings_data.h"
#include "php_project.h"
#include <wx/event.h>
#include "XDebugBreakpoint.h"
#include "imanager.h"
#include "IWorkspace.h"

// ------------------------------------------------------------------
// ------------------------------------------------------------------

#define CHECK_PHP_WORKSPACE() \
    if(!PHPWorkspace::Get()->IsOpen()) return
#define CHECK_PHP_WORKSPACE_RET_NULL() \
    if(!PHPWorkspace::Get()->IsOpen()) return NULL
#define CHECK_PHP_WORKSPACE_RET_FALSE() \
    if(!PHPWorkspace::Get()->IsOpen()) return NULL

// ------------------------------------------------------------------
// ------------------------------------------------------------------

class wxProgressDialog;
class PHPWorkspace : public IWorkspace
{
    static PHPWorkspace* ms_instance;

protected:
    wxFileName m_workspaceFile;
    PHPProject::Map_t m_projects;
    PHPExecutor m_executor;
    IManager* m_manager;
    wxStringSet_t m_inSyncProjects;
    wxEvtHandler* m_projectSyncOwner;
    // IWorkspace API
public:
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    virtual wxArrayString GetWorkspaceProjects() const;
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    virtual wxString GetFilesMask() const;
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;

public:
    static PHPWorkspace* Get();
    static void Release();
    void SetPluginManager(IManager* manager) { this->m_manager = manager; }
    JSONItem ToJSON(JSONItem& e) const;
    void FromJSON(const JSONItem& e);

public:
    PHPWorkspace();
    virtual ~PHPWorkspace();

protected:
    void DoNotifyFilesRemoved(const wxArrayString& files);
    void DoPromptWorkspaceModifiedDialog();
    void OnProjectSyncEnd(clCommandEvent& event);

public:
    PHPProject::Ptr_t GetProject(const wxString& project) const;
    PHPProject::Ptr_t GetActiveProject() const;
    wxString GetPrivateFolder() const;

    /**
     * @brief check if we can create a project with the given file name
     * This function checks that the project's path is not already included
     * in any other projects and vise versa. i.e. the new project path does
     * not include any of the other project path
     */
    bool CanCreateProjectAtPath(const wxFileName& projectFileName, bool prompt) const;

    /**
     * @brief sync the workspace with the file system, but do this in a background thread
     */
    void SyncWithFileSystemAsync(wxEvtHandler* owner);

    /**
     * @brief return the project that owns filename
     */
    PHPProject::Ptr_t GetProjectForFile(const wxFileName& filename) const;

    /**
     * @brief restore the session for this workspace
     */
    void RestoreWorkspaceSession();

    /**
     * @brief return the PHPExecutor terminal emulator object
     */
    TerminalEmulator* GetTerminalEmulator();

    /**
     * @brief re-parse the workspace
     */
    void ParseWorkspace(bool full);

    /**
     * @brief return true if project exists
     */
    bool HasProject(const wxString& projectname) const;

    /**
     * @brief close the currently opened workspace
     */
    bool Close(bool saveBeforeClose, bool saveSession);

    /**
     * @brief open a workspace
     * @param view the view that will receive the start/end events
     */
    bool Open(const wxString& filename, wxEvtHandler* view, bool createIfMissing = false);

    /**
     * @brief create an empty workspace
     */
    bool Create(const wxString& filename);

    /**
     * @brief return true if there a workspace is opened
     */
    bool IsOpen() const;
    /**
     * @brief return the currently opened workspace filename
     */
    const wxFileName& GetFilename() const { return m_workspaceFile; }
    /**
     * @brief as defined in IWorkspace
     */
    wxFileName GetFileName() const { return GetFilename(); }

    /**
     * @brief return the workspace name
     */
    wxString GetWorkspaceName() const { return m_workspaceFile.GetName(); }

    /**
     * @brief rename the workspace
     * @param newname
     */
    void Rename(const wxString& newname);

    /////////////////////////////////////
    // Sanity
    /////////////////////////////////////
    /**
     * @brief synonym to HasProject()
     */
    bool IsProjectExists(const wxString& project);

    /////////////////////////////////////
    // Workspace construction
    /////////////////////////////////////
    void CreateProject(const PHPProject::CreateData& createData);
    void DeleteProject(const wxString& project);

    /**
     * @brief add an existing project file to the workspace
     * @param projectFile
     */
    bool AddProject(const wxFileName& projectFile, wxString& errmsg);

    void SetProjectActive(const wxString& project);
    /**
     * @brief delete a file from a project/folder
     */
    void DelFile(const wxString& project, const wxString& filename);

    /**
     * @brief return map with all projects
     */
    const PHPProject::Map_t& GetProjects() const;
    /**
     * @brief return list of files from the entire workspace that matches given filter (Contains())
     * note that files are returned as fullpath
     */
    void GetWorkspaceFiles(wxStringSet_t& workspaceFiles, const wxString& filter) const;
    /**
     * @brief return list of files from the entire workspace
     * note that files are returned as fullpath
     */
    void GetWorkspaceFiles(wxStringSet_t& workspaceFiles, wxProgressDialog* progress = NULL) const;
    /**
     * @brief same as above, but return the files in the form of an array
     */
    void GetWorkspaceFiles(wxArrayString& workspaceFiles, wxProgressDialog* progress) const;
    /**
     * @brief return the active project name
     */
    wxString GetActiveProjectName() const;
    /**
     * @brief save the workspace file
     */
    void Save();

    ////////////////////////////////////////////
    // Project execution
    ////////////////////////////////////////////
    bool RunProject(bool debugging, const wxString& urlOrFilePath, const wxString& projectName = wxEmptyString,
                    const wxString& xdebugSessionName = wxEmptyString);
    bool IsProjectRunning() const { return m_executor.IsRunning(); }
    void StopExecutedProgram() { m_executor.Stop(); }
};

wxDECLARE_EVENT(wxEVT_PHP_WORKSPACE_FILES_SYNC_START, clCommandEvent);
wxDECLARE_EVENT(wxEVT_PHP_WORKSPACE_FILES_SYNC_END, clCommandEvent);
#endif // PHPWORKSPACE_H
