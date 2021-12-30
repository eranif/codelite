#ifndef REMOTEWORKSPACE_HPP
#define REMOTEWORKSPACE_HPP

#include "IWorkspace.h" // Base class: IWorkspace
#include "LSP/LSPEvent.h"
#include "clCodeLiteRemoteProcess.hpp"
#include "clFileSystemEvent.h"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clRemoteFinderHelper.hpp"
#include "clRemoteTerminal.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"
#include "wx/event.h"

#include <deque>
#include <functional>
#include <wx/arrstr.h>

#define WORKSPACE_TYPE_NAME "Remote over SSH"

class RemotyWorkspaceView;
class RemotyWorkspace : public IWorkspace
{
private:
    bool m_eventsConnected = false;
    RemotyWorkspaceView* m_view = nullptr;
    SSHAccountInfo m_account;
    wxString m_remoteWorkspaceFile;
    wxString m_localWorkspaceFile;
    wxString m_localUserWorkspaceFile;
    clFileSystemWorkspaceSettings m_settings;
    clCodeLiteRemoteProcess m_codeliteRemoteBuilder;
    clCodeLiteRemoteProcess m_codeliteRemoteFinder;
    long m_execPID = wxNOT_FOUND;
    clRemoteTerminal::ptr_t m_remote_terminal;
    wxArrayString m_workspaceFiles;
    clRemoteFinderHelper m_remoteFinder;
    bool m_buildInProgress = false;
    std::unordered_map<wxString, bool> m_old_servers_state;
    wxArrayString m_installedLSPs;

public:
    RemotyWorkspace();
    RemotyWorkspace(bool dummy);
    virtual ~RemotyWorkspace();

protected:
    void ConfigureLsp(const wxString& output);
    void DoConfigureLSP(const wxString& lsp_name, const wxString& command, const std::vector<wxString>& languages,
                        size_t priority, const wxString& working_directory);

    void BindEvents();
    void UnbindEvents();
    void Initialise();
    void OnOpenWorkspace(clCommandEvent& event);
    void OnCloseWorkspace(clCommandEvent& event);
    void DoClose(bool notify);
    void StartCodeLiteRemote(clCodeLiteRemoteProcess* proc, const wxString& context, bool restart = false);
    void OnOpenResourceFile(clCommandEvent& event);
    void OnShutdown(clCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);
    void OnLSPOpenFile(LSPEvent& event);
    void OnDownloadFile(clCommandEvent& event);

    // keep the LSPs state as it were before we added our remote ones
    // and disable them
    void LSPStoreAndDisableCurrent();
    // restore the LSPs state
    void LSPRestore();

    /// codelite-remote exec handlers
    void DoProcessBuildOutput(const wxString& output, bool is_completed);

    /// open a workspace file
    void DoOpen(const wxString& path, const wxString& account);

    void DeleteLspEntries();
    void OnCodeLiteRemoteTerminated(clCommandEvent& event);

    IProcess* DoRunSSHProcess(const wxString& scriptContent, bool sync = false);
    wxString GetTargetCommand(const wxString& target) const;
    void DoPrintBuildMessage(const wxString& message);
    void GetExecutable(wxString& exe, wxString& args, wxString& wd);

    void OnBuildStarting(clBuildEvent& event);
    void OnIsBuildInProgress(clBuildEvent& event);
    void OnStopBuild(clBuildEvent& event);
    void OnBuildHotspotClicked(clBuildEvent& event);
    void OnCustomTargetMenu(clContextMenuEvent& event);
    void OnNewWorkspace(clCommandEvent& event);
    void OnDebugStarting(clDebugEvent& event);
    void OnDebugEnded(clDebugEvent& event);
    void OnRun(clExecuteEvent& event);
    void OnStop(clExecuteEvent& event);
    void OnIsProgramRunning(clExecuteEvent& event);
    void OnExecProcessTerminated(clProcessEvent& event);
    void OnFindSwapped(clFileSystemEvent& event);
    void OnCodeLiteRemoteBuildOutput(clProcessEvent& event);
    void OnCodeLiteRemoteBuildOutputDone(clProcessEvent& event);

    // codelite-remote
    void OnCodeLiteRemoteFindProgress(clFindInFilesEvent& event);
    void OnCodeLiteRemoteFindDone(clFindInFilesEvent& event);

    void OnCodeLiteRemoteListLSPsOutputDone(clCommandEvent& event);
    void OnCodeLiteRemoteListLSPsOutput(clCommandEvent& event);

    void OnCodeLiteRemoteListFilesProgress(clCommandEvent& event);
    void OnCodeLiteRemoteListFilesDone(clCommandEvent& event);

    wxString GetRemoteWorkingDir() const;
    wxString CreateEnvScriptContent() const;
    wxString UploadScript(const wxString& content, const wxString& script_path = wxEmptyString) const;
    /**
     * @brief scan for remote lsps and configure them
     */
    void ScanForLSPs();

public:
    // IWorkspace
    wxString GetActiveProjectName() const override { return wxEmptyString; }
    wxFileName GetFileName() const override;
    wxString GetFilesMask() const override;
    wxFileName GetProjectFileName(const wxString& projectName) const override;
    void GetProjectFiles(const wxString& projectName, wxArrayString& files) const override;
    wxString GetProjectFromFile(const wxFileName& filename) const override;
    void GetWorkspaceFiles(wxArrayString& files) const override;
    wxArrayString GetWorkspaceProjects() const override;
    bool IsBuildSupported() const override;
    bool IsProjectSupported() const override;
    wxString GetName() const override;
    void SetProjectActive(const wxString& name) override;

    // API
    bool IsOpened() const;
    const SSHAccountInfo& GetAccount() const { return m_account; }
    const wxString& GetLocalWorkspaceFile() const { return m_localWorkspaceFile; }
    const wxString& GetRemoteWorkspaceFile() const { return m_remoteWorkspaceFile; }
    const clFileSystemWorkspaceSettings& GetSettings() const { return m_settings; }
    clFileSystemWorkspaceSettings& GetSettings() { return m_settings; }
    void BuildTarget(const wxString& kind);

    /**
     * @brief save the settings to the remote machine
     */
    void SaveSettings();
    /**
     * @brief refresh the workspace files list (by scanning them on the remote machine)
     */
    void ScanForWorkspaceFiles();

    /**
     * @brief perform find in files
     */
    void FindInFiles(const wxString& root_dir, const wxString& file_extensions, const wxString& find_what,
                     bool whole_word, bool icase);
};

#endif // RemoteWorkspace_HPP
