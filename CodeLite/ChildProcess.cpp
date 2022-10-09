#include "ChildProcess.h"

#include "cl_exception.h"
#include "file_logger.h"
#include "fileutils.h"
#include "processreaderthread.h"

#include <asyncprocess.h>

#if !USE_IPROCESS
#include "UnixProcess.h"
#endif

ChildProcess::ChildProcess() {}

ChildProcess::~ChildProcess()
{
#if USE_IPROCESS
    if(m_process) {
        m_process->Detach();
    }
    wxDELETE(m_process);
#else
    if(m_childProcess) {
        m_childProcess->Detach();
    }
    wxDELETE(m_childProcess);
#endif
}

namespace
{
wxString& wrap_with_quotes(wxString& str)
{
    if(!str.empty() && str.Contains(" ") && !str.StartsWith("\"") && !str.EndsWith("\"")) {
        str.Prepend("\"").Append("\"");
    }
    return str;
}
} // namespace

void ChildProcess::Start(const wxArrayString& args)
{
    if(args.IsEmpty()) {
        return;
    }
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
    if(!m_process) {
        throw clException(wxString() << "Failed to execute process: " << command);
    };
#else
    m_childProcess = new UnixProcess(this, args);
#endif
}

void ChildProcess::Write(const wxString& message) { Write(FileUtils::ToStdString(message)); }

void ChildProcess::Write(const std::string& message)
{
    if(!IsOk()) {
        return;
    }
#if USE_IPROCESS
    m_process->WriteRaw(message);
#else
    m_childProcess->Write(message);
#endif
}

bool ChildProcess::IsOk() const
{
#if USE_IPROCESS
    return m_process != nullptr;
#else
    return m_childProcess != nullptr;
#endif
}
