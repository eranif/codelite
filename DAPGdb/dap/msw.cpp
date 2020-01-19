#ifdef __WIN32__
#include "Process.hpp"
#include "StringUtils.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <windows.h>

using namespace std;
namespace dap
{
static bool CheckIsAlive(HANDLE hProcess)
{
    DWORD dwExitCode;
    if(GetExitCodeProcess(hProcess, &dwExitCode)) {
        if(dwExitCode == STILL_ACTIVE) return true;
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
        if(!WriteFile(hStdin, buffer.c_str() + offset, bytesLeft, &dwWritten, NULL)) { return false; }
        if(!CheckIsAlive(hProcess)) { return false; }
        if(dwWritten == 0) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
        bytesLeft -= dwWritten;
        offset += dwWritten;
        ++retryCount;
    }
    return true;
}

class ProcessMSW : public Process
{
public:
    HANDLE m_hChildStdinRd = NULL;
    HANDLE m_hChildStdinWr = NULL;
    HANDLE m_hChildStdinWrDup = NULL;
    HANDLE m_hChildStdoutRd = NULL;
    HANDLE m_hChildStdoutWr = NULL;
    HANDLE m_hChildStdoutRdDup = NULL;
    HANDLE m_hChildStderrRd = NULL;
    HANDLE m_hChildStderrWr = NULL;
    HANDLE m_hChildStderrRdDup = NULL;
    HANDLE m_hSaveStdin = NULL;
    HANDLE m_hSaveStdout = NULL;
    HANDLE m_hSaveStderr = NULL;

    DWORD m_dwProcessId = -1;
    PROCESS_INFORMATION m_piProcInfo;
    char m_buffer[65537];

protected:
    bool DoReadFromPipe(HANDLE pipe, string& buff);

public:
    ProcessMSW() {}
    virtual ~ProcessMSW() { Cleanup(); }
    bool Read(string& str, string& err_buff);
    bool Write(const string& str);
    bool IsAlive() const;
    void Cleanup();
    void Terminate();
};

Process* ExecuteProcess(const string& cmd, const string& workingDir, size_t flags)
{
    SECURITY_ATTRIBUTES saAttr;
    BOOL fSuccess;

    string wd(workingDir);

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // The steps for redirecting child process's STDOUT:
    //     1. Save current STDOUT, to be restored later.
    //     2. Create anonymous pipe to be STDOUT for child process.
    //     3. Set STDOUT of the parent process to be write handle to
    //        the pipe, so it is inherited by the child process.
    //     4. Create a noninheritable duplicate of the read handle and
    //        close the inheritable read handle.

    ProcessMSW* prc = new ProcessMSW();

    // Save the handle to the current STDOUT.
    prc->m_hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    // Create a pipe for the child process's STDOUT.
    if(!CreatePipe(&prc->m_hChildStdoutRd, &prc->m_hChildStdoutWr, &saAttr, 0)) {
        delete prc;
        return NULL;
    }

    // Set a write handle to the pipe to be STDOUT.
    if(!SetStdHandle(STD_OUTPUT_HANDLE, prc->m_hChildStdoutWr)) {
        delete prc;
        return NULL;
    }

    // Create noninheritable read handle and close the inheritable read handle.
    fSuccess = DuplicateHandle(GetCurrentProcess(), prc->m_hChildStdoutRd, GetCurrentProcess(),
                               &prc->m_hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
    if(!fSuccess) {
        delete prc;
        return NULL;
    }
    CloseHandle(prc->m_hChildStdoutRd);

    // The steps for redirecting child process's STDERR:
    //     1. Save current STDERR, to be restored later.
    //     2. Create anonymous pipe to be STDERR for child process.
    //     3. Set STDERR of the parent process to be write handle to
    //        the pipe, so it is inherited by the child process.
    //     4. Create a noninheritable duplicate of the read handle and
    //        close the inheritable read handle.

    // Save the handle to the current STDERR.
    prc->m_hSaveStderr = GetStdHandle(STD_ERROR_HANDLE);

    // Create a pipe for the child process's STDERR.
    if(!CreatePipe(&prc->m_hChildStderrRd, &prc->m_hChildStderrWr, &saAttr, 0)) {
        delete prc;
        return NULL;
    }

    // Set a write handle to the pipe to be STDERR.
    if(!SetStdHandle(STD_ERROR_HANDLE, prc->m_hChildStderrWr)) {
        delete prc;
        return NULL;
    }

    // Create noninheritable read handle and close the inheritable read handle.
    fSuccess = DuplicateHandle(GetCurrentProcess(), prc->m_hChildStderrRd, GetCurrentProcess(),
                               &prc->m_hChildStderrRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
    if(!fSuccess) {
        delete prc;
        return NULL;
    }
    CloseHandle(prc->m_hChildStderrRd);

    // The steps for redirecting child process's STDIN:
    //     1.  Save current STDIN, to be restored later.
    //     2.  Create anonymous pipe to be STDIN for child process.
    //     3.  Set STDIN of the parent to be the read handle to the
    //         pipe, so it is inherited by the child process.
    //     4.  Create a noninheritable duplicate of the write handle,
    //         and close the inheritable write handle.

    // Save the handle to the current STDIN.
    prc->m_hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

    // Create a pipe for the child process's STDIN.
    if(!CreatePipe(&prc->m_hChildStdinRd, &prc->m_hChildStdinWr, &saAttr, 0)) {
        delete prc;
        return NULL;
    }
    // Set a read handle to the pipe to be STDIN.
    if(!SetStdHandle(STD_INPUT_HANDLE, prc->m_hChildStdinRd)) {
        delete prc;
        return NULL;
    }
    // Duplicate the write handle to the pipe so it is not inherited.
    fSuccess =
        DuplicateHandle(GetCurrentProcess(), prc->m_hChildStdinWr, GetCurrentProcess(), &prc->m_hChildStdinWrDup, 0,
                        FALSE, // not inherited
                        DUPLICATE_SAME_ACCESS);
    if(!fSuccess) {
        delete prc;
        return NULL;
    }
    CloseHandle(prc->m_hChildStdinWr);

    // Execute the child process
    STARTUPINFO siStartInfo;

    // Set up members of STARTUPINFO structure.
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);

    siStartInfo.wShowWindow = flags & IProcessCreateConsole ? SW_SHOW : SW_HIDE;
    DWORD creationFlags = flags & IProcessCreateConsole ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW;

    if(flags & IProcessCreateWithHiddenConsole) {
        siStartInfo.wShowWindow = SW_HIDE;
        creationFlags = CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP;
    }

    siStartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    siStartInfo.hStdInput = prc->m_hChildStdinRd;
    siStartInfo.hStdOutput = prc->m_hChildStdoutWr;
    siStartInfo.hStdError = prc->m_hChildStdoutWr;
    siStartInfo.hStdError = prc->m_hChildStderrWr;

    BOOL ret = CreateProcess(NULL,
                             (char*)cmd.c_str(),  // shell line execution command
                             NULL,                // process security attributes
                             NULL,                // primary thread security attributes
                             TRUE,                // handles are inherited
                             creationFlags,       // creation flags
                             NULL,                // use parent's environment
                             NULL,                // CD to tmp dir
                             &siStartInfo,        // STARTUPINFO pointer
                             &prc->m_piProcInfo); // receives PROCESS_INFORMATION
    if(ret) {
        prc->m_dwProcessId = prc->m_piProcInfo.dwProcessId;
    } else {
        int err = GetLastError();
        (void)err;
        delete prc;
        return NULL;
    }

    // After process creation, restore the saved STDIN, STDOUT and STDERR
    if(!SetStdHandle(STD_INPUT_HANDLE, prc->m_hSaveStdin)) {
        delete prc;
        return NULL;
    }

    if(!SetStdHandle(STD_OUTPUT_HANDLE, prc->m_hSaveStdout)) {
        delete prc;
        return NULL;
    }

    if(!SetStdHandle(STD_OUTPUT_HANDLE, prc->m_hSaveStderr)) {
        delete prc;
        return NULL;
    }
    return prc;
}

bool ProcessMSW::DoReadFromPipe(HANDLE pipe, string& buff)
{
    DWORD dwRead;
    DWORD dwMode;
    DWORD dwTimeout;
    memset(m_buffer, 0, sizeof(m_buffer));

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    dwTimeout = 1000;
    SetNamedPipeHandleState(pipe, &dwMode, NULL, &dwTimeout);

    bool read_something = false;
    while(true) {
        BOOL bRes = ReadFile(pipe, m_buffer, sizeof(m_buffer), &dwRead, NULL);
        if(bRes) {
            string tmpBuff;
            // Success read
            m_buffer[dwRead / sizeof(char)] = 0;
            tmpBuff = m_buffer;
            buff += tmpBuff;
            read_something = true;
            continue;
        }
        break;
    }
    return read_something;
}

bool ProcessMSW::Read(string& buff, string& buffErr)
{
    DWORD le1(-1);
    DWORD le2(-1);
    buff.clear();
    buffErr.clear();

    // Read data from STDOUT and STDERR
    if(!DoReadFromPipe(m_hChildStderrRdDup, buffErr)) { le2 = GetLastError(); }
    if(!DoReadFromPipe(m_hChildStdoutRdDup, buff)) { le1 = GetLastError(); }
    if((le1 == ERROR_NO_DATA) && (le2 == ERROR_NO_DATA)) {
        if(IsAlive()) {
            this_thread::sleep_for(chrono::milliseconds(15));
            return true;
        }
    }
    bool success = !buff.empty() || !buffErr.empty();
    return success;
}

bool ProcessMSW::Write(const string& buff)
{
    DWORD dwMode;
    DWORD dwTimeout;
    char chBuf[4097];

    string tmpCmd = buff;
    StringUtils::Rtrim(tmpCmd);
    tmpCmd += "\n";
    strcpy(chBuf, tmpCmd.c_str());

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    dwTimeout = 30000;
    UNUSED(dwTimeout);
    SetNamedPipeHandleState(m_hChildStdinWrDup, &dwMode, NULL,
                            NULL); // Timeout of 30 seconds
    return WriteStdin(buff, m_hChildStdinWrDup, m_piProcInfo.hProcess);
}

bool ProcessMSW::IsAlive() const { return CheckIsAlive(m_piProcInfo.hProcess); }

void ProcessMSW::Terminate()
{
    // terminate and perform cleanup
    Cleanup();
}

void ProcessMSW::Cleanup()
{
    if(IsAlive()) TerminateProcess(m_piProcInfo.hProcess, 255);

    if(m_hChildStdinRd) { CloseHandle(m_hChildStdinRd); }
    if(m_hChildStdinWrDup) { CloseHandle(m_hChildStdinWrDup); }
    if(m_hChildStdoutWr) { CloseHandle(m_hChildStdoutWr); }
    if(m_hChildStdoutRdDup) { CloseHandle(m_hChildStdoutRdDup); }
    if(m_hChildStderrWr) { CloseHandle(m_hChildStderrWr); }
    if(m_hChildStderrRdDup) { CloseHandle(m_hChildStderrRdDup); }
    if(m_piProcInfo.hProcess) { CloseHandle(m_piProcInfo.hProcess); }
    if(m_piProcInfo.hThread) { CloseHandle(m_piProcInfo.hThread); }

    m_hChildStdinRd = NULL;
    m_hChildStdoutWr = NULL;
    m_hChildStdinWrDup = NULL;
    m_hChildStdoutRdDup = NULL;
    m_hChildStderrWr = NULL;
    m_hChildStderrRdDup = NULL;
    m_piProcInfo.hProcess = NULL;
    m_piProcInfo.hThread = NULL;
}
};     // namespace dap
#endif //__WXMSW__
