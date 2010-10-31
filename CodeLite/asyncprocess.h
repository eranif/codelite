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

enum IProcessCreateFlags {
	IProcessCreateDefault = 0x0000000, // Default: create process with no console window
	IProcessCreateConsole = 0x0000001  // Create with console window shown
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
	wxEvtHandler *m_parent;
	int           m_pid;
	int           m_exitCode;
	bool          m_hardKill;

public:
	IProcess(wxEvtHandler *parent) : m_parent(parent), m_pid(-1), m_exitCode(0), m_hardKill(false) {}
	virtual ~IProcess() {}

	// Read from process stdout - return immediately if no data is available
	virtual bool Read(wxString& buff) = 0;

	// Write to the process stdin
	virtual bool Write(const wxString& buff) = 0;

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

	int GetExitCode() const {
		return m_exitCode;
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
