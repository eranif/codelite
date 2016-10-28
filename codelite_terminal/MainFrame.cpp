#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include <wx/regex.h>
#include <errno.h>
#include <wx/settings.h>
#include <wx/colordlg.h>
#include "SettingsDlg.h"
#include <wx/filedlg.h>
#include <wx/clipbrd.h>
#include <wx/ffile.h>
#include <wx/filename.h>

#ifndef __WXMSW__
#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <libutil.h>
#elif defined(__WXGTK__)
#include <pty.h>
#else
#include <util.h>
#endif
#include "unixprocess_impl.h"
#endif

static const int ID_FG_COLOR = ::wxNewId();
static const int ID_BG_COLOR = ::wxNewId();
static const int ID_SIGKILL = ::wxNewId();
static const int ID_SIGHUP = ::wxNewId();
static const int ID_SIGTERM = ::wxNewId();
static const int ID_SIGINT = ::wxNewId();

static void WrapInShell(wxString& cmd)
{
    wxString command;
#ifdef __WXMSW__
    wxChar* shell = wxGetenv(wxT("COMSPEC"));
    if(!shell) shell = (wxChar*)wxT("\\COMMAND.COM");

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

MainFrame::MainFrame(wxWindow* parent, const TerminalOptions& options, long style)
    : MainFrameBaseClass(parent, wxID_ANY, "codelite-terminal", wxDefaultPosition, wxDefaultSize, style)
    , m_process(NULL)
    , m_ptyCllback(this)
    , m_fromPos(0)
    , m_options(options)
    , m_exitOnNextKey(false)
{
    SetTitle(m_options.GetTitle());
    m_stc->SetFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT));
    wxString tty = StartTTY();
    SetCartAtEnd();
    m_stc->MarkerDefine(MARKER_ID, wxSTC_MARK_ARROWS);
    m_stc->MarkerSetBackground(MARKER_ID, *wxBLACK);
    // m_stc->MarkerSetAlpha(MARKER_ID, 5);
    SetSize(m_config.GetTerminalSize());
    SetPosition(m_config.GetTerminalPosition());

    DoApplySettings();
    CallAfter(&MainFrame::DoExecStartCommand);

    // Connect color menu items
    Connect(ID_BG_COLOR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSelectBgColour), NULL, this);
    Connect(ID_FG_COLOR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSelectFgColour), NULL, this);
    Connect(ID_SIGKILL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSignal), NULL, this);
    Connect(ID_SIGINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSignal), NULL, this);
    Connect(ID_SIGTERM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSignal), NULL, this);
    Connect(ID_SIGHUP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSignal), NULL, this);
}

MainFrame::~MainFrame()
{
    StopTTY();
    m_config.SetTerminalPosition(GetPosition());
    m_config.SetTerminalSize(GetSize());
    m_config.SetBgColour(m_stc->StyleGetBackground(0));
    m_config.SetFgColour(m_stc->StyleGetForeground(0));
    m_config.Save();

    // Call this so the clipboard is still available after codelite-terminal exits
    wxTheClipboard->Flush();
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
    info.SetCopyright(_("by Eran Ifrah"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("A terminal emulator designed for codelite IDE"));
    ::wxAboutBox(info);
}

void MainFrame::OnKeyDown(wxKeyEvent& event)
{
    if(m_exitOnNextKey) {

        if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
            Close();

        } else if(event.GetModifiers() == wxMOD_CONTROL && event.GetKeyCode() == 'C') {
            // allow copy
            event.Skip();

        } else {
            return;
        }
    }

    if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        if(m_dummyProcess && m_options.HasFlag(TerminalOptions::kDebuggerTerminal)) {
            // write the output to the dummy process
            wxString cmd = GetCurrentLine();
            AppendNewLine();
            m_dummyProcess->Write(cmd);
        }

        if(m_process) {
            wxString cmd = GetCurrentLine();
            AppendNewLine();
            m_process->Write(cmd);

        } else {
            DoExecuteCurrentLine();
        }

    } else if(event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_NUMPAD_UP) {
        // TODO: show history here

    } else if(event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_NUMPAD_DOWN) {
        // TODO: show history here

    } else if(event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
        event.Skip();

    } else if(event.GetKeyCode() == WXK_BACK) {
        if((m_stc->GetCurrentPos() - 1) < m_fromPos) {
            return;
        }
        event.Skip();

    } else {
        event.Skip();
    }
}

void MainFrame::DoExecuteCurrentLine(const wxString& command)
{
    bool async = false;
    wxString cmd = command.IsEmpty() ? GetCurrentLine() : command;
    cmd.Trim().Trim(false);
    if(cmd.EndsWith("&")) {
        // Run in the background
        cmd.RemoveLast();
        async = true;
    }

    if(command.IsEmpty()) {
        AppendNewLine();
    }

    if(cmd.IsEmpty()) {
        SetCartAtEnd();
        return;
    }

    m_process = NULL;
    static wxRegEx reCD("cd[ \t]+");
    if(reCD.Matches(cmd)) {
        reCD.Replace(&cmd, "");
        if(::wxSetWorkingDirectory(cmd)) {
            m_stc->AppendText("current directory: " + ::wxGetCwd() + "\n");

        } else {
            m_stc->AppendText(wxString(strerror(errno)) + "\n");
        }
        SetCartAtEnd();

    } else if(cmd == "tty") {
        m_stc->AppendText(m_tty + "\n");
        SetCartAtEnd();

    } else {
        WrapInShell(cmd);
        IProcess* proc =
            ::CreateAsyncProcessCB(this, new MyCallback(this), cmd, IProcessCreateWithHiddenConsole, ::wxGetCwd());
        if(!async) {
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
    // m_stc->SetReadOnly( m_stc->GetCurrentPos() < m_fromPos );
}

void MainFrame::SetCartAtEnd()
{
    int len = m_stc->GetLength();
    m_stc->SetInsertionPoint(len);
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
#if defined(__WXGTK__) || defined(__WXMAC__)
    m_process = NULL;

    char __name[128];
    memset(__name, 0, sizeof(__name));

    int master(-1);
    m_slave = -1;
    if(openpty(&master, &m_slave, __name, NULL, NULL) != 0) return wxT("");

    // disable ECHO
    struct termios termio;
    tcgetattr(master, &termio);
    termio.c_lflag = ICANON;
    termio.c_oflag = ONOCR | ONLRET;
    tcsetattr(master, TCSANOW, &termio);

    m_tty = wxString(__name, wxConvUTF8);

    // Start a listener on the tty
    m_dummyProcess = new UnixProcessImpl(this);
    m_dummyProcess->SetCallback(&m_ptyCllback);

    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetReadHandle(master);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetWriteHandler(master);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetPid(wxNOT_FOUND);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->StartReaderThread();
#endif
    return m_tty;
}

void MainFrame::StopTTY()
{
#if defined(__WXGTK__) || defined(__WXMAC__)
    wxDELETE(m_dummyProcess);
    m_tty.Clear();
    ::close(m_slave);
    m_slave = -1;
#endif
}

void MainFrame::DoExecStartCommand()
{
    if(!m_options.GetCommand().IsEmpty()) {
        // m_stc->AppendText( m_options.GetCommand() );
        DoExecuteCurrentLine(m_options.GetCommand());
    }
}

void MainFrame::Exit()
{
    if(m_options.HasFlag(TerminalOptions::kPauseBeforeExit)) {

        m_outoutBuffer << "\nHit ENTER to continue...";
        FlushOutputBuffer();
        m_exitOnNextKey = true;

    } else {
        Close();
    }
}

void MainFrame::OnAddMarker(wxTimerEvent& event)
{
    int lastLine = m_stc->LineFromPosition(m_stc->GetLength());
    if(m_process == NULL) {
        m_stc->MarkerDeleteAll(MARKER_ID);
        m_stc->MarkerAdd(lastLine, MARKER_ID);
    }
}

void MainFrame::OnClearView(wxCommandEvent& event)
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_fromPos = 0;
    m_stc->SetFocus();
}

void MainFrame::OnTerminateInfirior(wxCommandEvent& event)
{
    if(m_process) {
        m_process->Terminate();
    }
}

void MainFrame::OnTerminateInfiriorUI(wxUpdateUIEvent& event) { event.Enable(m_process != NULL); }

void MainFrame::OnClearViewUI(wxUpdateUIEvent& event) { event.Enable(!m_stc->IsEmpty()); }

void MainFrame::OnSelectBgColour(wxCommandEvent& e)
{
    wxColour col = ::wxGetColourFromUser(this);
    DoSetColour(col, true);
}

void MainFrame::OnSelectFgColour(wxCommandEvent& e)
{
    wxColour col = ::wxGetColourFromUser(this);
    DoSetColour(col, false);
}

void MainFrame::DoSetFont(wxFont font)
{
    if(font.IsOk()) {
        for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
            m_stc->StyleSetFont(i, font);
        }
    }
}

void MainFrame::DoSetColour(const wxColour& colour, bool bgColour)
{
    if(colour.IsOk()) {
        for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
            if(bgColour) {
                m_stc->StyleSetBackground(i, colour);

            } else {
                m_stc->StyleSetForeground(i, colour);
            }
        }
        if(!bgColour) {
            m_stc->SetCaretForeground(colour);
            m_stc->MarkerSetForeground(MARKER_ID, colour);
        }
    }
}

void MainFrame::OnSignal(wxCommandEvent& e)
{
    if(m_process) {
        int sigid = e.GetId();
        if(sigid == ID_SIGHUP)
            wxKill(m_process->GetPid(), wxSIGHUP);

        else if(sigid == ID_SIGINT)
            wxKill(m_process->GetPid(), wxSIGINT);

        else if(sigid == ID_SIGKILL)
            wxKill(m_process->GetPid(), wxSIGKILL);

        else if(sigid == wxSIGTERM)
            wxKill(m_process->GetPid(), wxSIGTERM);
    }
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    SettingsDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        m_config.SetFgColour(dlg.GetFgColour());
        m_config.SetBgColour(dlg.GetBgColour());
        m_config.SetFont(dlg.GetFont());
        m_config.Save();
        DoApplySettings();
    }
}

void MainFrame::DoApplySettings()
{
    DoSetColour(m_config.GetFgColour(), false);
    DoSetColour(m_config.GetBgColour(), true);
    DoSetFont(m_config.GetFont());
}

void MainFrame::OnSaveContent(wxCommandEvent& event)
{
    const wxString ALL(wxT("All Files (*)|*"));
    wxFileDialog dlg(this,
                     _("Save As"),
                     ::wxGetCwd(),
                     "codelite-terminal.txt",
                     ALL,
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                     wxDefaultPosition);
    if(dlg.ShowModal() == wxID_OK) {
        m_stc->SaveFile(dlg.GetPath());
    }
}

void MainFrame::OnSaveContentUI(wxUpdateUIEvent& event) { event.Enable(!m_stc->IsEmpty()); }

void MainFrame::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    FlushOutputBuffer();
}

#define wxMEGA_BYTE 1024 * 1024

void MainFrame::AppendOutputText(const wxString& text)
{
    m_outoutBuffer << text;
    if(m_outoutBuffer.length() > wxMEGA_BYTE) {
        FlushOutputBuffer();
    }
}

void MainFrame::FlushOutputBuffer()
{
    if(!m_outoutBuffer.IsEmpty()) {
        m_stc->AppendText(m_outoutBuffer);
        SetCartAtEnd();
        m_outoutBuffer.Clear();
    }
}
