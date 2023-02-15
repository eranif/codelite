#include "clRemoteHost.hpp"

#include "StringUtils.h"
#include "clModuleLogger.hpp"
#include "clSSHInteractiveChannel.hpp"
#include "event_notifier.h"

#include <wx/utils.h> // wxBusyCursor

#if USE_SFTP

INITIALISE_SSH_LOG(LOG, "Remote-Host");

namespace
{
clRemoteHost* ms_instance{ nullptr };

class clSSHChannelProcessAdapter : public IProcess
{
    clSSHChannel::Ptr_t m_channel = nullptr;

public:
    clSSHChannelProcessAdapter(wxEvtHandler* parent, clSSHChannel::Ptr_t channel)
        : IProcess(parent)
        , m_channel(channel)
    {
    }
    // Stop notifying the parent window about input/output from the process
    // this is useful when we wish to terminate the process onExit but we don't want
    // to know about its termination
    void Detach() override {}

    /// Read from process stdout - return immediately if no data is available
    /// we return both converted buffer as string and the raw buffer (unconverted)
    bool Read(wxString& buff, wxString& buffErr, std::string& raw_buff, std::string& raw_buffErr) override
    {
        wxUnusedVar(buff);
        wxUnusedVar(buffErr);
        wxUnusedVar(raw_buff);
        wxUnusedVar(raw_buffErr);
        return false;
    }

    // Write to the process stdin
    // This version add LF to the buffer
    bool Write(const wxString& buff) override
    {
        wxUnusedVar(buff);
        return false;
    }

    // ANSI version
    // This version add LF to the buffer
    bool Write(const std::string& buff) override
    {
        wxUnusedVar(buff);
        return false;
    }

    // Write to the process stdin
    bool WriteRaw(const wxString& buff) override
    {
        wxUnusedVar(buff);
        return false;
    }

    // ANSI version
    bool WriteRaw(const std::string& buff) override
    {
        wxUnusedVar(buff);
        return false;
    }

    /**
     * @brief this method is mostly needed on MSW where writing a password
     * is done directly on the console buffer rather than its stdin
     */
    bool WriteToConsole(const wxString& buff) override
    {
        wxUnusedVar(buff);
        return false;
    }

    // Return true if the process is still alive
    bool IsAlive() override { return m_channel && m_channel->IsOpen(); }

    // Clean the process resources and kill the process if it is
    // still alive
    void Cleanup() override {}

    // Terminate the process. It is recommended to use this method
    // so it will invoke the 'Cleaup' procedure and the process
    // termination event will be sent out
    void Terminate() override {}

    void Signal(wxSignal sig) override
    {
        if(m_channel) {
            m_channel->SendSignal(sig);
        }
    }
};
} // namespace

clRemoteHost::clRemoteHost()
{
    Bind(wxEVT_WORKSPACE_LOADED, &clRemoteHost::OnWorkspaceOpened, this);
    Bind(wxEVT_WORKSPACE_CLOSED, &clRemoteHost::OnWorkspaceClosed, this);
}

clRemoteHost::~clRemoteHost()
{
    Unbind(wxEVT_WORKSPACE_CLOSED, &clRemoteHost::OnWorkspaceClosed, this);
    Unbind(wxEVT_WORKSPACE_LOADED, &clRemoteHost::OnWorkspaceOpened, this);
}

clRemoteHost* clRemoteHost::Instance()
{
    if(ms_instance == nullptr) {
        ms_instance = new clRemoteHost{};
    }
    return ms_instance;
}

void clRemoteHost::Release()
{
    delete ms_instance;
    ms_instance = nullptr;
}

void clRemoteHost::OnWorkspaceOpened(clWorkspaceEvent& event)
{
    event.Skip();
    m_activeAccount.clear();
    if(event.IsRemote()) {
        /// Initialise the remote host executor
        m_activeAccount = event.GetRemoteAccount();
    }
}

void clRemoteHost::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    for(auto proc : m_interactiveProcesses) {
        // terminate the bg thread + close the channel
        proc->Terminate();
    }
    /// terminate all currently running commands
    m_executor.shutdown();
    m_interactiveProcesses.clear();
    m_sessions.clear();
    m_activeAccount.clear();
}

void clRemoteHost::AddSshSession(clSSH::Ptr_t ssh_session) { m_sessions.push_back(ssh_session); }

clSSH::Ptr_t clRemoteHost::CreateSshSession()
{
    clSSH::Ptr_t ssh_session;
    wxBusyCursor bc;
    LOG_DEBUG(LOG) << "Creating new ssh session" << endl;
    // create new session
    auto account = SSHAccountInfo::LoadAccount(m_activeAccount);
    if(account.GetHost().empty()) {
        LOG_WARNING(LOG) << "could not find account:" << m_activeAccount << endl;
        return nullptr;
    }

    /// open channel
    try {
        ssh_session.reset(
            new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));
        wxString message;

        ssh_session->Open();
        if(!ssh_session->AuthenticateServer(message)) {
            ssh_session->AcceptServerAuthentication();
        }
        ssh_session->Login();
    } catch(clException& e) {
        LOG_ERROR(LOG) << "Failed to open ssh channel to account:" << account.GetAccountName() << "." << e.What()
                       << endl;
        return nullptr;
    }
    LOG_DEBUG(LOG) << "Initializing for account:" << m_activeAccount << "completed successfully" << endl;
    return ssh_session;
}

clSSH::Ptr_t clRemoteHost::GetSshSession()
{
    clSSH::Ptr_t ssh_session;
    if(m_sessions.empty()) {
        LOG_DEBUG(LOG) << "No ssh session in cache, will create new one" << endl;
        ssh_session = CreateSshSession();

    } else {
        LOG_DEBUG(LOG) << "Re-using ssh session from the cache" << endl;
        ssh_session = m_sessions.back();
        m_sessions.pop_back();
    }
    return ssh_session;
}

void clRemoteHost::run_command_with_callback(const std::vector<wxString>& command, const wxString& wd,
                                             const clEnvList_t& env, execute_callback&& cb)
{
    m_executor.execute_with_callback(clRemoteExecutor::Cmd::from(command, wd, env), m_activeAccount, std::move(cb));
}

void clRemoteHost::run_command_with_callback(const wxString& command, const wxString& wd, const clEnvList_t& env,
                                             execute_callback&& cb)
{
    auto wxargv = StringUtils::BuildArgv(command);
    std::vector<wxString> argv{ wxargv.begin(), wxargv.end() };
    run_command_with_callback(argv, wd, env, std::move(cb));
}

IProcess::Ptr_t clRemoteHost::run_interactive_process(wxEvtHandler* parent, const wxArrayString& command, size_t flags,
                                                      const wxString& wd, const clEnvList_t& env)
{
    // interactive commands are running using their own ssh session
    auto ssh_session = CreateSshSession();

    if(!ssh_session) {
        LOG_ERROR(LOG) << "no ssh session available" << endl;
        return IProcess::Ptr_t{};
    }

    LOG_DEBUG(LOG) << "Launching remote process:" << command << endl;
    std::vector<wxString> argv{ command.begin(), command.end() };

    IProcess::Ptr_t proc(
        clSSHInteractiveChannel::Create(parent, ssh_session, argv, flags, wd, env.empty() ? nullptr : &env));
    if(proc) {
        m_interactiveProcesses.push_back(proc);
    }
    return proc;
}

IProcess::Ptr_t clRemoteHost::run_interactive_process(wxEvtHandler* parent, const wxString& command, size_t flags,
                                                      const wxString& wd, const clEnvList_t& env)
{
    auto wxargv = StringUtils::BuildArgv(command);
    return run_interactive_process(parent, wxargv, flags, wd, env);
}

void clRemoteHost::StopRunningCommands() { m_executor.shutdown(); }

bool clRemoteHost::run_command_sync(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env,
                                    std::string* output)
{
    return m_executor.execute_command(clRemoteExecutor::Cmd::from(command, wd, env), m_activeAccount, output);
}

bool clRemoteHost::run_command_sync(const wxString& command, const wxString& wd, const clEnvList_t& env,
                                    std::string* output)
{
    auto wxargv = StringUtils::BuildArgv(command);
    return run_command_sync(wxargv, wd, env, output);
}

IProcess::Ptr_t clRemoteHost::run_process(wxEvtHandler* parent, const wxString& command, const wxString& wd,
                                          const clEnvList_t& env)
{
    auto wxargv = StringUtils::BuildArgv(command);
    return run_process(parent, wxargv, wd, env);
}

IProcess::Ptr_t clRemoteHost::run_process(wxEvtHandler* parent, const std::vector<wxString>& command,
                                          const wxString& wd, const clEnvList_t& env)
{
    clSSHChannel::Ptr_t channel{ m_executor.execute(clRemoteExecutor::Cmd::from(command, wd, env), m_activeAccount,
                                                    parent) };
    IProcess::Ptr_t adpter{ new clSSHChannelProcessAdapter{ parent, channel } };
    return adpter;
}

#endif // USE_SFTP
