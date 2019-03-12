#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#if defined(__WXGTK__) || defined(__WXOSX__)
#define USE_IPROCESS 0
#else
#define USE_IPROCESS 1
#endif

#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <wx/string.h>
#include "codelite_exports.h"
#include <wx/arrstr.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <atomic>
#if !USE_IPROCESS
#include <sys/wait.h>
#include <unistd.h>
#endif
#include "cl_exception.h"

class IProcess;
#if !USE_IPROCESS
// Wrapping pipe in a class makes sure they are closed when we leave scope
class CPipe
{
private:
    int fd[2];

public:
    const inline int read_fd() const { return fd[0]; }
    const inline int write_fd() const { return fd[1]; }
    CPipe()
    {
        if(pipe(fd)) {
            throw clException("Failed to create pipe");
        }
    }
    void close()
    {
        ::close(fd[0]);
        ::close(fd[1]);
    }
    ~CPipe() { close(); }
    typedef wxSharedPtr<CPipe> Ptr_t;
};
#endif

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CHILD_PROCESS_STDOUT, clProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CHILD_PROCESS_STDERR, clProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CHILD_PROCESS_EXIT, clProcessEvent);
class WXDLLIMPEXP_CL ChildProcess : public wxEvtHandler
{
private:
#if !USE_IPROCESS
    CPipe::Ptr_t write_pipe;
    CPipe::Ptr_t read_pipe;
    std::thread* m_reader = nullptr;
    std::atomic_bool m_goingDown;
#else
    IProcess* m_process = nullptr;
#endif

    // sync operations
#if !USE_IPROCESS
    static bool DoReadAll(int fd, std::string& content, int timeoutMilliseconds);
    static bool DoWrite(int fd, const std::string& message);
    static void OnDataRead(ChildProcess* process, const std::string& dataStdout, const std::string& dataStderr);
#endif

#if USE_IPROCESS
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
#else
    void DoAsyncRead();
    void ReaderThreadExit();
#endif

    // wait for process termination
    int Wait();
    void Cleanup();

public:
    int child_pid = wxNOT_FOUND;
    typedef wxSharedPtr<ChildProcess> Ptr_t;
    
    /**
     * @brief construct and start child process. argv[0] should contain the commands + arguments similar to main()
     */
    ChildProcess();
    virtual ~ChildProcess();
    
    /**
     * @brief return an execution line from an array of argv
     */
    static wxString BuildCommand(const wxArrayString &argv);
    
    /**
     * @brief start the process
     */
    void Start(const wxArrayString& argv, const wxString& workingDirectory = "");

    // stop the running process
    void Stop();
    
    /**
     * @brief is the process running?
     */
    bool IsRunning() const;
    
    void Write(const std::string& message);
    void Write(const wxString& message);
};
#endif // CHILD_PROCESS_H
