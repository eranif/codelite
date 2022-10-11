#ifndef CLFILESYSTEMWORKSPACE_HPP
#define CLFILESYSTEMWORKSPACE_HPP

#include "IWorkspace.h"
#include "asyncprocess.h"
#include "clBacktickCache.hpp"
#include "clConsoleBase.h"
#include "clDebuggerTerminal.h"
#include "clFileCache.hpp"
#include "clFileSystemEvent.h"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clRemoteBuilder.hpp"
#include "clShellHelper.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "compiler.h"
#include "macros.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wx/arrstr.h>

class clFileSystemWorkspaceView;

class WXDLLIMPEXP_SDK clFileSystemWorkspace : public IWorkspace
{
    clFileCache m_files;
    wxFileName m_filename;
    bool m_isLoaded = false;
    bool m_showWelcomePage = false;
    bool m_dummy = true;
    IProcess* m_buildProcess = nullptr;
    clFileSystemWorkspaceSettings m_settings;
    clFileSystemWorkspaceView* m_view = nullptr;
    bool m_initialized = false;
    std::unordered_map<int, wxString> m_buildTargetMenuIdToName;
    clRemoteBuilder::Ptr_t m_remoteBuilder;
    clDebuggerTerminalPOSIX m_debuggerTerminal;
    int m_execPID = wxNOT_FOUND;
    clBacktickCache::ptr_t m_backtickCache;
    clShellHelper m_shell_helper;

protected:
    void CacheFiles(bool force = false);
    wxString CompileFlagsAsString(const wxArrayString& arr) const;
    wxString GetTargetCommand(const wxString& target) const;
    void DoPrintBuildMessage(const wxString& message);
    clEnvList_t GetEnvList();
    CompilerPtr GetCompiler();

    //===--------------------------
    // Event handlers
    //===--------------------------
    void OnExecProcessTerminated(clProcessEvent& event);
    void OnBuildStarting(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnIsBuildInProgress(clBuildEvent& event);
    void OnStopBuild(clBuildEvent& event);
    void OnExecute(clExecuteEvent& event);
    void OnStopExecute(clExecuteEvent& event);
    void OnIsProgramRunning(clExecuteEvent& event);
    void OnNewWorkspace(clCommandEvent& event);
    void OnOpenWorkspace(clCommandEvent& event);
    void OnCloseWorkspace(clCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnScanCompleted(clFileSystemEvent& event);
    void OnParseWorkspace(wxCommandEvent& event);
    void OnBuildProcessTerminated(clProcessEvent& event);
    void OnBuildProcessOutput(clProcessEvent& event);
    void OnSaveSession(clCommandEvent& event);
    void OnQuickDebugDlgShowing(clDebugEvent& event);
    void OnQuickDebugDlgDismissed(clDebugEvent& event);
    void OnCustomTargetMenu(clContextMenuEvent& event);
    void OnMenuCustomTarget(wxCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnSourceControlPulled(clSourceControlEvent& event);
    void OnDebug(clDebugEvent& event);
    void OnFileSystemUpdated(clFileSystemEvent& event);
    void OnReloadWorkspace(clCommandEvent& event);

protected:
    bool Load(const wxFileName& file);
    void DoOpen();
    void DoClose();
    void DoClear();
    void DoCreate(const wxString& name, const wxString& path, bool loadIfExists);
    void RestoreSession();
    void DoBuild(const wxString& target);
    void TriggerQuickParse();
    clFileSystemWorkspaceConfig::Ptr_t GetConfig() const;

public:
    ///===--------------------------
    /// IWorkspace interface
    ///===--------------------------
    wxString GetActiveProjectName() const override;
    wxString GetFileName() const override;
    wxString GetDir() const override;
    wxString GetFilesMask() const override;
    void GetProjectFiles(const wxString& projectName, wxArrayString& files) const override;
    wxString GetProjectFromFile(const wxFileName& filename) const override;
    void GetWorkspaceFiles(wxArrayString& files) const override;
    wxArrayString GetWorkspaceProjects() const override;
    bool IsBuildSupported() const override;
    bool IsProjectSupported() const override;
    wxFileName GetProjectFileName(const wxString& projectName) const override;
    void SetProjectActive(const wxString& project) override;
    wxString GetDebuggerName() const override;
    clEnvList_t GetEnvironment() const override;

    /**
     * @brief return the executable to run + args + working directory
     * this method also expands all macros/env variables
     */
    void GetExecutable(wxString& exe, wxString& args, wxString& wd);

    virtual wxString GetExcludeFolders() const;
    clFileSystemWorkspace(bool dummy);
    virtual ~clFileSystemWorkspace();

    static clFileSystemWorkspace& Get();

    /**
     * @brief open a workspace file. return true if the `filepath` is a valid
     * File System Workspace and the open succeeded
     */
    bool OpenWorkspace(const wxString& filepath);

    /**
     * @brief close the workspace. Return false if the a File System Workspace
     * was not already opened
     */
    bool CloseWorkspace();

    ///===--------------------------
    /// Specific API
    ///===--------------------------
    clFileSystemWorkspaceView* GetView() { return m_view; }

    /**
     * @brief return the backticks cache used by this workspace
     */
    clBacktickCache::ptr_t GetBackticksCache() { return m_backtickCache; }

    /**
     * @brief create an empty workspace at a given folder
     * @param folder
     */
    void New(const wxString& folder, const wxString& name = wxEmptyString);

    /**
     * @brief close the workspace
     */
    void Close();

    /**
     * @brief save the workspace settings
     */
    void Save(bool parse);

    /**
     * @brief parse the workspace
     */
    void Parse(bool fullParse);

    /**
     * @brief is this workspace opened?
     */
    bool IsOpen() const { return m_isLoaded; }

    const std::vector<wxFileName>& GetFiles() const { return m_files.GetFiles(); }

    wxString GetName() const override { return m_filename.GetName(); }
    void SetName(const wxString& name) { m_settings.SetName(name); }

    const clFileSystemWorkspaceSettings& GetSettings() const { return m_settings; }
    clFileSystemWorkspaceSettings& GetSettings() { return m_settings; }

    /**
     * @brief initialise the workspace (Create GUI etc)
     * This function does nothing if the workspace was already initialised
     */
    void Initialise();

    /**
     * @brief call this to update the workspace once a file system changes.
     * this method will re-cache the files + parse the workspace
     * Note that this method does NOT update the UI in anyways.
     */
    void FileSystemUpdated();

    /**
     * @brief create compile_flags.txt for the selected configuration
     * and fire generation event
     */
    void CreateCompileFlagsFile();
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FS_SCAN_COMPLETED, clFileSystemEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FS_NEW_WORKSPACE_FILE_CREATED, clFileSystemEvent);
#endif // CLFILESYSTEMWORKSPACE_HPP
