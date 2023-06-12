#include "wxTerminalCtrl.h"

#include "TextView.h"
#include "wxTerminalInputCtrl.hpp"

#include <wx/filename.h>
#include <wx/log.h>
#include <wx/process.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#include <wx/wupdlock.h>

wxTerminalCtrl::wxTerminalCtrl() {}

wxTerminalCtrl::wxTerminalCtrl(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                               const wxString& name)
{
    if(!Create(parent, winid, pos, size, style)) {
        return;
    }

    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_outputView = new TextView(this);
    m_outputView->SetSink(this);
    m_outputView->SetEditable(false);
    GetSizer()->Add(m_outputView, wxSizerFlags(1).Expand());

    m_inputCtrl = new wxTerminalInputCtrl(this);
    GetSizer()->Add(m_inputCtrl, wxSizerFlags(0).Expand());

    m_outputView->Bind(wxEVT_KEY_DOWN, &wxTerminalCtrl::OnCharHook, this);
    GetSizer()->Fit(this);
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
    m_outputView->Unbind(wxEVT_KEY_DOWN, &wxTerminalCtrl::OnCharHook, this);
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
}

void wxTerminalCtrl::AppendText(const std::string& text)
{
    m_outputView->SetEditable(true);
    m_outputView->StyleAndAppend(text);
    m_outputView->SetEditable(false);
    SetCaretAtEnd();
}

void wxTerminalCtrl::OnCharHook(wxKeyEvent& event)
{
    m_inputCtrl->GetEventHandler()->ProcessEvent(event);
    m_inputCtrl->CallAfter(&wxTerminalInputCtrl::SetCaretEnd);
}

void wxTerminalCtrl::SetCaretAtEnd()
{
    m_outputView->SetCaretEnd();
    m_inputCtrl->m_ctrl->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void wxTerminalCtrl::GenerateCtrlC()
{
    if(m_shell) {
        m_shell->Signal(wxSIGINT);
    }
}

void wxTerminalCtrl::DoProcessTerminated()
{
    wxDELETE(m_shell);
    if(m_terminating) {
        wxTerminalEvent outputEvent(wxEVT_TERMINAL_CTRL_DONE);
        outputEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(outputEvent);
        m_outputView->SetEditable(false);
    } else {
        StartShell();
    }
}

void wxTerminalCtrl::SetAttributes(const wxColour& bg_colour, const wxColour& text_colour, const wxFont& font)
{
    m_outputView->SetAttributes(bg_colour, text_colour, font);
    m_outputView->ReloadSettings();
}

void wxTerminalCtrl::Focus() { m_outputView->Focus(); }

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

void wxTerminalCtrl::ChangeToPasswordStateIfNeeded()
{
    wxString line = m_outputView->GetLineText(m_outputView->GetNumberOfLines() - 1);
    line = line.Lower();
    if(line.Contains("password:") || line.Contains("password for") || line.Contains("pin for")) {
        m_state = TerminalState::PASSWORD;

        // TODO: change the style from the prev position to "hidden"
    }
}

void wxTerminalCtrl::ClearScreen()
{
    m_outputView->SetEditable(true);
    m_outputView->GetCtrl()->ClearAll();
    m_outputView->SetEditable(false);
}

void wxTerminalCtrl::ClearLine() { m_inputCtrl->Clear(); }

void wxTerminalCtrl::Logout()
{
    m_inputCtrl->Clear();
    Run("exit");
}
