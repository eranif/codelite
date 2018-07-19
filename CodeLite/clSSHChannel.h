#ifndef CLSSHCHANNEL_H
#define CLSSHCHANNEL_H

#if USE_SFTP

#include "cl_ssh.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clSSHChannel
{
    clSSH::Ptr_t m_ssh;
    SSHChannel_t m_channel;

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
     */
    void Execute(const wxString& command);
    
    /**
     * @brief send an eof on the channel
     * this is needed after the remote execution is done
     */
    void SendEOF();
};
#endif
#endif // CLSSHCHANNEL_H
