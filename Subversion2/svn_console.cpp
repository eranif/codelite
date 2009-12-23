#include <wx/app.h>
#include <wx/textdlg.h>
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
	m_sci->SetLexer(wxSCI_LEX_SVN);
	m_sci->StyleClearAll();

	for (int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		m_sci->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		m_sci->StyleSetForeground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	}

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	m_sci->StyleSetFont(0, font);

	m_sci->SetHotspotActiveUnderline (true);
	m_sci->SetHotspotActiveForeground(true, wxT("BLUE"));
	m_sci->SetHotspotSingleLine(true);
	m_sci->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
	m_sci->SetMarginMask(4, wxSCI_MASK_FOLDERS);

	m_sci->SetMarginWidth(0, 0);
	m_sci->SetMarginWidth(1, 0);
	m_sci->SetMarginWidth(2, 0);

	m_sci->SetWrapMode(wxSCI_WRAP_CHAR);
	m_sci->SetWrapStartIndent(4);
	m_sci->SetWrapVisualFlags(2);
	m_sci->SetScrollWidthTracking(true);

	/////////////////////////////////////////////////////////////////////////////
	// Set SVN styles
	/////////////////////////////////////////////////////////////////////////////
	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_INFO,     wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_INFO,     wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_ADDED,    wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_ADDED,    wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_MERGED,   wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_MERGED,   wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_UPDATED,  wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_UPDATED,  wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_CONFLICT, wxT("RED") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_CONFLICT, wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_DELETED,  wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_DELETED,  wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW ) );

	m_sci->StyleSetFont ( wxSCI_LEX_SVN_INFO,     font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_ADDED,    font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_MERGED,   font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_UPDATED,  font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_CONFLICT, font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_DELETED,  font );

	m_sci->SetReadOnly(true);
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

	wxString s (ped->GetData());
	s.MakeLower();

	if (m_printProcessOutput)
		AppendText( ped->GetData() );

	if (s.Contains(wxT("(r)eject, accept (t)emporarily or accept (p)ermanently"))) {
		AppendText( wxT("\n(Answering 'p')\n") );
		m_process->Write(wxT("p"));

		wxString message;
		message << wxT(" ***********************************************\n");
		message << wxT(" * MESSAGE:                                    *\n");
		message << wxT(" * Terminating SVN process.                    *\n");
		message << wxT(" * Please run cleanup from the Subversion View,*\n");
		message << wxT(" * And re-try again                            *\n");
		message << wxT(" ***********************************************\n");

		wxThread::Sleep(100);
		AppendText( message );
		m_process->Terminate();

	}
	delete ped;
}

void SvnConsole::OnProcessEnd(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	delete ped;

	if (m_handler) {

		if (m_handler->TestLoginRequired(m_output)) {
			// re-issue the last command but this time with login dialog
			m_handler->GetPlugin()->GetConsole()->AppendText(wxT("Authentication failed. Retrying...\n"));
			m_handler->ProcessLoginRequired(m_workingDirectory);

		} else if (m_handler->TestVerificationFailed(m_output)) {
			m_handler->GetPlugin()->GetConsole()->AppendText(wxT("Server certificate verification failed. Retrying...\n"));
			m_handler->ProcessVerificationRequired();

		} else {
			// command ended successfully, invoke the "success" callback
			m_handler->Process(m_output);
			AppendText(wxT("-----\n"));
		}


		delete m_handler;
		m_handler = NULL;
	}

	if ( m_process ) {
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
		if (handler)
			delete handler;
		return false;
	}

	m_output.Clear();
	m_handler = handler;

	EnsureVisible();

	// Print the command?
	if (printCommand)
		AppendText(cmd + wxT("\n"));

	// Wrap the command in the OS Shell
	wxString cmdShell (cmd);
	//WrapInShell(cmdShell);

	m_process = CreateAsyncProcess(this, cmdShell, workingDirectory);
	if (!m_process) {
		AppendText(wxT("Failed to launch Subversion client.\n"));
		return false;
	}
	m_workingDirectory = workingDirectory;
	return true;
}

void SvnConsole::AppendText(const wxString& text)
{
	m_sci->SetReadOnly(false);
	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());

	m_sci->AppendText(text);

	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());
	m_sci->EnsureCaretVisible();
	m_sci->SetReadOnly(true);
}

void SvnConsole::Clear()
{
	m_sci->SetReadOnly(false);
	m_sci->ClearAll();
	m_sci->SetReadOnly(true);
}

void SvnConsole::Stop()
{
	if (m_process) {
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
	return m_sci->GetText().IsEmpty();
}

void SvnConsole::EnsureVisible()
{
	// Make sure that the Output View pane is visible
	wxAuiPaneInfo &info = m_plugin->GetManager()->GetDockingManager()->GetPane(wxT("Output View"));
	if (info.IsOk() && !info.IsShown()) {
		info.Show();
		m_plugin->GetManager()->GetDockingManager()->Update();
	}

	// Select the Subversion tab
	Notebook *book = m_plugin->GetManager()->GetOutputPaneNotebook();
	size_t where = book->GetPageIndex(m_plugin->GetConsole());
	if (where != Notebook::npos) {
		book->SetSelection(where);
	}
}
