#include "main_app.h"
#include "file_logger.h"
#include <cl_standard_paths.h>
#include <wx/filename.h>
#include "ChildProcess.h"
#include "LSP/Request.h"
#include "LSP/InitializeRequest.h"
#include "LSP/DidOpenTextDocumentRequest.h"
#include <processreaderthread.h>
#include <wx/filename.h>
#include <fileutils.h>

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
    m_child->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &MainApp::OnProcessOutput, this);
    m_child->Bind(wxEVT_ASYNC_PROCESS_STDERR, &MainApp::OnProcessStderr, this);
    m_child->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &MainApp::OnProcessTerminated, this);
    m_child->Start(argv);

    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if(!DoParseCommandLine(parser)) { return false; }

    LSP::InitializeRequest::Ptr_t r =
        LSP::MessageWithParams::MakeRequest(new LSP::InitializeRequest("/home/eran/devl/codelite"));

    // place your initialization code here
    m_child->Write(r->ToString());

    wxFileName fn("/home/eran/devl/codelite/LiteEditor/frame.cpp");
    wxString fileContent;
    FileUtils::ReadFileContent(fn, fileContent);
    LSP::DidOpenTextDocumentRequest::Ptr_t openReq =
        LSP::MessageWithParams::MakeRequest(new LSP::DidOpenTextDocumentRequest(fn, fileContent, "cpp"));
    m_child->Write(openReq->ToString());
    return true;
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    wxUnusedVar(parser);
    return true;
}

void MainApp::OnProcessOutput(clProcessEvent& event) { clDEBUG() << "STDOUT:" << event.GetOutput(); }

void MainApp::OnProcessStderr(clProcessEvent& event) { clDEBUG() << "STDERR:" << event.GetOutput(); }

void MainApp::OnProcessTerminated(clProcessEvent& event)
{
    clDEBUG() << "Process terminated";
    wxDELETE(m_child);
    wxExit();
}
