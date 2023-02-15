#ifndef CLSSHCHANNEL_H
#define CLSSHCHANNEL_H

#if USE_SFTP

#include "clSSHChannelCommon.hpp"
#include "codelite_exports.h"
#include "ssh/cl_ssh.h"

#include <wx/msgqueue.h>

enum class clRemoteCommandStatus {
    STDOUT,
    STDERR,
    DONE,
    DONE_WITH_ERROR,
};

class clJoinableThread;
class clRemoteExecutor;
typedef std::function<void(const std::string&, clRemoteCommandStatus)> execute_callback;
class WXDLLIMPEXP_CL clSSHChannel : public wxEvtHandler
{
public:
    typedef std::shared_ptr<clSSHChannel> Ptr_t;

public:
    struct Message {
        wxString buffer;
        bool raw = false;
    };

protected:
    clRemoteExecutor* m_parent = nullptr;
    clSSH::Ptr_t m_ssh;
    SSHChannel_t m_channel = nullptr;
    clJoinableThread* m_thread = nullptr;
    wxMessageQueue<Message> m_Queue;
    bool m_wantStderr = false;
    execute_callback m_callback = nullptr;

protected:
    wxString BuildError(const wxString& prefix) const;

    // translate from `wxEVT_SSH_CHANNEL_*` events into `clProcessEvent`
    void OnReadError(clCommandEvent& event);
    void OnWriteError(clCommandEvent& event);
    void OnReadOutput(clCommandEvent& event);
    void OnReadStderr(clCommandEvent& event);
    void OnChannelClosed(clCommandEvent& event);

    void Destroy();

public:
    clSSHChannel(clRemoteExecutor* parent, clSSH::Ptr_t ssh, bool wantStderrEvents = false);
    virtual ~clSSHChannel();

    /**
     * @brief is the channel opened?
     */
    bool IsOpen() const { return m_channel != nullptr; }

    /**
     * @brief Open the channel
     */
    bool Open();

    /**
     * @brief close the channel
     */
    void Close();

    /**
     * @brief execute remote command
     * if `cb` is provided, the channel will use it to deliver the output
     * otherwise, standard `clProcessEvent` are used
     */
    bool Execute(const wxString& command, execute_callback&& cb = nullptr);

    /// execute a command and returns its output.
    bool Execute(const wxString& command, std::string* output);

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
