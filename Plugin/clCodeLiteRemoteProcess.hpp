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
#include <wx/event.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clCodeLiteRemoteProcess : public wxEvtHandler
{
protected:
    typedef bool (clCodeLiteRemoteProcess::*CallbackFunc)(const wxString&, clCommandEvent&);

protected:
    IProcess* m_process = nullptr;
    std::deque<CallbackFunc> m_completionCallbacks;
    std::function<void()> m_terminateCallback;
    wxString m_outputRead;

    void OnProcessOutput(clProcessEvent& e);
    void OnProcessTerminated(clProcessEvent& e);
    void Cleanup();
    void ProcessOutput();

    // prepare an event from list command output
    bool OnListFiles(const wxString& output, clCommandEvent& e);

public:
    clCodeLiteRemoteProcess();
    ~clCodeLiteRemoteProcess();

    void StartInteractive(const SSHAccountInfo& account, const wxString& scriptPath);
    void StartSync(const SSHAccountInfo& account, const wxString& scriptPath, const std::vector<wxString>& args,
                   wxString& output);
    void SetTerminateCallback(std::function<void()> callback);
    void Stop();
    bool IsRunning() const
    {
        return m_process != nullptr;
    }
    void ListFiles(const wxString& root_dir, const wxString& extensions);
};
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CODELITE_REMOTE_LIST_FILES, clCommandEvent);
#endif // CLCODELITEREMOTEPROCESS_HPP
