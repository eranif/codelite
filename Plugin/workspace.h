//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : workspace.h
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
#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "IWorkspace.h"
#include "JSON.h"
#include "codelite_exports.h"
#include "configuration_mapping.h"
#include "localworkspace.h"
#include "optionsconfig.h"
#include "project.h"
#include "singleton.h"
#include "wx/filename.h"
#include "wx/string.h"
#include "wxStringHash.h"

#include <cl_command_event.h>
#include <map>
#include <wx/event.h>
#include <wx/xml/xml.h>

#define CURRENT_WORKSPACE_VERSION 11000
#define CURRENT_WORKSPACE_VERSION_STR wxString("11000")
#define DEFAULT_CURRENT_WORKSPACE_VERSION 10000
#define DEFAULT_CURRENT_WORKSPACE_VERSION_STR wxString("10000")

/*!
 * \brief
 * Workspace manager class
 *
 */
class LocalWorkspace;
class WXDLLIMPEXP_SDK clCxxWorkspace : public IWorkspace
{
    friend class clCxxWorkspaceST;

public:
    void GetProjectFiles(const wxString& projectName, wxArrayString& files) const override;
    void GetWorkspaceFiles(wxArrayString& files) const override;
    wxString GetFilesMask() const override;
    bool IsBuildSupported() const override;
    bool IsProjectSupported() const override;
    wxString GetProjectFromFile(const wxFileName& filename) const override;
    void SetProjectActive(const wxString& project) override;

public:
    typedef std::unordered_map<wxString, ProjectPtr> ProjectMap_t;

protected:
    wxXmlDocument m_doc;
    wxFileName m_fileName;
    ProjectMap_t m_projects;
    wxString m_startupDir;
    time_t m_modifyTime;
    bool m_saveOnExit;
    BuildMatrixPtr m_buildMatrix;
    LocalWorkspace* m_localWorkspace = nullptr;
    wxStringMap_t m_backticks;

public:
    /// Constructor
    clCxxWorkspace();

    /// Destructor
    virtual ~clCxxWorkspace();
    /**
     * @brief return the XML version of this workspace
     */
    wxString GetVersion() const;

    /**
     * @brief return the local workspace settings associated with this workspace
     * @return
     */
    LocalWorkspace* GetLocalWorkspace() const { return m_localWorkspace; }

    // Backtick cache
    bool HasBacktick(const wxString& backtick) const;
    bool GetBacktickValue(const wxString& backtick, wxString& value) const;
    void SetBacktickValue(const wxString& backtick, const wxString& value);
    void ClearBacktickCache();

private:
    void DoUpdateBuildMatrix();
    /**
     * @brief mark all projects as non-active
     */
    void DoUnselectActiveProject();

    /**
     * @brief load projects from the XML file
     */
    void DoLoadProjectsFromXml(wxXmlNode* parentNode, const wxString& folder, std::vector<wxXmlNode*>& removedChildren);

    // return the wxXmlNode instance for the give path
    // the path is separated by "/"
    // return NULL if no such virtual directory exists
    wxXmlNode* DoGetWorkspaceFolderXmlNode(const wxString& path);

    /**
     * @brief return the wxXmlNode for this project
     */
    wxXmlNode* DoGetProjectXmlNode(const wxString& projectName);

    /**
     * @brief create workspace folder and return the wxXmlNode. If this path already exists, return it
     * @param path the path is separated by "/".e.g. "projects/plugins/C++"
     */
    wxXmlNode* DoCreateWorkspaceFolder(const wxString& path);

    bool DoLoadWorkspace(const wxString& fileName, wxString& errMsg);

    /**
     * @brief return list of all projects nodes in this workspace
     */
    std::list<wxXmlNode*> DoGetProjectsXmlNodes() const;

    void DoVisitWorkspaceFolders(wxXmlNode* parent, const wxString& curpath, wxArrayString& paths) const;

    /**
     * @brief user clicked on a build output line with file:line
     */
    void OnBuildHotspotClicked(clBuildEvent& event);

public:
    /**
     * @brief move 'projectName' to folder. Create the folder if it does not exists
     */
    bool MoveProjectToFolder(const wxString& projectName, const wxString& folderPath, bool saveAndReload);

    /**
     * @brief create workspace folder
     * @param path the path is separated by "/".e.g. "projects/plugins/C++"
     */
    bool CreateWorkspaceFolder(const wxString& path);

    /**
     * @brief delete workspace folder. Notice that this will also remove (but not delete) all the projects from the
     * workspace
     */
    void DeleteWorkspaceFolder(const wxString& path);

    /**
     * @brief return list of workspace folders in the form of:
     * a/b/c
     * a/b/d
     * e/f/g
     */
    wxArrayString GetWorkspaceFolders() const;

    /**
     * @brief create 'compile_commands' json object for the workspace projects (only the enabled ones)
     * or compile_flags.txt file
     */
    cJSON* CreateCompileCommandsJSON(bool createCompileFlagsTxt, wxArrayString* generated_paths) const;

    /**
     * @brief generate compile_flags.txt for each project
     */
    void CreateCompileFlags() const;

    wxFileName GetFileName() const override { return GetWorkspaceFileName(); }
    void SetStartupDir(const wxString& startupDir) { this->m_startupDir = startupDir; }
    const wxString& GetStartupDir() const { return m_startupDir; }

    /**
     * @brief return list of compilers used by all the projects (this is done for the active configuration only)
     */
    void GetCompilers(wxStringSet_t& compilers);

    /**
     * @brief replace compilers for all projects and build configurations
     * @param compilers a map of compilers where the key is the old compiler and the value is the new compiler
     */
    void ReplaceCompilers(const wxStringMap_t& compilers);

    /**
     * Returns the workspace file name
     */
    const wxFileName& GetWorkspaceFileName() const { return m_fileName; }

    /**
     * \brief
     * Create a new workspace
     *
     * \param name
     * workspace name
     *
     * \param path
     * workspace path
     *
     * \returns
     * true on success false otherwise
     */
    bool CreateWorkspace(const wxString& name, const wxString& path, wxString& errMsg);

    /**
     * \brief
     * Open an existing workspace
     *
     * \param fileName
     * Workspace file name (including extesion)
     *
     * \returns
     * true on success false otherwise
     */
    bool OpenWorkspace(const wxString& fileName, wxString& errMsg);

    /**
     * @brief this function opens the workspace, but does not open the tags
     * completion database etc. It only loads the XML and then adds all the
     * projects XML. It is mainly needed when you just need to explore the
     * workspace from a secondary thread with no events / UI to get in the
     * way
     */
    bool OpenReadOnly(const wxString& fileName, wxString& errMsg);

    /**
     * Close the currently opened workspace
     */
    void CloseWorkspace();

    /**
     * \brief close the currently open workspace without saving any changes made to it,
     * and reload it again
     */
    void ReloadWorkspace();

    /**
     * \brief
     * Create a new project with given name, path and type
     *
     * \param name
     * project name
     *
     * \param path
     * project path
     *
     * \param type
     * Can be PROJECT_TYPE_EXECUTABLE, PROJECT_TYPE_STATIC_LIBRARY or PROJECT_TYPE_DYNAMIC_LIBRARY
     * \returns
     * true on success false otherwise
     */
    bool CreateProject(const wxString& name, const wxString& path, const wxString& type,
                       const wxString& workspaceFolder, bool addToBuildMatrix, wxString& errMsg);

    /**
     * @brief rename a project
     * @param oldname current name
     * @param newname new name for the proejct
     */
    void RenameProject(const wxString& oldname, const wxString& newname);

    /**
     * \brief get a string property from the worksapce file
     * \returns property value or wxEmptyString
     */
    wxString GetStringProperty(const wxString& propName, wxString& errMsg);

    /**
     * Find a project by name
     * \param projName project name
     * \param errMsg [output] incase an error, report the error to the caller
     * \returns ProjectPtr or NULL if no match was found
     */
    ProjectPtr FindProjectByName(const wxString& projName, wxString& errMsg) const;

    /**
     * Return all project names under this workspace
     */
    void GetProjectList(wxArrayString& list) const;

    /**
     * Add an existing project to the workspace. If no workspace is open,
     * this function does nothing
     * \param path project file path name to add
     * \param errMsg [output] incase an error, report the error to the caller
     */
    bool AddProject(const wxString& path, wxString& errMsg);
    /**
     * @brief add project to the workspace into a given workspace folder
     */
    bool AddProject(const wxString& path, const wxString& workspaceFolder, wxString& errMsg);
    /**
     * Remove project from the workspace. This function does not delete
     * any file related to the project but simply removes it from the workspace
     * \param name project name to remove
     * \param errMsg [output] incase an error, report the error to the caller
     * \return true on success false otherwise
     */
    bool RemoveProject(const wxString& name, wxString& errMsg, const wxString& workspaceFolder);

    /**
     * \return The active project name or wxEmptyString
     */
    wxString GetActiveProjectName() const override;

    /**
     * @brief return the paths of all projects in the workspace (full paths)
     */
    wxArrayString GetAllProjectPaths();

    /**
     * Set project as active
     * \param name  project name
     */
    void SetActiveProject(const wxString& name);

    /**
     * Add new virtual directoy to workspace
     * \param vdFullPath virtual directory full path
     * \param errMsg [output] incase an error, report the error to the caller
     * \return true on success false otherwise
     */
    bool CreateVirtualDirectory(const wxString& vdFullPath, wxString& errMsg, bool mkPath = false);

    /**
     * @brief return true if the given virtual directory already exists
     */
    bool IsVirtualDirectoryExists(const wxString& vdFullPath);

    /**
     * Remove virtual directoy to workspace
     * \param vdFullPath virtual directory full path
     * \param errMsg [output] incase an error, report the error to the caller
     * \return true on success false otherwise
     */
    bool RemoveVirtualDirectory(const wxString& vdFullPath, wxString& errMsg);

    /**
     * Add new file to project. The project name is taken from the virtual directory full path
     * \param vdFullPath vritual directory full path including project
     * \param fileName file name to add
     * \param errMsg output
     * \return true on success, false otherwise
     */
    bool AddNewFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg);

    /**
     * Remove file from a project. The project name is taken from the virtual directory full path
     * \param vdFullPath vritual directory full path including project
     * \param fileName file name to remove
     * \param errMsg output
     * \return true on success, false otherwise
     */
    bool RemoveFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg);

    /**
     * Save workspace & projects settings
     */
    void Save();

    /**
     * Return a node pointing to any workspace-wide editor preferences
     */
    wxXmlNode* GetWorkspaceEditorOptions() const;

    /**
     * Add or update local workspace options
     */
    void SetWorkspaceEditorOptions(LocalOptionsConfigPtr opts);

    /**
     * Return the configuration mapping for the workspace. 'Configuration Mapping' is
     * the build matrix that should be used for this workspace, it contains all possible
     * configurations and which project specific configuration should be built in that
     * configuration
     */
    BuildMatrixPtr GetBuildMatrix() const;

    /**
     * Store configuration mapping for the workspace, overriding the current one
     * \param mapping new mapping to store
     */
    void SetBuildMatrix(BuildMatrixPtr mapping);

    /**
     * Return the workspace name
     */
    wxString GetName() const override;

    /**
     * return the project build configuration that matches the
     * workspace selected configuration from the build matrix
     */
    BuildConfigPtr GetProjBuildConf(const wxString& projectName, const wxString& confName) const;

    /**
     * \brief add project to the workspace build matrix. By default CodeLite will try to match the best project
     * configuration
     * to the workspace configuration (i.e. Debug -> Debug, if no suitable match found, it will use the first one)
     * \param prj
     */
    void AddProjectToBuildMatrix(ProjectPtr prj);

    //----------------------------------
    // Workspace Parser Macros
    //----------------------------------
    /**
     * @brief return the workspace environment variables
     */
    wxString GetParserMacros();

    //----------------------------------
    // Workspace environment variables
    //----------------------------------
    /**
     * @brief return the workspace environment variables
     */
    wxString GetEnvironmentVariabels();

    /**
     * @brief set the workspace environment variables. These environment variables
     * will be kept in the workspace file (i.e. they are portable)
     */
    void SetEnvironmentVariabels(const wxString& envvars);

    /**
     * @brief return the selected workspace configuration
     */
    WorkspaceConfigurationPtr GetSelectedConfig() const;

    //----------------------------------
    // File modifications
    //----------------------------------

    /**
     * return the last modification time (on disk) of editor's underlying file
     */
    time_t GetFileLastModifiedTime() const;

    /**
     * return/set the last modification time that was made by the editor
     */
    time_t GetWorkspaceLastModifiedTime() const { return m_modifyTime; }
    void SetWorkspaceLastModifiedTime(time_t modificationTime) { m_modifyTime = modificationTime; }

    /**
     * @brief return true if a workspace is opened
     */
    bool IsOpen() const;

    /**
     * @brief return the path to the workspace private folder (WORKSPACE_PATH/.codelite)
     * @return the path to the folder, if there is no workspace open, return an empty path
     */
    wxString GetPrivateFolder() const;

    /**
     * @brief return the tags database full path
     */
    wxFileName GetTagsFileName() const;

    /**
     * @brief return project by name
     */
    ProjectPtr GetProject(const wxString& name) const;

    /**
     * @brief return the active project
     */
    ProjectPtr GetActiveProject() const;

    /**
     * @brief clear the workspace include path cache (for each project)
     */
    void ClearIncludePathCache();

    /**
     * @brief return the underlying file for a given project name
     */
    wxFileName GetProjectFileName(const wxString& projectName) const override;

    /**
     * @brief return list of projects for this workspace
     */
    wxArrayString GetWorkspaceProjects() const override;

    /**
     * @brief return list of files that are exluded for a given workspace configuration
     */
    size_t GetExcludeFilesForConfig(std::vector<wxString>& files, const wxString& workspaceConfigName = "");

private:
    /**
     * Do the actual add project
     * \param path project file path
     * \param errMsg [output] incase an error, report the error to the caller
     */
    ProjectPtr DoAddProject(const wxString& path, const wxString& projectVirtualFolder, wxString& errMsg);
    ProjectPtr DoAddProject(ProjectPtr proj);

    void RemoveProjectFromBuildMatrix(ProjectPtr prj);

    bool SaveXmlFile();

    void SyncToLocalWorkspaceSTParserPaths();
    void SyncFromLocalWorkspaceSTParserPaths();
    void SyncToLocalWorkspaceSTParserMacros();
    void SyncFromLocalWorkspaceSTParserMacros();
};

class WXDLLIMPEXP_SDK clCxxWorkspaceST
{
public:
    static clCxxWorkspace* Get();
    static void Free();
};

#endif // WORKSPACE_H
