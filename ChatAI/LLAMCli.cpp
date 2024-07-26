#include "LLAMCli.hpp"

#include "AsyncProcess/processreaderthread.h"
#include "StringUtils.h"
#include "event_notifier.h"
#include "globals.h"

wxDEFINE_EVENT(wxEVT_LLAMACLI_STARTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LLAMACLI_STDOUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LLAMACLI_STDERR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LLAMACLI_TERMINATED, clCommandEvent);

namespace
{
const wxString PromptFile = "ChatAI.prompt";
}

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

void LLAMCli::OnProcessStderr(clProcessEvent& event)
{
    clCommandEvent event_stderr{ wxEVT_LLAMACLI_STDERR };
    event_stderr.SetString(event.GetOutput());
    EventNotifier::Get()->AddPendingEvent(event_stderr);
}

void LLAMCli::Stop()
{
    if (m_process) {
        m_process->Terminate();
    }
}

void LLAMCli::Send(const wxString& prompt)
{
    if (IsRunning() || !IsOk()) {
        return;
    }

    wxString prompt_file = wxFileName(clStandardPaths::Get().GetTempDir(), PromptFile).GetFullPath();
    FileUtils::WriteFileContent(prompt_file, prompt);

    std::vector<wxString> command = {
        GetConfig().GetLlamaCli(),
        "--log-disable",
        "--simple-io",
        "-m",
        GetConfig().GetSelectedModel()->m_modelFile,
        "-f",
        prompt_file,
    };

    m_process = ::CreateAsyncProcess(this, command, IProcessCreateWithHiddenConsole | IProcessStderrEvent);
    if (!m_process) {
        ::wxMessageBox(wxString() << _("Failed to launch command: '") << GetConfig().GetLlamaCli() << "'", "CodeLite",
                       wxOK | wxCENTER | wxICON_ERROR);
        FileUtils::Deleter deleter{ prompt_file };
        return;
    }

    clCommandEvent event_start{ wxEVT_LLAMACLI_STARTED };
    EventNotifier::Get()->AddPendingEvent(event_start);
}
