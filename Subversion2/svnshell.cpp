#include "svnshell.h"
#include "processreaderthread.h"

SvnShell::SvnShell(wxWindow *parent)
		: wxTerminal(parent)
		, m_handler(NULL)
{
	SetReadOnly(true);
	SetPromptFormat(wxT("%w>"));
}

SvnShell::~SvnShell()
{
}

void SvnShell::OnReadProcessOutput(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	if (ped) {
		m_output.Append(ped->GetData().c_str());
	}

	wxTerminal::OnReadProcessOutput(event);
}

void SvnShell::OnProcessEnd(wxCommandEvent& event)
{
	wxTerminal::OnProcessEnd(event);
	if (m_handler) {
		m_handler->Process(m_output);
		delete m_handler;
		m_handler = NULL;
	}
}

bool SvnShell::Run(const wxString& cmd, const wxString& workingDirectory, SvnCommandHandler* handler)
{
	if (IsRunning()) {
		return false;
	}

	m_output.Clear();
	m_handler = handler;

	Clear();
	SetReadOnly(false);

	SetWorkingDirectory(workingDirectory);
	wxTerminal::Execute(cmd);

	SetReadOnly(true);
	return true;
}
