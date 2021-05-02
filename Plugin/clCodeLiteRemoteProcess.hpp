#ifndef CLCODELITEREMOTEPROCESS_HPP
#define CLCODELITEREMOTEPROCESS_HPP

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "ssh_account_info.h"
#include <deque>
#include <functional>
#include <queue>
#include <vector>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clCodeLiteRemoteProcess : public wxEvtHandler
{
protected:
    typedef void (clCodeLiteRemoteProcess::*CallbackFunc)(const wxString&, bool);

protected:
    IProcess* m_process = nullptr;
    std::deque<CallbackFunc> m_completionCallbacks;
    std::function<void()> m_terminateCallback;
    wxString m_outputRead;
    size_t m_fif_matches_count = 0;
    size_t m_fif_files_scanned = 0;

protected:
    void OnProcessOutput(clProcessEvent& e);
    void OnProcessTerminated(clProcessEvent& e);
    void Cleanup();
    void ProcessOutput();
    bool GetNextBuffer(wxString& output, bool& is_completed);
    void ResetStates();

    // prepare an event from list command output
    void OnListFilesOutput(const wxString& output, bool is_completed);
    void OnFindOutput(const wxString& buffer, bool is_completed);
    void OnExecOutput(const wxString& buffer, bool is_completed);

public:
    clCodeLiteRemoteProcess();
    ~clCodeLiteRemoteProcess();

    void StartInteractive(const SSHAccountInfo& account, const wxString& scriptPath);
    void StartSync(const SSHAccountInfo& account, const wxString& scriptPath, const std::vector<wxString>& args,
                   wxString& output);
    void SetTerminateCallback(std::function<void()> callback);
    void Stop();
    bool IsRunning() const { return m_process != nullptr; }
    // API
    /**
     * @brief find all files on a remote machine from a given directory that matches the extensions list
     */
    void ListFiles(const wxString& root_dir, const wxString& extensions);
    /**
     * @brief find in files on a remote machine
     */
    void Search(const wxString& root_dir, const wxString& extensions, const wxString& find_what, bool whole_word,
                bool icase);
    /**
     * @brief execute a command on the remote machine
     */
    void Exec(const wxArrayString& args, const wxString& working_directory, const clEnvList_t& env);
    /**
     * @brief execute a command on the remote machine
     */
    void Exec(const wxString& cmd, const wxString& working_directory, const clEnvList_t& env);
};
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CODELITE_REMOTE_LIST_FILES, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CODELITE_REMOTE_LIST_FILES_DONE, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CODELITE_REMOTE_FIND_RESULTS, clFindInFilesEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CODELITE_REMOTE_FIND_RESULTS_DONE, clFindInFilesEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CODELITE_REMOTE_EXEC_OUTPUT, clProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CODELITE_REMOTE_EXEC_DONE, clProcessEvent);
#endif // CLCODELITEREMOTEPROCESS_HPP
