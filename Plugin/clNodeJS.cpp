#include "clNodeJS.h"
#include "NodeJSLocator.h"
#include "globals.h"
#include "file_logger.h"
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "clConsoleBase.h"

clNodeJS::clNodeJS() {}

clNodeJS::~clNodeJS() {}

bool clNodeJS::Initialise(const wxArrayString& hints)
{
    if(!IsInitialised()) {
        // bind the events once
        BindEvents();
    }

    NodeJSLocator locator;
    locator.Locate(hints);
    m_node = locator.GetNodejs();
    m_npm = locator.GetNpm();
    m_initialised = m_node.FileExists() && m_npm.FileExists();
    return m_initialised;
}

bool clNodeJS::NpmInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                          wxEvtHandler* sink)
{
    if(!IsInitialised()) { return false; }

    // Create the working directory if not exists
    wxFileName wd(workingDirectory, "");
    wd.Mkdir(wxS_DEFAULT, wxPATH_MKDIR_FULL);

    wxString _args;
    _args.Prepend("install ");
    _args << package;
    if(!args.IsEmpty()) {
        _args << " " << args; // --save
    }

    clConsoleBase::Ptr_t console = clConsoleBase::GetTerminal();
    console->SetWorkingDirectory(wd.GetPath());
    console->SetCommand(GetNpm().GetFullPath(), _args);
    console->SetWaitWhenDone(true);
    console->SetTerminalNeeded(true);
    console->SetSink(sink);
    return console->Start();
}

void clNodeJS::OnProcessOutput(clProcessEvent& event) { wxUnusedVar(event); }

void clNodeJS::OnProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    wxDELETE(m_process);
}

clNodeJS& clNodeJS::Get()
{
    static clNodeJS nodejs;
    return nodejs;
}

void clNodeJS::Shutdown()
{
    m_initialised = false;
    m_node.Clear();
    m_npm.Clear();

    // Unbind before we kill the process
    UnBindEvents();

    m_process->Terminate();
    wxDELETE(m_process);
}

void clNodeJS::BindEvents()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clNodeJS::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clNodeJS::OnProcessOutput, this);
}

void clNodeJS::UnBindEvents()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clNodeJS::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clNodeJS::OnProcessOutput, this);
}

bool clNodeJS::NpmInit(const wxString& workingDirectory, wxEvtHandler* sink)
{
    if(!IsInitialised()) { return false; }

    // Create the working directory if not exists
    wxFileName wd(workingDirectory, "");
    wd.Mkdir(wxS_DEFAULT, wxPATH_MKDIR_FULL);

    clConsoleBase::Ptr_t console = clConsoleBase::GetTerminal();
    console->SetWorkingDirectory(wd.GetPath());
    console->SetCommand(GetNpm().GetFullPath(), "init");
    console->SetWaitWhenDone(true);
    console->SetTerminalNeeded(true);
    console->SetSink(sink);
    return console->Start();
}
