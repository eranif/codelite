#include "wxTerminalCtrl.h"
#include <wx/sizer.h>
#include <processreaderthread.h>
#include <ColoursAndFontsManager.h>
#include <wx/regex.h>
#include "procutils.h"

wxTerminalCtrl::wxTerminalCtrl() {}

wxTerminalCtrl::wxTerminalCtrl(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                               const wxString& name)
{
    if(!Create(parent, winid, pos, size, style)) { return; }
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_textCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_ENTER | wxTE_NOHIDESEL);
    GetSizer()->Add(m_textCtrl, 1, wxEXPAND);
    CallAfter(&wxTerminalCtrl::PostCreate);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminalCtrl::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &wxTerminalCtrl::OnProcessStderr, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminalCtrl::OnProcessTerminated, this);
    Bind(wxEVT_CHAR_HOOK, &wxTerminalCtrl::OnKeyDown, this);

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        wxFont font = lexer->GetFontForSyle(0);
        const StyleProperty& sp = lexer->GetProperty(0);
        wxColour bgColour = sp.GetBgColour();
        wxColour fgColour = sp.GetFgColour();
        m_textCtrl->SetBackgroundColour(bgColour);
        m_textCtrl->SetDefaultStyle(wxTextAttr(fgColour, bgColour, font));
    }
    m_colourHandler.SetCtrl(m_textCtrl);
}

wxTerminalCtrl::~wxTerminalCtrl()
{
    if(m_shell) {
        m_shell->Detach();
        wxDELETE(m_shell);
    }

    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminalCtrl::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_STDERR, &wxTerminalCtrl::OnProcessStderr, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminalCtrl::OnProcessTerminated, this);
    Unbind(wxEVT_CHAR_HOOK, &wxTerminalCtrl::OnKeyDown, this);
}

bool wxTerminalCtrl::Create(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                            const wxString& name)
{
    m_style = style & ~wxWINDOW_STYLE_MASK; // Remove all wxWindow style masking (Hi Word)
    return wxWindow::Create(parent, winid, pos, size, style & wxWINDOW_STYLE_MASK);
}

void wxTerminalCtrl::PostCreate()
{
    wxString shell;
#ifdef __WXMSW__
    shell = "C:\\Windows\\System32\\cmd.exe";
#else
    shell = wxGetenv("SHELL");
#endif
    m_shell =
        ::CreateAsyncProcess(this, shell, IProcessCreateDefault | IProcessRawOutput | IProcessCreateWithHiddenConsole);

    // Keep a list of initial processes that we DONT want to kill
    std::vector<long> children;
    ProcUtils::GetChildren(m_shell->GetPid(), children);
    for(long pid : children) {
        m_initialProcesses.insert(pid);
    }
}

void wxTerminalCtrl::Run(const wxString& command)
{
    if(m_shell) {
        m_shell->Write(command);
        if(!command.IsEmpty()) {
            AppendText("\n");
            m_history.Add(command);
        }
    }
}

void wxTerminalCtrl::OnProcessOutput(clProcessEvent& event) { AppendText(event.GetOutput()); }

void wxTerminalCtrl::OnProcessStderr(clProcessEvent& event) { AppendText(event.GetOutput()); }

void wxTerminalCtrl::OnProcessTerminated(clProcessEvent& event) {}

void wxTerminalCtrl::AppendText(const wxString& text)
{
    //    wxRegEx reJob("\\[([0-9]+)\\][ \\t]+([0-9]+)", wxRE_ADVANCED);
    //    if(reJob.IsValid() && reJob.Matches(text)) {
    //        // A process started in the background
    //        wxString process_id = reJob.GetMatch(text, 2);
    //        long nProcessNumber;
    //        if(process_id.ToCLong(&nProcessNumber)) { m_backgroundProcesses.insert(nProcessNumber); }
    //    }
    m_colourHandler << text;
    m_commandOffset = m_textCtrl->GetLastPosition();
    CallAfter(&wxTerminalCtrl::SetFocus);
}

void wxTerminalCtrl::OnKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_NUMPAD_ENTER || event.GetKeyCode() == WXK_RETURN) {
        // Execute command
        Run(GetShellCommand());
    } else if(event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_NUMPAD_UP) {
        m_history.Up();
        SetShellCommand(m_history.Get());
    } else if(event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_NUMPAD_DOWN) {
        m_history.Down();
        SetShellCommand(m_history.Get());
    } else if((event.GetKeyCode() == 'C') && event.ControlDown()) {
        // Generate Ctrl-C
        GenerateCtrlC();
    } else if((event.GetKeyCode() == 'L') && event.ControlDown()) {
        // Generate Ctrl-C
        ClearScreen();
    } else {
        int pos = m_textCtrl->GetInsertionPoint();
        if(event.GetKeyCode() == WXK_BACK || event.GetKeyCode() == WXK_LEFT) {
            // going backward
            event.Skip(pos > m_commandOffset);
        } else {
            event.Skip(pos >= m_commandOffset);
        }
    }
}

wxString wxTerminalCtrl::GetShellCommand() const
{
    return m_textCtrl->GetRange(m_commandOffset, m_textCtrl->GetLastPosition());
}

void wxTerminalCtrl::SetShellCommand(const wxString& command)
{
    if(command.IsEmpty()) { return; }
    m_textCtrl->Replace(m_commandOffset, m_textCtrl->GetLastPosition(), command);
    CallAfter(&wxTerminalCtrl::SetCaretAtEnd);
}

void wxTerminalCtrl::SetCaretAtEnd()
{
    m_textCtrl->SelectNone();
    m_textCtrl->SetInsertionPointEnd();
    m_textCtrl->CallAfter(&wxTextCtrl::SetFocus);
}

#ifdef __WXMSW__
static void KillPorcessByPID(DWORD pid)
{
    HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
    if(hProcess != INVALID_HANDLE_VALUE) { TerminateProcess(hProcess, 0); }
}
#endif

void wxTerminalCtrl::GenerateCtrlC()
{
    // Get list of the process children
    // and send them all SIGINT
    std::vector<long> children;
    ProcUtils::GetChildren(m_shell->GetPid(), children);
    for(long pid : children) {
        // Don't kill any initial process
        if(m_initialProcesses.count(pid)) { continue; }
#ifdef __WXMSW__
        KillPorcessByPID(pid);
#else
        wxKillError rc;
        ::wxKill(pid, wxSIGTERM, &rc, wxKILL_CHILDREN);
#endif
    }
}

void wxTerminalCtrl::ClearScreen()
{
    // Delete the entire content excluding the last list
    if(m_textCtrl->GetNumberOfLines() < 1) { return; }
    wxString curcmd = GetShellCommand();
    int lineLen = m_textCtrl->GetLineLength(m_textCtrl->GetNumberOfLines() - 1);
    m_commandOffset = (lineLen - curcmd.length());
    // clear everything but the last line
    long doclen = m_textCtrl->GetValue().length();
    long lastLineStartPos = (doclen - lineLen);
    m_textCtrl->Remove(0, lastLineStartPos);
}
