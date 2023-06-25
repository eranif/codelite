#include "wxTerminalCtrl.h"

#include "Platform.hpp"
#include "StringUtils.h"
#include "TextView.h"
#include "clModuleLogger.hpp"
#include "environmentconfig.h" // EnvSetter
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

INITIALISE_MODULE_LOG(TERM_LOG, "Terminal", "terminal.log");

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
    wxDELETE(m_inputCtrl);
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
    wxString bash_exec;
    if(!ThePlatform->Which("bash", &bash_exec)) {
        wxMessageBox(_("Unable to find bash. Can't start a terminal"), "CodeLite",
                     wxICON_WARNING | wxOK | wxCENTRE | wxOK_DEFAULT);
        return;
    }

    wxString path;
    ThePlatform->GetPath(&path, true);

    // override PATH with our own
    clEnvList_t env = { { "PATH", path } };
#ifdef __WXMSW__
    env.push_back({ "LOGINSHELL", "bash" });
    env.push_back({ "MSYS2_PATH", "/usr/local/bin:/usr/bin:/bin" });
    env.push_back({ "MSYS2_NOSTART", "yes" });
    env.push_back({ "MSYSTEM_PREFIX", "/usr" });
    env.push_back({ "MSYSTEM", "MSYS" });
    env.push_back({ "WD", wxFileName(bash_exec).GetPath() });
#endif

    LOG_DEBUG(TERM_LOG) << "Starting shell process:" << bash_exec << endl;
    m_shell = ::CreateAsyncProcess(this, bash_exec + " --login -i", IProcessCreateWithHiddenConsole | IProcessRawOutput,
                                   wxEmptyString, &env);
    if(m_shell) {
        wxTerminalEvent readyEvent(wxEVT_TERMINAL_CTRL_READY);
        readyEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(readyEvent);
    }
    m_inputCtrl->SetFocus();
}

void wxTerminalCtrl::Run(const wxString& command)
{
    if(!m_shell) {
        return;
    }
    LOG_DEBUG(TERM_LOG) << "-->" << command << endl;
    m_shell->WriteRaw(command + "\n");

    wxStringView sv{ command };
    AppendText(sv);

    wxStringView eol(wxT("\n"), 1);
    AppendText(eol);
}

void wxTerminalCtrl::AppendText(wxStringView text)
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

    // Clear the output buffer
    m_processOutput.clear();

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
    m_processOutput.push_back(event.GetOutput());
    ProcessOutputBuffer();
}

void wxTerminalCtrl::OnProcessError(clProcessEvent& event)
{
    m_processOutput.push_back(event.GetOutput());
    ProcessOutputBuffer();
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

bool wxTerminalCtrl::PromptForPasswordIfNeeded()
{
    wxString line = m_outputView->GetLineText(m_outputView->GetNumberOfLines() - 1);
    line = line.Lower();
    if(line.Contains("password:") || line.Contains("password for") || line.Contains("pin for")) {
        wxString pass = ::wxGetPasswordFromUser(line, "CodeLite", wxEmptyString, wxTheApp->GetTopWindow());
        if(pass.empty()) {
            GenerateCtrlC();
        } else if(m_shell) {
            m_shell->Write(pass);
        }
        return true;
    } else {
        return false;
    }
}

void wxTerminalCtrl::ClearScreen() { m_outputView->Clear(); }

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

bool wxTerminalCtrl::IsFocused() { return m_inputCtrl->IsFocused(); }

bool wxTerminalCtrl::GetOutputBuffer(wxString* buffer)
{
    if(m_processOutput.empty()) {
        return false;
    }
    constexpr int BUFSIZE = 8192;
    if(m_processOutput.begin()->size() > BUFSIZE) {
        *buffer = m_processOutput.begin()->substr(0, BUFSIZE);
        (*m_processOutput.begin()).erase(0, BUFSIZE);
    } else {
        *buffer = *m_processOutput.begin();
        m_processOutput.erase(m_processOutput.begin());
    }
    return true;
}

void wxTerminalCtrl::ProcessOutputBuffer()
{
    if(m_processOutput.empty()) {
        return;
    }

    wxString buffer_to_process;
    while(GetOutputBuffer(&buffer_to_process)) {
        LOG_DEBUG(TERM_LOG) << "<--" << buffer_to_process << endl;
        wxStringView sv{ buffer_to_process };
        AppendText(sv);
        // see if we need to prompt for password
        if(PromptForPasswordIfNeeded()) {
            return;
        }
    }

    if(m_processOutput.empty()) {
        m_inputCtrl->CallAfter(&wxTerminalInputCtrl::NotifyTerminalOutput);
    }
}