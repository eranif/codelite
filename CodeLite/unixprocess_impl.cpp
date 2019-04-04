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

#include "file_logger.h"
#include "unixprocess_impl.h"
#include <cstring>
#include "file_logger.h"
#include "fileutils.h"
#include "SocketAPI/clSocketBase.h"
#include <thread>
#include "StringUtils.h"

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
#else
#include <pty.h>
#include <utmp.h>
#endif
#else
#include <util.h>
#endif

static char** argv = NULL;
static int argc = 0;

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
    register char** scan;

    if(vector != NULL) {
        for(scan = vector; *scan != NULL; scan++) {
            free(*scan);
        }
        free(vector);
    }
}

char** dupargv(char** argv)
{
    int argc;
    char** copy;

    if(argv == NULL) return NULL;

    /* the vector */
    for(argc = 0; argv[argc] != NULL; argc++)
        ;
    copy = (char**)malloc((argc + 1) * sizeof(char*));
    if(copy == NULL) return NULL;

    /* the strings */
    for(argc = 0; argv[argc] != NULL; argc++) {
        int len = strlen(argv[argc]);
        copy[argc] = (char*)malloc(sizeof(char*) * (len + 1));
        if(copy[argc] == NULL) {
            freeargv(copy);
            return NULL;
        }
        strcpy(copy[argc], argv[argc]);
    }
    copy[argc] = NULL;
    return copy;
}

char** buildargv(const char* input)
{
    char* arg;
    char* copybuf;
    int squote = 0;
    int dquote = 0;
    int bsquote = 0;
    int argc = 0;
    int maxargc = 0;
    char** argv = NULL;
    char** nargv;

    if(input != NULL) {
        copybuf = (char*)alloca(strlen(input) + 1);
        /* Is a do{}while to always execute the loop once.  Always return an
        argv, even for null strings.  See NOTES above, test case below. */
        do {
            /* Pick off argv[argc] */
            while(ISBLANK(*input)) {
                input++;
            }
            if((maxargc == 0) || (argc >= (maxargc - 1))) {
                /* argv needs initialization, or expansion */
                if(argv == NULL) {
                    maxargc = INITIAL_MAXARGC;
                    nargv = (char**)malloc(maxargc * sizeof(char*));
                } else {
                    maxargc *= 2;
                    nargv = (char**)realloc(argv, maxargc * sizeof(char*));
                }
                if(nargv == NULL) {
                    if(argv != NULL) {
                        freeargv(argv);
                        argv = NULL;
                    }
                    break;
                }
                argv = nargv;
                argv[argc] = NULL;
            }
            /* Begin scanning arg */
            arg = copybuf;
            while(*input != EOS) {
                if(ISBLANK(*input) && !squote && !dquote && !bsquote) {
                    break;
                } else {
                    if(bsquote) {
                        bsquote = 0;
                        *arg++ = *input;
                    } else if(*input == '\\') {
                        bsquote = 1;
                    } else if(squote) {
                        if(*input == '\'') {
                            squote = 0;
                        } else {
                            *arg++ = *input;
                        }
                    } else if(dquote) {
                        if(*input == '"') {
                            dquote = 0;
                        } else {
                            *arg++ = *input;
                        }
                    } else {
                        if(*input == '\'') {
                            squote = 1;
                        } else if(*input == '"') {
                            dquote = 1;
                        } else {
                            *arg++ = *input;
                        }
                    }
                    input++;
                }
            }
            *arg = EOS;
            argv[argc] = strdup(copybuf);
            if(argv[argc] == NULL) {
                freeargv(argv);
                argv = NULL;
                break;
            }
            argc++;
            argv[argc] = NULL;

            while(ISBLANK(*input)) {
                input++;
            }
        } while(*input != EOS);
    }
    return (argv);
}

//-----------------------------------------------------

static void make_argv(const wxString& cmd)
{
    if(argc) {
        freeargv(argv);
        argc = 0;
    }

    argv = buildargv(cmd.mb_str(wxConvUTF8).data());
    argc = 0;

    for(char** targs = argv; *targs != NULL; targs++) {
        argc++;
    }
}

static void RemoveTerminalColoring(char* buffer)
{
    std::string cinput = buffer;
    std::string coutout;
    StringUtils::StripTerminalColouring(cinput, coutout);

    // coutout is ALWAYS <= cinput, so we can safely copy the content to the buffer
    strcpy(buffer, coutout.c_str());
}

UnixProcessImpl::UnixProcessImpl(wxEvtHandler* parent)
    : IProcess(parent)
    , m_readHandle(-1)
    , m_writeHandle(-1)
    , m_thr(NULL)
{
}

UnixProcessImpl::~UnixProcessImpl() { Cleanup(); }

void UnixProcessImpl::Cleanup()
{
    close(GetReadHandle());
    close(GetWriteHandle());
    if(GetStderrHandle() != wxNOT_FOUND) { close(GetStderrHandle()); }
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
    if(fd == wxNOT_FOUND) { return false; }
    if(FD_ISSET(fd, &rset)) {
        // there is something to read
        char buffer[BUFF_SIZE + 1]; // our read buffer
        int bytesRead = read(fd, buffer, sizeof(buffer));
        if(bytesRead > 0) {
            buffer[bytesRead] = 0; // always place a terminator

            // Remove coloring chars from the incomnig buffer
            // colors are marked with ESC and terminates with lower case 'm'
            if(!(this->m_flags & IProcessRawOutput)) { RemoveTerminalColoring(buffer); }
            wxString convBuff = wxString(buffer, wxConvUTF8);
            if(convBuff.IsEmpty()) { convBuff = wxString::From8BitData(buffer); }

            output = convBuff;
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
    if(m_stderrHandle != wxNOT_FOUND) { FD_SET(m_stderrHandle, &rs); }

    timeout.tv_sec = 0;      // 0 seconds
    timeout.tv_usec = 50000; // 50 ms

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

        if(errCode == EINTR || errCode == EAGAIN) { return true; }

        // Process terminated
        // the exit code will be set in the sigchld event handler
        return false;
    }
}

bool UnixProcessImpl::Write(const std::string& buff) { return WriteRaw(buff + "\n"); }

bool UnixProcessImpl::Write(const wxString& buff) { return Write(FileUtils::ToStdString(buff)); }

bool UnixProcessImpl::WriteRaw(const wxString& buff) { return WriteRaw(FileUtils::ToStdString(buff)); }
bool UnixProcessImpl::WriteRaw(const std::string& buff)
{
    std::string tmpbuf = buff;
    const int chunk_size = 1024;
    while(!tmpbuf.empty()) {
        int bytes_written =
            ::write(GetWriteHandle(), tmpbuf.c_str(), tmpbuf.length() > chunk_size ? chunk_size : tmpbuf.length());
        if(bytes_written <= 0) { return false; }
        tmpbuf.erase(0, bytes_written);
    }
    return true;
}

IProcess* UnixProcessImpl::Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                                   const wxString& workingDirectory, IProcessCallback* cb)
{
    wxString newCmd = cmd;
    if((flags & IProcessCreateAsSuperuser)) {
        if(wxFileName::Exists("/usr/bin/sudo")) {
            newCmd.Prepend("/usr/bin/sudo --askpass ");
            clDEBUG1() << "Executing command:" << newCmd << clEndl;

        } else {
            clWARNING() << "Unable to run command: '" << cmd
                        << "' as superuser: /usr/bin/sudo: no such file or directory" << clEndl;
        }
    } else {
        clDEBUG1() << "Executing command:" << newCmd << clEndl;
    }

    make_argv(newCmd);
    if(argc == 0) { return NULL; }

    // fork the child process
    wxString curdir = wxGetCwd();

    // Prentend that we are a terminal...
    int master, slave;
    char pts_name[1024];
    memset(pts_name, 0x0, sizeof(pts_name));
    openpty(&master, &slave, pts_name, NULL, NULL);

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

    int rc = fork();
    if(rc == 0) {
        //===-------------------------------------------------------
        // Child process
        //===-------------------------------------------------------
        struct termios termio;
        tcgetattr(slave, &termio);
        cfmakeraw(&termio);
        termio.c_lflag = ICANON;
        termio.c_oflag = ONOCR | ONLRET;
        tcsetattr(slave, TCSANOW, &termio);

        // Set 'slave' as STD{IN|OUT|ERR} and close slave FD
        login_tty(slave);
        close(master); // close the un-needed master end

        // Incase the user wants to get separate events for STDERR, dup2 STDERR to the PIPE write end
        // we opened earlier
        if(flags & IProcessStderrEvent) {
            // Dup stderrPipes[1] into stderr
            close(STDERR_FILENO);
            dup2(stderrPipes[1], STDERR_FILENO);
            close(stderrPipes[0]); // close the read end
        }
        close(slave);

        // at this point, slave is used as stdin/stdout/stderr
        // Child process
        if(workingDirectory.IsEmpty() == false) { wxSetWorkingDirectory(workingDirectory); }

        // execute the process
        errno = 0;
        if(execvp(argv[0], argv) < 0) { clERROR() << "execvp('" << newCmd << "') error:" << strerror(errno) << clEndl; }

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
        close(slave);
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
        
        if(!(proc->m_flags & IProcessCreateSync)) { proc->StartReaderThread(); }
        return proc;
    }
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

    tmpbuf << wxT("\n");
    int bytes = write(GetWriteHandle(), tmpbuf.mb_str(wxConvUTF8).data(), tmpbuf.Length());
    return bytes == (int)tmpbuf.length();
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

#endif //#if defined(__WXMAC )||defined(__WXGTK__)
