#include "clModuleLogger.hpp"

#include "fileutils.h"

#include <sys/time.h>
#include <wx/crt.h>
#include <wx/datetime.h>

clModuleLogger::clModuleLogger() {}

clModuleLogger::~clModuleLogger() { Flush(); }

bool clModuleLogger::CanLog() const { return FileLogger::CanLog(m_current_log_level); }

void clModuleLogger::Flush()
{
    if(m_buffer.empty()) {
        return;
    }

    if(m_buffer.Last() != '\n') {
        m_buffer.Append('\n');
    }
    FileUtils::AppendFileContent(m_logfile, m_buffer, wxConvUTF8);
    m_buffer.clear();
}

void clModuleLogger::Open(const wxFileName& filepath) { m_logfile = filepath; }

clModuleLogger& clModuleLogger::SetCurrentLogLevel(int level)
{
    if(!m_buffer.empty()) {
        // flush the current content
        Flush();
    }
    m_current_log_level = level;
    return *this;
}

wxString clModuleLogger::Prefix()
{
    if(!CanLog()) {
        return wxEmptyString;
    }

    wxString prefix;
    timeval tim;
    gettimeofday(&tim, NULL);
    int ms = (int)tim.tv_usec / 1000.0;

    wxString msStr = wxString::Format(wxT("%03d"), ms);
    prefix << wxT("[") << wxDateTime::Now().FormatISOTime() << wxT(":") << msStr;
    // add the thread ID
    prefix << wxT(" T:") << wxThread::GetCurrentId();

    switch(m_current_log_level) {
    case FileLogger::System:
        prefix << wxT(" SYSTEM]");
        break;
    case FileLogger::Error:
        prefix << wxT(" ERROR]");
        break;
    case FileLogger::Warning:
        prefix << wxT(" WARNING]");
        break;
    case FileLogger::Dbg:
        prefix << wxT(" DEBUG]");
        break;
    case FileLogger::Developer:
        prefix << wxT(" TRACE]");
        break;
    }
    return prefix;
}
