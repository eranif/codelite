#include "svncommand.h"
#include "svn_console.h"
#include "globals.h"
#include "subversion2.h"

BEGIN_EVENT_TABLE(SvnCommand, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  SvnCommand::OnProcessOutput)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, SvnCommand::OnProcessTerminated)
END_EVENT_TABLE()

SvnCommand::SvnCommand()
		: m_process(NULL)
		, m_handler(NULL)
{
}

SvnCommand::~SvnCommand()
{
	ClearAll();
}

bool SvnCommand::Execute(const wxString& command, const wxString& workingDirectory, SvnCommandHandler *handler)
{
	ClearAll();

	// Wrap the command in the OS Shell
	wxString cmdShell (command);
	WrapInShell(cmdShell);
//	if(handler) {
//		handler->GetPlugin()->GetShell()->AppendText(command + wxT("\n"));
//	}
	m_process = CreateAsyncProcess(this, command, workingDirectory);
	if ( !m_process ) {
		return false;
	}
	m_workingDirectory = workingDirectory.c_str();
	m_command          = command.c_str();
	m_handler          = handler;
	return true;
}

void SvnCommand::OnProcessOutput(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData*)event.GetClientData();
	if( ped ) {
		m_output.Append(ped->GetData().c_str());
		delete ped;
	}
}

void SvnCommand::OnProcessTerminated(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData*)event.GetClientData();
	if( ped ) {
		delete ped;
	}

	if ( m_handler ) {

		m_handler->Process(m_output);
		delete m_handler;
	}
}

void SvnCommand::ClearAll()
{
	m_workingDirectory.Clear();
	m_command.Clear();
	m_output.Clear();
	if (m_process) {
		delete m_process;
		m_process = NULL;
	}
	m_handler = NULL;
}
