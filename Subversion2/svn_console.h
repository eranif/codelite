#ifndef SVNSHELL_H
#define SVNSHELL_H

#include "subversion2_ui.h"
#include "svncommandhandler.h"

class IProcess;
class Subversion2;

class SvnConsole : public SvnShellBase
{
	SvnCommandHandler *m_handler;
	wxString           m_output;
	IProcess*          m_process;
	Subversion2*       m_plugin;
	bool               m_printProcessOutput;
	
protected:
	DECLARE_EVENT_TABLE()
	virtual void OnReadProcessOutput(wxCommandEvent& event);
	virtual void OnProcessEnd       (wxCommandEvent& event);

public:
	SvnConsole(wxWindow *parent, Subversion2* plugin);
	virtual ~SvnConsole();

	bool Execute(const wxString &cmd, const wxString &workingDirectory, SvnCommandHandler *handler, bool printCommand = true, bool printProcessOutput = true);
	void Clear  ();
	void Stop   ();
	void AppendText(const wxString &text);
	bool IsRunning();
	bool IsEmpty();
};
#endif // SVNSHELL_H
