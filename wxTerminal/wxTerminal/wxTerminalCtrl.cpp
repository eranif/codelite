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
    m_shell = ::CreateAsyncProcess(this, shell, IProcessCreateDefault | IProcessRawOutput);
}

void wxTerminalCtrl::Run(const wxString& command)
{
    if(m_shell) {
        m_shell->Write(command);
        if(!command.IsEmpty()) { AppendText("\n"); }
    }
}

void wxTerminalCtrl::OnProcessOutput(clProcessEvent& event) { AppendText(event.GetOutput()); }

void wxTerminalCtrl::OnProcessStderr(clProcessEvent& event) { AppendText(event.GetOutput()); }

void wxTerminalCtrl::OnProcessTerminated(clProcessEvent& event) {}

void wxTerminalCtrl::AppendText(const wxString& text)
{
    m_colourHandler << text;
    m_commandOffset = m_textCtrl->GetLastPosition();
    CallAfter(&wxTerminalCtrl::SetFocus);
}

void wxTerminalCtrl::OnKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_NUMPAD_ENTER || event.GetKeyCode() == WXK_RETURN) {
        // Execute command
        Run(GetShellCommand());
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
