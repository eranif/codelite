#pragma once
#include "file_logger.h"

#include <wx/stopwatch.h>

class BlockTimer final
{
public:
    BlockTimer(const wxString& label, FileLogger::LogLevel log_level = FileLogger::Dbg)
        : m_label(label)
        , m_logLevel(log_level)
    {
        m_sw.Start();
    }

    ~BlockTimer()
    {
        if (FileLogger::CanLog(m_logLevel)) {
            FileLogger(m_logLevel) << FileLogger::Prefix(m_logLevel) << "(" << m_label << ")"
                                   << " duration:" << m_sw.TimeInMicro() << "microseconds" << endl;
        }
    }

private:
    wxStopWatch m_sw;
    wxString m_label;
    FileLogger::LogLevel m_logLevel{FileLogger::Dbg};
};