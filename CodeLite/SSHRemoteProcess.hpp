#ifndef SSHREMOTEPROCESS_HPP
#define SSHREMOTEPROCESS_HPP

#include "asyncprocess.h" // Base class: IProcess
#include "cl_ssh.h"
#include "codelite_exports.h"
#include "clSSHChannel.h"

class WXDLLIMPEXP_CL SSHRemoteProcess : public IProcess
{
	clSSHChannel::Ptr_t m_channel;
	
public:
    SSHRemoteProcess(clSSH::Ptr_t ssh, clSSHChannel::eChannelType type);
    virtual ~SSHRemoteProcess();
	
	static IProcess* Create(clSSH::Ptr_t ssh, const wxString& command, bool interactive);
	
public:
    virtual void Cleanup();
    virtual void Detach();
    virtual bool IsAlive();
    virtual bool Read(wxString& buff, wxString& buffErr);
    virtual void Terminate();
    virtual bool Write(const std::string& buff);
    virtual bool Write(const wxString& buff);
    virtual bool WriteRaw(const std::string& buff);
    virtual bool WriteRaw(const wxString& buff);
    virtual bool WriteToConsole(const wxString& buff);
};

#endif // SSHREMOTEPROCESS_HPP
