#include "wxTerminalCtrl.h"

#include "Platform.hpp"
#include "StringUtils.h"
#include "TextView.h"
#include "event_notifier.h"
#include "ssh/ssh_account_info.h"
#include "wxTerminalInputCtrl.hpp"

#include <wx/app.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#include <wx/textdlg.h>
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
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &wxTerminalCtrl::OnWorkspaceLoaded, this);
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
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &wxTerminalCtrl::OnWorkspaceLoaded, this);
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

void wxTerminalCtrl::AppendText(const wxString& text)
{
    wxString window_title;
    m_outputView->StyleAndAppend(text, &window_title);
    m_outputView->SetCaretEnd();
    m_inputCtrl->SetWritePositionEnd();

    if(!window_title.empty()) {
        wxTerminalEvent titleEvent(wxEVT_TERMINAL_CTRL_SET_TITLE);
        titleEvent.SetEventObject(this);
        titleEvent.SetString(window_title);
        GetEventHandler()->AddPendingEvent(titleEvent);
    }
}

void wxTerminalCtrl::GenerateCtrlC()
{
    if(!m_shell) {
        return;
    }

    wxString ctrlc;
    ctrlc.append(1, (char)0x3);
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

void wxTerminalCtrl::OnProcessOutput(clProcessEvent& event)
{
    AppendText(event.GetOutputRaw());
    PromptForPasswordIfNeeded();
}

void wxTerminalCtrl::OnProcessError(clProcessEvent& event)
{
    AppendText(event.GetOutputRaw());
    PromptForPasswordIfNeeded();
}

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

void wxTerminalCtrl::PromptForPasswordIfNeeded()
{
    wxString line = m_outputView->GetLineText(m_outputView->GetNumberOfLines() - 1);
    line = line.Lower();
    if(line.Contains("password:") || line.Contains("password for") || line.Contains("pin for")) {
        wxString pass = ::wxGetPasswordFromUser(line, "CodeLite", wxEmptyString, wxTheApp->GetTopWindow());
        if(pass.empty()) {
            GenerateCtrlC();
            return;
        } else if(m_shell) {
            m_shell->Write(pass);
        }
    }
}

void wxTerminalCtrl::ClearScreen()
{
    auto ctrl = m_outputView->GetCtrl();
    if(ctrl->GetLineCount() <= 1) {
        return;
    }
    int last_pos = ctrl->PositionFromLine(ctrl->GetLineCount() - 1);
    int start_pos = 0;
    ctrl->DeleteRange(start_pos, last_pos);
    m_inputCtrl->UpdateTextDeleted(last_pos - start_pos);
}

void wxTerminalCtrl::Logout()
{
    if(!m_shell) {
        return;
    }
    wxString ctrld;
    ctrld.append(1, (char)0x4);
#if defined(__WXMSW__)
    m_shell->WriteRaw(ctrld);
#else
    m_shell->WriteRaw(ctrld + "\n");
#endif
}

void wxTerminalCtrl::SendTab() {}

void wxTerminalCtrl::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip(); // allways skip this event
}

void wxTerminalCtrl::SSHAndSetWorkingDirectory(const wxString& ssh_account, const wxString& path)
{
#if USE_SFTP
    wxString ssh_exe;
    if(!ThePlatform->Which("ssh", &ssh_exe)) {
        return;
    }
    auto account = SSHAccountInfo::LoadAccount(ssh_account);
    if(account.GetAccountName().empty()) {
        return;
    }
    // build the ssh command
    wxString command;
    command << StringUtils::WrapWithDoubleQuotes(ssh_exe) << " -tt ";
    if(!account.GetUsername().empty()) {
        command << account.GetUsername() << "@" << account.GetHost();
    }
    wxString remote_dir = path;
    if(!remote_dir.empty()) {
        command << " \"cd " << path << "; $SHELL\"";
    }
    Run(command);
#else
    wxUnusedVar(ssh_account);
    wxUnusedVar(remote_workspace_file);
#endif
}

void wxTerminalCtrl::SetTerminalWorkingDirectory(const wxString& path)
{
    m_shell->Write(wxString() << "cd " << StringUtils::WrapWithDoubleQuotes(path));
}

bool wxTerminalCtrl::IsFocused() { return m_outputView->GetCtrl()->HasFocus(); }