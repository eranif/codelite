#include "LLAMCli.hpp"

#include "AsyncProcess/processreaderthread.h"
#include "StringUtils.h"
#include "event_notifier.h"
#include "globals.h"

#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

wxDEFINE_EVENT(wxEVT_LLAMACLI_STARTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LLAMACLI_STDOUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LLAMACLI_STDERR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LLAMACLI_TERMINATED, clCommandEvent);

#ifdef __WXMSW__
#include <windows.h>
#endif

namespace
{
const wxString PromptFile = "ChatAI.prompt";
const wxString CHATAI_PROMPT_STRING = "ChatAI is ready to assist!";
} // namespace

LLAMCli::LLAMCli()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LLAMCli::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &LLAMCli::OnProcessStderr, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LLAMCli::OnProcessOutput, this);
}

LLAMCli::~LLAMCli()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &LLAMCli::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_STDERR, &LLAMCli::OnProcessStderr, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &LLAMCli::OnProcessOutput, this);

    wxDELETE(m_process);
}

bool LLAMCli::IsOk() const
{
    return m_config.GetSelectedModel() != nullptr && ::wxFileExists(m_config.GetLlamaCli()) &&
           ::wxFileExists(m_config.GetSelectedModel()->m_modelFile);
}

void LLAMCli::OnProcessOutput(clProcessEvent& event)
{
    clCommandEvent event_stdout{ wxEVT_LLAMACLI_STDOUT };
    event_stdout.SetString(event.GetOutput());
    EventNotifier::Get()->AddPendingEvent(event_stdout);
}

void LLAMCli::OnProcessTerminated(clProcessEvent& event)
{
    clCommandEvent event_exit{ wxEVT_LLAMACLI_TERMINATED };
    EventNotifier::Get()->AddPendingEvent(event_exit);
    wxDELETE(m_process);

    wxString prompt_file = wxFileName(clStandardPaths::Get().GetTempDir(), PromptFile).GetFullPath();
    FileUtils::Deleter deleter{ prompt_file };
}

void LLAMCli::OnProcessStderr(clProcessEvent& event) { clDEBUG() << event.GetOutput() << endl; }

void LLAMCli::Stop()
{
    if (m_process) {
        m_process->Terminate();
    }
}

bool LLAMCli::StartProcess()
{
    if (IsRunning()) {
        // already running
        return true;
    }

    std::vector<wxString> command = { GetConfig().GetLlamaCli(),
                                      "-m",
                                      GetConfig().GetSelectedModel()->m_modelFile,
                                      "-t",
                                      "2",
                                      "--ctx-size",
                                      "8192",
                                      "-p",
                                      CHATAI_PROMPT_STRING,
                                      "-cnv" /* converstion mode */ };

    m_process = ::CreateAsyncProcess(
        this, command, IProcessCreateWithHiddenConsole | IProcessStderrEvent | IProcessWrapInShell);
    if (!m_process) {
        ::wxMessageBox(wxString() << _("Failed to launch command: '") << GetConfig().GetLlamaCli() << "'",
                       "CodeLite",
                       wxOK | wxCENTER | wxICON_ERROR);
        return false;
    }

    clCommandEvent event_start{ wxEVT_LLAMACLI_STARTED };
    EventNotifier::Get()->AddPendingEvent(event_start);

    return true;
}

void LLAMCli::Send(const wxString& prompt)
{
    if (!IsOk() || !IsRunning()) {
        return;
    }

    wxString modified_prompt = prompt;
    modified_prompt.Replace("\r\n", "\n");
    modified_prompt.Replace("\n", "\\\n");
    m_process->Write(modified_prompt);
}

void LLAMCli::Interrupt()
{
    if (!IsRunning()) {
        return;
    }

    wxString ctrlc;
    ctrlc.append(1, (char)0x3);

#ifdef __WXMSW__
    // Send CTRL+C signal to the child process
    ctrlc << "\n";
#endif
    m_process->WriteRaw(ctrlc);
}
