#include "main_app.h"
#include <SocketAPI/clSocketBase.h>
#include <SocketAPI/clSocketServer.h>
#include <clJoinableThread.h>
#include <iostream>
#include <wx/filename.h>

IMPLEMENT_APP_CONSOLE(MainApp)

static const wxCmdLineEntryDesc cmdLineDesc[] = { { wxCMD_LINE_PARAM, NULL, NULL, "Input file", wxCMD_LINE_VAL_STRING,
                                                    wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
                                                  { wxCMD_LINE_NONE } };

MainApp::MainApp()
{
    Bind(wxEVT_STDIN_MESSAGE, &MainApp::OnStdin, this);
    Bind(wxEVT_STDIN_EXIT, &MainApp::OnStdinExit, this);
}

MainApp::~MainApp()
{
    Unbind(wxEVT_STDIN_MESSAGE, &MainApp::OnStdin, this);
    Unbind(wxEVT_STDIN_EXIT, &MainApp::OnStdinExit, this);
}

int MainApp::OnExit()
{
    m_process.Stop();

    // Tell the stdin thread to quit by sending it a command ".exit"
    m_stdin->Write(".exit");
    wxDELETE(m_stdin);

    return TRUE;
}

bool MainApp::OnInit()
{
    wxCmdLineParser parser;
    parser.SetDesc(cmdLineDesc);
    parser.SetCmdLine(wxAppConsole::argc, wxAppConsole::argv);
    if(!DoParseCommandLine(parser)) { return false; }

    if(!wxGetEnv("NDB_NODEJS_PATH", &m_nodejs)) {
        std::cerr << "Please set the environment variable NDB_NODEJS_PATH to point to your Nodejs executable"
                  << std::endl;
        return false;
    }
    if(!wxFileName::FileExists(m_nodejs)) {
        std::cerr << "NDB_NODEJS_PATH points to a non existing file" << std::endl;
        return false;
    }

    m_process.Debug(m_nodejs, m_scriptPath);
    m_stdin = new NodeStdinThread(this);
    m_stdin->Start();

    // place your initialization code here
    return true;
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    wxUnusedVar(parser);
    parser.Parse();
    if(parser.GetParamCount() == 0) {
        std::cerr << "No input script provided" << std::endl;
        return false;
    }
    m_scriptPath = parser.GetParam(0);
    return true;
}

void MainApp::OnStdin(clCommandEvent& event) { m_process.Write(event.GetString()); }
void MainApp::OnStdinExit(clCommandEvent& event) { ExitMainLoop(); }
