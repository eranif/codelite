#ifndef SVNCOMMAND_H
#define SVNCOMMAND_H

#include <wx/event.h> // Base class
#include "svncommandhandler.h"
#include "asyncprocess.h"
#include "processreaderthread.h"

class SvnCommand : public wxEvtHandler
{
	IProcess *         m_process;
	wxString           m_command;
	wxString           m_workingDirectory;
	SvnCommandHandler *m_handler;
	wxString           m_output;

protected:

	DECLARE_EVENT_TABLE();

	// Event handlers
	void OnProcessTerminated(wxCommandEvent &event);
	void OnProcessOutput    (wxCommandEvent &event);

public:
	SvnCommand();
	virtual ~SvnCommand();

	bool Execute(const wxString &command, const wxString &workingDirectory, SvnCommandHandler *handler);

	void SetProcess(IProcess* process) {
		this->m_process = process;
	}

	void SetWorkingDirectory(const wxString& workingDirectory) {
		this->m_workingDirectory = workingDirectory;
	}

	const wxString& GetCommand() const {
		return m_command;
	}

	IProcess* GetProcess() {
		return m_process;
	}

	const wxString& GetWorkingDirectory() const {
		return m_workingDirectory;
	}

	void ClearAll();
	bool IsBusy() {
		return m_process != NULL;
	}
};

#endif // SVNCOMMAND_H
