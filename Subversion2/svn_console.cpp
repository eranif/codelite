#include <wx/app.h>
#include "environmentconfig.h"
#include "editor_config.h"
#include <wx/textdlg.h>
#include "svn_console.h"
#include "subversion_strings.h"
#include <wx/tokenzr.h>
#include <wx/aui/framemanager.h>
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "processreaderthread.h"
#include "subversion2.h"
#include "lexer_configuration.h"
#include "notebook_ex.h"

//-------------------------------------------------------------
BEGIN_EVENT_TABLE(SvnConsole, SvnShellBase)
	EVT_COMMAND      (wxID_ANY, wxEVT_PROC_DATA_READ,  SvnConsole::OnReadProcessOutput)
	EVT_COMMAND      (wxID_ANY, wxEVT_PROC_TERMINATED, SvnConsole::OnProcessEnd       )
	EVT_SCI_UPDATEUI (wxID_ANY, SvnConsole::OnUpdateUI)
	EVT_SCI_CHARADDED(wxID_ANY, SvnConsole::OnCharAdded)
END_EVENT_TABLE()

SvnConsole::SvnConsole(wxWindow *parent, Subversion2* plugin)
		: SvnShellBase(parent)
		, m_process(NULL)
		, m_plugin (plugin)
		, m_inferiorEnd(0)
{
	m_sci->SetLexer(wxSCI_LEX_SVN);
	m_sci->StyleClearAll();
	m_sci->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(SvnConsole::OnKeyDown), NULL, this);
	
	DoInitializeFontsAndColours();
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

	if (m_currCmd.printProcessOutput)
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
	
	if(m_currCmd.handler) {
		// command ended successfully, invoke the "success" callback
		m_currCmd.handler->Process(m_output);
		AppendText(wxT("-----\n"));
		delete m_currCmd.handler;
	}
	
	// do we have more commands to process?
	if(!m_queue.empty()) {
		DoProcessNextCommand();
		
	} else {
		// Do some cleanup
		m_output.Clear();
		m_url.Clear();
		m_currCmd.clean();
	}
}

void SvnConsole::ExecuteURL(const wxString& cmd, const wxString& url, SvnCommandHandler* handler, bool printProcessOutput)
{
	DoExecute(cmd, handler, wxT(""), printProcessOutput);
	m_url = url;
}

void SvnConsole::Execute(const wxString& cmd, const wxString& workingDirectory, SvnCommandHandler* handler, bool printProcessOutput)
{
	DoExecute(cmd, handler, workingDirectory, printProcessOutput);
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
	
	DoInitializeFontsAndColours();
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
	Notebook* book = m_plugin->GetManager()->GetOutputPaneNotebook();
	for(size_t i=0; i<book->GetPageCount(); i++) {
		if(this == book->GetPage(i)) {
			book->SetSelection(i);
			break;
		}
	}
}

void SvnConsole::DoProcessNextCommand()
{
	// If another process is running, we try again when the process is termianted
	if (m_process) {
		return;
	}
	
	if(m_queue.empty())
		return;
	
	// Remove the command from the queue
	SvnConsoleCommand *command = m_queue.front();
	m_queue.pop_front();
	m_output.Clear();
	
	m_currCmd.clean();
	m_currCmd = *command;
	
	// Delete it
	delete command;
	
	EnsureVisible();

	// Print the command?
	AppendText(m_currCmd.cmd + wxT("\n"));

	// Wrap the command in the OS Shell
	wxString cmdShell (m_currCmd.cmd);

	// Apply the environment variables before executing the command
	StringMap om;
	om[wxT("LC_ALL")] = wxT("C");

	bool useOverrideMap = m_plugin->GetSettings().GetFlags() & SvnUsePosixLocale;
	EnvSetter env(m_plugin->GetManager()->GetEnv(), useOverrideMap ? &om : NULL);

	m_process = CreateAsyncProcess(this, cmdShell, IProcessCreateWithHiddenConsole, m_currCmd.workingDirectory);
	if (!m_process) {
		AppendText(_("Failed to launch Subversion client.\n"));
		return;
	}
	m_sci->SetFocus();
}

void SvnConsole::DoExecute(const wxString& cmd, SvnCommandHandler* handler, const wxString &workingDirectory, bool printProcessOutput)
{
	SvnConsoleCommand *consoleCommand = new SvnConsoleCommand();
	consoleCommand->cmd                = cmd.c_str();
	consoleCommand->handler            = handler;
	consoleCommand->printProcessOutput = printProcessOutput;
	consoleCommand->workingDirectory   = workingDirectory.c_str();
	m_queue.push_back(consoleCommand);
	
	DoProcessNextCommand();
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

void SvnConsole::DoInitializeFontsAndColours()
{
	for (int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		m_sci->StyleSetBackground(i, DrawingUtils::GetOutputPaneBgColour());
		m_sci->StyleSetForeground(i, DrawingUtils::GetOutputPaneFgColour());
	}
	
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	
	LexerConfPtr cppLexer = EditorConfigST::Get()->GetLexer(wxT("C++"));
	if(cppLexer) {
		std::list<StyleProperty> styles = cppLexer->GetProperties();
		std::list<StyleProperty>::iterator iter = styles.begin();
		for (; iter != styles.end(); iter++) {
			if(iter->GetId() == wxSCI_C_DEFAULT) {
				StyleProperty sp        = (*iter);
				int           size      = sp.GetFontSize();
				wxString      face      = sp.GetFaceName();
				bool          italic    = sp.GetItalic();
				
				font = wxFont(size, wxFONTFAMILY_TELETYPE, italic ? wxITALIC : wxNORMAL , wxNORMAL, false, face);
			}
		}
	}
	
	m_sci->StyleSetFont(0, font);
	m_sci->SetHotspotActiveUnderline (true);
	m_sci->SetHotspotActiveForeground(true, wxT("BLUE"));
	m_sci->SetHotspotSingleLine(true);

	m_sci->SetMarginWidth(0, 0);
	m_sci->SetMarginWidth(1, 0);
	m_sci->SetMarginWidth(2, 0);
	m_sci->SetMarginWidth(4, 0);
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
	
}

