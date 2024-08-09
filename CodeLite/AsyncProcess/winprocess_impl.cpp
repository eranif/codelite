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
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif

#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define NTDDI_VERSION 0x0A000006
#define _WIN32_WINNT 0x0600

typedef VOID* HPCON;

#if !defined(_MSC_VER)
typedef HRESULT(WINAPI* CreatePseudoConsole_T)(COORD size, HANDLE hInput, HANDLE hOutput, DWORD dwFlags, HPCON* phPC);
typedef VOID(WINAPI* ClosePseudoConsole_T)(HPCON hPC);

thread_local bool loadOnce = true;
thread_local CreatePseudoConsole_T CreatePseudoConsole = nullptr;
thread_local ClosePseudoConsole_T ClosePseudoConsole = nullptr;
#endif

#ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
#endif

#include "winprocess_impl.h"

#include "file_logger.h"
#include "fileutils.h"
#include "processreaderthread.h"
#include "procutils.h"

#include <atomic>
#include <memory>
#include <wx/filefn.h>
#include <wx/msgqueue.h>
#include <wx/string.h>

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

typedef HANDLE HPCON;

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
    if(hStdin == INVALID_HANDLE_VALUE || hProcess == INVALID_HANDLE_VALUE) {
        clWARNING() << "Unable read from process Stdin: invalid handle" << endl;
        return false;
    }

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_WAIT;
    SetNamedPipeHandleState(hStdin, &dwMode, NULL, NULL);
    DWORD bytesLeft = buffer.length();
    long offset = 0;
    static constexpr int max_retry_count = 100;
    size_t retryCount = 0;
    while(bytesLeft > 0 && (retryCount < max_retry_count)) {
        DWORD dwWritten = 0;
        if(!WriteFile(hStdin, buffer.c_str() + offset, bytesLeft, &dwWritten, NULL)) {
            int errorCode = GetLastError();
            LOG_IF_DEBUG { clDEBUG() << ">> WriteStdin: (WriteFile) error:" << errorCode << endl; }
            return false;
        }
        if(!CheckIsAlive(hProcess)) {
            LOG_IF_DEBUG { clDEBUG() << "WriteStdin failed. Process is not alive" << endl; }
            return false;
        }
        if(dwWritten == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        bytesLeft -= dwWritten;
        offset += dwWritten;
        ++retryCount;
    }

    if(retryCount >= max_retry_count) {
        clERROR() << "Failed to write to process after" << max_retry_count << "retries. Written"
                  << (buffer.length() - bytesLeft) << "/" << buffer.length() << "bytes" << endl;
        return false;
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
                WriteStdin(cstr, hStdin, thr->m_hProcess);
            }
        }
        LOG_IF_TRACE { clDEBUG1() << "Write thread going down"; }
    }

    void Write(const std::string& buffer) { m_outgoingQueue.Post(buffer); }
};

namespace
{
wxString ArrayJoin(const wxArrayString& args, size_t flags)
{
    wxString command;
    if(flags & IProcessWrapInShell) {
        // CMD /C [command] ...
        // Make sure that the first command is wrapped with "" if it contains spaces
        LOG_IF_TRACE
        {
            clDEBUG1() << "==> ArrayJoin called for" << args << endl;
            clDEBUG1() << "args[2] is:" << args[2] << endl;
        }

        if((args.size() > 3) && (!args[2].StartsWith("\"")) && (args[2].Contains(" "))) {
            LOG_IF_DEBUG { clDEBUG() << "==> Fixing" << args << endl; }
            wxArrayString tmparr = args;
            wxString& firstCommand = tmparr[2];
            firstCommand.Prepend("\"").Append("\"");
            command = wxJoin(tmparr, ' ', 0);
        } else {
            command = wxJoin(args, ' ', 0);
        }
    } else if(flags & IProcessCreateSSH) {
        // simple join
        command = wxJoin(args, ' ', 0);
    } else {
        wxArrayString arr;
        arr.reserve(args.size());
        arr = args;
        for(auto& arg : arr) {
            if(arg.Contains(" ")) {
                // escape any " before we start escaping
                arg.Replace("\"", "\\\"");
                // now wrap with double quotes
                arg.Prepend("\"").Append("\"");
            }
            command << arg << " ";
        }
    }
    command.Trim().Trim(false);
    return command;
}

// Initializes the specified startup info struct with the required properties and
// updates its thread attribute list with the specified ConPTY handle
HRESULT PrepareStartupInformation(HPCON hpc, STARTUPINFOEX* psi)
{
    // Prepare Startup Information structure
    STARTUPINFOEX si;
    ZeroMemory(&si, sizeof(si));
    si.StartupInfo.cb = sizeof(STARTUPINFOEX);

    // Discover the size required for the list
    SIZE_T bytesRequired;
    InitializeProcThreadAttributeList(NULL, 1, 0, &bytesRequired);

    // Allocate memory to represent the list
    si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, bytesRequired);
    if(!si.lpAttributeList) {
        return E_OUTOFMEMORY;
    }

    // Initialize the list memory location
    if(!InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &bytesRequired)) {
        HeapFree(GetProcessHeap(), 0, si.lpAttributeList);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Set the pseudoconsole information into the list
    if(!UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, hpc, sizeof(hpc), NULL,
                                  NULL)) {
        HeapFree(GetProcessHeap(), 0, si.lpAttributeList);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    *psi = si;

    return S_OK;
}

} // namespace

IProcess* WinProcessImpl::Execute(wxEvtHandler* parent, const wxArrayString& args, size_t flags,
                                  const wxString& workingDirectory, IProcessCallback* cb)
{
    wxString cmd = ArrayJoin(args, flags);
    LOG_IF_TRACE { clDEBUG1() << "Windows process starting:" << cmd << endl; }
    return Execute(parent, cmd, flags, workingDirectory, cb);
}

IProcess* WinProcessImpl::ExecuteConPTY(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                                        const wxString& workingDir)
{
    // - Close these after CreateProcess of child application with pseudoconsole object.
    HANDLE inputReadSide, outputWriteSide;

    // - Hold onto these and use them for communication with the child through the pseudoconsole.
    HANDLE outputReadSide, inputWriteSide;
    HPCON hPC = 0;

    // Create the in/out pipes:
    if(!CreatePipe(&inputReadSide, &inputWriteSide, NULL, 0)) {
        return nullptr;
    }
    if(!CreatePipe(&outputReadSide, &outputWriteSide, NULL, 0)) {
        ::CloseHandle(inputReadSide);
        ::CloseHandle(inputWriteSide);
        return nullptr;
    }

#if !defined(_MSC_VER)
    // Create the Pseudo Console, using the pipes
    if(loadOnce) {
        loadOnce = false;
        auto hDLL = ::LoadLibrary(L"Kernel32.dll");
        if(hDLL) {
            CreatePseudoConsole = (CreatePseudoConsole_T)::GetProcAddress(hDLL, "CreatePseudoConsole");
            ClosePseudoConsole = (ClosePseudoConsole_T)::GetProcAddress(hDLL, "ClosePseudoConsole");
            FreeLibrary(hDLL);
        }
    }
#endif

    if(!CreatePseudoConsole || !ClosePseudoConsole) {
        ::CloseHandle(inputReadSide);
        ::CloseHandle(outputWriteSide);
        ::CloseHandle(inputWriteSide);
        ::CloseHandle(outputReadSide);
        return nullptr;
    }
    auto hr = CreatePseudoConsole({ 1000, 32 }, inputReadSide, outputWriteSide, 0, &hPC);
    if(FAILED(hr)) {
        ::CloseHandle(inputReadSide);
        ::CloseHandle(outputWriteSide);
        ::CloseHandle(inputWriteSide);
        ::CloseHandle(outputReadSide);
        return nullptr;
    }

    // Prepare the StartupInfoEx structure attached to the ConPTY.
    STARTUPINFOEX siEx{};
    PrepareStartupInformation(hPC, &siEx);

    WinProcessImpl* prc = new WinProcessImpl(parent);
    ::ZeroMemory(&prc->piProcInfo, sizeof(prc->piProcInfo));

    auto fSuccess = CreateProcess(nullptr, (wchar_t*)cmd.wc_str(), nullptr, nullptr, FALSE,
                                  EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, &siEx.StartupInfo, &prc->piProcInfo);

    if(!fSuccess) {
        clERROR() << "Failed to launch process:" << cmd << "." << GetLastError() << endl;
        wxDELETE(prc);
        return nullptr;
    }
    ::CloseHandle(inputReadSide);
    ::CloseHandle(outputWriteSide);

    if(!(prc->m_flags & IProcessCreateSync)) {
        prc->StartReaderThread();
    }
    prc->m_writerThread = new WinWriterThread(prc->piProcInfo.hProcess, inputWriteSide);
    prc->m_writerThread->Start();

    prc->m_callback = nullptr;
    prc->m_flags = flags;
    prc->m_pid = prc->piProcInfo.dwProcessId;
    prc->hChildStdoutRdDup = outputReadSide;
    prc->m_hPseudoConsole = hPC;
    return prc;
}

IProcess* WinProcessImpl::ExecuteConPTY(wxEvtHandler* parent, const std::vector<wxString>& args, size_t flags,
                                        const wxString& workingDir)
{
    wxArrayString wxarr;
    wxarr.reserve(args.size());
    for(const auto& arg : args) {
        wxarr.Add(arg);
    }
    wxString cmd = ArrayJoin(wxarr, flags);
    return ExecuteConPTY(parent, cmd, flags, workingDir);
}

/*static*/
IProcess* WinProcessImpl::Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags, const wxString& workingDir,
                                  IProcessCallback* cb)
{
    if(flags & IProcessPseudoConsole) {
        return ExecuteConPTY(parent, cmd, flags, workingDir);
    }

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

    LOG_IF_TRACE { clDEBUG1() << "Running process:" << cmd << endl; }
    BOOL ret = FALSE;
    {
        ConsoleAttacher ca(prc->GetPid());
        ret = CreateProcess(NULL,
                            cmd.wchar_str(),   // shell line execution command
                            NULL,              // process security attributes
                            NULL,              // primary thread security attributes
                            TRUE,              // handles are inherited
                            creationFlags,     // creation flags
                            NULL,              // use parent's environment
                            NULL,              // CD to tmp dir
                            &siStartInfo,      // STARTUPINFO pointer
                            &prc->piProcInfo); // receives PROCESS_INFORMATION
    }

    if(ret) {
        prc->dwProcessId = prc->piProcInfo.dwProcessId;
    } else {
        int err = GetLastError();
        wxUnusedVar(err);
        wxDELETE(prc);
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

bool WinProcessImpl::Read(wxString& buff, wxString& buffErr, std::string& raw_buff, std::string& raw_buff_err)
{
    DWORD le1 = wxNOT_FOUND;
    DWORD le2 = wxNOT_FOUND;

    buff.clear();
    buffErr.clear();

    // Sanity
    if(!IsRedirect()) {
        return false;
    }

    // Read data from STDOUT and STDERR
    if(m_flags & IProcessStderrEvent) {
        // we want separate stderr events
        if(!DoReadFromPipe(hChildStderrRdDup, buffErr, raw_buff_err)) {
            le2 = GetLastError();
        }
    } else {
        if(!DoReadFromPipe(hChildStderrRdDup, buff, raw_buff)) {
            le2 = GetLastError();
        }
    }

    // read stdout
    if(!DoReadFromPipe(hChildStdoutRdDup, buff, raw_buff)) {
        le1 = GetLastError();
    }

    if((le1 == ERROR_NO_DATA) && (le2 == ERROR_NO_DATA)) {
        if(IsAlive()) {
            wxThread::Sleep(1);
            return true;
        }
    }
    bool success = !buff.empty() || !buffErr.empty();
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

inline void CLOSE_HANDLE(HANDLE h)
{
    if(h != INVALID_HANDLE_VALUE) {
        ::CloseHandle(h);
    }
}

void WinProcessImpl::Cleanup()
{
    if(m_writerThread) {
        m_writerThread->Stop();
        wxDELETE(m_writerThread);
    }

    if(m_hPseudoConsole) {
        ClosePseudoConsole(m_hPseudoConsole);
        m_hPseudoConsole = nullptr;
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
        CLOSE_HANDLE(hChildStdinRd);
        CLOSE_HANDLE(hChildStdinWrDup);
        CLOSE_HANDLE(hChildStdoutWr);
        CLOSE_HANDLE(hChildStdoutRdDup);
        CLOSE_HANDLE(hChildStderrWr);
        CLOSE_HANDLE(hChildStderrRdDup);
    }

    CLOSE_HANDLE(piProcInfo.hProcess);
    CLOSE_HANDLE(piProcInfo.hThread);

    hChildStdinRd = INVALID_HANDLE_VALUE;
    hChildStdoutRdDup = INVALID_HANDLE_VALUE;
    hChildStdoutWr = INVALID_HANDLE_VALUE;
    hChildStderrWr = INVALID_HANDLE_VALUE;
    hChildStdinWrDup = INVALID_HANDLE_VALUE;
    hChildStderrRdDup = INVALID_HANDLE_VALUE;
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

bool WinProcessImpl::DoReadFromPipe(HANDLE pipe, wxString& buff, std::string& raw_buff)
{
    DWORD dwRead = 0;
    DWORD dwMode;
    DWORD dwTimeout;

    if(pipe == INVALID_HANDLE_VALUE || pipe == 0x0) {
        SetLastError(ERROR_NO_DATA);
        return false;
    }

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    dwTimeout = 100;
    SetNamedPipeHandleState(pipe, &dwMode, NULL, &dwTimeout);

    bool read_something = false;
    while(true) {
        BOOL bRes = ReadFile(pipe, m_buffer, BUFFER_SIZE - 1, &dwRead, NULL);
        if(bRes && (dwRead > 0)) {
            wxString tmpBuff;
            tmpBuff.reserve(dwRead * 2); // make enough room for the conversion
            raw_buff.append(m_buffer, dwRead);

            // Success read
            tmpBuff = wxString(m_buffer, wxConvUTF8, dwRead);
            if(tmpBuff.IsEmpty() && dwRead > 0) {
                // conversion failed
                tmpBuff = wxString::From8BitData(m_buffer, dwRead);
            }
            buff.reserve(buff.size() + tmpBuff.size() + 1);
            buff.Append(tmpBuff);
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
    std::vector<INPUT_RECORD> pKeyEvents(pass.Len());

    for(size_t i = 0; i < pass.Len(); i++) {
        pKeyEvents[i].EventType = KEY_EVENT;
        pKeyEvents[i].Event.KeyEvent.bKeyDown = TRUE;
        pKeyEvents[i].Event.KeyEvent.wRepeatCount = 1;
        pKeyEvents[i].Event.KeyEvent.wVirtualKeyCode = LOBYTE(::VkKeyScan(pass[i]));
        pKeyEvents[i].Event.KeyEvent.wVirtualScanCode = 0;
        pKeyEvents[i].Event.KeyEvent.uChar.UnicodeChar = pass[i];
        pKeyEvents[i].Event.KeyEvent.dwControlKeyState = 0;
    }

    DWORD dwTextWritten;
    if(::WriteConsoleInput(hStdIn, pKeyEvents.data(), pass.Len(), &dwTextWritten) == FALSE) {
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
