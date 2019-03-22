#include "main_app.h"
#include "file_logger.h"
#include <cl_standard_paths.h>
#include <wx/filename.h>
#include "ChildProcess.h"

IMPLEMENT_APP_CONSOLE(MainApp)

MainApp::MainApp() {}

MainApp::~MainApp() { wxDELETE(m_child); }

int MainApp::OnExit() { return TRUE; }

bool MainApp::OnInit()
{
    SetAppName("codelite-stdio");

    // Open the log file
    wxFileName logfile(clStandardPaths::Get().GetUserDataDir(), "codelite-stdio.log");
    logfile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    FileLogger::OpenLog(logfile.GetFullName(), 2);

    clSYSTEM() << "Started";

    if(wxAppConsole::argc < 2) { return false; }

    wxArrayString argv = wxAppConsole::argv.GetArguments();
    argv.RemoveAt(0);

    m_child = new ChildProcess();
    m_child->Start(argv);

    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if(!DoParseCommandLine(parser)) { return false; }

    // place your initialization code here
    return true;
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    wxUnusedVar(parser);
    return true;
}
