#include "clSSHInteractiveChannel.hpp"

#include "StringUtils.h"
#include "asyncprocess.h"
#include "file_logger.h"
#include "processreaderthread.h"

#include <thread>
#include <typeinfo>
#include <vector>

#if USE_SFTP
#include <libssh/libssh.h>

#define START_MARKER "START_MARKER"

#include "clModuleLogger.hpp"

INITIALISE_SSH_LOG(LOG, "Interactive-Channel");

#define CHECK_ARG(arg, msg)              \
    if(!(arg)) {                         \
        LOG_WARNING(LOG) << msg << endl; \
        return nullptr;                  \
    }

#define CHECK_ARG_VOID(arg, msg)         \
    if(!(arg)) {                         \
        LOG_WARNING(LOG) << msg << endl; \
        return;                          \
    }

namespace
{
/// commands
struct CmdWrite {
    std::string content;
};

struct CmdShutdown {
};

struct CmdSignal {
    std::string signal_prefix;
};

std::thread* start_helper_thread(SSHChannel_t channel, wxEvtHandler* handler, wxMessageQueue<wxAny>& Q)
{
    LOG_DEBUG(LOG) << "start_helper_thread is called" << endl;
    // our helper thread
    // 1) poll the channel for incoming data
    // 2) poll the `Q` for data to write to the remote process
    std::thread* thr = new std::thread([channel, &Q, handler]() mutable {
        LOG_DEBUG(LOG) << "helper thread started" << endl;
        while(true) {
            // Poll the channel for output
            auto stdout_res = ssh::channel_read(channel, handler, false, true);
            if(stdout_res == ssh::read_result::SSH_SUCCESS) {
                // got something
                continue;
            }

            if(!ssh::result_ok(stdout_res)) {
                // error occured (but not timeout)
                break;
            }

            auto stderrr_res = ssh::channel_read(channel, handler, true, true);
            if(stderrr_res == ssh::read_result::SSH_SUCCESS) {
                // got something
                continue;
            }

            if(!ssh::result_ok(stdout_res)) {
                // error occured (but not timeout)
                break;
            }

            // fall: timeout
            wxAny msg;
            if(Q.ReceiveTimeout(1, msg) == wxMSGQUEUE_NO_ERROR) {
                LOG_DEBUG(LOG) << "got request from the queue" << endl;
                // got a message
                if(msg.CheckType<CmdShutdown>()) {
                    // shutdown, terminate the channel
                    LOG_DEBUG(LOG) << "shutting down" << endl;
                    break;

                } else if(msg.CheckType<CmdWrite>()) {

                    // write buffer to remote
                    CmdWrite write_command;
                    msg.GetAs(&write_command);

                    LOG_DEBUG(LOG) << "writing:" << write_command.content << endl;
                    int rc = ssh_channel_write(channel, write_command.content.c_str(), write_command.content.size());
                    if(rc != write_command.content.size()) {
                        LOG_WARNING(LOG) << "failed to write:" << write_command.content << "to ssh channel" << endl;
                        clCommandEvent event(wxEVT_SSH_CHANNEL_WRITE_ERROR);
                        handler->QueueEvent(event.Clone());
                        break;
                    } else {
                        LOG_DEBUG(LOG) << "successfully written:" << write_command.content << "to ssh channel" << endl;
                    }

                } else if(msg.CheckType<CmdSignal>()) {
                    // send signal
                    // write buffer to remote
                    CmdSignal cmd;
                    msg.GetAs(&cmd);

                    LOG_DEBUG(LOG) << "sending signal:" << cmd.signal_prefix << endl;
                    int rc = ssh_channel_request_send_signal(channel, cmd.signal_prefix.c_str());
                    if(rc != SSH_OK) {
                        LOG_WARNING(LOG) << "failed to send signal:" << cmd.signal_prefix << "to ssh channel" << endl;
                        clCommandEvent event(wxEVT_SSH_CHANNEL_WRITE_ERROR);
                        handler->QueueEvent(event.Clone());
                        break;
                    } else {
                        LOG_DEBUG(LOG) << "successfully sent signal:" << cmd.signal_prefix << "to ssh channel" << endl;
                    }

                } else {
                    LOG_WARNING(LOG) << "received unknown command." << endl;
                }
            }
        }
        LOG_DEBUG(LOG) << "helper thread is going down" << endl;
    });
    return thr;
}
}; // namespace

clSSHInteractiveChannel::Ptr_t clSSHInteractiveChannel::Create(wxEvtHandler* parent, clSSH::Ptr_t ssh,
                                                               const std::vector<wxString>& args, size_t flags,
                                                               const wxString& workingDir, const clEnvList_t* env)
{
    CHECK_ARG(!args.empty(), "cant start remote interactive process. empty command");
    CHECK_ARG(ssh, "cant start remote interactive process with null clSSH");
    CHECK_ARG(parent, "cant start remote interactive process with sink object");

    auto session = ssh->GetSession();
    CHECK_ARG(session, "cant start remote interactive process with null ssh session");

    // Create the remote script before we request a channel
    clEnvList_t envlist;
    if(env) {
        envlist = *env;
    }

    wxString content = ssh::build_script_content(args, workingDir, envlist);
    wxString remote_script = "/tmp/clssh_" + FileUtils::NormaliseFilename(args[0]);
    LOG_DEBUG(LOG) << "executing remote script:" << remote_script << endl;
    auto res = ssh::write_remote_file_content(ssh, remote_script, content);
    if(!res) {
        LOG_WARNING(LOG) << "SSH failed to write remote file." << res.error_message() << endl;
        return nullptr;
    }

    auto channel = ssh_channel_new(session);
    if(!channel) {
        LOG_WARNING(LOG) << "Failed to allocate new ssh channel." << ssh_get_error(session) << endl;
        return nullptr;
    }

    // open the session
    int rc = SSH_OK;
    rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK) {
        LOG_WARNING(LOG) << "Failed to open the session." << ssh_get_error(session) << endl;
        ssh_channel_free(channel);
        return nullptr;
    }

    // request a shell
    rc = ssh_channel_request_shell(channel);
    if(rc != SSH_OK) {
        LOG_WARNING(LOG) << "SSH request shell error." << ssh_get_error(session) << endl;
        ssh_channel_free(channel);
        return nullptr;
    }

    // create the Channel object, start the helper thread
    clSSHInteractiveChannel* process = new clSSHInteractiveChannel(parent, ssh, channel);
    process->m_flags = flags;
    process->m_thread = start_helper_thread(channel, process, process->m_queue);

    // remote servers might often return some prompt messages
    // in order to filter these messages out, we send a echo
    // command and wait for the reponse
    std::string message;
    message.append("echo ").append(START_MARKER).append("\n");
    process->Write(message);

    process->Write(remote_script);
    return IProcess::Ptr_t{ process };
}

clSSHInteractiveChannel::clSSHInteractiveChannel(wxEvtHandler* parent, clSSH::Ptr_t ssh, SSHChannel_t channel)
    : IProcess(parent)
    , m_ssh(ssh)
    , m_channel(channel)
{
    Bind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHInteractiveChannel::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHInteractiveChannel::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHInteractiveChannel::OnChannelStdout, this);
    Bind(wxEVT_SSH_CHANNEL_READ_STDERR, &clSSHInteractiveChannel::OnChannelStderr, this);
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHInteractiveChannel::OnChannelClosed, this);
}

clSSHInteractiveChannel::~clSSHInteractiveChannel()
{
    LOG_DEBUG(LOG) << "Unbinding events" << endl;
    Unbind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHInteractiveChannel::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHInteractiveChannel::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHInteractiveChannel::OnChannelStdout, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_STDERR, &clSSHInteractiveChannel::OnChannelStderr, this);
    Unbind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHInteractiveChannel::OnChannelClosed, this);
    Cleanup();
}

void clSSHInteractiveChannel::Detach() { StopThread(); }

bool clSSHInteractiveChannel::IsAlive() { return m_channel != nullptr; }
void clSSHInteractiveChannel::Cleanup() { Terminate(); }

void clSSHInteractiveChannel::StopThread()
{
    if(m_thread) {
        // we just stop the reader thread
        CmdShutdown shutdown;
        m_queue.Post(shutdown);

        m_thread->join();
        wxDELETE(m_thread);

        // notify about this (will only be done once)
        clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
        event.SetInt(0);
        ProcessEvent(event);
    }
}

void clSSHInteractiveChannel::Terminate()
{
    // stop the reader thread
    StopThread();

    if(m_channel) {
        ssh_channel_close(m_channel);
        ssh_channel_free(m_channel);
    }

    m_channel = nullptr;
    m_queue.Clear();
    m_closeEventFired = false;
    m_waitingBuffer.clear();
    m_waiting = true;
    m_ssh = nullptr;
}

bool clSSHInteractiveChannel::Read(wxString& buff, wxString& buffErr, std::string& raw_buff, std::string& raw_buffErr)
{
    wxUnusedVar(buff);
    wxUnusedVar(buffErr);
    wxUnusedVar(raw_buff);
    wxUnusedVar(raw_buffErr);
    return false;
}

bool clSSHInteractiveChannel::Write(const std::string& buff)
{
    if(!m_thread) {
        return false;
    }
    CmdWrite cmd;
    cmd.content = buff + "\n";
    m_queue.Post(cmd);
    return true;
}

bool clSSHInteractiveChannel::Write(const wxString& buff)
{
    std::string str = StringUtils::ToStdString(buff);
    return Write(str);
}

bool clSSHInteractiveChannel::WriteRaw(const wxString& buff)
{
    std::string str = StringUtils::ToStdString(buff);
    return WriteRaw(str);
}

bool clSSHInteractiveChannel::WriteRaw(const std::string& buff)
{
    if(!m_thread) {
        return false;
    }
    CmdWrite cmd;
    cmd.content = buff;
    m_queue.Post(cmd);
    return true;
}

void clSSHInteractiveChannel::WaitForTerminate(wxString& output)
{
    LOG_ERROR(LOG) << "WaitForTerminate is not supported for interactive shell commands" << endl;
}

bool clSSHInteractiveChannel::WriteToConsole(const wxString& buff) { return Write(buff); }

void clSSHInteractiveChannel::Signal(wxSignal sig)
{
    CHECK_ARG_VOID(m_ssh, "Signal failed. null ssh session");
    CHECK_ARG_VOID(m_channel, "Signal failed. null ssh session");

    const char* prefix = nullptr;
    switch(sig) {
    case wxSIGABRT:
        prefix = "ABRT";
        break;
    case wxSIGALRM:
        prefix = "ALRM";
        break;
    case wxSIGFPE:
        prefix = "FPE";
        break;
    case wxSIGHUP:
        prefix = "HUP";
        break;
    case wxSIGILL:
        prefix = "ILL";
        break;
    case wxSIGINT:
        prefix = "INT";
        break;
    case wxSIGKILL:
        prefix = "KILL";
        break;
    case wxSIGPIPE:
        prefix = "PIPE";
        break;
    case wxSIGQUIT:
        prefix = "QUIT";
        break;
    case wxSIGSEGV:
        prefix = "SEGV";
        break;
    case wxSIGTERM:
        prefix = "TERM";
        break;
    default:
        break;
    }
    if(!prefix) {
        LOG_ERROR(LOG) << "unknown signal" << endl;
        return;
    }

    CmdSignal signal{ prefix };
    m_queue.Post(signal);
}

void clSSHInteractiveChannel::ResumeAsyncReads()
{
    LOG_ERROR(LOG) << "ResumeAsyncReads is not supported for interactive shell commands" << endl;
}

void clSSHInteractiveChannel::SuspendAsyncReads()
{
    LOG_ERROR(LOG) << "SuspendAsyncReads is not supported for interactive shell commands" << endl;
}

void clSSHInteractiveChannel::OnChannelStdout(clCommandEvent& event)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG) << event.GetStringRaw() << endl; }

    if(!m_waiting) {
        LOG_DEBUG(LOG) << "sending wxEVT_ASYNC_PROCESS_OUTPUT event" << endl;
        clProcessEvent event_stdout{ wxEVT_ASYNC_PROCESS_OUTPUT };
        event_stdout.SetProcess(nullptr);
        event_stdout.SetOutputRaw(event.GetStringRaw());
        event_stdout.SetOutput(event.GetStringRaw());
        AddPendingEvent(event_stdout);
    } else {
        std::string marker;
        marker.append(START_MARKER).append("\n");
        m_waitingBuffer.append(event.GetStringRaw());

        std::string::size_type where = m_waitingBuffer.find(marker);
        if(where != std::string::npos) {
            m_waiting = false;

            // found the marker
            LOG_DEBUG(LOG) << "found the marker" << endl;

            // remove the marker from the input string
            wxString remainder = m_waitingBuffer.substr(where + marker.length());
            // fire an event with the remainder
            if(!remainder.empty()) {
                clProcessEvent event_stdout{ wxEVT_ASYNC_PROCESS_OUTPUT };
                event_stdout.SetProcess(nullptr);
                event_stdout.SetOutputRaw(StringUtils::ToStdString(remainder));
                event_stdout.SetOutput(remainder);
                AddPendingEvent(event_stdout);
                LOG_DEBUG(LOG) << "stdout (active): `" << remainder << "`" << endl;
            }
            m_waitingBuffer.clear();
        } else {
            LOG_DEBUG(LOG) << "stdout (waiting): `" << event.GetStringRaw() << "`" << endl;
        }
    }
}

void clSSHInteractiveChannel::OnChannelStderr(clCommandEvent& event)
{
    if(!m_waiting) {
        clProcessEvent event_stdout{ m_flags & IProcessStderrEvent ? wxEVT_ASYNC_PROCESS_STDERR
                                                                   : wxEVT_ASYNC_PROCESS_OUTPUT };
        event_stdout.SetProcess(nullptr);
        event_stdout.SetOutputRaw(event.GetStringRaw());
        event_stdout.SetOutput(event.GetStringRaw());
        AddPendingEvent(event_stdout);
        LOG_DEBUG(LOG) << "stderr (active): `" << event.GetStringRaw() << "`" << endl;
    } else {
        LOG_DEBUG(LOG) << "stderr (waiting): `" << event.GetStringRaw() << "`" << endl;
    }
}

void clSSHInteractiveChannel::OnChannelClosed(clCommandEvent& event)
{
    if(!m_closeEventFired) {
        clProcessEvent event_terminated{ wxEVT_ASYNC_PROCESS_TERMINATED };
        event_terminated.SetProcess(nullptr);
        AddPendingEvent(event_terminated);
        LOG_DEBUG(LOG) << "channel closed" << endl;
        m_closeEventFired = true;
    }
}

void clSSHInteractiveChannel::OnChannelError(clCommandEvent& event)
{
    LOG_DEBUG(LOG) << "channel error." << ssh_get_error(m_ssh->GetSession()) << endl;
    clProcessEvent event_terminated{ wxEVT_ASYNC_PROCESS_TERMINATED };
    event_terminated.SetProcess(nullptr);
    AddPendingEvent(event_terminated);
}
#endif // USE_SFTP
