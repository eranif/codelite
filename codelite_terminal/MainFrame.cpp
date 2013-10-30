#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include <wx/regex.h>
#include <errno.h>
#include <wx/settings.h>

#ifndef __WXMSW__
#if defined(__WXGTK__)
#   include <pty.h>
#else
#   include <util.h>
#endif
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

#define MARKER_ID 1

MainFrame::MainFrame(wxWindow* parent, const TerminalOptions &options)
    : MainFrameBaseClass(parent)
    , m_process(NULL)
    , m_ptyCllback(this)
    , m_fromPos(0)
    , m_options(options)
    , m_exitOnNextKey(false)
{
    SetTitle( m_options.GetTitle() );
    m_stc->SetFont( wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT) );
    StartTTY();
    SetCartAtEnd();
    m_stc->MarkerDefine(MARKER_ID, wxSTC_MARK_BACKGROUND);
    m_stc->MarkerSetBackground(MARKER_ID, *wxBLUE);
    m_stc->MarkerSetAlpha(MARKER_ID, 5);
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
    bool async = false;
    wxString cmd = GetCurrentLine();
    cmd.Trim().Trim(false);
    if ( cmd.EndsWith("&") ) {
        // Run in the background
        cmd.RemoveLast();
        async = true;
    }

    AppendNewLine();
    if ( cmd.IsEmpty() ) {
        SetCartAtEnd();
        return;
    }

    m_process = NULL;
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
        IProcess *proc = ::CreateAsyncProcessCB(this, new MyCallback(this), cmd, IProcessCreateWithHiddenConsole, ::wxGetCwd());
        if ( !async ) {
            // keep the process handle for sync commands only
            m_process = proc;
        }
        SetCartAtEnd();
    }
}

wxString MainFrame::GetCurrentLine() const
{
    wxString curline = m_stc->GetTextRange(m_fromPos, m_stc->GetLength());
    curline.Trim().Trim(false);
    return curline;
}

void MainFrame::OnStcUpdateUI(wxStyledTextEvent& event)
{
    m_stc->SetReadOnly( m_stc->GetCurrentPos() < m_fromPos );
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
    m_fromPos = m_stc->GetLength();
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

void MainFrame::OnIdle(wxIdleEvent& event)
{
    int lastLine = m_stc->LineFromPosition(m_stc->GetLength());
    m_stc->MarkerDeleteAll(MARKER_ID);
    m_stc->MarkerAdd( lastLine, MARKER_ID );
}

// void MainFrame::OnChange(wxStyledTextEvent& event)
// {
//     if (event.GetModificationType() & wxSTC_MOD_INSERTTEXT || event.GetModificationType() & wxSTC_MOD_DELETETEXT) {
//         int numlines(event.GetLinesAdded());
//         int curline (m_stc->LineFromPosition(event.GetPosition()));
//         if ( numlines == 0 ) {
//
//             // probably only the current line was modified
//             m_stc->MarginSetText (curline, PROMPT);
//             m_stc->MarginSetStyle(curline, STYLE_ID);
//
//         } else {
//
//             for (int i=0; i<=numlines; ++i) {
//                 m_stc->MarginSetText(curline+i, PROMPT);
//                 m_stc->MarginSetStyle(curline+i, STYLE_ID);
//             }
//         }
//     }
// }
//
