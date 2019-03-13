#include "ChildProcess.h"
#include <signal.h>
#include <string.h>
#include "fileutils.h"
#include "cl_exception.h"
#include <cstring>
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "file_logger.h"

static wxString wrap_spaces(const wxString& str)
{
    wxString s = str;
    if(s.Contains(" ")) { s.Prepend("\"").Append("\""); }
    return s;
}

wxDEFINE_EVENT(wxEVT_CHILD_PROCESS_STDOUT, clProcessEvent);
wxDEFINE_EVENT(wxEVT_CHILD_PROCESS_STDERR, clProcessEvent);
wxDEFINE_EVENT(wxEVT_CHILD_PROCESS_EXIT, clProcessEvent);

ChildProcess::ChildProcess()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ChildProcess::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &ChildProcess::OnProcessStderr, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ChildProcess::OnProcessTerminated, this);
}

ChildProcess::~ChildProcess()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &ChildProcess::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_STDERR, &ChildProcess::OnProcessStderr, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &ChildProcess::OnProcessTerminated, this);
    Stop();
}

void ChildProcess::Start(const wxArrayString& argv, const wxString& workingDirectory)
{
    m_process = ::CreateAsyncProcess(this, BuildCommand(argv), IProcessCreateDefault, workingDirectory);
    if(!m_process) { clERROR() << "CreateAsyncProcess() error." << BuildCommand(argv); }
}
int ChildProcess::Wait() { return 0; }

void ChildProcess::Stop() { Cleanup(); }

void ChildProcess::Write(const wxString& message)
{
    std::string cmessage = FileUtils::ToStdString(message);
    Write(cmessage);
}

void ChildProcess::Write(const std::string& message)
{
    if(m_process) { m_process->Write(message); }
}

void ChildProcess::OnProcessOutput(clProcessEvent& event)
{
    clProcessEvent evtStdout(wxEVT_CHILD_PROCESS_STDOUT);
    evtStdout.SetOutput(event.GetOutput());
    AddPendingEvent(evtStdout);
}

void ChildProcess::OnProcessStderr(clProcessEvent& event)
{
    clProcessEvent evtStdout(wxEVT_CHILD_PROCESS_STDERR);
    evtStdout.SetOutput(event.GetOutput());
    AddPendingEvent(evtStdout);
}

void ChildProcess::OnProcessTerminated(clProcessEvent& event)
{
    clProcessEvent exitEvent(wxEVT_CHILD_PROCESS_EXIT);
    AddPendingEvent(exitEvent);
}

bool ChildProcess::IsRunning() const { return m_process != nullptr; }

wxString ChildProcess::BuildCommand(const wxArrayString& argv)
{
    wxString command;
    for(const wxString& arg : argv) {
        wxString c = arg;
        if(c.Contains(" ")) { c = wrap_spaces(c); }
        if(!command.IsEmpty()) { command << " "; }
        command << c;
    }
    return command;
}

void ChildProcess::Cleanup() { wxDELETE(m_process); }
