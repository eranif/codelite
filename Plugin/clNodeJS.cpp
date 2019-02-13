#include "clNodeJS.h"
#include "NodeJSLocator.h"
#include "globals.h"
#include "file_logger.h"
#include "asyncprocess.h"
#include "processreaderthread.h"

clNodeJS::clNodeJS() {}

clNodeJS::~clNodeJS() {}

bool clNodeJS::Initialise()
{
    if(IsInitialised()) { return true; }

    BindEvents();

    NodeJSLocator locator;
    locator.Locate();
    m_node = locator.GetNodejs();
    m_npm = locator.GetNpm();
    return m_node.FileExists() && m_npm.FileExists();
}

bool clNodeJS::NpmInstall(const wxString& package, const wxString& workingDirectory, const wxString& args)
{
    if(!IsInitialised()) { return false; }
    if(!IsRunning()) { return false; }

    wxString command;
    command << GetNpm().GetFullPath();
    ::WrapWithQuotes(command);

    command << " " << package;
    if(!args.IsEmpty()) { command << " " << args; }

    // Create the working directory if not exists
    wxFileName wd(workingDirectory, "");
    wd.Mkdir(wxS_DEFAULT, wxPATH_MKDIR_FULL);

    clDEBUG() << "Executing command:" << command;
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateConsole | IProcessNoRedirect, wd.GetPath());
    if(!m_process) { return false; }
    return true;
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
