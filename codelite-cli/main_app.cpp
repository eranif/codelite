#include "cl_standard_paths.h"
#include "csConfig.h"
#include "file_logger.h"
#include "main_app.h"
#include <SocketAPI/clSocketServer.h>
#include <iostream>
#include <wx/filename.h>
#include <wx/crt.h>

IMPLEMENT_APP_CONSOLE(MainApp)

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    { wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_PARAM, "c", "command", "command", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_PARAM, "o", "options", "options", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
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
    SetAppName("codelite-cli");

    // Make sure that the 'local' folder exists
    wxFileName::Mkdir(clStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Mainly needed for Windows
    clSocketBase::Initialize();

    // Allocate the manager class
    m_manager = new csManager();

    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    DoParseCommandLine(parser);

    // Open the log file
    FileLogger::OpenLog("codelite-cli.log", FileLogger::Developer);

    // Startup
    return m_manager->Startup();
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    parser.SetDesc(cmdLineDesc);
    if(parser.Parse(false) != 0) {
        return false;
    }
    
    m_manager->GetCommand() = parser.GetParam(0);
    m_manager->GetOptions() = parser.GetParam(1);
    
    if(parser.Found("v")) {
        // Print version and exit
        std::cout << "codelite-cli v1.0" << std::endl;
        m_manager->SetExitNow(true);
        return true;
    }
    
    if(parser.Found("h")) {
        m_manager->SetExitNow(true);
        PrintUsage(parser);
        return true;
    }

    if(m_manager->GetCommand().IsEmpty()) {
        // Try to fetch the options from the INI file
        m_manager->LoadCommandFromINI();
        if(m_manager->GetCommand().IsEmpty()) {
            return false;
        }
    }
    return true;
}

void MainApp::PrintUsage(const wxCmdLineParser& parser)
{
    wxString usageString = parser.GetUsageString();
    std::cerr << usageString.mb_str(wxConvUTF8).data() << std::endl;
}
