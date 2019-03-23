#include "ChildProcess.h"
#include <asyncprocess.h>
#include "file_logger.h"
#include "fileutils.h"
#include "processreaderthread.h"

#if !USE_IPROCESS
#include "UnixProcess.h"
#endif

ChildProcess::ChildProcess() {}

ChildProcess::~ChildProcess()
{
#if USE_IPROCESS
    m_process->Detach();
    wxDELETE(m_process);
#else
    m_childProcess->Detach();
    wxDELETE(m_childProcess);
#endif
}

static wxString& wrap_with_quotes(wxString& str)
{
    if(str.Contains(" ")) { str.Prepend("\"").Append("\""); }
    return str;
}

void ChildProcess::Start(const wxArrayString& args)
{
    if(args.IsEmpty()) { return; }
#if USE_IPROCESS
    // Build command line from the array
    wxString command;
    command << args[0];

    wrap_with_quotes(command);
    for(size_t i = 1; i < args.size(); ++i) {
        wxString argument = args[i];
        wrap_with_quotes(argument);
        command << " " << argument;
    }

    // Launch the process
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessStderrEvent);
#else
    m_childProcess = new UnixProcess(this, args);
#endif
}

void ChildProcess::Write(const wxString& message) { Write(FileUtils::ToStdString(message)); }

void ChildProcess::Write(const std::string& message)
{
#if USE_IPROCESS
    m_process->WriteRaw(message);
#else
    m_childProcess->Write(FileUtils::ToStdString(message));
#endif
}
