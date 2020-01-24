//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : winprocess.cpp
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

#ifdef __WXMSW__
#include "file_logger.h"
#include "fileutils.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "smart_ptr.h"
#include "winprocess_impl.h"
#include <atomic>
#include <memory>
#include <wx/filefn.h>
#include <wx/msgqueue.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501 // Make AttachConsole(DWORD) visible

class MyDirGuard
{
    wxString _d;

public:
    MyDirGuard()
        : _d(wxGetCwd())
    {
    }
    ~MyDirGuard() { wxSetWorkingDirectory(_d); }
};

/**
 * @class ConsoleAttacher
 * @date 11/03/10
 * @brief a helper class to attach this process to a process' console
 * this allows us to write directly into that process console-input-buffer
 * One should check isAttached once this object is constructed
 */
class ConsoleAttacher
{
public:
    bool isAttached;

public:
    ConsoleAttacher(long pid) { isAttached = AttachConsole(pid); }

    ~ConsoleAttacher()
    {
        if(isAttached) {
            FreeConsole();
        }
        isAttached = false;
    }
};

static bool CheckIsAlive(HANDLE hProcess)
{
    DWORD dwExitCode;
    if(GetExitCodeProcess(hProcess, &dwExitCode)) {
        if(dwExitCode == STILL_ACTIVE)
            return true;
    }
    return false;
}

template <typename T> bool WriteStdin(const T& buffer, HANDLE hStdin, HANDLE hProcess)
{
    DWORD dwMode;

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    SetNamedPipeHandleState(hStdin, &dwMode, NULL, NULL);
    DWORD bytesLeft = buffer.length();
    long offset = 0;
    size_t retryCount = 0;
    while(bytesLeft > 0 && (retryCount < 100)) {
        DWORD dwWritten = 0;
        if(!WriteFile(hStdin, buffer.c_str() + offset, bytesLeft, &dwWritten, NULL)) {
            int errorCode = GetLastError();
            clERROR() << ">> WriteStdin: (WriteFile) error:" << errorCode;
            return false;
        }
        if(!CheckIsAlive(hProcess)) {
            return false;
        }
        if(dwWritten == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        bytesLeft -= dwWritten;
        offset += dwWritten;
        ++retryCount;
    }
    return true;
}

class WXDLLIMPEXP_CL WinWriterThread
{
    std::thread* m_thread = nullptr;
    wxMessageQueue<std::string> m_outgoingQueue;
    std::atomic_bool m_shutdown;
    HANDLE m_hProcess = INVALID_HANDLE_VALUE;
    HANDLE m_hStdin = INVALID_HANDLE_VALUE;

public:
    WinWriterThread(HANDLE hProcess, HANDLE hStdin)
        : m_hProcess(hProcess)
        , m_hStdin(hStdin)
    {
        m_shutdown.store(false);
    }

    ~WinWriterThread() {}

    void Start() { m_thread = new std::thread(&WinWriterThread::Entry, this, m_hStdin); }
    void Stop()
    {
        if(m_thread) {
            m_shutdown.store(true);
            m_thread->join();
            wxDELETE(m_thread);
        }
    }
    static void Entry(WinWriterThread* thr, HANDLE hStdin)
    {
        auto& Q = thr->m_outgoingQueue;
        while(!thr->m_shutdown.load()) {
            std::string cstr;
            if(Q.ReceiveTimeout(50, cstr) == wxMSGQUEUE_NO_ERROR) {
                if(!WriteStdin(cstr, hStdin, thr->m_hProcess)) {
                    clERROR() << "WriteFile error:" << GetLastError();
                } else {
                    clDEBUG1() << "Writer thread: wrote buffer of" << cstr.length() << "bytes";
                }
            }
        }
        clDEBUG1() << "Write thread going down";
    }

    void Write(const std::string& buffer) { m_outgoingQueue.Post(buffer); }
};

/*static*/
IProcess* WinProcessImpl::Execute(wxEvtHandler* parent, const wxString& cmd, wxString& errMsg, size_t flags,
                                  const wxString& workingDir, IProcessCallback* cb)
{
    SECURITY_ATTRIBUTES saAttr;
    BOOL fSuccess;

    MyDirGuard dg;

    wxString wd(workingDir);
    if(workingDir.IsEmpty()) {
        wd = wxGetCwd();
    }
    wxSetWorkingDirectory(wd);

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    WinProcessImpl* prc = new WinProcessImpl(parent);
    prc->m_callback = cb;
    prc->m_flags = flags;

    bool redirectOutput = !(flags & IProcessNoRedirect);

    // The steps for redirecting child process's STDOUT:
    //     1. Save current STDOUT, to be restored later.
    //     2. Create anonymous pipe to be STDOUT for child process.
    //     3. Set STDOUT of the parent process to be write handle to
    //        the pipe, so it is inherited by the child process.
    //     4. Create a noninheritable duplicate of the read handle and
    //        close the inheritable read handle.

    if(redirectOutput) {
        // Save the handle to the current STDOUT.
        prc->hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        // Create a pipe for the child process's STDOUT.
        if(!CreatePipe(&prc->hChildStdoutRd, &prc->hChildStdoutWr, &saAttr, 0)) {
            delete prc;
            return NULL;
        }

        // Set a write handle to the pipe to be STDOUT.
        if(!SetStdHandle(STD_OUTPUT_HANDLE, prc->hChildStdoutWr)) {
            delete prc;
            return NULL;
        }

        // Create noninheritable read handle and close the inheritable read handle.
        fSuccess = DuplicateHandle(GetCurrentProcess(), prc->hChildStdoutRd, GetCurrentProcess(),
                                   &prc->hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
        if(!fSuccess) {
            delete prc;
            return NULL;
        }
        CloseHandle(prc->hChildStdoutRd);

        // The steps for redirecting child process's STDERR:
        //     1. Save current STDERR, to be restored later.
        //     2. Create anonymous pipe to be STDERR for child process.
        //     3. Set STDERR of the parent process to be write handle to
        //        the pipe, so it is inherited by the child process.
        //     4. Create a noninheritable duplicate of the read handle and
        //        close the inheritable read handle.

        // Save the handle to the current STDERR.
        prc->hSaveStderr = GetStdHandle(STD_ERROR_HANDLE);

        // Create a pipe for the child process's STDERR.
        if(!CreatePipe(&prc->hChildStderrRd, &prc->hChildStderrWr, &saAttr, 0)) {
            delete prc;
            return NULL;
        }

        // Set a write handle to the pipe to be STDERR.
        if(!SetStdHandle(STD_ERROR_HANDLE, prc->hChildStderrWr)) {
            delete prc;
            return NULL;
        }

        // Create noninheritable read handle and close the inheritable read handle.
        fSuccess = DuplicateHandle(GetCurrentProcess(), prc->hChildStderrRd, GetCurrentProcess(),
                                   &prc->hChildStderrRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
        if(!fSuccess) {
            delete prc;
            return NULL;
        }
        CloseHandle(prc->hChildStderrRd);

        // The steps for redirecting child process's STDIN:
        //     1.  Save current STDIN, to be restored later.
        //     2.  Create anonymous pipe to be STDIN for child process.
        //     3.  Set STDIN of the parent to be the read handle to the
        //         pipe, so it is inherited by the child process.
        //     4.  Create a noninheritable duplicate of the write handle,
        //         and close the inheritable write handle.

        // Save the handle to the current STDIN.
        prc->hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

        // Create a pipe for the child process's STDIN.
        if(!CreatePipe(&prc->hChildStdinRd, &prc->hChildStdinWr, &saAttr, 0)) {
            delete prc;
            return NULL;
        }
        // Set a read handle to the pipe to be STDIN.
        if(!SetStdHandle(STD_INPUT_HANDLE, prc->hChildStdinRd)) {
            delete prc;
            return NULL;
        }
        // Duplicate the write handle to the pipe so it is not inherited.
        fSuccess =
            DuplicateHandle(GetCurrentProcess(), prc->hChildStdinWr, GetCurrentProcess(), &prc->hChildStdinWrDup, 0,
                            FALSE, // not inherited
                            DUPLICATE_SAME_ACCESS);
        if(!fSuccess) {
            delete prc;
            return NULL;
        }
        CloseHandle(prc->hChildStdinWr);
    }

    // Execute the child process
    STARTUPINFO siStartInfo;

    // Set up members of STARTUPINFO structure.
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);

    siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
    if(redirectOutput) {
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
        siStartInfo.hStdInput = prc->hChildStdinRd;
        siStartInfo.hStdOutput = prc->hChildStdoutWr;
        siStartInfo.hStdError = prc->hChildStderrWr;
    }

    // Set the window to hide
    siStartInfo.wShowWindow = flags & IProcessCreateConsole ? SW_SHOW : SW_HIDE;
    DWORD creationFlags = flags & IProcessCreateConsole ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW;

    if(flags & IProcessCreateWithHiddenConsole) {
        siStartInfo.wShowWindow = SW_HIDE;
        creationFlags = CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP;
    }

    BOOL ret = CreateProcess(NULL,
                             cmd.wchar_str(),   // shell line execution command
                             NULL,              // process security attributes
                             NULL,              // primary thread security attributes
                             TRUE,              // handles are inherited
                             creationFlags,     // creation flags
                             NULL,              // use parent's environment
                             NULL,              // CD to tmp dir
                             &siStartInfo,      // STARTUPINFO pointer
                             &prc->piProcInfo); // receives PROCESS_INFORMATION
    if(ret) {
        prc->dwProcessId = prc->piProcInfo.dwProcessId;
    } else {
        int err = GetLastError();
        wxUnusedVar(err);
        delete prc;
        return NULL;
    }

    if(redirectOutput) {
        // After process creation, restore the saved STDIN and STDOUT.
        if(!SetStdHandle(STD_INPUT_HANDLE, prc->hSaveStdin)) {
            delete prc;
            return NULL;
        }
        if(!SetStdHandle(STD_OUTPUT_HANDLE, prc->hSaveStdout)) {
            delete prc;
            return NULL;
        }
        if(!SetStdHandle(STD_OUTPUT_HANDLE, prc->hSaveStderr)) {
            delete prc;
            return NULL;
        }
    }

    if((prc->m_flags & IProcessCreateConsole) || (prc->m_flags & IProcessCreateWithHiddenConsole)) {
        ConsoleAttacher ca(prc->GetPid());
        if(ca.isAttached) {
            freopen("CONOUT$", "wb", stdout); // reopen stout handle as console window output
            freopen("CONOUT$", "wb", stderr); // reopen stderr handle as console window output
        }
    }
    prc->SetPid(prc->dwProcessId);
    if(!(prc->m_flags & IProcessCreateSync)) {
        prc->StartReaderThread();
    }
    prc->m_writerThread = new WinWriterThread(prc->piProcInfo.hProcess, prc->hChildStdinWrDup);
    prc->m_writerThread->Start();
    return prc;
}

WinProcessImpl::WinProcessImpl(wxEvtHandler* parent)
    : IProcess(parent)
    , m_thr(NULL)
{
    hChildStdinRd = NULL;
    hChildStdinWrDup = NULL;
    hChildStdoutWr = NULL;
    hChildStdoutRdDup = NULL;
    hChildStderrWr = NULL;
    hChildStderrRdDup = NULL;
    piProcInfo.hProcess = NULL;
    piProcInfo.hThread = NULL;
}

WinProcessImpl::~WinProcessImpl() { Cleanup(); }

bool WinProcessImpl::Read(wxString& buff, wxString& buffErr)
{
    DWORD le1(-1);
    DWORD le2(-1);
    buff.Clear();
    buffErr.Clear();

    // Sanity
    if(!IsRedirect()) {
        return false;
    }

    // Read data from STDOUT and STDERR
    if(!DoReadFromPipe(hChildStderrRdDup, ((m_flags & IProcessStderrEvent) ? buffErr : buff))) {
        le2 = GetLastError();
    }
    if(!DoReadFromPipe(hChildStdoutRdDup, buff)) {
        le1 = GetLastError();
    }
    if((le1 == ERROR_NO_DATA) && (le2 == ERROR_NO_DATA)) {
        if(IsAlive()) {
            wxThread::Sleep(10);
            return true;
        }
    }
    bool success = !buff.IsEmpty() || !buffErr.IsEmpty();
    if(!success) {
        DWORD dwExitCode;
        if(GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode)) {
            SetProcessExitCode(GetPid(), (int)dwExitCode);
        }
    }
    return success;
}

bool WinProcessImpl::Write(const wxString& buff)
{
    // Sanity
    return Write(FileUtils::ToStdString(buff));
}

bool WinProcessImpl::Write(const std::string& buff) { return WriteRaw(buff + "\r\n"); }

bool WinProcessImpl::WriteRaw(const wxString& buff) { return WriteRaw(FileUtils::ToStdString(buff)); }

bool WinProcessImpl::WriteRaw(const std::string& buff)
{
    // Sanity
    if(!IsRedirect()) {
        return false;
    }
    m_writerThread->Write(buff);
    return true;
}

bool WinProcessImpl::IsAlive()
{
    DWORD dwExitCode;
    if(GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode)) {
        if(dwExitCode == STILL_ACTIVE)
            return true;
    }
    return false;
}

void WinProcessImpl::Cleanup()
{
    if(m_writerThread) {
        m_writerThread->Stop();
        wxDELETE(m_writerThread);
    }

    // Under windows, the reader thread is detached
    if(m_thr) {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;

    // terminate the process
    if(IsAlive()) {
        std::map<unsigned long, bool> tree;
        ProcUtils::GetProcTree(tree, GetPid());

        for(const auto& vt : tree) {
            // don't kill ourself
            if((long)vt.first == GetPid()) {
                continue;
            }
            wxLogNull NoLog;
            wxKillError rc;
            wxKill(vt.first, wxSIGKILL, &rc);
        }
        ::TerminateProcess(piProcInfo.hProcess, 0);
    }

    if(IsRedirect()) {
        CloseHandle(hChildStdinRd);
        CloseHandle(hChildStdinWrDup);
        CloseHandle(hChildStdoutWr);
        CloseHandle(hChildStdoutRdDup);
        CloseHandle(hChildStderrWr);
        CloseHandle(hChildStderrRdDup);
    }

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    hChildStdinRd = NULL;
    hChildStdoutWr = NULL;
    hChildStdinWrDup = NULL;
    hChildStdoutRdDup = NULL;
    hChildStderrWr = NULL;
    hChildStderrRdDup = NULL;
    piProcInfo.hProcess = NULL;
    piProcInfo.hThread = NULL;
}

void WinProcessImpl::StartReaderThread()
{
    // Launch the 'Reader' thread
    m_thr = new ProcessReaderThread();
    m_thr->SetProcess(this);
    m_thr->SetNotifyWindow(m_parent);
    m_thr->Start();
}

bool WinProcessImpl::DoReadFromPipe(HANDLE pipe, wxString& buff)
{
    DWORD dwRead;
    DWORD dwMode;
    DWORD dwTimeout;

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    dwTimeout = 1000;
    SetNamedPipeHandleState(pipe, &dwMode, NULL, &dwTimeout);

    bool read_something = false;
    while(true) {
        BOOL bRes = ReadFile(pipe, m_buffer, sizeof(m_buffer) - 1, &dwRead, NULL);
        if(bRes) {
            wxString tmpBuff;
            // Success read
            m_buffer[dwRead / sizeof(char)] = 0;
            tmpBuff = wxString(m_buffer, wxConvUTF8);
            if(tmpBuff.IsEmpty() && dwRead > 0) {
                // conversion failed
                tmpBuff = wxString::From8BitData(m_buffer);
            }
            buff << tmpBuff;
            read_something = true;
            continue;
        }
        break;
    }
    return read_something;
}

void WinProcessImpl::Terminate()
{
    // terminate the process
    if(IsAlive()) {
        std::map<unsigned long, bool> tree;
        ProcUtils::GetProcTree(tree, GetPid());

        for(const auto& vt : tree) {
            if((long)vt.first == GetPid()) {
                continue;
            }
            wxLogNull NoLOG;
            wxKillError rc;
            wxKill(vt.first, wxSIGKILL, &rc);
        }
        TerminateProcess(piProcInfo.hProcess, 0);
    }
}

bool WinProcessImpl::WriteToConsole(const wxString& buff)
{
    wxString pass(buff);
    pass.Trim().Trim(false);

    // To write password, we need to attach to the child process console
    if(!(m_flags & (IProcessCreateWithHiddenConsole | IProcessCreateConsole)))
        return false;

    ConsoleAttacher ca(GetPid());
    if(ca.isAttached == false)
        return false;

    HANDLE hStdIn = ::CreateFile(L"CONIN$", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                 OPEN_EXISTING, 0, 0);
    if(hStdIn == INVALID_HANDLE_VALUE) {
        return false;
    }

    pass += wxT("\r\n");
    SmartPtr<INPUT_RECORD> pKeyEvents(new INPUT_RECORD[pass.Len()]);

    for(size_t i = 0; i < pass.Len(); i++) {
        (pKeyEvents.Get())[i].EventType = KEY_EVENT;
        (pKeyEvents.Get())[i].Event.KeyEvent.bKeyDown = TRUE;
        (pKeyEvents.Get())[i].Event.KeyEvent.wRepeatCount = 1;
        (pKeyEvents.Get())[i].Event.KeyEvent.wVirtualKeyCode = LOBYTE(::VkKeyScan(pass[i]));
        (pKeyEvents.Get())[i].Event.KeyEvent.wVirtualScanCode = 0;
        (pKeyEvents.Get())[i].Event.KeyEvent.uChar.UnicodeChar = pass[i];
        (pKeyEvents.Get())[i].Event.KeyEvent.dwControlKeyState = 0;
    }

    DWORD dwTextWritten;
    if(::WriteConsoleInput(hStdIn, pKeyEvents.Get(), pass.Len(), &dwTextWritten) == FALSE) {
        CloseHandle(hStdIn);
        return false;
    }
    CloseHandle(hStdIn);
    return true;
}

void WinProcessImpl::Detach()
{
    if(m_thr) {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;
}

void WinProcessImpl::Signal(wxSignal sig) { wxKill(GetPid(), sig, NULL, wxKILL_CHILDREN); }

#endif //__WXMSW__
