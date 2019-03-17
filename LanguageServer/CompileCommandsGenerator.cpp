#include "CompileCommandsGenerator.h"
#include "processreaderthread.h"
#include "file_logger.h"
#include "CompileCommandsCreateor.h"
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
            wxStringSet_t uniqueIncludePaths;
            std::vector<wxString> files;
            if(scanner.Scan(wxFileName(compile_commands).GetPath(), files, "compile_flags.txt")) {
                for(const wxString& file : files) {
                    wxString data;
                    if(FileUtils::ReadFileContent(file, data)) {
                        wxArrayString lines = ::wxStringTokenize(data, "\n", wxTOKEN_STRTOK);
                        for(size_t i = 0; i < lines.GetCount(); ++i) {
                            wxString& line = lines.Item(i);
                            line.Trim().Trim(false);
                            if(line.StartsWith("-I")) {
                                line.Remove(0, 2); // remove the "-I"
                                if(uniqueIncludePaths.count(line) == 0) {
                                    uniqueIncludePaths.insert(line);
                                    includePaths.Add(line);
                                }
                            }
                        }
                    }
                }
            }

            // Process the compile_commands.json file now
            JSON json(compile_commands);
            JSONItem arr = json.toElement();
            const int count = arr.arraySize();
            for(int i = 0; i < count; ++i) {
                wxString command = arr.arrayItem(i).namedObject("command").toString();
                CompilerCommandLineParser cclp(command);
                const wxArrayString& paths = cclp.GetIncludes();
                for(const wxString& path : paths) {
                    if(uniqueIncludePaths.count(path) == 0) {
                        uniqueIncludePaths.insert(path);
                        includePaths.Add(path);
                    }
                }
            }

            clDEBUG() << "wxEVT_COMPILE_COMMANDS_JSON_GENERATED paths\n" << includePaths;

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
