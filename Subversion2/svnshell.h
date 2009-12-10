#ifndef SVNSHELL_H
#define SVNSHELL_H

#include "wxterminal.h"
#include "svncommandhandler.h"

class SvnShell : public wxTerminal
{
	SvnCommandHandler *m_handler;
	wxString           m_output;
public:
	SvnShell(wxWindow *parent);
	virtual ~SvnShell();
	virtual void OnReadProcessOutput(wxCommandEvent& event);
	virtual void OnProcessEnd(wxCommandEvent& event);
	virtual bool Run(const wxString &cmd, const wxString &workingDirectory, SvnCommandHandler *handler);
};
#endif // SVNSHELL_H
