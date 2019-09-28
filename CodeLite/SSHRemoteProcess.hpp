#ifndef SSHREMOTEPROCESS_HPP
#define SSHREMOTEPROCESS_HPP

#if USE_SFTP
#include "asyncprocess.h" // Base class: IProcess
#include "cl_ssh.h"
#include "codelite_exports.h"
#include "clSSHChannel.h"

class WXDLLIMPEXP_CL SSHRemoteProcess : public IProcess
{
    clSSHChannel::Ptr_t m_channel;
    wxEvtHandler* m_owner = nullptr;
    wxString m_pty;

public:
    SSHRemoteProcess(wxEvtHandler* owner, clSSH::Ptr_t ssh, clSSHChannel::eChannelType type);
    virtual ~SSHRemoteProcess();

    static IProcess* Create(wxEvtHandler* owner, clSSH::Ptr_t ssh, const wxString& command, bool interactive);

    wxEvtHandler* GetOwner() { return m_owner; }

    void SetPty(const wxString& pty) { this->m_pty = pty; }
    const wxString& GetPty() const { return m_pty; }

protected:
    void OnError(clCommandEvent& event);
    void OnTerminate(clCommandEvent& event);
    void OnOutput(clCommandEvent& event);
    void OnPty(clCommandEvent& event);

public:
    virtual void Signal(wxSignal sig);
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
#endif
#endif // SSHREMOTEPROCESS_HPP
