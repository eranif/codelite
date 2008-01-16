#ifndef CONSOLEFINDER_H
#define CONSOLEFINDER_H

#include "precompiled_header.h"

/**
 * \ingroup SDK
 * a misc class that allows user to locate a free pty terminal
 * this is used by the debugger
 * \version 1.0
 * first version
 *
 * \date 10-14-2007
 *
 * \author Eran
 *
 */
class ConsoleFinder {
	wxString m_ConsoleTty;
	int m_nConsolePid;
	
private:
	int RunConsole(const wxString &title);
	wxString GetConsoleTty(int ConsolePid);
	
public:	
	ConsoleFinder();
	virtual ~ConsoleFinder();

	bool FindConsole(const wxString &title, wxString &consoleName);
	void FreeConsole();
	wxString GetConsoleName();
};

#endif 

