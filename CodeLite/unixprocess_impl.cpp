//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : unixprocess_impl.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "unixprocess_impl.h"

#include "SocketAPI/clSocketBase.h"
#include "StringUtils.h"
#include "cl_exception.h"
#include "file_logger.h"
#include "fileutils.h"

#include <cstring>
#include <string>
#include <thread>
#include <wx/buffer.h>

#if defined(__WXMAC__) || defined(__WXGTK__)

#include "procutils.h"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#ifdef __WXGTK__
#ifdef __FreeBSD__
#include <libutil.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(__NetBSD__)
#include <sys/ioctl.h>
#include <termios.h>
#include <util.h>
#else
#include <pty.h>
#include <utmp.h>
#endif
#else
#include <util.h>
#endif

// ----------------------------------------------
#define ISBLANK(ch) ((ch) == ' ' || (ch) == '\t')
#define BUFF_SIZE 1024 * 64

/*  Routines imported from standard C runtime libraries. */

#ifdef __STDC__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#else /* !__STDC__ */

#if !defined _WIN32 || defined __GNUC__
extern char* memcpy();  /* Copy memory region */
extern int strlen();    /* Count length of string */
extern char* malloc();  /* Standard memory allocater */
extern char* realloc(); /* Standard memory reallocator */
extern void free();     /* Free malloc'd memory */
extern char* strdup();  /* Duplicate a string */
#endif

#endif /* __STDC__ */

#ifndef NULL
#define NULL 0
#endif

#ifndef EOS
#define EOS '\0'
#endif

#define INITIAL_MAXARGC 8 /* Number of args + NULL in initial argv */

static void freeargv(char** vector)
{
    char** scan;
    if(vector != NULL) {
        for(scan = vector; *scan != NULL; scan++) {
            free(*scan);
        }
        delete[] vector;
    }
}

//-----------------------------------------------------

static char** make_argv(const wxArrayString& args, int& argc)
{
    char** argv = new char*[args.size() + 1];
    for(size_t i = 0; i < args.size(); ++i) {
        argv[i] = strdup(args[i].mb_str(wxConvUTF8).data());
    }
    argv[args.size()] = 0;
    argc = args.size();
    return argv;
}

static void RemoveTerminalColoring(char* buffer)
{
    std::string cinput = buffer;
    std::string coutout;
    StringUtils::StripTerminalColouring(cinput, coutout);

    // coutout is ALWAYS <= cinput, so we can safely copy the content to the buffer
    if(coutout.length() < cinput.length()) {
        strcpy(buffer, coutout.c_str());
    }
}

UnixProcessImpl::UnixProcessImpl(wxEvtHandler* parent)
    : IProcess(parent)
    , m_readHandle(-1)
    , m_writeHandle(-1)
{
}

UnixProcessImpl::~UnixProcessImpl() { Cleanup(); }

void UnixProcessImpl::Cleanup()
{
    close(GetReadHandle());
    close(GetWriteHandle());
    if(GetStderrHandle() != wxNOT_FOUND) {
        close(GetStderrHandle());
    }
    if(m_thr) {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;

    if(GetPid() != wxNOT_FOUND) {
        wxKill(GetPid(), GetHardKill() ? wxSIGKILL : wxSIGTERM, NULL, wxKILL_CHILDREN);
        // The Zombie cleanup is done in app.cpp in ::ChildTerminatedSingalHandler() signal handler
        int status(0);
        waitpid(-1, &status, WNOHANG);
    }
}

bool UnixProcessImpl::IsAlive() { return kill(m_pid, 0) == 0; }

bool UnixProcessImpl::ReadFromFd(int fd, fd_set& rset, wxString& output)
{
    if(fd == wxNOT_FOUND) {
        return false;
    }
    if(FD_ISSET(fd, &rset)) {
        // there is something to read
        char buffer[BUFF_SIZE + 1]; // our read buffer
        int bytesRead = read(fd, buffer, sizeof(buffer));
        if(bytesRead > 0) {
            buffer[bytesRead] = 0; // always place a terminator

            // Remove coloring chars from the incomnig buffer
            // colors are marked with ESC and terminates with lower case 'm'
            if(!(this->m_flags & IProcessRawOutput)) {
                RemoveTerminalColoring(buffer);
            }
            wxString convBuff = wxString(buffer, wxConvUTF8);
            if(convBuff.IsEmpty()) {
                convBuff = wxString::From8BitData(buffer);
            }

            output.swap(convBuff);
            return true;
        }
    }
    return false;
}

bool UnixProcessImpl::Read(wxString& buff, wxString& buffErr)
{
    fd_set rs;
    timeval timeout;

    memset(&rs, 0, sizeof(rs));
    FD_SET(GetReadHandle(), &rs);
    if(m_stderrHandle != wxNOT_FOUND) {
        FD_SET(m_stderrHandle, &rs);
    }

    timeout.tv_sec = 0;       // 0 seconds
    timeout.tv_usec = 250000; // 250 ms

    int errCode(0);
    errno = 0;

    buff.Clear();
    int maxFd = wxMax(GetStderrHandle(), GetReadHandle());
    int rc = select(maxFd + 1, &rs, NULL, NULL, &timeout);
    errCode = errno;
    if(rc == 0) {
        // timeout
        return true;

    } else if(rc > 0) {
        // We differentiate between stdout and stderr?
        bool stderrRead = ReadFromFd(GetStderrHandle(), rs, buffErr);
        bool stdoutRead = ReadFromFd(GetReadHandle(), rs, buff);
        return stderrRead || stdoutRead;

    } else {

        if(errCode == EINTR || errCode == EAGAIN) {
            return true;
        }

        // Process terminated
        // the exit code will be set in the sigchld event handler
        return false;
    }
}

namespace
{
bool do_write(int fd, const wxMemoryBuffer& buffer)
{
    static constexpr int max_retry_count = 10;
    size_t bytes_written = 0;
    size_t bytes_left = buffer.GetDataLen();

    char* pdata = (char*)buffer.GetData();
    std::string str(pdata, bytes_left);
    clDEBUG1() << "do_write() buffer:" << str << endl;
    clDEBUG1() << "do_write() length:" << str.length() << endl;
    while(bytes_left) {
        int bytes_sent = ::write(fd, (const char*)pdata, bytes_left);
        clDEBUG1() << "::do_write() completed. number of bytes sent:" << bytes_sent << endl;
        if(bytes_sent <= 0) {
            return false;
        }
        pdata += bytes_sent;
        bytes_left -= bytes_sent;
    }
    return true;
}
} // namespace

bool UnixProcessImpl::Write(const std::string& buff) { return WriteRaw(buff + "\n"); }

bool UnixProcessImpl::Write(const wxString& buff) { return Write(FileUtils::ToStdString(buff)); }

bool UnixProcessImpl::WriteRaw(const wxString& buff) { return WriteRaw(FileUtils::ToStdString(buff)); }
bool UnixProcessImpl::WriteRaw(const std::string& buff)
{
    int fd = GetWriteHandle();
    wxMemoryBuffer mb;
    mb.AppendData(buff.c_str(), buff.length());
    return do_write(GetWriteHandle(), mb);
}

IProcess* UnixProcessImpl::Execute(wxEvtHandler* parent, const wxArrayString& args, size_t flags,
                                   const wxString& workingDirectory, IProcessCallback* cb)
{
    int argc = 0;
    char** argv = make_argv(args, argc);
    if(argc == 0 || argv == nullptr) {
        return nullptr;
    }

    // fork the child process
    wxString curdir = wxGetCwd();

    // Prentend that we are a terminal...
    int master;
    char pts_name[1024];
    memset(pts_name, 0x0, sizeof(pts_name));

    // Create a one-way communication channel (pipe).
    // If successful, two file descriptors are stored in stderrPipes;
    // bytes written on stderrPipes[1] can be read from stderrPipes[0].
    // Returns 0 if successful, -1 if not.
    int stderrPipes[2] = { 0, 0 };
    if(flags & IProcessStderrEvent) {
        errno = 0;
        if(pipe(stderrPipes) < 0) {
            clERROR() << "Failed to create pipe for stderr redirecting." << strerror(errno);
            flags &= ~IProcessStderrEvent;
        }
    }

    int rc = forkpty(&master, pts_name, nullptr, nullptr);
    if(rc == 0) {
        //===-------------------------------------------------------
        // Child process
        //===-------------------------------------------------------
        for(int i = 0; i < FD_SETSIZE; ++i) {
            if(i != STDERR_FILENO && i != STDOUT_FILENO && i != STDIN_FILENO) {
                if((i == stderrPipes[1]) && (flags & IProcessStderrEvent)) {
                    // skip it
                } else {
                    ::close(i);
                }
            }
        }

        // Incase the user wants to get separate events for STDERR, dup2 STDERR to the PIPE write end
        // we opened earlier
        if(flags & IProcessStderrEvent) {
            // Dup stderrPipes[1] into stderr
            dup2(stderrPipes[1], STDERR_FILENO);
            close(stderrPipes[1]);
        }

        // at this point, slave is used as stdin/stdout/stderr
        // Child process
        if(workingDirectory.IsEmpty() == false) {
            wxSetWorkingDirectory(workingDirectory);
        }

        // execute the process
        errno = 0;
        if(execvp(argv[0], argv) < 0) {
            clERROR() << "execvp" << args << "error:" << strerror(errno) << clEndl;
        }

        // if we got here, we failed...
        exit(0);

    } else if(rc < 0) {
        // Error

        // restore the working directory
        wxSetWorkingDirectory(curdir);

        return NULL;

    } else {
        //===-------------------------------------------------------
        // Parent
        //===-------------------------------------------------------
        struct termios tios;
        tcgetattr(master, &tios);
        tios.c_lflag &= ~(ECHO | ECHONL);
        tcsetattr(master, TCSAFLUSH, &tios);

        freeargv(argv);
        argc = 0;

        // disable ECHO
        struct termios termio;
        tcgetattr(master, &termio);
        cfmakeraw(&termio);
        termio.c_lflag = ICANON;
        termio.c_oflag = ONOCR | ONLRET;
        tcsetattr(master, TCSANOW, &termio);

        // restore the working directory
        wxSetWorkingDirectory(curdir);

        UnixProcessImpl* proc = new UnixProcessImpl(parent);
        proc->m_callback = cb;
        if(flags & IProcessStderrEvent) {
            close(stderrPipes[1]); // close the write end
            // set the stderr handle
            proc->SetStderrHandle(stderrPipes[0]);
        }

        proc->SetReadHandle(master);
        proc->SetWriteHandler(master);
        proc->SetPid(rc);
        proc->m_flags = flags; // Keep the creation flags

        // Keep the terminal name, we will need it
        proc->SetTty(pts_name);

        if(!(proc->m_flags & IProcessCreateSync)) {
            proc->StartReaderThread();
        }
        return proc;
    }
}

IProcess* UnixProcessImpl::Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                                   const wxString& workingDirectory, IProcessCallback* cb)
{
    wxArrayString args = StringUtils::BuildArgv(cmd);
    clDEBUG() << "Executing:" << cmd << endl;
    clDEBUG() << "As array:" << args << endl;
    return Execute(parent, args, flags, workingDirectory, cb);
}

void UnixProcessImpl::StartReaderThread()
{
    // Launch the 'Reader' thread
    m_thr = new ProcessReaderThread();
    m_thr->SetProcess(this);
    m_thr->SetNotifyWindow(m_parent);
    m_thr->Start();
}

void UnixProcessImpl::Terminate()
{
    wxKill(GetPid(), GetHardKill() ? wxSIGKILL : wxSIGTERM, NULL, wxKILL_CHILDREN);
    int status(0);
    // The real cleanup is done inside ::ChildTerminatedSingalHandler() signal handler (app.cpp)
    waitpid(-1, &status, WNOHANG);
}

bool UnixProcessImpl::WriteToConsole(const wxString& buff)
{
    wxString tmpbuf = buff;
    tmpbuf.Trim().Trim(false);
    tmpbuf << "\n";
    int fd = GetWriteHandle();
    wxMemoryBuffer mb;
    wxCharBuffer cb = buff.mb_str(wxConvUTF8).data();
    mb.AppendData(cb.data(), cb.length());
    return do_write(GetWriteHandle(), mb);
}

void UnixProcessImpl::Detach()
{
    if(m_thr) {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;
}

void UnixProcessImpl::Signal(wxSignal sig) { wxKill(GetPid(), sig, NULL, wxKILL_CHILDREN); }

#endif //#if defined(__WXMAC )||defined(__WXGTK__)
