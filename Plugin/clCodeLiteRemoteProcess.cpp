#include "StringUtils.h"
#include "asyncprocess.h"
#include "clCodeLiteRemoteProcess.hpp"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "processreaderthread.h"

clCodeLiteRemoteProcess::clCodeLiteRemoteProcess()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clCodeLiteRemoteProcess::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clCodeLiteRemoteProcess::OnProcessOutput, this);
}

clCodeLiteRemoteProcess::~clCodeLiteRemoteProcess()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clCodeLiteRemoteProcess::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clCodeLiteRemoteProcess::OnProcessOutput, this);
    wxDELETE(m_process);
}

void clCodeLiteRemoteProcess::StartInteractive(const SSHAccountInfo& account, const wxString& scriptPath)
{
    if(m_process) {
        return;
    }

    // wrap the command in ssh
    vector<wxString> command = { "ssh", "-o", "ServerAliveInterval=10", "-o", "StrictHostKeyChecking=no" };
    command.push_back(account.GetUsername() + "@" + account.GetHost());
    command.push_back("-p");
    command.push_back(wxString() << account.GetPort());

    // start the process in interactive mode
    command.push_back("python3 " + scriptPath + " --i");

    // start the process
    m_process = ::CreateAsyncProcess(this, command, IProcessStderrEvent | IProcessCreateDefault);
}

void clCodeLiteRemoteProcess::StartSync(const SSHAccountInfo& account, const wxString& scriptPath,
                                        const std::vector<wxString>& args, wxString& output)
{
    // wrap the command in ssh
    vector<wxString> command = { "ssh", "-o", "ServerAliveInterval=10", "-o", "StrictHostKeyChecking=no" };
    command.push_back(account.GetUsername() + "@" + account.GetHost());
    command.push_back("-p");
    command.push_back(wxString() << account.GetPort());

    // start the process in interactive mode
    wxString script_argv;
    for(const auto& arg : args) {
        script_argv << arg << " ";
    }

    if(script_argv.empty()) {
        script_argv.RemoveLast();
    }
    command.push_back("python3 " + scriptPath + " \"" << script_argv << "\"");

    wxString cmdline;
    for(const wxString& arg : command) {
        cmdline << arg << " ";
    }
    cmdline.RemoveLast();
    clDEBUG() << "Running:" << cmdline << endl;

    // start the process
    IProcess::Ptr_t proc(::CreateSyncProcess(cmdline));
    proc->WaitForTerminate(output);
}

void clCodeLiteRemoteProcess::AsyncCommand(const wxString& json_command, std::function<void(const wxString&)> callback)
{
    // send the command
    if(!m_process || !callback) {
        return;
    }
    m_process->Write(json_command + "\n");
    m_completionCallbacks.push_back(std::move(callback));
}

void clCodeLiteRemoteProcess::OnProcessOutput(clProcessEvent& e)
{
    m_outputRead << e.GetOutput();
    ProcessOutput();
}

void clCodeLiteRemoteProcess::OnProcessTerminated(clProcessEvent& e)
{
    if(m_terminateCallback) {
        m_terminateCallback();
    }
    Cleanup();
}

void clCodeLiteRemoteProcess::Stop()
{
    if(m_process) {
        m_process->Write(wxString("exit\n"));
    }
    wxDELETE(m_process);
    Cleanup();
}

void clCodeLiteRemoteProcess::SetTerminateCallback(std::function<void()> callback)
{
    m_terminateCallback = std::move(callback);
}

void clCodeLiteRemoteProcess::Cleanup()
{
    while(!m_completionCallbacks.empty()) {
        m_completionCallbacks.pop_back();
    }
    wxDELETE(m_process);
    m_terminateCallback = nullptr;
}

void clCodeLiteRemoteProcess::ProcessOutput()
{
    size_t where = m_outputRead.find('\n');
    while(where != wxString::npos) {
        wxString msg = m_outputRead.Mid(0, where);
        m_outputRead.erase(0, where + 1); // remove the consumed line + the "\n"
        if(m_completionCallbacks.empty()) {
            where = m_outputRead.find('\n');
            continue;
        }
        auto cb = std::move(m_completionCallbacks.front());
        cb(msg);
        m_completionCallbacks.pop_front();
        where = m_outputRead.find('\n');
    }
}
