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
    IProcess* m_process = nullptr;
    std::deque<std::function<void(const wxString&)>> m_completionCallbacks;
    std::function<void()> m_terminateCallback;
    wxString m_outputRead;

protected:
    void OnProcessOutput(clProcessEvent& e);
    void OnProcessTerminated(clProcessEvent& e);
    void Cleanup();
    void ProcessOutput();

public:
    clCodeLiteRemoteProcess();
    ~clCodeLiteRemoteProcess();

    void StartInteractive(const SSHAccountInfo& account, const wxString& scriptPath);
    void StartSync(const SSHAccountInfo& account, const wxString& scriptPath, const std::vector<wxString>& args,
                   wxString& output);
    void SetTerminateCallback(std::function<void()> callback);
    void AsyncCommand(const wxString& json_command, std::function<void(const wxString&)> callback);
    void Stop();
    bool IsRunning() const
    {
        return m_process != nullptr;
    }
};

#endif // CLCODELITEREMOTEPROCESS_HPP
