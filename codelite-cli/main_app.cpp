#include "cl_standard_paths.h"
#include "csConfig.h"
#include "file_logger.h"
#include "main_app.h"
#include <SocketAPI/clSocketServer.h>
#include <wx/filename.h>
#include <iostream>

IMPLEMENT_APP_CONSOLE(MainApp)

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    { wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "c", "command", "command to execute", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
    { wxCMD_LINE_OPTION, "o", "options", "the command optinos, in JSON format", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

MainApp::MainApp()
    : m_manager(nullptr)
{
}

MainApp::~MainApp() {}

int MainApp::OnExit()
{
    clDEBUG() << "Going down";
    wxDELETE(m_manager);
    return TRUE;
}

bool MainApp::OnInit()
{
    SetAppName("codelite-server");

    // Mainly needed for Windows
    clSocketBase::Initialize();

    // Allocate the manager class
    m_manager = new csManager();

    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if(!DoParseCommandLine(parser)) {
        PrintUsage(parser);
        return false;
    }

    // Make sure that the 'local' folder exists
    wxFileName::Mkdir(clStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Open the log file
    FileLogger::OpenLog("codelite-server.log", FileLogger::Developer);

    // Startup
    return m_manager->Startup();
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    parser.SetDesc(cmdLineDesc);
    if(parser.Parse(false) != 0) {
        return false;
    }

    if(!parser.Found("c", &m_manager->GetCommand())) {
        return false;
    }
    
    // read the options
    parser.Found("o", &m_manager->GetOptions());
    return true;
}

void MainApp::PrintUsage(const wxCmdLineParser& parser)
{
    wxString usageString = parser.GetUsageString();
    std::cerr << usageString.mb_str(wxConvUTF8).data() << std::endl;
}
