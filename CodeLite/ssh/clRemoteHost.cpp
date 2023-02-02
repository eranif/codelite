#include "clRemoteHost.hpp"

#include "StringUtils.h"
#include "clModuleLogger.hpp"
#include "event_notifier.h"

#if USE_SFTP
extern clModuleLogger REMOTE_LOG;

namespace
{
clRemoteHost* ms_instance{ nullptr };
}

clRemoteHost::clRemoteHost()
{
    m_executor.Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clRemoteHost::OnCommandStdout, this);
    m_executor.Bind(wxEVT_ASYNC_PROCESS_STDERR, &clRemoteHost::OnCommandStderr, this);
    m_executor.Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clRemoteHost::OnCommandCompleted, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clRemoteHost::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clRemoteHost::OnWorkspaceClosed, this);
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
    if(m_callbacks.empty()) {
        LOG_WARNING(REMOTE_LOG) << "no callback found for command output" << endl;
        return;
    }
    LOG_DEBUG(REMOTE_LOG) << "stderr:" << event.GetStringRaw().size() << "bytes" << endl;
    // call the callback
    m_callbacks.front().first(output, clRemoteCommandStatus::STDERR);
}

void clRemoteHost::OnCommandStdout(clProcessEvent& event)
{
    const std::string& output = event.GetStringRaw();
    if(m_callbacks.empty()) {
        LOG_WARNING(REMOTE_LOG) << "no callback found for command output" << endl;
        return;
    }
    LOG_DEBUG(REMOTE_LOG) << "stdout:" << event.GetStringRaw().size() << "bytes" << endl;
    // call the callback
    m_callbacks.front().first(output, clRemoteCommandStatus::STDOUT);
}

void clRemoteHost::OnCommandCompleted(clProcessEvent& event)
{
    if(m_callbacks.empty()) {
        LOG_WARNING(REMOTE_LOG) << "no callback found for command output" << endl;
        return;
    }

    // call the callback and consume it from the queue
    LOG_DEBUG(REMOTE_LOG) << "command completed. exit status:" << event.GetInt() << endl;
    m_callbacks.front().first("", event.GetInt() == 0 ? clRemoteCommandStatus::DONE
                                                      : clRemoteCommandStatus::DONE_WITH_ERROR);
    delete m_callbacks.front().second;
    m_callbacks.erase(m_callbacks.begin());
}

#endif // USE_SFTP
