#include "clRemoteHost.hpp"

#include "clModuleLogger.hpp"

#if USE_SFTP
extern clModuleLogger REMOTE_LOG;

clRemoteHost* clRemoteHost::ms_instance{ nullptr };

clRemoteHost::clRemoteHost()
{
    m_executor.Bind(wxEVT_SHELL_ASYNC_REMOTE_PROCESS_TERMINATED, &clRemoteHost::OnCommandCompleted, this);
}

clRemoteHost::~clRemoteHost()
{
    m_executor.Unbind(wxEVT_SHELL_ASYNC_REMOTE_PROCESS_TERMINATED, &clRemoteHost::OnCommandCompleted, this);
    DrainPendingCommands();
}

void clRemoteHost::DrainPendingCommands()
{
    while(!m_callbacks.empty()) {
        // terminate any pending commands
        m_callbacks.back().second->Close();
        delete m_callbacks.back().second;
        m_callbacks.pop_back();
    }
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
    m_executor.shutdown();

    if(event.IsRemote()) {
        /// Initialise the remote host executor
        m_executor.startup(event.GetRemoteAccount());
    }
}

void clRemoteHost::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    DrainPendingCommands();
    m_executor.shutdown();
}

void clRemoteHost::run_command_with_callback(const std::vector<wxString>& command, const wxString& wd,
                                             const clEnvList_t& env, execute_callback&& cb)
{
    clSSHChannel* channel = m_executor.try_execute(clRemoteExecutor::Cmd::from(command, wd, env));
    if(channel) {
        m_callbacks.emplace_back(std::make_pair(std::move(cb), channel));
    }
}

void clRemoteHost::OnCommandCompleted(clShellProcessEvent& event)
{
    const std::string& output = event.GetStringRaw();
    if(m_callbacks.empty()) {
        LOG_WARNING(REMOTE_LOG) << "no callback found for command output" << endl;
        return;
    }

    // call the callback and consume it from the queue
    m_callbacks.front().first(output);
    delete m_callbacks.front().second;
    m_callbacks.erase(m_callbacks.begin());
}

#endif // USE_SFTP
