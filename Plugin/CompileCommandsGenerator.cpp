#include "CompileCommandsGenerator.h"
#include "CompileCommandsJSON.h"
#include "CompileFlagsTxt.h"
#include "JSON.h"
#include "clFilesCollector.h"
#include "cl_config.h"
#include "clcommandlineparser.h"
#include "compiler_command_line_parser.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "workspace.h"
#include "wxmd5.h"
#include <macros.h>
#include <thread>
#include "clFileSystemWorkspace.hpp"

wxDEFINE_EVENT(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

CompileCommandsGenerator::CompileCommandsGenerator()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
}

CompileCommandsGenerator::~CompileCommandsGenerator()
{
    // If the child process is still running, detach from it. i.e. OnProcessTeraminated() event is not called
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
    if(m_process) { m_process->Detach(); }
    wxDELETE(m_process);
}

typedef wxString CheckSum_t;
static CheckSum_t ComputeFileCheckSum(const wxFileName& fn) { return wxMD5::GetDigest(fn); }

void CompileCommandsGenerator::OnProcessTeraminated(clProcessEvent& event)
{
    // dont call event.Skip() so we will delete the m_process ourself
    wxDELETE(m_process);
    clGetManager()->SetStatusMessage(_("Ready"));

    static std::unordered_map<wxString, CheckSum_t> m_checksumCache;

    bool generateCompileCommands = false;
    generateCompileCommands = clConfig::Get().Read(wxString("GenerateCompileCommands"), generateCompileCommands);

    // Process the compile_flags.txt files starting from the "compile_commands.json" root folder
    // Notify about completion
    std::thread thr(
        [=](const wxString& compile_commands) {
            // Calculate the new file checksum
            CheckSum_t ck = ComputeFileCheckSum(compile_commands);
            CheckSum_t oldCk;
            clDEBUG() << "New checksum is:" << ck;
            if(m_checksumCache.count(compile_commands)) {
                oldCk = m_checksumCache.find(compile_commands)->second;
            } else {
                clDEBUG() << "File:" << compile_commands << "is not found in the cache";
            }
            clDEBUG() << "Old checksum is:" << oldCk;
            if(ck == oldCk) {
                clDEBUG() << "No changes detected in file:" << compile_commands << "processing is ignored";
                // We fire this event with empty content. This ensures that
                // a LSP restart will take place
                clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
                EventNotifier::Get()->QueueEvent(eventCompileCommandsGenerated.Clone());
                return;
            }

            // store the new checksum
            m_checksumCache.erase(compile_commands);
            m_checksumCache.insert({ compile_commands, ck });

            // Process compile_flags.txt files
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

            if(generateCompileCommands) {
                CompileCommandsJSON compileCommands(compile_commands);
                const wxArrayString& paths = compileCommands.GetIncludes();
                for(const wxString& path : paths) {
                    if(includeSet.count(path) == 0) {
                        includeSet.insert(path);
                        includePaths.Add(path);
                    }
                }
            }
            clDEBUG() << "wxEVT_COMPILE_COMMANDS_JSON_GENERATED paths:\n" << includePaths;

            // Notify about it
            clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
            // compile_commands.json
            eventCompileCommandsGenerated.SetFileName(compile_commands);
            // include paths found and gathered from all the compile_flags.txt files scanned
            eventCompileCommandsGenerated.SetStrings(includePaths);
            EventNotifier::Get()->QueueEvent(eventCompileCommandsGenerated.Clone());
        },
        m_outputFile.GetFullPath());
    thr.detach();
}

void CompileCommandsGenerator::GenerateCompileCommands()
{
    // Kill any previous process running
    if(m_process) { m_process->Detach(); }
    wxDELETE(m_process);
    
    if(!clCxxWorkspaceST::Get()->IsOpen()) { return; }
    if(!clCxxWorkspaceST::Get()->GetActiveProject()) { return; }

    wxFileName codeliteMake(clStandardPaths::Get().GetBinFolder(), "codelite-make");
#ifdef __WXMSW__
    codeliteMake.SetExt("exe");
#endif

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

    bool generateCompileCommands = false;
    generateCompileCommands = clConfig::Get().Read(wxString("GenerateCompileCommands"), generateCompileCommands);

    command << " --workspace=" << workspaceFile << " --verbose";

    // if we are required to generate compile_commands.json, pass the --json flags
    // not passing it means only compile_flags.txt files are generated
    if(generateCompileCommands) {
        command << " --json ";
    } else {
        command << " --compile-flags ";
    }
    command << " --config=" << configName;

    // since we might be activated with a different settings directory
    // pass the build_settings.xml to codelite-make

    wxFileName xmlFile(clStandardPaths::Get().GetUserDataDir(), "build_settings.xml");
    xmlFile.AppendDir("config");
    wxString xmlPath = xmlFile.GetFullPath();
    ::WrapWithQuotes(xmlPath);
    command << " --settings=" << xmlPath;

    clDEBUG() << "Executing:" << command;

    EnvSetter env(clCxxWorkspaceST::Get()->GetActiveProject());
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault);

    m_outputFile = workspaceFile;
    m_outputFile.SetFullName("compile_commands.json");

    clGetManager()->SetStatusMessage(wxString() << _("Generating ") << m_outputFile.GetFullPath(), 2);
}
