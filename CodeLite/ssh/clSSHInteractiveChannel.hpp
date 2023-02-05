#ifndef CLSSHINTERACTIVECHANNEL_HPP
#define CLSSHINTERACTIVECHANNEL_HPP

#include "asyncprocess.h" // IProcess
#include "codelite_exports.h"

#include <thread>
#include <vector>
#include <wx/any.h>
#include <wx/msgqueue.h>
#include <wx/string.h>

#if USE_SFTP

#include "clSSHChannelCommon.hpp"
#include "cl_ssh.h"

class WXDLLIMPEXP_CL clSSHInteractiveChannel : public IProcess
{
    clSSH::Ptr_t m_ssh;
    SSHChannel_t m_channel = nullptr;
    wxMessageQueue<wxAny> m_queue;
    bool m_waiting = true;
    wxString m_waitingBuffer;
    std::thread* m_thread = nullptr;
    bool m_closeEventFired = false;

private:
    clSSHInteractiveChannel(wxEvtHandler* parent, clSSH::Ptr_t ssh, SSHChannel_t channel);
    void StopThread();

    void OnChannelStdout(clCommandEvent& event);
    void OnChannelStderr(clCommandEvent& event);
    void OnChannelClosed(clCommandEvent& event);
    void OnChannelError(clCommandEvent& event);

public:
    virtual ~clSSHInteractiveChannel();

    static clSSHInteractiveChannel::Ptr_t Create(wxEvtHandler* parent, clSSH::Ptr_t ssh,
                                                 const std::vector<wxString>& args, size_t flags,
                                                 const wxString& workingDir, const clEnvList_t* env);

    /// Stop notifying the parent window about input/output from the process
    /// this is useful when we wish to terminate the process onExit but we don't want
    /// to know about its termination
    virtual void Detach();

    /// Read from process stdout - return immediately if no data is available
    /// we return both converted buffer as string and the raw buffer (unconverted)
    virtual bool Read(wxString& buff, wxString& buffErr, std::string& raw_buff, std::string& raw_buffErr);

    /// Write to the process stdin
    /// This version add LF to the buffer
    virtual bool Write(const wxString& buff);

    /// ANSI version
    /// This version add LF to the buffer
    virtual bool Write(const std::string& buff);

    /// Write to the process stdin
    virtual bool WriteRaw(const wxString& buff);

    /// ANSI version
    virtual bool WriteRaw(const std::string& buff);

    /**
     * @brief wait for process to terminate and return all its output to the caller
     * Note that this function is blocking
     */
    virtual void WaitForTerminate(wxString& output);

    /**
     * @brief this method is mostly needed on MSW where writing a password
     * is done directly on the console buffer rather than its stdin
     */
    virtual bool WriteToConsole(const wxString& buff);

    /// Return true if the process is still alive
    virtual bool IsAlive();

    /// Clean the process resources and kill the process if it is
    /// still alive
    virtual void Cleanup();

    /// Terminate the process. It is recommended to use this method
    /// so it will invoke the 'Cleaup' procedure and the process
    /// termination event will be sent out
    virtual void Terminate();

    /**
     * @brief send signal to the process
     */
    virtual void Signal(wxSignal sig);

    /**
     * @brief do we have process redirect enabled?
     */
    bool IsRedirect() const { return !(m_flags & IProcessNoRedirect); }

    /**
     * @brief stop reading process output in the background thread
     */
    void SuspendAsyncReads();
    /**
     * @brief resume reading process output in the background
     */
    void ResumeAsyncReads();
};
#endif // USE_SFTP
#endif // CLSSHINTERACTIVECHANNEL_HPP
