#include "SSHRemoteProcess.hpp"
#include "file_logger.h"

bool do_ssh_write(clSSHChannel::Ptr_t channel, const wxString& buffer)
{
    try {
        if(!channel->IsInteractive()) {
            channel->Execute(buffer);
        } else {
            channel->Write(buffer);
        }
    } catch(clException& e) {
        clERROR() << e.What();
        return false;
    }
    return true;
}

SSHRemoteProcess::SSHRemoteProcess(clSSH::Ptr_t ssh, clSSHChannel::eChannelType type)
    : IProcess(NULL)
{
    m_channel.reset(new clSSHChannel(ssh, type, this));
}

SSHRemoteProcess::~SSHRemoteProcess() {}

void SSHRemoteProcess::Cleanup() { Detach(); }

void SSHRemoteProcess::Detach()
{
    m_channel->Close();
    m_channel.reset(nullptr);
}

bool SSHRemoteProcess::IsAlive() { return m_channel->IsOpen(); }

bool SSHRemoteProcess::Read(wxString& buff, wxString& buffErr)
{
    wxUnusedVar(buff);
    wxUnusedVar(buffErr);
    return false;
}

void SSHRemoteProcess::Terminate() { Detach(); }

bool SSHRemoteProcess::Write(const std::string& buff) { return do_ssh_write(m_channel, buff); }

bool SSHRemoteProcess::Write(const wxString& buff) { return do_ssh_write(m_channel, buff); }

bool SSHRemoteProcess::WriteRaw(const std::string& buff) { return do_ssh_write(m_channel, buff); }

bool SSHRemoteProcess::WriteRaw(const wxString& buff) { return do_ssh_write(m_channel, buff); }

bool SSHRemoteProcess::WriteToConsole(const wxString& buff) { return do_ssh_write(m_channel, buff); }

IProcess* SSHRemoteProcess::Create(clSSH::Ptr_t ssh, const wxString& command, bool interactive)
{
    try {
        SSHRemoteProcess* process =
            new SSHRemoteProcess(ssh, interactive ? clSSHChannel::kInterativeMode : clSSHChannel::kRemoteCommand);
        if(!command.IsEmpty()) { process->Write(command); }
        return process;
    } catch(clException& e) {
        return NULL;
    }
}
