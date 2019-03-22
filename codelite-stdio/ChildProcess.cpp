#include "ChildProcess.h"
#include <asyncprocess.h>
#include "file_logger.h"
#include "fileutils.h"
#include "processreaderthread.h"
#include <globals.h>

ChildProcess::ChildProcess()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ChildProcess::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ChildProcess::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &ChildProcess::OnProcessStderr, this);
}

ChildProcess::~ChildProcess()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &ChildProcess::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &ChildProcess::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_STDERR, &ChildProcess::OnProcessStderr, this);
}

void ChildProcess::Start(const wxArrayString& args)
{
    if(args.IsEmpty()) { return; }

    // Build command line from the array
    wxString command;
    command << args[0];

    ::WrapWithQuotes(command);
    for(size_t i = 1; i < args.size(); ++i) {
        wxString argument = args[i];
        ::WrapWithQuotes(argument);
        command << " " << argument;
    }
    
    // Launch the process
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessStderrEvent);
    if(m_process) {
        wxString content;
        FileUtils::ReadFileContent(wxFileName("C:/src/wxCustomControls/init.msg"), content);
        m_process->Write(content);
    }
}

void ChildProcess::OnProcessTerminated(clProcessEvent& event) { clDEBUG() << "Process terminated"; }

void ChildProcess::OnProcessOutput(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }

void ChildProcess::OnProcessStderr(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }
