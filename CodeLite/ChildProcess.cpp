#include "ChildProcess.h"
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include "fileutils.h"
#include "cl_exception.h"
#include <cstring>
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "SocketAPI/clSocketBase.h"
#include "file_logger.h"

static wxString wrap_spaces(const wxString& str)
{
    wxString s = str;
    if(s.Contains(" ")) { s.Prepend("\"").Append("\""); }
    return s;
}

wxDEFINE_EVENT(wxEVT_CHILD_PROCESS_STDOUT, clProcessEvent);
wxDEFINE_EVENT(wxEVT_CHILD_PROCESS_STDERR, clProcessEvent);
wxDEFINE_EVENT(wxEVT_CHILD_PROCESS_EXIT, clProcessEvent);

ChildProcess::ChildProcess()
{
#if USE_IPROCESS
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ChildProcess::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &ChildProcess::OnProcessStderr, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ChildProcess::OnProcessTerminated, this);
#endif
}

void ChildProcess::Start(const wxArrayString& argv, const wxString& workingDirectory)
{
#if !USE_IPROCESS
    Cleanup();
    child_pid = fork();
    if(child_pid == -1) { throw clException(wxString() << "fork error: " << strerror(errno)); }
    if(child_pid == 0) {

        // create c-style array
        wxString commandToExecute;
        char** pargv = new char*[argv.size() + 1];
        for(size_t i = 0; i < argv.size(); ++i) {
            std::string cstr = FileUtils::ToStdString(wrap_spaces(argv[i]));
            pargv[i] = new char[cstr.length() + 1];
            strcpy(pargv[i], cstr.c_str());
            commandToExecute << pargv[i] << " ";
            
        }
        pargv[argv.size()] = NULL;
        
        clDEBUG() << "Will execute the command:" << commandToExecute;
        
        // In child process
        dup2(write_pipe->read_fd(), STDIN_FILENO);
        dup2(read_pipe->write_fd(), STDOUT_FILENO);
        write_pipe->close();
        read_pipe->close();
        // Change the working directory
        if(!workingDirectory.IsEmpty()) { ::wxSetWorkingDirectory(workingDirectory); }
        
        // Before we start the process, let the parent process do its magic 
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int result = execvp(pargv[0], const_cast<char* const*>(pargv));
        if(result == -1) {
            // Note: no point writing to stdout here, it has been redirected
            std::cerr << "Error: Failed to launch program" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        // parent process
        close(write_pipe->read_fd());
        close(read_pipe->write_fd());

        // Start the reader thread here
        DoAsyncRead();
    }
#else

    m_process = ::CreateAsyncProcess(this, BuildCommand(argv), IProcessCreateDefault, workingDirectory);
    if(!m_process) { throw clException(wxString() << ":CreateAsyncProcess()." << strerror(errno)); }
#endif
}
#if !USE_IPROCESS
bool ChildProcess::DoReadAll(int fd, std::string& content, int timeoutMilliseconds)
{
    fd_set rset;
    char buff[4096];
    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    int seconds = timeoutMilliseconds / 1000;
    int ms = timeoutMilliseconds % 1000;

    // clSocketBase c(fd);
    // c.MakeSocketBlocking(false);

    struct timeval tv = { seconds, ms * 1000 }; //  10 milliseconds timeout
    int rc = ::select(fd + 1, &rset, nullptr, nullptr, &tv);
    int selectErrno = errno;
    if(rc > 0) {
        memset(buff, 0, sizeof(buff));
        errno = 0;
        int bytes = read(fd, buff, sizeof(buff) - 1);
        int readErrno = errno;
        if(bytes > 0) {
            // read success
            content.append(buff);
            return true;
        } else if(bytes == 0) {
            // process terminated
            return false;
        } else {
            // read error
            return (readErrno == EWOULDBLOCK) || (readErrno == EAGAIN) || (readErrno == EINTR);
        }
    } else if(rc == 0) {
        // select timeout
        return true;
    } else {
        // select error
        return (selectErrno == EWOULDBLOCK) || (selectErrno == EAGAIN) || (selectErrno == EINTR);
    }
    return false;
}

bool ChildProcess::DoWrite(int fd, const std::string& message)
{
    int bytes = 0;
    std::string tmp = std::move(message);
    while(!tmp.empty()) {
        errno = 0;
        bytes = ::write(fd, tmp.c_str(), tmp.length());
        int errCode = errno;
        if(bytes < 0) {
            if(errCode == EINTR || errCode == EWOULDBLOCK || errCode == EAGAIN) { continue; }
            break;

        } else if(bytes) {
            tmp.erase(0, bytes);
        }
    }
    return tmp.empty();
}
#endif

int ChildProcess::Wait()
{
#if !USE_IPROCESS
    int status = 0;
    waitpid(child_pid, &status, WNOHANG);
    return WEXITSTATUS(status);
#else
    return 0;
#endif
}

void ChildProcess::Stop() { Cleanup(); }

void ChildProcess::Write(const wxString& message)
{
    std::string cmessage = FileUtils::ToStdString(message);
    Write(cmessage);
}

void ChildProcess::Write(const std::string& message)
{
#if !USE_IPROCESS
    std::thread worker([](int fd, const std::string& message) { ChildProcess::DoWrite(fd, message); },
                       write_pipe->write_fd(), message);
    worker.detach();
#else
    if(m_process) { m_process->Write(message); }
#endif
}

#if !USE_IPROCESS
void ChildProcess::OnDataRead(ChildProcess* process, const std::string& dataStdout, const std::string& dataStderr)
{
    if(!dataStdout.empty()) {
        clProcessEvent event(wxEVT_CHILD_PROCESS_STDOUT);
        event.SetOutput(wxString() << dataStdout);
        process->AddPendingEvent(event);
    }
    if(!dataStderr.empty()) {
        clProcessEvent event(wxEVT_CHILD_PROCESS_STDERR);
        event.SetOutput(wxString() << dataStderr);
        process->AddPendingEvent(event);
    }
}

void ChildProcess::DoAsyncRead()
{
    m_reader = new std::thread(
        [](ChildProcess* process, int fd) {
            std::string content;
            std::string errContent;
            while(true) {
                if(!ChildProcess::DoReadAll(fd, content, 100)) { break; }

                if(!content.empty()) { ChildProcess::OnDataRead(process, content, errContent); }
                content.clear();
                // are we going down?
                if(process->m_goingDown.load()) { break; }
            }
            
            // Notify about process termination event here
            process->CallAfter(&ChildProcess::ReaderThreadExit);
        },
        this, read_pipe->read_fd());
}
#endif

#if USE_IPROCESS
void ChildProcess::OnProcessOutput(clProcessEvent& event)
{
    clProcessEvent evtStdout(wxEVT_CHILD_PROCESS_STDOUT);
    evtStdout.SetOutput(event.GetOutput());
    AddPendingEvent(evtStdout);
}

void ChildProcess::OnProcessStderr(clProcessEvent& event)
{
    clProcessEvent evtStdout(wxEVT_CHILD_PROCESS_STDERR);
    evtStdout.SetOutput(event.GetOutput());
    AddPendingEvent(evtStdout);
}

void ChildProcess::OnProcessTerminated(clProcessEvent& event)
{
    clProcessEvent exitEvent(wxEVT_CHILD_PROCESS_EXIT);
    AddPendingEvent(exitEvent);
}
#endif

ChildProcess::~ChildProcess()
{
#if USE_IPROCESS
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &ChildProcess::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_STDERR, &ChildProcess::OnProcessStderr, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &ChildProcess::OnProcessTerminated, this);
#endif
    Stop();
}

bool ChildProcess::IsRunning() const
{
#if USE_IPROCESS
    return m_process != nullptr;
#else
    return (child_pid != wxNOT_FOUND) && (::kill(child_pid, 0) == 0);
#endif
}

wxString ChildProcess::BuildCommand(const wxArrayString& argv)
{
    wxString command;
    for(const wxString& arg : argv) {
        wxString c = arg;
        if(c.Contains(" ")) { c = wrap_spaces(c); }
        if(!command.IsEmpty()) { command << " "; }
        command << c;
    }
    return command;
}

void ChildProcess::Cleanup()
{
#if !USE_IPROCESS
    m_goingDown.store(true);
    if(m_reader) {
        m_reader->join();
        wxDELETE(m_reader);
    }
    if(child_pid != wxNOT_FOUND) {
        ::kill(child_pid, SIGTERM);
        Wait();
    }
    // Prepare it for next execution
    m_goingDown.store(false);
    read_pipe.reset(new CPipe());
    write_pipe.reset(new CPipe());
    child_pid = wxNOT_FOUND;
#else
    wxDELETE(m_process);
#endif
}

#if !USE_IPROCESS
void ChildProcess::ReaderThreadExit()
{
    clProcessEvent exitEvent(wxEVT_CHILD_PROCESS_EXIT);
    AddPendingEvent(exitEvent);
    // Make sure we clean this object
    Cleanup();
}
#endif
