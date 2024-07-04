#ifndef UNIX_PROCESS_H
#define UNIX_PROCESS_H
#if defined(__WXGTK__) || defined(__WXOSX__)
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <wx/thread.h>
#include <wx/msgqueue.h>
#include <atomic>
#include <wx/event.h>

// Wrapping pipe in a class makes sure they are closed when we leave scope
#define CLOSE_FD(fd)        \
    if(fd != wxNOT_FOUND) { \
        ::close(fd);        \
        fd = wxNOT_FOUND;   \
    }

class CPipe
{
private:
    int m_readFd = wxNOT_FOUND;
    int m_writeFd = wxNOT_FOUND;

public:
    const inline int GetReadFd() const { return m_readFd; }
    const inline int GetWriteFd() const { return m_writeFd; }
    CPipe() {}
    void Close()
    {
        CLOSE_FD(m_readFd);
        CLOSE_FD(m_writeFd);
    }
    ~CPipe() { Close(); }
    bool Open()
    {
        int fd[2];
        if(pipe(fd) == 0) {
            m_readFd = fd[0];
            m_writeFd = fd[1];
            return true;
        }
        return false;
    }
    void CloseWriteFd() { CLOSE_FD(m_writeFd); }
    void CloseReadFd() { CLOSE_FD(m_readFd); }
};

class UnixProcess
{
private:
    CPipe m_childStdin;
    CPipe m_childStdout;
    CPipe m_childStderr;
    std::thread* m_writerThread = nullptr;
    std::thread* m_readerThread = nullptr;
    wxMessageQueue<std::string> m_outgoingQueue;
    std::atomic_bool m_goingDown;
    wxEvtHandler* m_owner = nullptr;

protected:
    // sync operations
    static bool ReadAll(int fd, std::string& content, int timeoutMilliseconds);
    static bool Write(int fd, const std::string& message, std::atomic_bool& shutdown);

    void StartWriterThread();
    void StartReaderThread();

public:
    int child_pid = -1;

    UnixProcess(wxEvtHandler* owner, const wxArrayString& args);
    ~UnixProcess();

    // wait for process termination
    int Wait();

    // Async calls with callbacks
    void Write(const std::string& message);

    // stop the running process
    void Stop();

    /**
     * @brief stop sending events from the process
     */
    void Detach();
};
#endif // defined(__WXGTK__)||defined(__WXOSX__)
#endif // UNIX_PROCESS_H
