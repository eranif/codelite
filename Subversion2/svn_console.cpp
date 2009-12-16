#include "svn_console.h"
#include "subversion_strings.h"
#include <wx/tokenzr.h>
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
		, m_printProcessOutput(true)
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
	if(m_printProcessOutput)
		AppendText( ped->GetData() );

	delete ped;
}

void SvnConsole::OnProcessEnd(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	delete ped;

	if (m_handler) {

		if(m_handler->TestLoginRequired(m_output)) {
			// re-issue the last command but this time with login dialog
			m_handler->GetPlugin()->GetConsole()->AppendText(wxT("Authentication failed. Retrying...\n"));
			m_handler->ProcessLoginRequired();

		} else {
			// command ended successfully, invoke the "success" callback
			m_handler->Process(m_output);
			AppendText(wxT("-----\n"));
		}

		delete m_handler;
		m_handler = NULL;
	}

	if( m_process ) {
		delete m_process;
		m_process = NULL;
	}
}

bool SvnConsole::Execute(const wxString& cmd, const wxString& workingDirectory, SvnCommandHandler* handler, bool printCommand, bool printProcessOutput)
{
	m_printProcessOutput = printProcessOutput;
	if (m_process) {
		// another process is already running...
		//AppendText(svnANOTHER_PROCESS_RUNNING);
		if(handler)
			delete handler;
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
	size_t where = book->GetPageIndex(m_plugin->GetConsole());
	if(where != Notebook::npos) {
		book->SetSelection(where);
	}

	// Print the command?
	if(printCommand)
		AppendText(cmd + wxT("\n"));

	// Wrap the command in the OS Shell
	wxString cmdShell (cmd);
	//WrapInShell(cmdShell);

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

	// Call scrolllines with the number of lines added
	// +1
	// Count number of newlines (i.e lines)
    int lines = 0;
    const wxChar* cstr = text.c_str();
    for( ; *cstr ; ++cstr )
        if( *cstr == wxT('\n') )
            ++lines;

	m_textCtrlOutput->ScrollLines(lines + 1);
	m_textCtrlOutput->ShowPosition(m_textCtrlOutput->GetLastPosition());
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
	AppendText(wxT("--------\n"));
}

bool SvnConsole::IsRunning()
{
	return m_process != NULL;
}

bool SvnConsole::IsEmpty()
{
	return m_textCtrlOutput->IsEmpty();
}
