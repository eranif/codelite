#include "cl_make_generator_app.h"
#include <workspace.h>
#include <wx/filename.h>
#include <builder_gnumake.h>
#include <configuration_mapping.h>
#include <macromanager.h>
#include <wx/crt.h>
#include <globals.h>
#include <build_settings_config.h>
#include <algorithm>

IMPLEMENT_APP_CONSOLE(clMakeGeneratorApp)

static const wxCmdLineEntryDesc g_cmdDesc[] = { { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
                                                    wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, "w", "workspace", "CodeLite workspace file", wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_OPTION_MANDATORY },
    { wxCMD_LINE_OPTION, "c", "config", "Workspace configuration name to use", wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_OPTION_MANDATORY },
    { wxCMD_LINE_OPTION, "d", "command",
        "Which command to run? possible values are: build, clean or rebuild. The default is to build" },
    { wxCMD_LINE_OPTION, "p", "project",
        "Project to build, if non given CodeLite will build the active project as defined in the workspace",
        wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_SWITCH, "v", "verbose", "Run in verbose mode and print all log lines to the stdout/stderr" },
    { wxCMD_LINE_SWITCH, "e", "execute", "Instead of printing the command line, execute it" },
    { wxCMD_LINE_OPTION, "s", "settings",
        "The full path of the build_settings.xml file.\n"
        "By default, CodeLite-make will load the compiler definitions from\n"
        "%appdata%\\CodeLite\\config\\build_settings.xml (or the equivalent path on\n"
        "Unix systems). Passing -s|--settings will override the default search\n"
        "location",
        wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_NONE } };

clMakeGeneratorApp::clMakeGeneratorApp()
    : m_verbose(false)
    , m_executeCommand(false)
    , m_exitCode(0)
    , m_commandType(kBuild)
{
}

clMakeGeneratorApp::~clMakeGeneratorApp() {}

int clMakeGeneratorApp::OnExit()
{
    BuildSettingsConfigST::Free();
    return TRUE;
}

bool clMakeGeneratorApp::OnInit()
{
    SetAppName("codelite");
    wxLog::EnableLogging(false);
    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if(!DoParseCommandLine(parser)) return false;

    // Load compilers settings
    if(!BuildSettingsConfigST::Get()->Load("2.1", m_buildSettingsXml)) {
        Error("Could not load build settings configuration object (Version 2.1 / build_settings.xml)");
        return false;
    }

    wxFileName fnWorkspace(m_workspaceFile);
    if(fnWorkspace.IsRelative()) {
        fnWorkspace.MakeAbsolute(m_workingDirectory);
    }

    Info(wxString() << "-- Generating makefile for workspace file " << fnWorkspace.GetFullPath());
    wxString errmsg;
    if(!clCxxWorkspaceST::Get()->OpenWorkspace(fnWorkspace.GetFullPath(), errmsg)) {
        Error(wxString() << "Error while loading workspace: " << fnWorkspace.GetFullPath() << ". " << errmsg);
        return false;
    }

    if(m_project.IsEmpty()) {
        m_project = clCxxWorkspaceST::Get()->GetActiveProjectName();
    }

    // Set the active project to the configuration set the by the user
    BuildMatrixPtr buildMatrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    WorkspaceConfigurationPtr workspaceConfig = buildMatrix->GetConfigurationByName(m_configuration);
    if(!workspaceConfig) {
        Error(wxString() << "Could not find workspace configuration: " << m_configuration);
        return false;
    }
    const WorkspaceConfiguration::ConfigMappingList& mapping = workspaceConfig->GetMapping();
    WorkspaceConfiguration::ConfigMappingList::const_iterator iter = std::find_if(
        mapping.begin(), mapping.end(), [&](const ConfigMappingEntry& entry) { return entry.m_project == m_project; });
    if(iter == mapping.end()) {
        Error(wxString() << "Unable to locate a project configuration that matches the workspace configuration '"
                         << m_configuration << "'");
        return false;
    }
    buildMatrix->SetSelectedConfigurationName(m_configuration);
    wxString projectConfiguraion = iter->m_name;
    Info(wxString() << "-- Building project configuration: " << projectConfiguraion);
    
    // Which makefile should we create?
    BuilderGnuMake builder;
    ProjectPtr project = clCxxWorkspaceST::Get()->FindProjectByName(m_project, errmsg);
    if(!project) {
        Error(wxString() << "Could not find project " << m_project << ". " << errmsg);
        return false;
    }

    // Load the build configuration
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(m_project, projectConfiguraion);
    if(!bldConf) {
        Error(wxString() << "Could not find configuration " << projectConfiguraion << " for project " << m_project);
        return false;
    }

    if(bldConf->IsCustomBuild()) {
        Notice(wxString() << "Configuration " << projectConfiguraion << " for project " << m_project
                          << " is using a custom build - will not generate makefile");
        Notice(wxString() << "Instead, here is the command line to use:");
        wxString command;
        wxString workingDirectory = MacroManager::Instance()->Expand(
                       bldConf->GetCustomBuildWorkingDir(), NULL, m_project, bldConf->GetName());
        if(wxFileName::DirExists(workingDirectory) == false) {
            Info(wxString() << "-- Creating build directory: " << workingDirectory);
            wxFileName::Mkdir(workingDirectory, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        command << "cd "
                << workingDirectory
                << " && "
                << MacroManager::Instance()->Expand(bldConf->GetCustomBuildCmd(), NULL, m_project, bldConf->GetName());
        Out(command);
        if(m_executeCommand) {
            CallAfter(&clMakeGeneratorApp::DoExecCommand, command);
        } else {
            CallAfter(&clMakeGeneratorApp::DoExitApp);
        }
        return true;
    }

    wxString args = bldConf->GetBuildSystemArguments();
    if(!builder.Export(m_project, projectConfiguraion, args, false, true, errmsg)) {
        Error(wxString() << "Error while exporting makefile. " << errmsg);
        return false;
    }

    wxString commandToRun;
    switch(m_commandType) {
    case kBuild:
        commandToRun = builder.GetBuildCommand(m_project, projectConfiguraion, args);
        break;
    case kClean:
        commandToRun = builder.GetCleanCommand(m_project, projectConfiguraion, args);
        break;
    case kRebuild:
        commandToRun = builder.GetCleanCommand(m_project, projectConfiguraion, args);
        // append the build command
        commandToRun << " && " << builder.GetBuildCommand(m_project, projectConfiguraion, args);
        break;
    }

    wxString workspace_path = fnWorkspace.GetPath();
    if(workspace_path.Contains(" ") || workspace_path.Contains("\t")) {
        workspace_path.Prepend("\"").Append("\"");
    }

    Info("-- Makefile generation completed successfully!");
    wxString command;
    command << "cd " << workspace_path << " && " << commandToRun;

    if(m_executeCommand) {
        CallAfter(&clMakeGeneratorApp::DoExecCommand, command);

    } else {
        Info("-- To use the makefile, run the following commands from a terminal:");
        Out(command);
        CallAfter(&clMakeGeneratorApp::DoExitApp);
    }
    return true;
}

bool clMakeGeneratorApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    parser.SetDesc(g_cmdDesc);
    parser.AddUsageText(_("A makefile generator based on codelite's workspace"));

    int res = parser.Parse(false);
    if(res == wxNOT_FOUND) return false;

    if(!parser.Found("w", &m_workspaceFile)) {
        parser.Usage();
        return false;
    }

    if(!parser.Found("c", &m_configuration)) {
        parser.Usage();
        return false;
    }

    if(parser.Found("e")) {
        m_executeCommand = true;
    }

    parser.Found("s", &m_buildSettingsXml);

    wxString command;
    if(parser.Found("d", &command)) {
        if(command == "build") {
            m_commandType = kBuild;
        } else if(command == "rebuild") {
            m_commandType = kRebuild;
        } else if(command == "clean") {
            m_commandType = kClean;
        } else {
            parser.Usage();
            return false;
        }
    }

    parser.Found("p", &m_project);
    m_verbose = (parser.FoundSwitch("v") == wxCMD_SWITCH_ON);
    m_workingDirectory = ::wxGetCwd();
    return true;
}

void clMakeGeneratorApp::DoExitApp()
{
    ExitMainLoop();
    // Force an exit here
    exit(m_exitCode);
}

// Log functions
void clMakeGeneratorApp::Error(const wxString& msg)
{
    wxString e;
    e << "[ERROR ] " << msg;
    wxFprintf(stderr, "%s\n", e);
}

void clMakeGeneratorApp::Notice(const wxString& msg)
{
    if(m_verbose) {
        wxString e;
        e << "[NOTICE] " << msg;
        wxFprintf(stderr, "%s\n", e);
    }
}

void clMakeGeneratorApp::Info(const wxString& msg)
{
    if(m_verbose) {
        wxString e;
        e << "[INFO  ] " << msg;
        wxFprintf(stdout, "%s\n", e);
    }
}

void clMakeGeneratorApp::Out(const wxString& msg) { wxPrintf("%s\n", msg); }

void clMakeGeneratorApp::DoExecCommand(const wxString& command)
{
    wxString cmd = command;
    WrapInShell(cmd);
    wxPrintf(cmd + "\n");
    m_exitCode = ::wxExecute(cmd, wxEXEC_SYNC | wxEXEC_NOHIDE | wxEXEC_SHOW_CONSOLE);
    CallAfter(&clMakeGeneratorApp::DoExitApp);
}
