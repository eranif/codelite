#include "main_app.h"
#include <SocketAPI/clSocketServer.h>

IMPLEMENT_APP_CONSOLE(MainApp)

MainApp::MainApp() {}

MainApp::~MainApp() {}

int MainApp::OnExit() { return TRUE; }

bool MainApp::OnInit()
{
    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if(!DoParseCommandLine(parser)) {
        return false;
    }
    clSocketServer server;
    server.Start("tcp://127.0.0.1:5050");
    server.WaitForNewConnection(5);
    
    // place your initialization code here
    return true;
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    wxUnusedVar(parser);
    return true;
}
