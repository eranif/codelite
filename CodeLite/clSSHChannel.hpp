#ifndef CLSSHCHANNEL_H
#define CLSSHCHANNEL_H

#if USE_SFTP

#include "cl_ssh.h"
#include "codelite_exports.h"
#include <wx/msgqueue.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_WRITE_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_OUTPUT, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_STDERR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_CLOSED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_PTY, clCommandEvent);

class clJoinableThread;
class WXDLLIMPEXP_CL clSSHChannel : public wxEvtHandler
{
public:
    typedef wxSharedPtr<clSSHChannel> Ptr_t;
    enum eChannelType {
        kInterativeMode,
        kRemoteCommand,
    };

public:
    struct Message {
        wxString buffer;
        bool raw = false;
    };

protected:
    clSSH::Ptr_t m_ssh;
    SSHChannel_t m_channel = nullptr;
    clJoinableThread* m_thread = nullptr;
    wxMessageQueue<Message> m_Queue;
    wxEvtHandler* m_owner = nullptr;
    eChannelType m_type = kRemoteCommand;
    bool m_wantStderr = false;

protected:
    wxString BuildError(const wxString& prefix) const;
    void DoWrite(const wxString& buffer, bool raw);

    void OnReadError(clCommandEvent& event);
    void OnWriteError(clCommandEvent& event);
    void OnReadOutput(clCommandEvent& event);
    void OnReadStderr(clCommandEvent& event);
    void OnChannelClosed(clCommandEvent& event);
    void OnChannelPty(clCommandEvent& event);

public:
    clSSHChannel(clSSH::Ptr_t ssh, clSSHChannel::eChannelType type, wxEvtHandler* owner, bool wantStderrEvents = false);
    virtual ~clSSHChannel();

    /**
     * @brief is the channel opened?
     */
    bool IsOpen() const { return m_channel != nullptr; }

    /**
     * @brief Open the channel
     * @throw clException
     */
    void Open();

    /**
     * @brief close the channel
     */
    void Close();

    /**
     * @brief execute remote command
     * The reply will be returned to 'sink' object in form of events
     */
    void Execute(const wxString& command);

    /**
     * @brief write message to the channel
     */
    void Write(const wxString& message);

    /**
     * @brief write the buffer as is
     */
    void WriteRaw(const wxString& message);

    bool IsInteractive() const { return m_type == kInterativeMode; }

    /**
     * @brief Send a signal to remote process
     */
    void SendSignal(wxSignal sig);

    /**
     * @brief detach from the remote process (this does not kill it)
     */
    void Detach();
};
#endif
#endif // CLSSHCHANNEL_H
