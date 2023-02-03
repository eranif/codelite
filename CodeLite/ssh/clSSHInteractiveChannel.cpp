#include "clSSHInteractiveChannel.hpp"

#include "file_logger.h"

#include <thread>
#include <typeinfo>
#include <vector>

#if USE_SFTP
#include <libssh/libssh.h>

#define CHECK_ARG(arg, msg)         \
    if(!(arg)) {                    \
        clWARNING() << msg << endl; \
        return nullptr;             \
    }

#include "clModuleLogger.hpp"
INITIALISE_SSH_LOG(LOG, "Interactive Channel");

namespace
{
struct CmdWrite {
    std::string content;
};

struct CmdShutdown {
};

thread_local CmdWrite CMD_STUB_WRITE;
thread_local CmdShutdown CMD_STUB_SHUTDOWN;

void start_helper_thread(SSHChannel_t channel, wxEvtHandler* handler, wxMessageQueue<std::any>& Q)
{
    // our helper thread
    // 1) poll the channel for incoming data
    // 2) poll the `Q` for data to write to the remote process
    std::thread thr([channel, &Q, handler]() mutable {
        while(true) {

            std::any msg;
            if(Q.ReceiveTimeout(1, msg) == wxMSGQUEUE_NO_ERROR) {
                // got a message
                if(msg.type() == typeid(CMD_STUB_SHUTDOWN)) {
                    // shutdown
                    LOG_DEBUG(LOG) << "shutting down" << endl;
                    break;

                } else if(msg.type() == typeid(CMD_STUB_WRITE)) {
                    // write buffer to remote
                    auto write_command = std::any_cast<CmdWrite>(msg);
                    LOG_DEBUG(LOG) << "writing:" << write_command.content << endl;

                } else {
                    LOG_WARNING(LOG) << "received unknown command." << msg.type().name() << endl;
                }
            }

            // First, poll the channel
            if(!clSSHCHannelRead(channel, handler, false, true) || !clSSHCHannelRead(channel, handler, true, true)) {
                break;
            }
        }
    });
    thr.detach();
}
}; // namespace

clSSHInteractiveChannel* clSSHInteractiveChannel::Create(wxEvtHandler* parent, clSSH::Ptr_t ssh,
                                                         const std::vector<wxString>& args, const wxString& workingDir,
                                                         const clEnvList_t* env)
{
    CHECK_ARG(ssh, "cant start remote interactive process with null ssh session");
    CHECK_ARG(parent, "cant start remote interactive process with sink object");

    auto session = ssh->GetSession();
    auto channel = ssh_channel_new(session);
    if(!channel) {
        clWARNING() << "Failed to allocate new ssh channel" << endl;
        return nullptr;
    }

    // request a shell
    int rc = ssh_channel_request_shell(channel);
    if(rc != SSH_OK) {
        clWARNING() << "SSH request channel error." << ssh_get_error(session) << endl;
        ssh_channel_free(channel);
        return nullptr;
    }

    // create the Channel object, start the helper thread
    clSSHInteractiveChannel* process = new clSSHInteractiveChannel(parent, ssh, channel);
    // remote servers might often return some prompt messages
    // in order to filter these messages out, we send a echo
    // command and wait for the reponse
    std::string message = "echo START_MARKER\n";
    process->Write(message);
    start_helper_thread(channel, process->m_parent, process->m_queue);
    return process;
}

clSSHInteractiveChannel::clSSHInteractiveChannel(wxEvtHandler* parent, clSSH::Ptr_t ssh, SSHChannel_t channel)
    : IProcess(parent)
    , m_ssh(ssh)
    , m_channel(channel)
{
}

clSSHInteractiveChannel::~clSSHInteractiveChannel() {}

void clSSHInteractiveChannel::Detach()
{
    if(m_channel) {
        ssh_channel_close(m_channel);
        ssh_channel_free(m_channel);
        m_channel = nullptr;
    }
}

bool clSSHInteractiveChannel::IsAlive() { return m_ssh && m_channel; }
bool clSSHInteractiveChannel::Read(wxString& buff, wxString& buffErr, std::string& raw_buff, std::string& raw_buffErr)
{
    return false;
}

bool clSSHInteractiveChannel::Write(const std::string& buff) { return false; }
bool clSSHInteractiveChannel::Write(const wxString& buff) { return false; }
bool clSSHInteractiveChannel::WriteRaw(const wxString& buff) { return false; }
bool clSSHInteractiveChannel::WriteRaw(const std::string& buff) { return false; }
void clSSHInteractiveChannel::WaitForTerminate(wxString& output) {}
bool clSSHInteractiveChannel::WriteToConsole(const wxString& buff) { return false; }
void clSSHInteractiveChannel::Cleanup() {}
void clSSHInteractiveChannel::Terminate() {}
void clSSHInteractiveChannel::Signal(wxSignal sig) {}
void clSSHInteractiveChannel::SuspendAsyncReads() {}

#endif // USE_SFTP
