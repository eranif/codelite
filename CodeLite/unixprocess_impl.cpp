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
#include "file_logger.h"

#if defined(__WXMAC__) || defined(__WXGTK__)

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include "procutils.h"

#ifdef __WXGTK__
#ifdef __FreeBSD__
#include <sys/ioctl.h>
#include <termios.h>
#include <libutil.h>
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
#include <string.h>
#include <stdlib.h>

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

#define BUFF_STATE_NORMAL 0
#define BUFF_STATE_IN_ESC 1

static void RemoveTerminalColoring(char* buffer)
{
    char* saved_buff = buffer;
    char tmpbuf[BUFF_SIZE + 1];
    memset(tmpbuf, 0, sizeof(tmpbuf));

    short state = BUFF_STATE_NORMAL;
    size_t i(0);

    while((*buffer) != 0) {
        switch(state) {
        case BUFF_STATE_NORMAL:
            if(*buffer == 0x1B) { // found ESC char
                state = BUFF_STATE_IN_ESC;

            } else {
                tmpbuf[i] = *buffer;
                i++;
            }
            break;
        case BUFF_STATE_IN_ESC:
            if(*buffer == 'm') { // end of color sequence
                state = BUFF_STATE_NORMAL;
            }
            break;
        }
        buffer++;
    }
    memset(saved_buff, 0, BUFF_SIZE);
    memcpy(saved_buff, tmpbuf, strlen(tmpbuf));
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

bool UnixProcessImpl::Read(wxString& buff)
{
    fd_set rs;
    timeval timeout;

    memset(&rs, 0, sizeof(rs));
    FD_SET(GetReadHandle(), &rs);
    timeout.tv_sec = 0;      // 0 seconds
    timeout.tv_usec = 50000; // 50 ms

    int errCode(0);
    errno = 0;

    buff.Clear();
    int rc = select(GetReadHandle() + 1, &rs, NULL, NULL, &timeout);
    errCode = errno;
    if(rc == 0) {
        // timeout
        return true;

    } else if(rc > 0) {
        // there is something to read
        char buffer[BUFF_SIZE + 1]; // our read buffer
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(GetReadHandle(), buffer, sizeof(buffer));
        if(bytesRead > 0) {
            buffer[BUFF_SIZE] = 0; // allways place a terminator

            // Remove coloring chars from the incomnig buffer
            // colors are marked with ESC and terminates with lower case 'm'
            RemoveTerminalColoring(buffer);

            wxString convBuff = wxString(buffer, wxConvUTF8);
            if(convBuff.IsEmpty()) {
                convBuff = wxString::From8BitData(buffer);
            }

            buff = convBuff;
            return true;
        }
        return false;

    } else {

        if(errCode == EINTR || errCode == EAGAIN) {
            return true;
        }

        // Process terminated
        // the exit code will be set in the sigchld event handler
        return false;
    }
}

bool UnixProcessImpl::Write(const wxString& buff)
{
    wxString tmpbuf = buff;
    tmpbuf << wxT("\n");
    int bytes = write(GetWriteHandle(), tmpbuf.mb_str(wxConvUTF8).data(), tmpbuf.Length());
    return bytes == (int)tmpbuf.length();
}

IProcess* UnixProcessImpl::Execute(
    wxEvtHandler* parent, const wxString& cmd, size_t flags, const wxString& workingDirectory, IProcessCallback* cb)
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
    if(argc == 0) {
        return NULL;
    }

    // fork the child process
    wxString curdir = wxGetCwd();

    // Prentend that we are a terminal...
    int master, slave;
    openpty(&master, &slave, NULL, NULL, NULL);

    int rc = fork();
    if(rc == 0) {
        login_tty(slave);
        close(master); // close the un-needed master end

        // at this point, slave is used as stdin/stdout/stderr
        // Child process
        if(workingDirectory.IsEmpty() == false) {
            wxSetWorkingDirectory(workingDirectory);
        }

        // execute the process
        errno = 0;
        if(execvp(argv[0], argv) < 0) {
            clERROR() << "execvp('" << newCmd << "') error:" << strerror(errno) << clEndl;
        }

        // if we got here, we failed...
        exit(0);

    } else if(rc < 0) {
        // Error

        // restore the working directory
        wxSetWorkingDirectory(curdir);

        return NULL;

    } else {

        // Parent
        close(slave);
        freeargv(argv);
        argc = 0;

        // disable ECHO
        struct termios termio;
        tcgetattr(master, &termio);
        termio.c_lflag = ICANON;
        termio.c_oflag = ONOCR | ONLRET;
        tcsetattr(master, TCSANOW, &termio);

        // restore the working directory
        wxSetWorkingDirectory(curdir);

        UnixProcessImpl* proc = new UnixProcessImpl(parent);
        proc->m_callback = cb;
        proc->SetReadHandle(master);
        proc->SetWriteHandler(master);
        proc->SetPid(rc);
        proc->m_flags = flags; // Keep the creation flags

        if(!(proc->m_flags & IProcessCreateSync)) {
            proc->StartReaderThread();
        }
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
