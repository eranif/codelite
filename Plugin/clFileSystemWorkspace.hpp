#ifndef CLFILESYSTEMWORKSPACE_HPP
#define CLFILESYSTEMWORKSPACE_HPP

#include "codelite_exports.h"
#include "IWorkspace.h"
#include <wx/arrstr.h>
#include <vector>
#include "cl_command_event.h"
#include <unordered_map>
#include "clFileSystemEvent.h"
#include "macros.h"
#include "asyncprocess.h"
#include "clFileSystemWorkspaceConfig.hpp"

class clFileSystemWorkspaceView;
class WXDLLIMPEXP_SDK clFileSystemWorkspace : public IWorkspace
{
    std::vector<wxFileName> m_files;
    wxFileName m_filename;
    bool m_isLoaded = false;
    bool m_showWelcomePage = false;
    bool m_dummy = true;
    IProcess* m_buildProcess = nullptr;
    IProcess* m_execProcess = nullptr;
    clFileSystemWorkspaceSettings m_settings;
    clFileSystemWorkspaceView* m_view = nullptr;
    bool m_initialized = false;
    std::unordered_map<int, wxString> m_buildTargetMenuIdToName;

protected:
    void CacheFiles();
    wxString CompileFlagsAsString(const wxArrayString& arr) const;
    wxString GetTargetCommand(const wxString& target) const;
    void DoPrintBuildMessage(const wxString& message);
    clEnvList_t GetEnvList();

    //===--------------------------
    // Event handlers
    //===--------------------------
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

protected:
    bool Load(const wxFileName& file);
    void DoOpen();
    void DoClose();
    void DoClear();
    void DoCreate(const wxString& name, const wxString& path, bool loadIfExists);
    void RestoreSession();
    void DoBuild(const wxString& target);
    clFileSystemWorkspaceConfig::Ptr_t GetConfig();

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
    const std::vector<wxFileName>& GetFiles() const { return m_files; }

    const wxString& GetName() const { return m_settings.GetName(); }
    void SetName(const wxString& name) { m_settings.SetName(name); }

    const clFileSystemWorkspaceSettings& GetSettings() const { return m_settings; }
    clFileSystemWorkspaceSettings& GetSettings() { return m_settings; }

    /**
     * @brief initialise the workspace (Create GUI etc)
     * This function does nothing if the workspace was already initialised
     */
    void Initialise();
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FS_SCAN_COMPLETED, clFileSystemEvent);
#endif // CLFILESYSTEMWORKSPACE_HPP
