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
CompileCommandsGenerator::~CompileCommandsGenerator() {}

void CompileCommandsGenerator::OnProcessOutput(clProcessEvent& event) { clSYSTEM() << event.GetOutput(); }

void CompileCommandsGenerator::OnProcessTeraminated(clProcessEvent& event)
{
    m_childProcess.reset(nullptr);
    clGetManager()->SetStatusMessage(_("Ready"));

    // Notify about completion
    clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
    eventCompileCommandsGenerated.SetFileName(m_outputFile.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventCompileCommandsGenerated);
}

void CompileCommandsGenerator::GenerateCompileCommands()
{
    m_childProcess.reset(new ChildProcess());
    m_childProcess->Bind(wxEVT_CHILD_PROCESS_STDOUT, &CompileCommandsGenerator::OnProcessOutput, this);
    m_childProcess->Bind(wxEVT_CHILD_PROCESS_EXIT, &CompileCommandsGenerator::OnProcessTeraminated, this);

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

    wxArrayString args;
    args.Add(command);
    args.Add("--workspace=" + workspaceFile);
    args.Add("--verbose");
    args.Add("--json");
    args.Add("--config=" + configName);

    EnvSetter env(clCxxWorkspaceST::Get()->GetActiveProject());
    m_childProcess->Start(args, wxFileName(workspaceFile).GetPath());

    m_outputFile = workspaceFile;
    m_outputFile.SetFullName("compile_commands.json");

    clGetManager()->SetStatusMessage(wxString() << _("Generating ") << m_outputFile.GetFullPath(), 2);
}
