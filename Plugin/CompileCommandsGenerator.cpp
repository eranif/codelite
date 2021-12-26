#include "CompileCommandsGenerator.h"

#include "CompileCommandsJSON.h"
#include "CompileFlagsTxt.h"
#include "JSON.h"
#include "clFileSystemWorkspace.hpp"
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

wxDEFINE_EVENT(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

CompileCommandsGenerator::CompileCommandsGenerator()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &CompileCommandsGenerator::OnProcessOutput, this);
}

CompileCommandsGenerator::~CompileCommandsGenerator()
{
    // If the child process is still running, detach from it. i.e. OnProcessTeraminated() event is not called
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &CompileCommandsGenerator::OnProcessOutput, this);
    if(m_process) {
        m_process->Detach();
    }
    wxDELETE(m_process);
}

typedef wxString CheckSum_t;
static CheckSum_t ComputeFileCheckSum(const wxFileName& fn) { return wxMD5::GetDigest(fn); }

void CompileCommandsGenerator::OnProcessOutput(clProcessEvent& event) { m_capturedOutput << event.GetOutput(); }

void CompileCommandsGenerator::OnProcessTeraminated(clProcessEvent& event)
{
    // dont call event.Skip() so we will delete the m_process ourself
    wxDELETE(m_process);
    clGetManager()->SetStatusMessage(_("Ready"));

    wxArrayString generated_paths = ::wxStringTokenize(m_capturedOutput, "\n\r", wxTOKEN_STRTOK);
    m_capturedOutput.clear();

    static std::unordered_map<wxString, CheckSum_t> m_checksumCache;

    bool generateCompileCommands = false;
    generateCompileCommands = clConfig::Get().Read(wxString("GenerateCompileCommands"), generateCompileCommands);

    // Process the compile_flags.txt files starting from the "compile_commands.json" root folder
    // Notify about completion
    std::thread thr(
        [=](const wxArrayString& paths) {
            clDEBUG() << "Checking paths for changes:" << paths << endl;
            bool event_is_needed = false;
            for(const wxString& path : paths) {
                // Calculate the new file checksum
                CheckSum_t ck = ComputeFileCheckSum(path);
                CheckSum_t oldCk;
                clDEBUG() << "New checksum is: [" << ck << "]" << endl;
                if(m_checksumCache.count(path)) {
                    oldCk = m_checksumCache.find(path)->second;
                } else {
                    clDEBUG() << "File:" << path << "is not found in the cache";
                }
                clDEBUG() << "Old checksum is: [" << oldCk << "]";
                bool file_exists = wxFileName::FileExists(path);
                clDEBUG() << "File:" << path << "exists:" << file_exists << endl;

                if(ck != oldCk || !file_exists) {
                    event_is_needed = true;
                }

                // update the checksum in the cache
                m_checksumCache.erase(path);
                m_checksumCache.insert({ path, ck });
            }

            if(!event_is_needed) {
                clDEBUG() << "No changes detected for paths:" << paths << endl;
                // We fire this event with empty content. This ensures that
                // a LSP restart will take place
                // clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
                // EventNotifier::Get()->QueueEvent(eventCompileCommandsGenerated.Clone());
                return;
            }

            // Notify about it
            clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
            eventCompileCommandsGenerated.SetStrings(paths);
            EventNotifier::Get()->QueueEvent(eventCompileCommandsGenerated.Clone());
        },
        generated_paths);
    thr.detach();
}

void CompileCommandsGenerator::GenerateCompileCommands()
{
    // Kill any previous process running
    if(m_process) {
        m_process->Detach();
    }
    wxDELETE(m_process);

    if(!clCxxWorkspaceST::Get()->IsOpen()) {
        return;
    }
    if(!clCxxWorkspaceST::Get()->GetActiveProject()) {
        return;
    }

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

    command << " --workspace=" << workspaceFile;

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
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessWrapInShell);
    m_capturedOutput.clear();

    wxString filename = generateCompileCommands ? wxString("compile_commands.json") : wxString("compile_flags.txt");
    clGetManager()->SetStatusMessage(wxString() << _("Generating ") << filename, 2);
}
