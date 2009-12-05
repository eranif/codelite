//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compiler_action.h
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
#ifndef COMPILER_ACTION_H
#define COMPILER_ACTION_H

#include "project.h"
#include "queuecommand.h"
#include "wx/event.h"

class IManager;
class IProcess;

extern const wxEventType wxEVT_SHELL_COMMAND_ADDLINE;
extern const wxEventType wxEVT_SHELL_COMMAND_STARTED;
extern const wxEventType wxEVT_SHELL_COMMAND_PROCESS_ENDED;
extern const wxEventType wxEVT_SHELL_COMMAND_STARTED_NOCLEAN;

/**
 * \class CompilerAction
 * \brief
 * \author Eran
 * \date 07/22/07
 */
class ShellCommand : public wxEvtHandler
{
protected:
	IProcess*     m_proc;
	wxEvtHandler *m_owner;
	wxArrayString m_lines;
	QueueCommand  m_info;

protected:
	virtual void DoPrintOutput      (const wxString &out);
	virtual void OnProcessOutput    (wxCommandEvent &e);
	virtual void OnProcessTerminated(wxCommandEvent &e);

	void CleanUp();
	void DoSetWorkingDirectory(ProjectPtr proj, bool isCustom, bool isFileOnly);

public:
	bool IsBusy() const {
		return m_proc != NULL;
	}

	void Stop();

	bool GetLines(wxArrayString &lines) {
		lines = m_lines;
		return true;
	}

	void SetInfo(const QueueCommand& info) {
		this->m_info = info;
	}
	const QueueCommand& GetInfo() const {
		return m_info;
	}
public:
	//construct a compiler action
	// \param owner the window owner for this action
	ShellCommand(wxEvtHandler *owner, const QueueCommand &buildInfo);

	virtual ~ShellCommand() {}
	virtual void Process(IManager *manager) = 0;

	void AppendLine(const wxString &line);
	void SendStartMsg();
	void SendEndMsg();
	DECLARE_EVENT_TABLE()
};
#endif
