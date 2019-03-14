#include "CompileCommandsGenerator.h"
#include "processreaderthread.h"
#include "file_logger.h"
#include "CompileCommandsCreateor.h"
#include "event_notifier.h"
#include "workspace.h"
#include "globals.h"
#include "environmentconfig.h"
#include "imanager.h"

CompileCommandsGenerator::CompileCommandsGenerator() {}

CompileCommandsGenerator::~CompileCommandsGenerator()
{
    // If the child process is still running, detach from it. i.e. OnProcessTeraminated() event is not called
    if(m_process) { m_process->Detach(); }
}

void CompileCommandsGenerator::OnProcessTeraminated(wxProcessEvent& event)
{
    // dont call event.Skip() so we will delete the m_process ourself
    wxDELETE(m_process);
    clGetManager()->SetStatusMessage(_("Ready"));

    // Notify about completion
    clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
    eventCompileCommandsGenerated.SetFileName(m_outputFile.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventCompileCommandsGenerated);
}

void CompileCommandsGenerator::GenerateCompileCommands()
{
    if(m_process) { m_process->Detach(); }
    m_process = new wxProcess();
    m_process->Bind(wxEVT_END_PROCESS, &CompileCommandsGenerator::OnProcessTeraminated, this);

    wxFileName codeliteMake(clStandardPaths::Get().GetBinFolder(), "codelite-make");
#ifdef __WXMSW__
    codeliteMake.SetExt("exe");
#endif

    if(!clCxxWorkspaceST::Get()->IsOpen()) { return; }

    if(!codeliteMake.FileExists()) {
        clWARNING() << "Could not find" << codeliteMake;
        return;
    }

    wxString command;
    command << codeliteMake.GetFullPath();
    ::WrapWithQuotes(command);

    wxString workspaceFile = clCxxWorkspaceST::Get()->GetFileName().GetFullPath();
    ::WrapWithQuotes(workspaceFile);

    wxString configName =
        clCxxWorkspaceST::Get()->GetSelectedConfig() ? clCxxWorkspaceST::Get()->GetSelectedConfig()->GetName() : "";
    command << " --workspace=" << workspaceFile << " --verbose --json --config=" << configName;

    EnvSetter env(clCxxWorkspaceST::Get()->GetActiveProject());
    ::wxExecute(command, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_HIDE_CONSOLE, m_process);

    m_outputFile = workspaceFile;
    m_outputFile.SetFullName("compile_commands.json");

    clGetManager()->SetStatusMessage(wxString() << _("Generating ") << m_outputFile.GetFullPath(), 2);
}
