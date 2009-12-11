#ifndef SVNSHELL_H
#define SVNSHELL_H

#include "subversion2_ui.h"
#include "svncommandhandler.h"

class IProcess;
class SvnShell : public SvnShellBase
{
	SvnCommandHandler *m_handler;
	wxString           m_output;
	IProcess*          m_process;

protected:
	void AppendText(const wxString &text);

	DECLARE_EVENT_TABLE()
	virtual void OnReadProcessOutput(wxCommandEvent& event);
	virtual void OnProcessEnd       (wxCommandEvent& event);

public:
	SvnShell(wxWindow *parent);
	virtual ~SvnShell();

	bool Execute(const wxString &cmd, const wxString &workingDirectory, SvnCommandHandler *handler, bool printCommand = true);
	void Clear  ();
	void Stop   ();
};
#endif // SVNSHELL_H
