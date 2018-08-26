#ifndef CLSSHCHANNEL_H
#define CLSSHCHANNEL_H

#if USE_SFTP

#include "cl_ssh.h"
#include "codelite_exports.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_OUTPUT, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_CLOSED, clCommandEvent);

class clJoinableThread;
class WXDLLIMPEXP_CL clSSHChannel
{
    clSSH::Ptr_t m_ssh;
    SSHChannel_t m_channel;
    clJoinableThread* m_readerThread;

public:
    typedef wxSharedPtr<clSSHChannel> Ptr_t;

protected:
    wxString BuildError(const wxString& prefix) const;
    /**
     * @brief read data from the channel.
     * @return true on success, false on timeout
     * @throw clException on error
     */
    bool DoRead(wxString& output, int timeout_ms = -1);

public:
    clSSHChannel(clSSH::Ptr_t ssh);
    virtual ~clSSHChannel();

    /**
     * @brief read all data from the channel
     * @param output
     * @param timeout_ms
     * @return true on success, false on timeout
     * @throw clException on error
     */
    bool ReadAll(wxString& output, int timeout_ms = -1);

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
    void Execute(const wxString& command, wxEvtHandler* sink);
};
#endif
#endif // CLSSHCHANNEL_H
