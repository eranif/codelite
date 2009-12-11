#include "wxterminal.h"
#include "asyncprocess.h"
#include <wx/filename.h>
#include "processreaderthread.h"

#ifdef __WXMSW__
#include "windows.h"
#define SHELL_PREFIX  wxT("cmd /c ")
#define SHELL_WRAPPER wxT("\"")
#else
#include <wait.h>
#include <signal.h>
#define SHELL_PREFIX  wxT("/bin/sh -c ")
#define SHELL_WRAPPER wxT("'")
#endif

static wxString WrapInShell(const wxString &cmd)
{
	wxString shellCommand;
	shellCommand << SHELL_PREFIX << SHELL_WRAPPER << cmd << SHELL_WRAPPER;
	return shellCommand;
}

wxTerminalHistory::wxTerminalHistory()
		: m_historyCursor(wxString::npos)
{
}

wxTerminalHistory::~wxTerminalHistory()
{
	m_history.Clear();
	m_historyCursor = wxString::npos;
}

void wxTerminalHistory::AddCommand(const wxString& command)
{
	if (m_history.Index(command) != wxNOT_FOUND) {
		m_history.Remove(command);
	}
	m_history.Add( command );
	m_historyCursor = m_history.GetCount()-1;
}

wxString wxTerminalHistory::ArrowDown()
{
	// Sanity
	if ( m_history.IsEmpty() )                    return wxT("");
	if ( m_historyCursor >= m_history.GetCount())
		m_historyCursor = 0;

	wxString cmd = m_history.Item(m_historyCursor);
	m_historyCursor++;
	return cmd;
}

wxString wxTerminalHistory::ArrowUp()
{
	// Sanity
	if ( m_history.IsEmpty() )                    return wxT("");

	if ( m_historyCursor >= m_history.GetCount())
		m_historyCursor = m_history.GetCount()-1;

	wxString cmd = m_history.Item(m_historyCursor);
	m_historyCursor--;
	return cmd;
}

//-------------------------------------------------------------
BEGIN_EVENT_TABLE(wxTerminal, wxTerminalBase)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  wxTerminal::OnReadProcessOutput)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, wxTerminal::OnProcessEnd       )
END_EVENT_TABLE()
wxTerminal::wxTerminal( wxWindow* parent )
		: wxTerminalBase  ( parent )
		, m_process(NULL)
		, m_inferiorEnd(wxNOT_FOUND)
		, m_exitWhenProcessDies(false)
		, m_exitOnKey(false)
{
	m_defaultStyle.SetFont( m_textCtrl->GetFont() );
	m_defaultStyle.SetTextColour( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)  );
	m_defaultStyle.SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_textCtrl->SetDefaultStyle( m_defaultStyle );

	m_promptStyle = m_defaultStyle;
	m_promptStyle.SetTextColour( wxT("BLUE") );	m_workingDir = wxGetCwd();

	// %h=hostname %H=hostname_up_to_dot %u=username %w=cwd %W=cwd basename only %$= either $, or # if root
	m_promptFormat = wxT("%u@%h: %w$");
	DoWritePrompt();
}

void wxTerminal::OnText( wxCommandEvent& event )
{
	event.Skip();
}

void wxTerminal::OnEnter( wxCommandEvent& event )
{
	event.Skip();
}

void wxTerminal::OnURL( wxTextUrlEvent& event )
{
	event.Skip();
}

void wxTerminal::OnKey(wxKeyEvent& event)
{
	if( m_textCtrl->IsEditable() == false) {
		return;
	}

	if(m_exitOnKey) {
		GetParent()->Close(true);
		return;
	}

	wxString cmd;
	switch ( event.GetKeyCode() ) {
	case 'U': {
		if( event.m_controlDown && m_process == NULL) {
			// clear the current line
			DoInsertLine(wxT(""));
		} else {
			DoFixSelection();
			event.Skip();
		}
		break;
	}
	case 'L': {
		if( event.m_controlDown  && m_process == NULL) {
			// clear the current line
			m_textCtrl->Clear();
			DoWritePrompt();
			m_textCtrl->DiscardEdits();
		} else {
			DoFixSelection();
			event.Skip();
		}
		break;
	}
	case 'C': {
		if( event.m_controlDown && m_process ) {
			// Send Ctrl-C
			DoCtrlC();
		} else {
			DoFixSelection();
			event.Skip();
		}
		break;
	}
	case WXK_HOME: {
		m_textCtrl->SetInsertionPoint( m_commandStart );
		break;
	}
	case WXK_UP: {
		cmd = m_history.ArrowUp();
		DoInsertLine(cmd);
		break;
	}
	case WXK_RETURN:
	case WXK_NUMPAD_ENTER: {
		m_textCtrl->AppendText(wxT("\n"));
		wxString cmd = DoGetLastLine();
		if ( !m_process ) {
			if (cmd.IsEmpty() == false ) {
				DoProcessCommand( cmd );
			} else {
				DoWritePrompt();
			}
		} else {
			// we got a process running, re-direct all input to it
			m_process->Write( cmd );
		}
		m_textCtrl->SetInsertionPointEnd();
		break;
	}
	case WXK_DOWN: {
		cmd = m_history.ArrowDown();
		DoInsertLine(cmd);
		break;
	}
	case WXK_DELETE:
	case WXK_BACK: {
		// Make sure we dont delete our prompt
		long from, to;
		m_textCtrl->GetSelection( &from,  &to );
		if ( to > from ) {
			// If there's a valid selection, delete it
			if ( to > m_commandStart ) {
				m_textCtrl->Remove( wxMax(from, m_commandStart), to );
			}
		} else {
			// Otherwise do a normal Del/Backspace
			if ( event.GetKeyCode() == WXK_BACK ) {
				long insertionPoint;
				insertionPoint = m_textCtrl->GetInsertionPoint();
				if (insertionPoint  > m_commandStart ) m_textCtrl->Remove( insertionPoint-1, insertionPoint );
			} else {
				long insertionPoint;
				insertionPoint = m_textCtrl->GetInsertionPoint();
				if ( insertionPoint >= m_commandStart && insertionPoint < m_textCtrl->GetLastPosition() ) m_textCtrl->Remove( wxMax(insertionPoint, m_commandStart), insertionPoint+1 );
			}
		}
		break;
	}
	default:{
		DoFixSelection();
		event.Skip();
		break;
	}
	}
	m_textCtrl->SetFocus();
}

wxString wxTerminal::DoGetLastLine()
{
	wxString cmd = m_textCtrl->GetRange(wxMax(m_commandStart, m_inferiorEnd), m_textCtrl->GetLastPosition());
	cmd.Trim().Trim(false);
	return cmd;
}

bool wxTerminal::CheckForCD( const wxString &command, wxString &path )
{
	if ( command.IsEmpty() )               return false; // Returning true tells caller there's nothing else to do
	if ( command.Left(2) != wxT("cd") )    return false; // Not a cd attempt so return false so that RunCommand takes over
	if ( wxIsalpha( command.GetChar(2) ) ) return false; // This must be a real command beginning with cd???

	if ( command == wxT("cd.") || command == wxT("cd .") )  {
		path = wxGetCwd();
		return true;
	}

	if ( command == wxT("cd") || command == wxT("cd~") || command == wxT("cd ~") ) {
		path = wxGetHomeDir();
		return true;

	} else if ( command.Find(wxT("&&")) != wxNOT_FOUND ) {
		// a complex command: cd <somewhere> && ...
		return false;

	} else {
		// Otherwise it should be a real dir. Remove the initial cd, plus any white-space
		path = command.Mid( 2 );
		path << wxFileName::GetPathSeparator();
		path.Trim(false);
		wxFileName fn(path);
		fn.MakeAbsolute(m_workingDir);
		fn.Normalize();
		if( fn.DirExists() ) {
			path = fn.GetFullPath();
			return true;
		}
		return false;
	}
}

void wxTerminal::DoProcessCommand(const wxString& command)
{
	wxString cmd ( command );
	cmd.Trim().Trim(false);

	if (!cmd.StartsWith(wxT("#"))) {
		// Execute it
		if (cmd.EndsWith(wxT("&"))) {
			cmd.RemoveLast();
		}

		wxString path;

		// Add the shell prefix
		wxString cmdShell = WrapInShell(cmd);
		if(CheckForCD(cmd, path)) {
			// CD command!
			m_workingDir = path;
			DoWritePrompt();

		} else {
			// real command
			IProcess *cmdPrc = CreateAsyncProcess(this, cmdShell, m_workingDir);
			if( cmdPrc ) {
				m_process = cmdPrc;
			} else {
				m_process = NULL;
				m_textCtrl->SetInsertionPointEnd();
				m_textCtrl->AppendText(wxString::Format(wxT("Failed to execute command: %s\nWorking Directory: %s\n"), cmdShell.c_str(), m_workingDir.c_str()));
				DoWritePrompt();
			}
		}
	} else {
		DoWritePrompt();
	}
	m_history.AddCommand( cmd );
}

void wxTerminal::DoInsertLine(const wxString& value)
{
	m_textCtrl->Freeze();
	int endPos  = m_textCtrl->GetLastPosition();
	int startPos = m_commandStart;
	if (startPos < 0) return;
	m_textCtrl->Replace(startPos, endPos, value);
	m_textCtrl->SetInsertionPointEnd();
	m_textCtrl->Thaw();
}

void wxTerminal::DoWritePrompt()
{
	wxString prompt;
	for ( size_t n=0; n < m_promptFormat.Len(); ++n ) {
		wxChar c = m_promptFormat[n];
		if ( c != wxT('%') ) prompt += c;             // If this is a standard character, add it to the string
		else {
			c = m_promptFormat[++n];                    // If it was '%', the next char determines what to do
			if ( c==wxT('%') ) {
				prompt += c;                          // %% really means it
				continue;
			}
			if ( c==wxT('H') ) {
				prompt += wxGetFullHostName();        // Full host-name
				continue;
			}
			if ( c==wxT('h') ) {
				prompt += wxGetHostName();            // Less-full host-name
				continue;
			}
			if ( c==wxT('u') ) {
				prompt += wxGetUserId();              // Add user-name to prompt
				continue;
			}
			if ( c==wxT('w') ) {
				prompt += m_workingDir;                 // Add Current Working Dir
				continue;
			}
			if ( c==wxT('W') ) {
				wxString tmpwd = m_workingDir;
				wxString sep   = wxFileName::GetPathSeparator();
				if(tmpwd.EndsWith(sep)) {
					tmpwd.RemoveLast();
				}
				prompt += tmpwd.AfterLast(wxFILE_SEP_PATH);    // Filename-bit only of cwd
				continue;
			}
		}
	}

	int promptStart = m_textCtrl->GetLastPosition();
	m_textCtrl->AppendText( prompt ); //   write the prompt,
	// Cancel it, whether there's a valid one or not:  even an old selection interferes
	m_textCtrl->SetSelection( m_textCtrl->GetLastPosition(), m_textCtrl->GetLastPosition() );
	m_textCtrl->SetInsertionPointEnd();
	m_commandStart = m_textCtrl->GetInsertionPoint();

	m_textCtrl->SetStyle(promptStart, m_commandStart-1, m_promptStyle);
	// restore default style
	m_textCtrl->SetDefaultStyle( m_defaultStyle );
	m_textCtrl->SetFocus();
}

void wxTerminal::OnProcessEnd(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	delete ped;
	m_inferiorEnd = wxNOT_FOUND;
	if( m_process ) {
		delete m_process;
		m_process = NULL;
	}
	if(m_exitWhenProcessDies) {
		m_textCtrl->SetInsertionPointEnd();
		m_textCtrl->AppendText(wxT("\nPress any key to continue..."));
		m_exitOnKey = true;
	} else {
		DoWritePrompt();
	}
}

void wxTerminal::OnReadProcessOutput(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	m_textCtrl->SetInsertionPointEnd();
	m_textCtrl->AppendText(ped->GetData());
	m_textCtrl->SetSelection(m_textCtrl->GetLastPosition(), m_textCtrl->GetLastPosition());
	m_inferiorEnd = m_textCtrl->GetLastPosition();
	delete ped;
}

void wxTerminal::DoFixSelection()
{
	long from, to;
	m_textCtrl->GetSelection( &from,  &to );
	if(to > from) {
		// we got a selection
		if( from < m_commandStart && to > m_commandStart ) {
			m_textCtrl->SetSelection(m_commandStart, to);
		}
	}
}


void wxTerminal::DoCtrlC()
{
	if(!m_process)
		return;

#ifdef __WXMSW__
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD) m_process->GetPid());
	if(hProcess) {
		TerminateProcess(hProcess, 0);
		CloseHandle(  hProcess );
	}
#else
	int status(0);
	wxKill(m_process->GetPid(), wxSIGKILL, NULL, wxKILL_CHILDREN);
	waitpid(m_process->GetPid(), &status, 0);
#endif
}

void wxTerminal::Execute(const wxString& command, bool exitWhenDone)
{
	m_textCtrl->AppendText( command  + wxT("\n") ); //   write the prompt,
	// Cancel it, whether there's a valid one or not:  even an old selection interferes
	m_textCtrl->SetSelection( m_textCtrl->GetLastPosition(), m_textCtrl->GetLastPosition() );
	m_textCtrl->SetInsertionPointEnd();
	m_commandStart = m_textCtrl->GetInsertionPoint();

	// restore default style
	m_textCtrl->SetDefaultStyle( m_defaultStyle );
	m_textCtrl->SetFocus();
	m_exitWhenProcessDies = exitWhenDone;
	DoProcessCommand(command);
}

void wxTerminal::KillInferior()
{
	DoCtrlC();
}

void wxTerminal::SetReadOnly(bool readOnly)
{
	m_textCtrl->SetEditable( !readOnly );
}

void wxTerminal::SetWorkingDirectory(const wxString& workingDirectory)
{
	m_workingDir = workingDirectory;
	wxString text;
	text << wxT("cd ") << workingDirectory << wxT("\n");
	m_textCtrl->AppendText( text );
	DoWritePrompt();
}

bool wxTerminal::IsRunning()
{
	return m_process != NULL;
}

void wxTerminal::Clear()
{
	m_textCtrl->Clear();
	DoWritePrompt();
}

void wxTerminal::SetPromptFormat(const wxString& promptFormat)
{
	m_promptFormat = promptFormat;
}
