#include "CompileCommandsGenerator.h"
#include "processreaderthread.h"
#include "file_logger.h"
#include "event_notifier.h"
#include "workspace.h"
#include "globals.h"
#include "environmentconfig.h"
#include "imanager.h"
#include <thread>
#include <macros.h>
#include "clFilesCollector.h"
#include "fileutils.h"
#include "JSON.h"
#include "clcommandlineparser.h"
#include "compiler_command_line_parser.h"
#include "CompileFlagsTxt.h"
#include "CompileCommandsJSON.h"

wxDEFINE_EVENT(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

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

    // Process the compile_flags.txt files starting from the "compile_commands.json" root folder
    // Notify about completion
    std::thread thr(
        [=](const wxString& compile_commands) {
            clFilesScanner scanner;
            wxArrayString includePaths;
            wxStringSet_t includeSet;
            std::vector<wxString> files;
            if(scanner.Scan(wxFileName(compile_commands).GetPath(), files, "compile_flags.txt")) {
                for(const wxString& file : files) {
                    CompileFlagsTxt f(file);
                    includePaths = f.GetIncludes();
                    includeSet.insert(includePaths.begin(), includePaths.end());
                }
            }

            CompileCommandsJSON compileCommands(compile_commands);
            const wxArrayString& paths = compileCommands.GetIncludes();
            for(const wxString& path : paths) {
                if(includeSet.count(path) == 0) {
                    includeSet.insert(path);
                    includePaths.Add(path);
                }
            }

            clDEBUG() << "wxEVT_COMPILE_COMMANDS_JSON_GENERATED paths:\n" << includePaths;

            // Notify about it
            clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
            eventCompileCommandsGenerated.SetFileName(compile_commands); // compile_commands.json
            eventCompileCommandsGenerated.SetStrings(
                includePaths); // include paths found and gathered from all the compile_flags.txt files scanned
            EventNotifier::Get()->AddPendingEvent(eventCompileCommandsGenerated);
        },
        m_outputFile.GetFullPath());
    thr.detach();
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
