#ifndef __winprocess__
#define __winprocess__

#ifdef __WXMSW__

#include <wx/string.h>
#include <Windows.h>

class WinProcess
{
public:
	WinProcess();
	virtual ~WinProcess();

	// Create process asynchronously and return a process object
	static WinProcess* Execute(const wxString& cmd, wxString &errMsg, const wxString &workingDir = wxEmptyString);

	// Read from process stdout - return immediately if no data is available
	bool Read(wxString& buff);

	// Write to the process stdin
	bool Write(const wxString& buff);

	// Return true if the process is still alive
	bool IsAlive();

	// Clean the process resources and kill the process if it is
	// still alive
	void Cleanup();
	
private:
	// WINDOWS implementation
	// Creating process related handles
	HANDLE	hChildStdinRd, hChildStdinWr, hChildStdinWrDup, 
			hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup, 
			hSaveStdin, hSaveStdout;

	// Child process id & information 
	DWORD dwProcessId;
	PROCESS_INFORMATION piProcInfo;
};

#endif

#endif // __winprocess__
