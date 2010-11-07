#include "wxterminal.h"
#include "asyncprocess.h"
#include <wx/filename.h>
#include "processreaderthread.h"

#ifdef __WXMSW__
#include "windows.h"
#define SHELL_PREFIX  wxT("cmd /c ")
#define SHELL_WRAPPER wxT("\"")
#else

#if defined(__WXMAC__) || defined(__FreeBSD__)
#include <sys/wait.h>
#else
#include <wait.h>
#endif

#include <signal.h>
#define SHELL_PREFIX  wxT("/bin/sh -c ")
#define SHELL_WRAPPER wxT("'")
#endif

#if defined(__WXGTK__)
#    include <pty.h>
#    include "unixprocess_impl.h"
#elif defined(__WXMAC__)
#    include <util.h>
#    include "unixprocess_impl.h"
#endif

static wxString WrapInShell(const wxString &cmd)
{
	wxString shellCommand;
	shellCommand << SHELL_PREFIX << SHELL_WRAPPER << cmd << SHELL_WRAPPER;
	return shellCommand;
}

//-------------------------------------------------------------
BEGIN_EVENT_TABLE(wxTerminal, wxTerminalBase)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  wxTerminal::OnReadProcessOutput)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, wxTerminal::OnProcessEnd       )
END_EVENT_TABLE()

wxTerminal::wxTerminal( wxWindow* parent )
	: wxTerminalBase  ( parent )
	, m_process(NULL)
	, m_exitWhenProcessDies(false)
	, m_exitOnKey(false)
	, m_inferiorEnd(0)
#if defined(__WXMAC__) || defined(__WXGTK__)
	, m_dummyProcess(NULL)
#endif
{
	m_defaultStyle.SetFont( m_textCtrl->GetFont() );
	m_defaultStyle.SetTextColour( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)  );
	m_defaultStyle.SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_textCtrl->SetDefaultStyle( m_defaultStyle );
}

wxTerminal::~wxTerminal()
{
#if defined(__WXMAC__) || defined(__WXGTK__)
	StopTTY();
#endif
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
	long curPos = m_textCtrl->GetInsertionPoint();
	if(curPos < m_inferiorEnd) {
		int keyCode = event.GetKeyCode();
		// Dont allow any key down when
		switch(keyCode) {
		case WXK_UP:
		case WXK_DOWN:
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_NUMPAD_UP:
		case WXK_NUMPAD_DOWN:
		case WXK_NUMPAD_LEFT:
		case WXK_NUMPAD_RIGHT:
			event.Skip();
			break;

		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			event.Skip();
			break;

		default:
			break;
		}

		return;
	}

#ifndef __WXMSW__
	if(m_dummyProcess) {
		switch(event.GetKeyCode()) {
		case WXK_NUMPAD_ENTER:
		case WXK_RETURN: {
			// get last line and pass it to the caller
			wxString lineText = m_textCtrl->GetLineText(m_textCtrl->GetNumberOfLines()-1);
			lineText.Trim().Trim(false);
			m_dummyProcess->Write(lineText);
			break;
		}
		}
	}
#endif
	event.Skip();
}

void wxTerminal::OnProcessEnd(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	delete ped;
	if( m_process ) {
		delete m_process;
		m_process = NULL;
	}
	if(m_exitWhenProcessDies) {
		m_textCtrl->SetInsertionPointEnd();
		m_textCtrl->AppendText(wxT("\nPress any key to continue..."));
		m_exitOnKey = true;
	}
}

void wxTerminal::OnReadProcessOutput(wxCommandEvent& event)
{
	ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
	m_textCtrl->SetInsertionPointEnd();

	wxString s;
	s = ped->GetData();
	m_textCtrl->AppendText(s);
	m_textCtrl->SetSelection(m_textCtrl->GetLastPosition(), m_textCtrl->GetLastPosition());
	m_inferiorEnd = m_textCtrl->GetLastPosition();
	delete ped;
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
//	waitpid(m_process->GetPid(), &status, 0);
#endif
}

void wxTerminal::Execute(const wxString& command, bool exitWhenDone, const wxString &workingDir)
{
	if(m_process)
		return;

	m_textCtrl->Clear();

	// restore default style
	m_textCtrl->SetDefaultStyle( m_defaultStyle );
	m_textCtrl->SetFocus();

	m_exitWhenProcessDies = exitWhenDone;
	m_workingDir          = workingDir;
	DoProcessCommand(command);
}

void wxTerminal::DoProcessCommand(const wxString& command)
{
	wxString cmd ( command );
	cmd.Trim().Trim(false);
	wxString path;
	// Add the shell prefix
	wxString cmdShell = WrapInShell(cmd);
	// real command
	IProcess *cmdPrc = CreateAsyncProcess(this, cmdShell, IProcessCreateDefault, m_workingDir);
	if( cmdPrc ) {
		m_process = cmdPrc;

	} else {
		m_process = NULL;
		m_textCtrl->SetInsertionPointEnd();
		m_textCtrl->AppendText(wxString::Format(wxT("Failed to execute command: %s\nWorking Directory: %s\n"), cmdShell.c_str(), m_workingDir.c_str()));

		if(m_exitWhenProcessDies) {
			m_textCtrl->SetInsertionPointEnd();
			m_textCtrl->AppendText(wxT("\nPress any key to continue..."));
			m_exitOnKey = true;
		}
	}
}

void wxTerminal::KillInferior()
{
	DoCtrlC();
}

bool wxTerminal::IsRunning()
{
	return m_process != NULL;
}

void wxTerminal::Clear()
{
	m_textCtrl->Clear();
}

#if defined(__WXGTK__)||defined(__WXMAC__)
wxString wxTerminal::StartTTY()
{
	m_process = NULL;

	char __name[128];
	memset(__name, 0, sizeof(__name));

	int master(-1);
	m_slave = -1;
	if(openpty(&master, &m_slave, __name, NULL, NULL) != 0)
		return wxT("");

	// disable ECHO
	struct termios termio;	tcgetattr(master, &termio);	termio.c_lflag = ICANON;
	termio.c_oflag = ONOCR | ONLRET;	tcsetattr(master, TCSANOW, &termio);

	m_tty = wxString(__name, wxConvUTF8);

	// Start a listener on the tty
	m_dummyProcess = new UnixProcessImpl(this);
	static_cast<UnixProcessImpl*>(m_dummyProcess)->SetReadHandle  (master);
	static_cast<UnixProcessImpl*>(m_dummyProcess)->SetWriteHandler(master);
	static_cast<UnixProcessImpl*>(m_dummyProcess)->SetPid(wxNOT_FOUND);
	static_cast<UnixProcessImpl*>(m_dummyProcess)->StartReaderThread();
	return m_tty;
}

void wxTerminal::StopTTY()
{
	if(m_dummyProcess) {
		delete m_dummyProcess;
	}

	m_dummyProcess = NULL;
	m_tty.Clear();
	close(m_slave);
	m_slave = -1;
}
#endif
