#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#include <io.h>
#include <limits.h>
#include <string>

int ExecuteProcessWIN(const std::string& commandline)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    char* cmdline = strdup(commandline.c_str());
    CreateProcess(NULL, TEXT(cmdline), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD ret;
    GetExitCodeProcess(pi.hProcess, &ret);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return ret;
}

int run_child_process(int argc, char** argv, const std::string& commandline)
{
    (void)argc;
    (void)argv;
    return ExecuteProcessWIN(commandline);
}

void file_write_content(const std::string& logfile, const std::string& content)
{
    // Open the file
    HANDLE hFile =
        ::CreateFile(logfile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(!hFile)
        return;

    OVERLAPPED ov;
    memset(&ov, 0, sizeof(ov));
    ov.hEvent = 0x0;
    ov.Offset = UINT_MAX;
    // Obtain the lock over the file
    BOOL res = ::LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK,
                            0,        // must be 0
                            UINT_MAX, // number of bytes to lock
                            0,        // crap, set it 0
                            &ov);
    // int error = GetLastError();
    if(!res) {
        ::CloseHandle(hFile);
        return;
    }

    // Move the write pointer to the end
    ::SetFilePointer(hFile, 0, NULL, FILE_END);

    char cwd[1024];
    memset(cwd, 0, sizeof(cwd));
    ::getcwd(cwd, sizeof(cwd));

    std::string line = content + "\n";

    DWORD dwBytesWritten = 0;
    ::WriteFile(hFile, line.c_str(), line.length(), &dwBytesWritten, NULL);

    // Unlock the entire file
    ::UnlockFile(hFile, 0, 0, UINT_MAX, 0);
    ::CloseHandle(hFile);
}

#endif
