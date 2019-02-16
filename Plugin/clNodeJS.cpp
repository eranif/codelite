#include "clNodeJS.h"
#include "NodeJSLocator.h"
#include "globals.h"
#include "file_logger.h"
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "clConsoleBase.h"
#include <algorithm>
#include "ieditor.h"
#include "globals.h"
#include "imanager.h"

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

void clNodeJS::OnProcessOutput(clProcessEvent& event)
{
    IProcess* process = event.GetProcess();
    if(m_processes.count(process)) {
        LintInfo& d = m_processes[process];
        d.m_output << event.GetOutput();
    }
}

void clNodeJS::OnProcessTerminated(clProcessEvent& event)
{
    IProcess* process = event.GetProcess();
    if(m_processes.count(process)) {
        const LintInfo& d = m_processes[process];
        if(!d.m_output.IsEmpty()) { ProcessLintOuput(d.filename, d.m_output); }
        m_processes.erase(process);
    }
    wxDELETE(process);
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

    std::for_each(m_processes.begin(), m_processes.end(),
                  [&](const std::unordered_map<IProcess*, LintInfo>::value_type& vt) {
                      IProcess* p = vt.first;
                      p->Terminate();
                      wxDELETE(p);
                  });
    m_processes.clear();
}

void clNodeJS::BindEvents()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clNodeJS::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clNodeJS::OnProcessTerminated, this);
}

void clNodeJS::UnBindEvents()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clNodeJS::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clNodeJS::OnProcessTerminated, this);
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

void clNodeJS::LintFile(const wxFileName& filename)
{
    if(!IsInitialised()) { return; }
    wxString wd = filename.GetPath();

    wxString command;
    command << GetNode().GetFullPath();
    ::WrapWithQuotes(command);

    command << " -c " << filename.GetFullName();
    IProcess* process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, wd);
    LintInfo d;
    d.filename = filename;
    if(process) { m_processes.insert({ process, d }); }
}

void clNodeJS::ProcessLintOuput(const wxFileName& fn, const wxString& output)
{
    // the first line in the output is "file:line\n"
    wxString where = output.BeforeFirst('\n');
    wxString line = where.AfterLast(':');
    wxString file = where.BeforeLast(':');
    wxString errorMessage = output.AfterFirst('\n');
    
    line.Trim().Trim(false);
    file.Trim().Trim(false);
    
    // Use the code font for this error message
    errorMessage.Prepend("<code>").Append("</code>");
    
    long nLineNumber = -1;
    line.ToCLong(&nLineNumber);
    
    // Mark the editor with error marker
    IEditor* editor = clGetManager()->FindEditor(fn.GetFullPath());
    if(editor && (nLineNumber != wxNOT_FOUND)) { editor->SetErrorMarker(nLineNumber - 1, errorMessage); }
}
