#include "wxTerminalCtrl.h"
#include <wx/sizer.h>
#include <processreaderthread.h>
#include <ColoursAndFontsManager.h>

wxTerminalCtrl::wxTerminalCtrl() {}

wxTerminalCtrl::wxTerminalCtrl(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                               const wxString& name)
{
    if(!Create(parent, winid, pos, size, style)) { return; }
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    CallAfter(&wxTerminalCtrl::PostCreate);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminalCtrl::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &wxTerminalCtrl::OnProcessStderr, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminalCtrl::OnProcessTerminated, this);
    Bind(wxEVT_CHAR_HOOK, &wxTerminalCtrl::OnKeyDown, this);

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) { lexer->Apply(m_ctrl); }
}

wxTerminalCtrl::~wxTerminalCtrl()
{
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
    m_shell = ::CreateAsyncProcess(this, shell, IProcessCreateDefault);
}

void wxTerminalCtrl::Run(const wxString& command)
{
    if(m_shell) { m_shell->Write(command); }
}

void wxTerminalCtrl::OnProcessOutput(clProcessEvent& event) { AppendText(event.GetOutput()); }

void wxTerminalCtrl::OnProcessStderr(clProcessEvent& event) { AppendText(event.GetOutput()); }

void wxTerminalCtrl::OnProcessTerminated(clProcessEvent& event) {}

void wxTerminalCtrl::AppendText(const wxString& text)
{
    m_ctrl->ClearSelections();
    m_ctrl->SetInsertionPointEnd();
    m_ctrl->AppendText(text);
    m_commandOffset = m_ctrl->GetLastPosition();
    m_ctrl->ScrollToEnd();
    m_ctrl->SetSelection(m_commandOffset, m_commandOffset);
    CallAfter(&wxTerminalCtrl::SetFocus);
}

void wxTerminalCtrl::OnKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_NUMPAD_ENTER || event.GetKeyCode() == WXK_RETURN) {
        // Execute command
        Run(GetShellCommand());
        AppendText("\n");
    } else {
        int pos = m_ctrl->GetCurrentPos();
        if(event.GetKeyCode() == WXK_BACK) {
            // going backward
            event.Skip(pos > m_commandOffset);
        } else {
            event.Skip(pos >= m_commandOffset);
        }
    }
}

wxString wxTerminalCtrl::GetShellCommand() const
{
    return m_ctrl->GetTextRange(m_commandOffset, m_ctrl->GetLastPosition());
}
