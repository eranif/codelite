#include "MainFrame.h"
#include <wx/aboutdlg.h>

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


MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
    , m_process(NULL)
    , m_callback(this)
    , m_fromPos(0)
{
}

MainFrame::~MainFrame()
{
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
    WrapInShell( cmd );
    AppendNewLine();
    m_process = ::CreateAsyncProcessCB(this, &m_callback, cmd, IProcessCreateWithHiddenConsole, ::wxGetCwd());
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
