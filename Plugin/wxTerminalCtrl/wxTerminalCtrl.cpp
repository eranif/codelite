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
    GetSizer()->Add(m_outputView, wxSizerFlags(1).Expand());

    m_inputCtrl = new wxTerminalInputCtrl(this, m_outputView->GetCtrl());
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
}

void wxTerminalCtrl::AppendText(const std::string& text)
{
    m_outputView->StyleAndAppend(text);
    m_outputView->SetCaretEnd();
    m_inputCtrl->SetWritePositionEnd();
}

void wxTerminalCtrl::GenerateCtrlC()
{
    if(!m_shell) {
        return;
    }

    std::string ctrlc{ 1, 0x3 };
#ifdef __WXMSW__
    ctrlc.append(1, '\n');
#endif
    m_shell->WriteRaw(ctrlc);
}

void wxTerminalCtrl::DoProcessTerminated()
{
    wxDELETE(m_shell);
    if(m_terminating) {
        wxTerminalEvent outputEvent(wxEVT_TERMINAL_CTRL_DONE);
        outputEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(outputEvent);
    } else {
        StartShell();
    }
}

void wxTerminalCtrl::SetAttributes(const wxColour& bg_colour, const wxColour& text_colour, const wxFont& font)
{
    m_outputView->SetAttributes(bg_colour, text_colour, font);
    m_outputView->ReloadSettings();
}

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
    m_outputView->Clear();
    m_inputCtrl->SetWritePositionEnd();

    if(!m_shell) {
        return;
    }
    m_shell->WriteRaw(std::string{ 1, '\n' });
}

void wxTerminalCtrl::Logout()
{
    if(!m_shell) {
        return;
    }
#if defined(__WXMSW__)
    std::string ctrld{ 1, 0x4 };
    m_shell->WriteRaw(ctrld);
#else
    std::string ctrld{ 1, 0x4 };
    m_shell->WriteRaw(ctrld + "\n");
#endif
}

void wxTerminalCtrl::SendTab()
{
#if defined(__WXMSW__)
    std::string ctrld{ 1, '\t' };
    m_shell->WriteRaw(ctrld);
#else
    std::string ctrld{ 1, '\t' };
    m_shell->WriteRaw(ctrld);
#endif
}
