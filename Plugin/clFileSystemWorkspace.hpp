#ifndef CLFILESYSTEMWORKSPACE_HPP
#define CLFILESYSTEMWORKSPACE_HPP

#include "IWorkspace.h"
#include "asyncprocess.h"
#include "clConsoleBase.h"
#include "clDebuggerTerminal.h"
#include "clFileSystemEvent.h"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clRemoteBuilder.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "compiler.h"
#include "macros.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wx/arrstr.h>
#include "clFileCache.hpp"

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

protected:
    void CacheFiles(bool force = false);
    wxString CompileFlagsAsString(const wxArrayString& arr) const;
    wxString GetTargetCommand(const wxString& target) const;
    void DoPrintBuildMessage(const wxString& message);
    clEnvList_t GetEnvList();
    CompilerPtr GetCompiler();

    /**
     * @brief return the executable to run + args
     * this method also expands all macros/env variables
     */
    void GetExecutable(wxString& exe, wxString& args);

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
    void OnParseThreadScanIncludeCompleted(wxCommandEvent& event);
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
    virtual wxString GetActiveProjectName() const;
    virtual wxFileName GetFileName() const;
    virtual wxString GetFilesMask() const;
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    virtual wxArrayString GetWorkspaceProjects() const;
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;

    clFileSystemWorkspace(bool dummy);
    virtual ~clFileSystemWorkspace();

    static clFileSystemWorkspace& Get();

    ///===--------------------------
    /// Specific API
    ///===--------------------------
    clFileSystemWorkspaceView* GetView() { return m_view; }

    /**
     * @brief create an empty workspace at a given folder
     * @param folder
     */
    void New(const wxString& folder);

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

    void UpdateParserPaths();
    const std::vector<wxFileName>& GetFiles() const { return m_files.GetFiles(); }

    const wxString& GetName() const { return m_settings.GetName(); }
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
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FS_SCAN_COMPLETED, clFileSystemEvent);
#endif // CLFILESYSTEMWORKSPACE_HPP
