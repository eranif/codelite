#include "cl_standard_paths.h"
#include "csConfig.h"
#include "file_logger.h"
#include "main_app.h"
#include <SocketAPI/clSocketServer.h>
#include <wx/filename.h>

IMPLEMENT_APP_CONSOLE(MainApp)

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

    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if(!DoParseCommandLine(parser)) { return false; }

    // Make sure that the 'local' folder exists
    wxFileName::Mkdir(clStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Open the log file
    FileLogger::OpenLog("codelite-server.log", FileLogger::Developer);
    
    // Startup
    m_manager = new csManager();
    m_manager->Startup();

    // place your initialization code here
    return true;
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    wxUnusedVar(parser);
    return true;
}
