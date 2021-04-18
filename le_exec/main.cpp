#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define wait_key getch
#include <iomanip>
#include <iostream>
#include <string.h>

bool hasSpaces(const char* str)
{
    char last = 0;
    while(str && *str) {
        if((*str == ' ' || *str == '\t') && last != '\\')
            return true;
        last = *str++;
    }
    return false;
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        std::cout << "Usage: codelite-exec <program_name> [arguments...]" << std::endl;
        return 1;
    }

    // count size of arguments
    int fullsize = 0;
    for(int i = 1; i < argc; ++i) {
        fullsize += strlen(argv[i]);
    }
    // add some slack for spaces between args plus quotes around executable
    fullsize += argc + 32;

    char* cmdline = new char[fullsize];
    memset(cmdline, 0, fullsize);

    // 1st arg (executable) enclosed in quotes to support filenames with spaces
    bool sp = hasSpaces(argv[1]);
    if(sp)
        strcat(cmdline, "\"");
    strcat(cmdline, argv[1]);
    if(sp)
        strcat(cmdline, "\"");
    strcat(cmdline, " ");

    for(int i = 2; i < argc; ++i) {
        sp = hasSpaces(argv[i]);
        if(sp)
            strcat(cmdline, "\"");
        strcat(cmdline, argv[i]);
        if(sp)
            strcat(cmdline, "\"");
        strcat(cmdline, " ");
    }

    // Windows's system() seems to not be able to handle parentheses in
    // the path, so we have to launch the program a different way.

    SetConsoleTitle(cmdline);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    DWORD tickStart = ::GetTickCount();
    // Start the child process.
    CreateProcess(NULL, TEXT(cmdline), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD tickEnd = ::GetTickCount();

    size_t totalElpasedInMS = tickEnd - tickStart;
    size_t seconds = totalElpasedInMS / 1000;
    size_t ms = totalElpasedInMS % 1000;
    size_t minutes = seconds / 60;
    seconds = seconds % 60;

    // Get the return value of the child process
    DWORD ret;
    GetExitCodeProcess(pi.hProcess, &ret);

    std::cout << "\r\n==== Program exited with exit code: " << ret << " ====" << std::endl;
    std::cout << "Time elapsed: " << std::setfill('0') << std::setw(3) << minutes << ":" << std::setw(2) << seconds
              << "." << std::setw(3) << ms << " (MM:SS.MS) " << std::endl;
    std::cout << "Press any key to continue..." << std::endl;
    wait_key();

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    delete[] cmdline;
    return ret;
}
