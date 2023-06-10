#include "wxTerminalCtrl.h"

#include "TextView.h"

#include <wx/filename.h>
#include <wx/log.h>
#include <wx/process.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#include <wx/wupdlock.h>

wxTerminalEvent::wxTerminalEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
    , m_answer(false)
    , m_allowed(true)
    , m_lineNumber(0)
    , m_selected(false)
{
}

wxTerminalEvent::wxTerminalEvent(const wxTerminalEvent& event)
    : wxCommandEvent(event)
    , m_answer(false)
    , m_allowed(true)
{
    *this = event;
}

wxTerminalEvent& wxTerminalEvent::operator=(const wxTerminalEvent& src)
{
    m_strings.clear();
    for(size_t i = 0; i < src.m_strings.size(); ++i) {
        m_strings.Add(src.m_strings.Item(i).c_str());
    }
    m_fileName = src.m_fileName;
    m_answer = src.m_answer;
    m_allowed = src.m_allowed;
    m_oldName = src.m_oldName;
    m_lineNumber = src.m_lineNumber;
    m_selected = src.m_selected;
    m_stringRaw = src.m_stringRaw;

    // Copy wxCommandEvent members here
    m_eventType = src.m_eventType;
    m_id = src.m_id;
    m_cmdString = src.m_cmdString;
    m_commandInt = src.m_commandInt;
    m_extraLong = src.m_extraLong;
    return *this;
}

wxTerminalEvent::~wxTerminalEvent() {}

wxEvent* wxTerminalEvent::Clone() const { return new wxTerminalEvent(*this); }

// A specialization of MyProcess for redirecting the output
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_READY, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_OUTPUT, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_STDERR, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_DONE, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_SET_TITLE, wxTerminalEvent);

///---------------------------------------------------------------
/// Helper methods
///---------------------------------------------------------------
static wxString ConvertString(const std::string& str, const wxMBConv& conv = wxConvISO8859_1)
{
    if(str.empty()) {
        return wxEmptyString;
    }
    wxString wx_str = wxString(str.c_str(), conv);
    if(wx_str.IsEmpty()) {
        // conversion failed
        wx_str = wxString::From8BitData(str.c_str());
    }
    return wx_str;
}

///---------------------------------------------------------------
///
///---------------------------------------------------------------

wxTerminalCtrl::wxTerminalCtrl() {}

wxTerminalCtrl::wxTerminalCtrl(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                               const wxString& name)
{
    if(!Create(parent, winid, pos, size, style)) {
        return;
    }
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_textCtrl = new TextView(this);
    m_textCtrl->SetSink(this);
    GetSizer()->Add(m_textCtrl, 1, wxEXPAND);

    Bind(wxEVT_CHAR_HOOK, &wxTerminalCtrl::OnCharHook, this);
    GetSizer()->Fit(this);
    m_textCtrl->GetCtrl()->Bind(wxEVT_LEFT_DOWN, &wxTerminalCtrl::OnLeftDown, this);
    CallAfter(&wxTerminalCtrl::StartShell);
}

wxTerminalCtrl::~wxTerminalCtrl()
{
    if(m_shell) {
        m_shell->Detach();
        wxDELETE(m_shell);
    }
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminalCtrl::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_STDERR, &wxTerminalCtrl::OnProcessError, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminalCtrl::OnProcessTerminated, this);
    Unbind(wxEVT_CHAR_HOOK, &wxTerminalCtrl::OnCharHook, this);
}

bool wxTerminalCtrl::Create(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                            const wxString& name)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminalCtrl::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &wxTerminalCtrl::OnProcessError, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminalCtrl::OnProcessTerminated, this);
    m_style = style & ~wxWINDOW_STYLE_MASK; // Remove all wxWindow style masking (Hi Word)
    return wxWindow::Create(parent, winid, pos, size,
                            style & wxWINDOW_STYLE_MASK); // Pass only the Windows related styles
}

void wxTerminalCtrl::StartShell()
{
    wxString shell;
#ifdef __WXMSW__
    shell = "C:\\Windows\\System32\\cmd.exe /Q"; // echo off
#else
    shell = "/bin/bash";
#endif
    m_shell =
        ::CreateAsyncProcess(this, shell, IProcessCreateDefault | IProcessRawOutput | IProcessCreateWithHiddenConsole);
    if(m_shell) {
        wxTerminalEvent readyEvent(wxEVT_TERMINAL_CTRL_READY);
        readyEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(readyEvent);
    }
}

void wxTerminalCtrl::Run(const wxString& command)
{
    if(!m_shell) {
        return;
    }

    m_shell->WriteRaw(command + "\n");
    AppendText("\n");
    m_history.Add(command);
}

void wxTerminalCtrl::AppendText(const std::string& text)
{
    m_textCtrl->StyleAndAppend(text);
    m_commandOffset = m_textCtrl->GetLastPosition();
    CallAfter(&wxTerminalCtrl::SetFocus);
    SetCaretAtEnd();
}

void wxTerminalCtrl::OnCharHook(wxKeyEvent& event)
{
    if(!m_textCtrl->IsEditable()) {
        return;
    }
    if(event.GetKeyCode() == WXK_NUMPAD_ENTER || event.GetKeyCode() == WXK_RETURN) {
        // Execute command
        Run(GetShellCommand());
        if(m_echoOff) {
            m_textCtrl->SetDefaultStyle(m_preEchoOffAttr);
            m_preEchoOffAttr = wxTextAttr();
            m_echoOff = false;
        }
    } else if(event.GetKeyCode() == WXK_HOME || event.GetKeyCode() == WXK_NUMPAD_HOME) {
        m_textCtrl->SetInsertionPoint(m_commandOffset);

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
        ClearScreen();
    } else if((event.GetKeyCode() == 'U') && event.ControlDown()) {
        ClearLine();
    } else if((event.GetKeyCode() == 'D') && event.ControlDown()) {
        Logout();
    } else if(event.GetKeyCode() == WXK_TAB) {
        // block it
    } else {
        SetEchoOff();
        int pos = m_textCtrl->GetInsertionPoint();
        if(event.GetKeyCode() == WXK_BACK || event.GetKeyCode() == WXK_LEFT) {
            // going backward
            event.Skip(pos > m_commandOffset);
        } else {
            if(pos < m_commandOffset) {
                m_textCtrl->SetInsertionPointEnd();
            }
            event.Skip(true);
        }
    }
}

wxString wxTerminalCtrl::GetShellCommand() const
{
    return m_textCtrl->GetRange(m_commandOffset, m_textCtrl->GetLastPosition());
}

void wxTerminalCtrl::SetShellCommand(const wxString& command)
{
    if(command.IsEmpty()) {
        return;
    }
    m_textCtrl->SetCommand(m_commandOffset, command);
    CallAfter(&wxTerminalCtrl::SetCaretAtEnd);
}

void wxTerminalCtrl::SetCaretAtEnd() { m_textCtrl->SetCaretEnd(); }

void wxTerminalCtrl::GenerateCtrlC() {}
void wxTerminalCtrl::ClearScreen()
{
    wxWindowUpdateLocker locker(m_textCtrl);
#if USE_STC
    m_textCtrl->Clear();
    m_commandOffset = 0;
    Run("");
#else
    // Delete the entire content excluding the last list
    if(m_textCtrl->GetNumberOfLines() < 1) {
        return;
    }
    long x, y;
    if(!m_textCtrl->PositionToXY(m_textCtrl->GetLastPosition(), &x, &y)) {
        return;
    }
    long insertPos = m_textCtrl->GetInsertionPoint();
    long lineStartPos = m_textCtrl->XYToPosition(0, y);
    m_textCtrl->Remove(0, lineStartPos);
    m_commandOffset -= lineStartPos;
    insertPos -= lineStartPos;
    m_textCtrl->SetInsertionPoint(insertPos);
#endif
}

void wxTerminalCtrl::ClearLine() { m_textCtrl->Remove(m_commandOffset, m_textCtrl->GetLastPosition()); }

void wxTerminalCtrl::Logout()
{
    // Loguot
    Run("exit");
}

void wxTerminalCtrl::SetEchoOff()
{
    if(!m_echoOff) {
        wxString line = m_textCtrl->GetLineText(m_textCtrl->GetNumberOfLines() - 1);
        line = line.Lower();
        if(line.Contains("password:") || line.Contains("password for")) {
            m_echoOff = true;
            m_preEchoOffAttr = m_textCtrl->GetDefaultStyle();
            wxTextAttr echoOffAttr = m_preEchoOffAttr;
            echoOffAttr.SetFontSize(0);
            echoOffAttr.SetTextColour(echoOffAttr.GetBackgroundColour());
            m_textCtrl->SetDefaultStyle(echoOffAttr);
        }
    }
}

void wxTerminalCtrl::DoProcessTerminated()
{
    wxDELETE(m_shell);
    if(m_terminating) {
        wxTerminalEvent outputEvent(wxEVT_TERMINAL_CTRL_DONE);
        outputEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(outputEvent);
        m_textCtrl->SetEditable(false);
    } else {
        StartShell();
    }
}

void wxTerminalCtrl::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    CallAfter(&wxTerminalCtrl::CheckInsertionPoint);
}

void wxTerminalCtrl::CheckInsertionPoint()
{
    int pos = m_textCtrl->GetInsertionPoint();
    m_textCtrl->SetEditable(pos >= m_commandOffset);
}

void wxTerminalCtrl::SetAttributes(const wxColour& bg_colour, const wxColour& text_colour, const wxFont& font)
{
    m_textCtrl->SetAttributes(bg_colour, text_colour, font);
    m_textCtrl->ReloadSettings();
}

void wxTerminalCtrl::Focus() { m_textCtrl->Focus(); }

void wxTerminalCtrl::OnProcessOutput(clProcessEvent& event) { AppendText(event.GetOutputRaw()); }

void wxTerminalCtrl::OnProcessError(clProcessEvent& event) { AppendText(event.GetOutputRaw()); }

void wxTerminalCtrl::OnProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    DoProcessTerminated();
}

void wxTerminalCtrl::Terminate()
{
    m_terminating = true;
    if(m_shell) {
        m_shell->Terminate();
    }
}
