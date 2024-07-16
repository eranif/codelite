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
}

clRemoteHost::clRemoteHost()
{
    m_executor.Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clRemoteHost::OnCommandStdout, this);
    m_executor.Bind(wxEVT_ASYNC_PROCESS_STDERR, &clRemoteHost::OnCommandStderr, this);
    m_executor.Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clRemoteHost::OnCommandCompleted, this);
    Bind(wxEVT_WORKSPACE_LOADED, &clRemoteHost::OnWorkspaceOpened, this);
    Bind(wxEVT_WORKSPACE_CLOSED, &clRemoteHost::OnWorkspaceClosed, this);
}

clRemoteHost::~clRemoteHost()
{
    m_executor.Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clRemoteHost::OnCommandStdout, this);
    m_executor.Unbind(wxEVT_ASYNC_PROCESS_STDERR, &clRemoteHost::OnCommandStderr, this);
    m_executor.Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clRemoteHost::OnCommandCompleted, this);

    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clRemoteHost::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clRemoteHost::OnWorkspaceOpened, this);
    DrainPendingCommands();
}

void clRemoteHost::DrainPendingCommands() { m_callbacks.clear(); }

clRemoteHost* clRemoteHost::Instance()
{
    if (ms_instance == nullptr) {
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
    if (event.IsRemote()) {
        /// Initialise the remote host executor
        m_activeAccount = event.GetRemoteAccount();
    }
}

void clRemoteHost::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    DrainPendingCommands();
    for (auto proc : m_interactiveProcesses) {
        // terminate the bg thread + close the channel
        proc->Terminate();
    }
    m_interactiveProcesses.clear();
    m_sessions.clear();
    m_activeAccount.clear();
}

void clRemoteHost::AddSshSession(clSSH::Ptr_t ssh_session) { m_sessions.push_back(ssh_session); }

clSSH::Ptr_t clRemoteHost::TakeSession()
{
    clSSH::Ptr_t ssh_session;
    if (m_sessions.empty()) {
        ssh_session = CreateSession(m_activeAccount);
    } else {
        ssh_session = m_sessions.back();
        m_sessions.pop_back();
    }
    return ssh_session;
}

clSSH::Ptr_t clRemoteHost::CreateSession(const wxString& account_name)
{
    clSSH::Ptr_t ssh_session;
    // create new session
    auto account = SSHAccountInfo::LoadAccount(account_name);
    if (account.GetHost().empty()) {
        LOG_WARNING(LOG()) << "could not find account:" << account_name << endl;
        return nullptr;
    }

    /// open channel
    try {
        ssh_session.reset(new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(),
                                    account.GetKeyFiles(), account.GetPort()));
        wxString message;

        ssh_session->Open();
        if (!ssh_session->AuthenticateServer(message)) {
            ssh_session->AcceptServerAuthentication();
        }
        ssh_session->Login();
    } catch (clException& e) {
        LOG_ERROR(LOG()) << "Failed to open ssh channel to account:" << account.GetAccountName() << "." << e.What()
                         << endl;
        return nullptr;
    }
    LOG_DEBUG(LOG()) << "Initializing for account:" << account_name << "completed successfully" << endl;
    return ssh_session;
}

void clRemoteHost::run_command_with_callback(const std::vector<wxString>& command, const wxString& wd,
                                             const clEnvList_t& env, execute_callback&& cb)
{
    auto proc = m_executor.try_execute(clRemoteExecutor::Cmd::from(command, wd, env));
    if (proc) {
        m_callbacks.emplace_back(std::make_pair(std::move(cb), proc));
    }
}

void clRemoteHost::run_command_with_callback(const wxString& command, const wxString& wd, const clEnvList_t& env,
                                             execute_callback&& cb)
{
    auto wxargv = StringUtils::BuildArgv(command);
    std::vector<wxString> argv{ wxargv.begin(), wxargv.end() };
    run_command_with_callback(argv, wd, env, std::move(cb));
}

void clRemoteHost::OnCommandStderr(clProcessEvent& event)
{
    const std::string& output = event.GetStringRaw();
    if (m_callbacks.empty()) {
        LOG_WARNING(LOG()) << "no callback found for command output" << endl;
        return;
    }
    LOG_DEBUG(LOG()) << "stderr:" << event.GetStringRaw().size() << "bytes" << endl;
    // call the callback
    m_callbacks.front().first(output, clRemoteCommandStatus::STDERR);
}

void clRemoteHost::OnCommandStdout(clProcessEvent& event)
{
    const std::string& output = event.GetStringRaw();
    if (m_callbacks.empty()) {
        LOG_WARNING(LOG()) << "no callback found for command output" << endl;
        return;
    }
    LOG_DEBUG(LOG()) << "stdout:" << event.GetStringRaw().size() << "bytes" << endl;
    // call the callback
    m_callbacks.front().first(output, clRemoteCommandStatus::STDOUT);
}

void clRemoteHost::OnCommandCompleted(clProcessEvent& event)
{
    if (m_callbacks.empty()) {
        LOG_WARNING(LOG()) << "no callback found for command output" << endl;
        return;
    }

    // call the callback and consume it from the queue
    LOG_DEBUG(LOG()) << "command completed. exit status:" << event.GetInt() << endl;
    m_callbacks.front().first("", event.GetInt() == 0 ? clRemoteCommandStatus::DONE
                                                      : clRemoteCommandStatus::DONE_WITH_ERROR);
    m_callbacks.erase(m_callbacks.begin());
}

IProcess::Ptr_t clRemoteHost::run_interactive_process(wxEvtHandler* parent, const wxArrayString& command, size_t flags,
                                                      const wxString& wd, const clEnvList_t& env)
{
    // create new ssh session
    auto ssh_session = CreateSession(m_activeAccount);
    if (!ssh_session) {
        LOG_ERROR(LOG()) << "no ssh session available" << endl;
        return IProcess::Ptr_t{};
    }

    LOG_DEBUG(LOG()) << "Launching remote process:" << command << endl;
    std::vector<wxString> argv{ command.begin(), command.end() };

    IProcess::Ptr_t proc(
        clSSHInteractiveChannel::Create(parent, ssh_session, argv, flags, wd, env.empty() ? nullptr : &env));
    if (proc) {
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

#endif // USE_SFTP
