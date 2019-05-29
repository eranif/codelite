#include "UnixProcess.h"
#if defined(__WXGTK__) || defined(__WXOSX__)
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include "file_logger.h"
#include <cl_command_event.h>
#include <processreaderthread.h>
#include <fileutils.h>

UnixProcess::UnixProcess(wxEvtHandler* owner, const wxArrayString& args)
    : m_owner(owner)
{
    m_goingDown.store(false);
    child_pid = fork();
    if(child_pid == -1) { clERROR() << _("Failed to start child process") << strerror(errno); }
    if(child_pid == 0) {
        // In child process
        dup2(m_childStdin.read_fd(), STDIN_FILENO);
        dup2(m_childStdout.write_fd(), STDOUT_FILENO);
        dup2(m_childStderr.write_fd(), STDERR_FILENO);
        m_childStdin.close();
        m_childStdout.close();
        m_childStderr.close();

        char** argv = new char*[args.size() + 1];
        for(size_t i = 0; i < args.size(); ++i) {
            std::string cstr_arg = FileUtils::ToStdString(args[i]);
            argv[i] = new char[cstr_arg.length() + 1];
            strcpy(argv[i], cstr_arg.c_str());
            argv[i][cstr_arg.length()] = 0;
        }
        argv[args.size()] = 0;
        int result = execvp(argv[0], const_cast<char* const*>(argv));
        int errNo = errno;
        if(result == -1) {
            // Note: no point writing to stdout here, it has been redirected
            clERROR() << "Error: Failed to launch program" << args << "." << strerror(errNo);
            exit(EXIT_FAILURE);
        }
    } else {
        // parent process
        close(m_childStdin.read_fd());
        close(m_childStdout.write_fd());
        close(m_childStderr.write_fd());

        // Start the reader and writer threads
        StartWriterThread();
        StartReaderThread();
    }
}

UnixProcess::~UnixProcess()
{
    Detach();

    // Kill the child process (if it is still alive)
    Stop();
    Wait();
}

bool UnixProcess::ReadAll(int fd, std::string& content, int timeoutMilliseconds)
{
    fd_set rset;
    char buff[1024];
    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    int seconds = timeoutMilliseconds / 1000;
    int ms = timeoutMilliseconds % 1000;

    struct timeval tv = { seconds, ms * 1000 }; //  10 milliseconds timeout
    int rc = ::select(fd + 1, &rset, nullptr, nullptr, &tv);
    if(rc > 0) {
        memset(buff, 0, sizeof(buff));
        if(read(fd, buff, (sizeof(buff) - 1)) > 0) {
            content.append(buff);
            return true;
        }
    } else if(rc == 0) {
        // timeout
        return true;
    }
    // error
    return false;
}

bool UnixProcess::Write(int fd, const std::string& message, std::atomic_bool& shutdown)
{
    int bytes = 0;
    std::string tmp = message;
    const int chunkSize = 4096;
    while(!tmp.empty() && !shutdown.load()) {
        errno = 0;
        bytes = ::write(fd, tmp.c_str(), tmp.length() > chunkSize ? chunkSize : tmp.length());
        int errCode = errno;
        if(bytes < 0) {
            if((errCode == EWOULDBLOCK) || (errCode == EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } else if(errCode == EINTR) {
                continue;
            } else {
                break;
            }
        } else if(bytes) {
            tmp.erase(0, bytes);
        }
    }
    clDEBUG() << "Wrote message of size:" << message.length();
    return tmp.empty();
}

int UnixProcess::Wait()
{
    int status = 0;
    waitpid(child_pid, &status, WNOHANG);
    return WEXITSTATUS(status);
}

void UnixProcess::Stop()
{
    if(child_pid != wxNOT_FOUND) { ::kill(child_pid, SIGTERM); }
}

void UnixProcess::Write(const std::string& message)
{
    if(!m_writerThread) { return; }
    m_outgoingQueue.Post(message);
}

void UnixProcess::StartWriterThread()
{
    m_writerThread = new std::thread(
        [](UnixProcess* process, int fd) {
            while(!process->m_goingDown.load()) {
                std::string buffer;
                if(process->m_outgoingQueue.ReceiveTimeout(10, buffer) == wxMSGQUEUE_NO_ERROR) {
                    UnixProcess::Write(fd, buffer, std::ref(process->m_goingDown));
                }
            }
            clDEBUG() << "UnixProcess writer thread: going down";
        },
        this, m_childStdin.write_fd());
}

void UnixProcess::StartReaderThread()
{
    m_readerThread = new std::thread(
        [](UnixProcess* process, int stdoutFd, int stderrFd) {
            while(!process->m_goingDown.load()) {
                std::string content;
                if(!ReadAll(stdoutFd, content, 10)) {
                    clProcessEvent evt(wxEVT_ASYNC_PROCESS_TERMINATED);
                    process->m_owner->AddPendingEvent(evt);
                    break;
                } else if(!content.empty()) {
                    clProcessEvent evt(wxEVT_ASYNC_PROCESS_OUTPUT);
                    evt.SetOutput(wxString() << content);
                    process->m_owner->AddPendingEvent(evt);
                }
                content.clear();
                if(!ReadAll(stderrFd, content, 10)) {
                    clProcessEvent evt(wxEVT_ASYNC_PROCESS_TERMINATED);
                    process->m_owner->AddPendingEvent(evt);
                    break;
                } else if(!content.empty()) {
                    clProcessEvent evt(wxEVT_ASYNC_PROCESS_STDERR);
                    evt.SetOutput(wxString() << content);
                    process->m_owner->AddPendingEvent(evt);
                }
            }
            clDEBUG() << "UnixProcess reader thread: going down";
        },
        this, m_childStdout.read_fd(), m_childStderr.read_fd());
}

void UnixProcess::Detach()
{
    m_goingDown.store(true);
    if(m_writerThread) {
        m_writerThread->join();
        wxDELETE(m_writerThread);
    }
    if(m_readerThread) {
        m_readerThread->join();
        wxDELETE(m_readerThread);
    }
}

#endif // OSX & GTK
