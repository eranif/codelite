#ifdef _WIN32
#include <Windows.h>
#include <string>
#include <conio.h>

int ExecuteProcessWIN(const std::string& commandline)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process.
    char* cmdline = strdup(commandline.c_str());
	CreateProcess( NULL, TEXT(cmdline), NULL, NULL, FALSE, 0,
	               NULL, NULL, &si, &pi );

	// Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
    DWORD ret;
	GetExitCodeProcess( pi.hProcess, &ret );
    CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
    return ret;
}

#endif
