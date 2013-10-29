#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include <wx/regex.h>
#include <errno.h>

#if defined(__WXGTK__) || defined(__WXMAC__)
#   include <pty.h>
#   include "unixprocess_impl.h"
#endif

static void WrapInShell(wxString& cmd)
{
    wxString command;
#ifdef __WXMSW__
    wxChar *shell = wxGetenv(wxT("COMSPEC"));
    if ( !shell )
        shell = (wxChar*) wxT("\\COMMAND.COM");

    command << shell << wxT(" /c \"");
    command << cmd << wxT("\"");
    cmd = command;
#else
    command << wxT("/bin/sh -c '");
    command << cmd << wxT("'");
    cmd = command;
#endif
}


MainFrame::MainFrame(wxWindow* parent, const TerminalOptions &options)
    : MainFrameBaseClass(parent)
    , m_process(NULL)
    , m_callback(this)
    , m_ptyCllback(this)
    , m_fromPos(0)
    , m_options(options)
    , m_exitOnNextKey(false)
{
    SetTitle( m_options.GetTitle() );
    m_stc->SetFont( wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT) );
    StartTTY();
    
#if 0
    wxString message;
    message << "codelite-terminal started. tty=" << m_tty << "\n";
    m_stc->AppendText( message );
    
    message.Clear();
    message << "Current working directory is set to: " << ::wxGetCwd() << "\n";
    m_stc->AppendText( message );
#endif

    SetCartAtEnd();
    
    SetSize( m_config.GetTerminalSize() );
    SetPosition( m_config.GetTerminalPosition() );
    CallAfter( &MainFrame::DoExecStartCommand );
}

MainFrame::~MainFrame()
{
    StopTTY();
    m_config.SetTerminalPosition( GetPosition() );
    m_config.SetTerminalSize( GetSize() );
    m_config.Save();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetName("codelite-terminal");
    info.SetCopyright(_("By Eran Ifrah"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("A terminal emulator designed for codelite IDE"));
    ::wxAboutBox(info);
}

void MainFrame::OnKeyDown(wxKeyEvent& event)
{
    if ( m_exitOnNextKey ) {
        Close();
        return;
    }
    
    if ( event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER ) {
        if ( m_process ) {
            wxString cmd = GetCurrentLine();
            AppendNewLine();
            m_process->Write( cmd );

        } else {
            DoExecuteCurrentLine();

        }
        
    } else if ( event.GetKeyCode() == WXK_UP ||event.GetKeyCode() == WXK_NUMPAD_UP ) {
        // TODO: show history here
        
    } else if ( event.GetKeyCode() == WXK_DOWN ||event.GetKeyCode() == WXK_NUMPAD_DOWN ) {
        // TODO: show history here
        
    } else if ( event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE ) {
        event.Skip();

    } else if ( event.GetKeyCode() == WXK_BACK ) {
        if ( (m_stc->GetCurrentPos() -1) < m_fromPos ) {
            return;
        }
        event.Skip();
        
    } else {
        event.Skip();
    }
}

void MainFrame::DoExecuteCurrentLine()
{
    wxString cmd = GetCurrentLine();
    cmd.Trim().Trim(false);
    AppendNewLine();
    if ( cmd.IsEmpty() )
        return;
    
    static wxRegEx reCD("cd[ \t]+");
    if ( reCD.Matches( cmd ) ) {
        reCD.Replace(&cmd, "");
        if ( ::wxSetWorkingDirectory( cmd ) ) {
            m_stc->AppendText("current directory: " + ::wxGetCwd() + "\n");
            
        } else {
            m_stc->AppendText( wxString(strerror(errno)) + "\n" );
        }
        SetCartAtEnd();
        
    } else {
        WrapInShell( cmd );
        m_process = ::CreateAsyncProcessCB(this, &m_callback, cmd, IProcessCreateWithHiddenConsole, ::wxGetCwd());
    }
    
    m_fromPos = m_stc->GetLength();
}

wxString MainFrame::GetCurrentLine() const
{
    wxString curline = m_stc->GetTextRange(m_fromPos, m_stc->GetLength());
    curline.Trim().Trim(false);
    return curline;
}

void MainFrame::OnStcUpdateUI(wxStyledTextEvent& event)
{
    // int curLine = m_stc->LineFromPosition( m_stc->GetCurrentPos() );
    // int lastLine = m_stc->LineFromPosition( m_stc->GetLastPosition() );
    // m_stc->SetReadOnly( curLine < lastLine );
}

void MainFrame::SetCartAtEnd()
{
    int len = m_stc->GetLength();
    m_stc->SetInsertionPoint( len );
    m_stc->SetCurrentPos(len);
    m_stc->SetSelection(len, len);
    m_stc->ScrollToEnd();
    m_fromPos = m_stc->GetLength();
}

void MainFrame::AppendNewLine()
{
    m_stc->AppendText("\n");
}

wxString MainFrame::StartTTY()
{
#if defined(__WXGTK__)||defined(__WXMAC__)
    m_process = NULL;

    char __name[128];
    memset(__name, 0, sizeof(__name));

    int master(-1);
    m_slave = -1;
    if(openpty(&master, &m_slave, __name, NULL, NULL) != 0)
        return wxT("");

    // disable ECHO
    struct termios termio;
    tcgetattr(master, &termio);
    termio.c_lflag = ICANON;
    termio.c_oflag = ONOCR | ONLRET;
    tcsetattr(master, TCSANOW, &termio);

    m_tty = wxString(__name, wxConvUTF8);

    // Start a listener on the tty
    m_dummyProcess = new UnixProcessImpl(this);
    m_dummyProcess->SetCallback( &m_ptyCllback );
    
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetReadHandle  (master);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetWriteHandler(master);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetPid(wxNOT_FOUND);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->StartReaderThread();
#endif
    return m_tty;
}

void MainFrame::StopTTY()
{
#if defined(__WXGTK__)||defined(__WXMAC__)
    wxDELETE(m_dummyProcess);
    m_tty.Clear();
    ::close(m_slave);
    m_slave = -1;
#endif
}

void MainFrame::DoExecStartCommand()
{
    if ( !m_options.GetCommand().IsEmpty() ) {
        m_stc->AppendText( m_options.GetCommand() );
        DoExecuteCurrentLine();
    }
}

void MainFrame::Exit()
{
    if ( m_options.HasFlag( TerminalOptions::kPauseBeforeExit ) ) {
        m_stc->AppendText("Hit any key to continue...");
        SetCartAtEnd();
        m_exitOnNextKey = true;
        
    } else {
        Close();
    }
}
