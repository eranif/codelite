#ifndef PHPWORKSPACE_H
#define PHPWORKSPACE_H

#include <map>
#include <vector>
#include <set>
#include "phpexecutor.h"
#include <json_node.h>
#include <wx/filename.h>
#include "PHPWrokspaceStorageInterface.h"
#include <macros.h>
#include "php_project_settings_data.h"
#include "php_project.h"
#include "php_folder.h"
#include <wx/event.h>
#include "XDebugBreakpoint.h"

// ------------------------------------------------------------------
// ------------------------------------------------------------------

#define CHECK_PHP_WORKSPACE() if ( !PHPWorkspace::Get()->IsOpen() ) return
#define CHECK_PHP_WORKSPACE_RET_NULL() if ( !PHPWorkspace::Get()->IsOpen() ) return NULL
#define CHECK_PHP_WORKSPACE_RET_FALSE() if ( !PHPWorkspace::Get()->IsOpen() ) return NULL

// ------------------------------------------------------------------
// ------------------------------------------------------------------

class PHPWorkspace : public wxEvtHandler
{
    static PHPWorkspace* ms_instance;


protected:
    wxFileName        m_workspaceFile;
    PHPProject::Map_t m_projects;
    PHPExecutor       m_executor;

public:
    static PHPWorkspace* Get();
    static void Release();

    JSONElement ToJSON(JSONElement& e) const;
    void FromJSON(const JSONElement& e);

private:
    PHPWorkspace();
    virtual ~PHPWorkspace();

protected:
    void DoNotifyFilesRemoved(const wxArrayString &files);
    void DoPromptWorkspaceModifiedDialog();
    
public:
    PHPProject::Ptr_t GetProject(const wxString &project) const;
    PHPProject::Ptr_t GetActiveProject() const;
    wxString GetPrivateFolder() const;
    
    /**
     * @brief return true if project exists
     */
    bool HasProject(const wxString &projectname) const;

    /**
     * @brief close the currently opened workspace
     */
    bool Close(bool saveBeforeClose = false);

    /**
     * @brief open a workspace
     */
    bool Open(const wxString &filename, bool createIfMissing = false);

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
    const wxFileName& GetFilename() const {
        return m_workspaceFile;
    }
    /**
     * @brief return the workspace name
     */
    wxString GetWorkspaceName() const {
        return m_workspaceFile.GetName();
    }

    /**
     * @brief rename the workspace
     * @param newname
     */
    void Rename(const wxString &newname);

    /////////////////////////////////////
    // Sanity
    /////////////////////////////////////
    /**
     * @brief synonym to HasProject()
     */
    bool IsProjectExists(const wxString &project);
    bool IsFolderExists(const wxString &project, const wxString &folder);

    /////////////////////////////////////
    // Workspace construction
    /////////////////////////////////////
    void CreateProject   (const wxString &project, bool active);
    void DeleteProject   (const wxString &project);
    void CreateFolder    (const wxString &project, const wxString &folder);
    bool AddFile         (const wxString &project, const wxString &folder, const wxString &file);
    void AddFiles        (const wxString &project, const wxString &folder, const wxArrayString &files, wxArrayString &filesAdded);
    void SetProjectActive(const wxString &project);
    /**
     * @brief delete a file from a project/folder
     */
    bool DelFile(const wxString &project, const wxString &folder, const wxString &filename);

    /**
     * @brief return map with all projects
     */
    const PHPProject::Map_t &GetProjects() const;
    /**
     * @brief return list of files from the entire workspace that matches given filter (Contains())
     * note that files are returned as fullpath
     */
    void GetWorkspaceFiles(wxStringSet_t &workspaceFiles, const wxString &filter) const;
    /**
     * @brief return list of files from the entire workspace
     * note that files are returned as fullpath
     */
    void GetWorkspaceFiles(wxStringSet_t &workspaceFiles) const;
    /**
     * @brief return the active project name
     */
    wxString GetActiveProjectName() const;
    /**
     * @brief return a list of files owned by 'filename' project
     */
    void GetFileProjectFiles(const wxString &filename, wxArrayString &files);

    /**
     * @brief save the workspace file
     */
    void Save();

    ////////////////////////////////////////////
    // Project execution
    ////////////////////////////////////////////
    bool RunProject(bool debugging, const wxString &projectName = wxEmptyString, const wxString &xdebugSessionName = wxEmptyString);
    bool IsProjectRunning() const {
        return m_executor.IsRunning();
    }
    void StopExecutedProgram() {
        m_executor.Stop();
    }
};

#endif // PHPWORKSPACE_H
