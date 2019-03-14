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

wxDEFINE_EVENT(wxEVT_NODE_COMMAND_TERMINATED, clProcessEvent);

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
                          wxEvtHandler* sink, const wxString& uid)
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
    console->SetSink(sink, uid);
    return console->Start();
}

void clNodeJS::OnProcessOutput(clProcessEvent& event)
{
    IProcess* process = event.GetProcess();
    if(m_processes.count(process)) {
        ProcessData& d = m_processes[process];
        d.GetOutput() << event.GetOutput();
    }
}

void clNodeJS::OnProcessTerminated(clProcessEvent& event)
{
    IProcess* process = event.GetProcess();
    if(m_processes.count(process)) {
        const ProcessData& d = m_processes[process];
        // Handle lint
        if(!d.GetOutput().IsEmpty() && d.GetUid() == "lint") { ProcessLintOuput(d.GetFilename(), d.GetOutput()); }
        if(d.GetSink()) {
            clProcessEvent evt(wxEVT_NODE_COMMAND_TERMINATED);
            evt.SetOutput(d.GetOutput());
            evt.SetString(d.GetUid()); // pass the unique ID
            d.GetSink()->AddPendingEvent(evt);
        }
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
                  [&](const std::unordered_map<IProcess*, ProcessData>::value_type& vt) {
                      IProcess* p = vt.first;
                      // Terminate the process
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
    if(process) {
        ProcessData d;
        d.SetFilename(filename);
        d.SetUid("lint");
        m_processes.insert({ process, d });
    }
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

bool clNodeJS::NpmSilentInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                                wxEvtHandler* sink, const wxString& uid)
{
    if(!IsInitialised()) { return false; }
    wxString command;
    command << GetNpm().GetFullPath();
    ::WrapWithQuotes(command);

    command << " install " << package << " --silent --quiet " << args;
    IProcess* process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, workingDirectory);
    if(process) {
        ProcessData d;
        d.SetUid(uid);
        d.SetSink(sink);
        m_processes.insert({ process, d });
    }
    return (process != nullptr);
}

wxProcess* clNodeJS::RunScript(const wxArrayString& argv, const wxString& workingDirectory, size_t execFlags)
{
    if(!IsInitialised() || argv.IsEmpty()) { return nullptr; }
    wxFileName scriptPath(argv.Item(0));
    if(!scriptPath.FileExists()) {
        clERROR() << "NodeJS: cant run script:" << scriptPath << ". No such file";
        return nullptr;
    }

    wxString command;
    for(const wxString& arg : argv) {
        // Build the command line
        wxString tmp = std::move(arg);
        ::WrapWithQuotes(tmp);
        command << tmp << " ";
    }

    wxProcess* process = new wxProcess();
    process->Redirect();

    if(::wxExecute(command, execFlags, process) <= 0) {
        wxDELETE(process);
        return nullptr;
    }
    return process;
}
