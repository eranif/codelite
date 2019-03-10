#include "CompileCommandsGenerator.h"
#include "processreaderthread.h"
#include "file_logger.h"
#include "CompileCommandsCreateor.h"
#include "event_notifier.h"
#include "workspace.h"
#include "globals.h"
#include "environmentconfig.h"
#include "imanager.h"

CompileCommandsGenerator::CompileCommandsGenerator()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &CompileCommandsGenerator::OnProcessTeraminated, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
}

CompileCommandsGenerator::~CompileCommandsGenerator()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &CompileCommandsGenerator::OnProcessTeraminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
}

void CompileCommandsGenerator::OnProcessOutput(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }

void CompileCommandsGenerator::OnProcessTeraminated(clProcessEvent& event)
{
    wxDELETE(m_process);

    clGetManager()->SetStatusMessage(_("Ready"));

    // Notify about completion
    clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
    eventCompileCommandsGenerated.SetFileName(m_outputFile.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventCompileCommandsGenerated);

    // Delete this instance
    //delete this;
}

void CompileCommandsGenerator::GenerateCompileCommands()
{
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
    wxString configName =
        clCxxWorkspaceST::Get()->GetSelectedConfig() ? clCxxWorkspaceST::Get()->GetSelectedConfig()->GetName() : "";
    ::WrapWithQuotes(workspaceFile);

    command << " --workspace=" << workspaceFile << " --verbose --json --config=" << configName;
    clDEBUG() << "Running: " << command;

    EnvSetter env(clCxxWorkspaceST::Get()->GetActiveProject());
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, wxFileName(workspaceFile).GetPath());

    m_outputFile = workspaceFile;
    m_outputFile.SetFullName("compile_commands.json");

    clGetManager()->SetStatusMessage(wxString() << _("Generating ") << m_outputFile.GetFullPath());
}
