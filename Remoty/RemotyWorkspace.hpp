#ifndef REMOTEWORKSPACE_HPP
#define REMOTEWORKSPACE_HPP

#include "IWorkspace.h" // Base class: IWorkspace
#include "JSON.h"
#include "LSP/LSPEvent.h"
#include "asyncprocess.h"
#include "clCodeLiteRemoteProcess.hpp"
#include "clFileSystemEvent.h"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clRemoteFinderHelper.hpp"
#include "clRemoteTerminal.hpp"
#include "clSFTPEvent.h"
#include "cl_command_event.h"
#include "ieditor.h"
#include "ssh_account_info.h"
#include "wx/event.h"

#include <deque>
#include <functional>
#include <wx/arrstr.h>

#define WORKSPACE_TYPE_NAME "Remote over SSH"

class RemotyWorkspaceView;

struct LSPParams {
    wxString lsp_name;
    wxString command;
    std::vector<wxString> languages;
    size_t priority = 80;
    wxString working_directory;
    clEnvList_t env;

    void From(const JSONItem& json);
    bool IsOk() const;
};

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
    wxString m_listLspOutput;

public:
    RemotyWorkspace();
    RemotyWorkspace(bool dummy);
    virtual ~RemotyWorkspace();

protected:
    void BindEvents();
    void UnbindEvents();
    void Initialise();
    void OnOpenWorkspace(clCommandEvent& event);
    void OnReloadWorkspace(clCommandEvent& event);
    void OnCloseWorkspace(clCommandEvent& event);
    void DoClose(bool notify);
    /**
     * @brief restart the remote process. If it is already running and `restart` is set to false
     * do nothing. Otherwise, stop and start it again
     */
    void RestartCodeLiteRemote(clCodeLiteRemoteProcess* proc, const wxString& context, bool restart = false);
    void OnOpenResourceFile(clCommandEvent& event);
    void OnShutdown(clCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);
    void OnLSPOpenFile(LSPEvent& event);
    void OnDownloadFile(clCommandEvent& event);
    void OnStopFindInFiles(clFindInFilesEvent& event);

    void OnSftpSaveError(clCommandEvent& event);
    void OnSftpSaveSuccess(clCommandEvent& event);

    /// codelite-remote exec handlers
    void DoProcessBuildOutput(const wxString& output, bool is_completed);

    /// open a workspace file
    void DoOpen(const wxString& path, const wxString& account);
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

    void OnCodeLiteRemoteListFilesProgress(clCommandEvent& event);
    void OnCodeLiteRemoteListFilesDone(clCommandEvent& event);

    wxString CreateEnvScriptContent() const;
    wxString UploadScript(const wxString& content, const wxString& script_path = wxEmptyString) const;

    void RestoreSession();

public:
    // IWorkspace
    wxString GetActiveProjectName() const override { return wxEmptyString; }
    wxString GetFileName() const override;
    wxString GetDir() const override;
    wxString GetFilesMask() const override;
    wxFileName GetProjectFileName(const wxString& projectName) const override;
    void GetProjectFiles(const wxString& projectName, wxArrayString& files) const override;
    wxString GetProjectFromFile(const wxFileName& filename) const override;
    void GetWorkspaceFiles(wxArrayString& files) const override;
    wxArrayString GetWorkspaceProjects() const override;
    bool IsBuildSupported() const override;
    bool IsProjectSupported() const override;
    wxString GetDebuggerName() const override;
    bool IsRemote() const override { return true; }
    wxString GetSshAccount() const override;

    /**
     * @brief return the remote workspace directory (on the remote machine)
     */
    wxString GetRemoteWorkingDir() const;
    wxString GetName() const override;
    void SetProjectActive(const wxString& name) override;
    /**
     * @brief open workspace with at a given path (remote) and ssh account
     * @param path workspace file path (on the remote machine)
     * @param account ssh account defined in CodeLite
     */
    void OpenWorkspace(const wxString& path, const wxString& account);
    /**
     * @brief Attempt to open a remote file and open it in the editor
     */
    IEditor* OpenFile(const wxString& remote_file_path);

    /**
     * @brief attempt to load and edit codelite-remote.json file for this workspace
     */
    void OpenAndEditCodeLiteRemoteJson();

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
