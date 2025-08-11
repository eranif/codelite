#include "ChildProcess.h"

#include "AsyncProcess/asyncprocess.h"
#include "StringUtils.h"

#if !USE_IPROCESS
#include "UnixProcess.h"
#endif

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

void ChildProcess::Start(const wxArrayString& args)
{
    if(args.IsEmpty()) { return; }
#if USE_IPROCESS
    // Build command line from the array
    wxString command;
    command << args[0];

    StringUtils::WrapWithQuotes(command);
    for(size_t i = 1; i < args.size(); ++i) {
        wxString argument = args[i];
        StringUtils::WrapWithQuotes(argument);
        command << " " << argument;
    }

    // Launch the process
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessStderrEvent);
#else
    m_childProcess = new UnixProcess(this, args);
#endif
}

void ChildProcess::Write(const wxString& message) { Write(StringUtils::ToStdString(message)); }

void ChildProcess::Write(const std::string& message)
{
#if USE_IPROCESS
    m_process->WriteRaw(message);
#else
    m_childProcess->Write(StringUtils::ToStdString(message));
#endif
}
