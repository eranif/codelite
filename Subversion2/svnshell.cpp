#include "svnshell.h"
#include "processreaderthread.cpp"
#include "globals.h"
#include "processreaderthread.h"

//-------------------------------------------------------------
BEGIN_EVENT_TABLE(SvnShell, SvnShellBase)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  SvnShell::OnReadProcessOutput)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, SvnShell::OnProcessEnd       )
END_EVENT_TABLE()

SvnShell::SvnShell(wxWindow *parent)
		: SvnShellBase(parent)
		, m_handler(NULL)
		, m_process(NULL)
{
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

	// print the output
	AppendText( ped->GetData() );
	delete ped;
}

void SvnShell::OnProcessEnd(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	delete ped;

	if (m_handler) {
		m_handler->Process(m_output);
		delete m_handler;
		m_handler = NULL;
	}

	if( m_process ) {
		delete m_process;
		m_process = NULL;
	}
}

bool SvnShell::Execute(const wxString& cmd, const wxString& workingDirectory, SvnCommandHandler* handler, bool printCommand)
{
	if (m_process) {
		// another process is already running...
		AppendText(wxT("MESSAGE: Another process is currently running...\n"));
		return false;
	}

	m_output.Clear();
	m_handler = handler;

	// Print the command?
	if(printCommand)
		AppendText(cmd + wxT("\n"));

	// Wrap the command in the OS Shell
	wxString cmdShell (cmd);
	WrapInShell(cmdShell);

	m_process = CreateAsyncProcess(this, cmdShell, workingDirectory);
	if(!m_process) {
		AppendText(wxT("Failed to launch Subversion client.\n"));
		return false;
	}
	return true;
}

void SvnShell::AppendText(const wxString& text)
{
	m_textCtrlOutput->SetInsertionPointEnd();
	m_textCtrlOutput->SetSelection(m_textCtrlOutput->GetLastPosition(), m_textCtrlOutput->GetLastPosition());
	m_textCtrlOutput->AppendText(wxString::Format(wxT(">%s"), text.c_str()));
}

void SvnShell::Clear()
{
	m_textCtrlOutput->Clear();
}

void SvnShell::Stop()
{
	if(m_process) {
		delete m_process;
		m_process = NULL;
	}
	AppendText(wxT("Aborted.\n"));
}
