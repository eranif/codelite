#include <wx/app.h>
#include "environmentconfig.h"
#include <wx/textdlg.h>
#include "svn_console.h"
#include "subversion_strings.h"
#include <wx/tokenzr.h>
#include <wx/aui/framemanager.h>
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "processreaderthread.h"
#include "subversion2.h"

//-------------------------------------------------------------
BEGIN_EVENT_TABLE(SvnConsole, SvnShellBase)
	EVT_COMMAND      (wxID_ANY, wxEVT_PROC_DATA_READ,  SvnConsole::OnReadProcessOutput)
	EVT_COMMAND      (wxID_ANY, wxEVT_PROC_TERMINATED, SvnConsole::OnProcessEnd       )
	EVT_SCI_UPDATEUI (wxID_ANY, SvnConsole::OnUpdateUI)
	EVT_SCI_CHARADDED(wxID_ANY, SvnConsole::OnCharAdded)
END_EVENT_TABLE()

SvnConsole::SvnConsole(wxWindow *parent, Subversion2* plugin)
		: SvnShellBase(parent)
		, m_handler(NULL)
		, m_process(NULL)
		, m_plugin (plugin)
		, m_printProcessOutput(true)
		, m_inferiorEnd(0)
{
	m_sci->SetLexer(wxSCI_LEX_SVN);
	m_sci->StyleClearAll();
	m_sci->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(SvnConsole::OnKeyDown), NULL, this);

	for (int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		m_sci->StyleSetBackground(i, DrawingUtils::GetTextCtrlBgColour());
		m_sci->StyleSetForeground(i, DrawingUtils::GetTextCtrlTextColour());
	}

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	m_sci->StyleSetFont(0, font);

	m_sci->SetHotspotActiveUnderline (true);
	m_sci->SetHotspotActiveForeground(true, wxT("BLUE"));
	m_sci->SetHotspotSingleLine(true);

	m_sci->SetMarginWidth(0, 0);
	m_sci->SetMarginWidth(1, 0);
	m_sci->SetMarginWidth(2, 0);
	m_sci->SetMarginWidth(4, 16);

	m_sci->SetWrapMode(wxSCI_WRAP_CHAR);
	m_sci->SetWrapStartIndent(4);
	m_sci->SetWrapVisualFlags(2);
	m_sci->SetScrollWidthTracking(true);
	m_sci->SetCaretWidth(2);
	
	/////////////////////////////////////////////////////////////////////////////
	// Set SVN styles
	/////////////////////////////////////////////////////////////////////////////
	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_INFO,     wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_INFO,     DrawingUtils::GetTextCtrlBgColour() );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_ADDED,    wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_ADDED,    DrawingUtils::GetTextCtrlBgColour() );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_MERGED,   wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_MERGED,   DrawingUtils::GetTextCtrlBgColour() );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_UPDATED,  wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_UPDATED,  DrawingUtils::GetTextCtrlBgColour() );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_CONFLICT, wxT("RED") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_CONFLICT, DrawingUtils::GetTextCtrlBgColour() );

	m_sci->StyleSetForeground ( wxSCI_LEX_SVN_DELETED,  wxT("FOREST GREEN") );
	m_sci->StyleSetBackground ( wxSCI_LEX_SVN_DELETED,  DrawingUtils::GetTextCtrlBgColour() );

	m_sci->StyleSetFont ( wxSCI_LEX_SVN_INFO,     font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_ADDED,    font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_MERGED,   font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_UPDATED,  font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_CONFLICT, font );
	m_sci->StyleSetFont ( wxSCI_LEX_SVN_DELETED,  font );

	//m_sci->SetReadOnly(true);
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

	wxArrayString lines = wxStringTokenize(s, wxT("\n"), wxTOKEN_STRTOK);
	if(lines.IsEmpty() == false && lines.Last().StartsWith(wxT("password for '"))) {
		m_output.Clear();
		wxString pass = wxGetPasswordFromUser(ped->GetData(), wxT("Subversion"));
		if(!pass.IsEmpty() && m_process) {
			m_process->WriteToConsole(pass);
		}
	}
	delete ped;
}

void SvnConsole::OnProcessEnd(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	delete ped;

	if ( m_process ) {
		delete m_process;
		m_process = NULL;
	}

	if (m_handler) {
		// command ended successfully, invoke the "success" callback
		m_handler->Process(m_output);
		AppendText(wxT("-----\n"));

		delete m_handler;
		m_handler = NULL;
	}

	m_output.Clear();
	m_workingDirectory.Clear();
	m_url.Clear();
}

bool SvnConsole::ExecuteURL(const wxString& cmd, const wxString& url, SvnCommandHandler* handler, bool printProcessOutput)
{
	if(!DoExecute(cmd, handler, wxT(""), printProcessOutput))
		return false;

	m_url = url;
	return true;
}

bool SvnConsole::Execute(const wxString& cmd, const wxString& workingDirectory, SvnCommandHandler* handler, bool printProcessOutput)
{
	if(!DoExecute(cmd, handler, workingDirectory, printProcessOutput))
		return false;

	m_workingDirectory = workingDirectory;
	return true;
}

void SvnConsole::AppendText(const wxString& text)
{
	// Make sure that the carete is at the end
	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());

	// Append the text
	m_sci->AppendText(text);

	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());

	m_sci->EnsureCaretVisible();
	m_inferiorEnd = m_sci->GetLength();
}

void SvnConsole::Clear()
{
	m_sci->ClearAll();
	m_inferiorEnd = 0;
}

void SvnConsole::Stop()
{
	if (m_process) {
		delete m_process;
		m_process = NULL;
	}
	AppendText(_("Aborted.\n"));
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
	wxBookCtrlBase *book = m_plugin->GetManager()->GetOutputPaneNotebook();

	for(size_t i=0; i<book->GetPageCount(); i++) {
		if(book->GetPage(i) == m_plugin->GetConsole()) {
			book->SetSelection(i);
		}
	}
}

bool SvnConsole::DoExecute(const wxString& cmd, SvnCommandHandler* handler, const wxString &workingDirectory, bool printProcessOutput)
{
	m_workingDirectory.Clear();
	m_url.Clear();

	m_printProcessOutput = printProcessOutput;
	if (m_process) {
		// another process is already running...
		AppendText(svnANOTHER_PROCESS_RUNNING);
		if (handler)
			delete handler;
		return false;
	}

	m_output.Clear();
	m_handler = handler;

	EnsureVisible();

	// Print the command?
	AppendText(cmd + wxT("\n"));

	// Wrap the command in the OS Shell
	wxString cmdShell (cmd);

	// Apply the environment variables before executing the command
	StringMap om;
	om[wxT("LC_ALL")] = wxT("C");

	bool useOverrideMap = m_plugin->GetSettings().GetFlags() & SvnUsePosixLocale;
	EnvSetter env(m_plugin->GetManager()->GetEnv(), useOverrideMap ? &om : NULL);

	m_process = CreateAsyncProcess(this, cmdShell, IProcessCreateWithHiddenConsole, workingDirectory);
	if (!m_process) {
		AppendText(_("Failed to launch Subversion client.\n"));
		return false;
	}
	m_sci->SetFocus();
	return true;
}

void SvnConsole::OnCharAdded(wxScintillaEvent& event)
{
	if(event.GetKey() == '\n') {
		// an enter was inserted
		// take the last inserted line and send it to the m_process
		wxString line = m_sci->GetTextRange(m_inferiorEnd, m_sci->GetLength());
		line.Trim();

		if(m_process) {
			m_process->Write(line);
		}
	}
	event.Skip();
}

void SvnConsole::OnUpdateUI(wxScintillaEvent& event)
{
	//if(m_sci->GetCurrentPos() < m_inferiorEnd) {
	//	m_sci->SetCurrentPos(m_inferiorEnd);
	//	m_sci->SetSelectionStart(m_inferiorEnd);
	//	m_sci->SetSelectionEnd(m_inferiorEnd);
	//	m_sci->EnsureCaretVisible();
	//}
	event.Skip();
}

void SvnConsole::OnKeyDown(wxKeyEvent& event)
{
	event.Skip();
}
