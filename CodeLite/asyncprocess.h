//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : asyncprocess.h
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

#ifndef I_PROCESS_H
#define I_PROCESS_H

#include <wx/string.h>
#include <wx/event.h>
#include "codelite_exports.h"
#include <map>

enum IProcessCreateFlags {
	IProcessCreateDefault             = 0x0000001, // Default: create process with no console window
	IProcessCreateConsole             = 0x0000002, // Create with console window shown
	IProcessCreateWithHiddenConsole   = 0x0000004  // Create process with a hidden console
};

/**
 * @class IProcess
 * @author eran
 * @date 10/09/09
 * @file i_process.h
 * @brief
 */
class WXDLLIMPEXP_CL IProcess
{
protected:
	wxEvtHandler *            m_parent;
	int                       m_pid;
	bool                      m_hardKill;
	static std::map<int, int> m_exitCodeMap;
	static wxCriticalSection  m_cs;

public:
	IProcess(wxEvtHandler *parent) : m_parent(parent), m_pid(-1), m_hardKill(false) {}
	virtual ~IProcess() {}

	// Handle process exit code. This is done this way this
	// under Linux / Mac the exit code is returned only after the signal child has been
	// handled by codelite
	static void SetProcessExitCode(int pid, int exitCode);
	static bool GetProcessExitCode(int pid, int &exitCode);

	// Read from process stdout - return immediately if no data is available
	virtual bool Read(wxString& buff) = 0;

	// Write to the process stdin
	virtual bool Write(const wxString& buff) = 0;

	/**
	 * @brief this method is mostly needed on MSW where writing a password
	 * is done directly on the console buffer rather than its stdin
	 */
	virtual bool WriteToConsole(const wxString &buff) = 0;

	// Return true if the process is still alive
	virtual bool IsAlive() = 0;

	// Clean the process resources and kill the process if it is
	// still alive
	virtual void Cleanup() = 0;

	// Terminate the process. It is recommended to use this method
	// so it will invoke the 'Cleaup' procedure and the process
	// termination event will be sent out
	virtual void Terminate() = 0;

	void SetPid(int pid) {
		this->m_pid = pid;
	}

	int GetPid() const {
		return m_pid;
	}

	void SetHardKill(bool hardKill) {
		this->m_hardKill = hardKill;
	}
	bool GetHardKill() const {
		return m_hardKill;
	}
};

// Help method
WXDLLIMPEXP_CL IProcess* CreateAsyncProcess(wxEvtHandler *parent, const wxString& cmd, IProcessCreateFlags flags = IProcessCreateDefault, const wxString &workingDir = wxEmptyString);
#endif // I_PROCESS_H
