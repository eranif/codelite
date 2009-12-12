#include "svn_console.h"
#include <wx/aui/framemanager.h>
#include <wx/xrc/xmlres.h>
#include "processreaderthread.cpp"
#include "globals.h"
#include "processreaderthread.h"
#include "subversion2.h"

//-------------------------------------------------------------
BEGIN_EVENT_TABLE(SvnConsole, SvnShellBase)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  SvnConsole::OnReadProcessOutput)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, SvnConsole::OnProcessEnd       )
END_EVENT_TABLE()

SvnConsole::SvnConsole(wxWindow *parent, Subversion2* plugin)
		: SvnShellBase(parent)
		, m_handler(NULL)
		, m_process(NULL)
		, m_plugin (plugin)
{
}

SvnConsole::~SvnConsole()
{
}

void SvnConsole::OnReadProcessOutput(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	if (ped) {
		m_output.Append(ped->GetData().c_str());
	}

	// print the output
	AppendText( ped->GetData() );
	delete ped;
}

void SvnConsole::OnProcessEnd(wxCommandEvent& event)
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
	AppendText(wxT("Done.\n"));
	AppendText(wxT("-----\n"));
}

bool SvnConsole::Execute(const wxString& cmd, const wxString& workingDirectory, SvnCommandHandler* handler, bool printCommand)
{
	if (m_process) {
		// another process is already running...
		AppendText(wxT("MESSAGE: Another process is currently running...\n"));
		return false;
	}

	m_output.Clear();
	m_handler = handler;
	
	// Make sure that the Output View pane is visible
	wxAuiPaneInfo &info = m_plugin->GetManager()->GetDockingManager()->GetPane(wxT("Output View"));
	if (info.IsOk() && !info.IsShown()) {
		info.Show();
		m_plugin->GetManager()->GetDockingManager()->Update();
	}
	
	// Select the Subversion tab
	Notebook *book = m_plugin->GetManager()->GetOutputPaneNotebook();
	size_t where = book->GetPageIndex(m_plugin->GetShell());
	if(where != Notebook::npos) {
		book->SetSelection(where);
	}
	
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

void SvnConsole::AppendText(const wxString& text)
{
	m_textCtrlOutput->SetInsertionPointEnd();
	m_textCtrlOutput->SetSelection(m_textCtrlOutput->GetLastPosition(), m_textCtrlOutput->GetLastPosition());
	m_textCtrlOutput->AppendText(text);
}

void SvnConsole::Clear()
{
	m_textCtrlOutput->Clear();
}

void SvnConsole::Stop()
{
	if(m_process) {
		delete m_process;
		m_process = NULL;
	}
	AppendText(wxT("Aborted.\n"));
}

