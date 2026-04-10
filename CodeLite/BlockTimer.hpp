#pragma once
#include "clModuleLogger.hpp"
#include "file_logger.h"

#include <wx/stopwatch.h>

INITIALISE_MODULE_LOG(BLOCK_TIMER_LOG, "Perf", "perf.log");

class BlockTimer final
{
public:
    BlockTimer(const wxString& label, FileLogger::LogLevel log_level = FileLogger::Developer)
        : m_label(label)
        , m_logLevel(log_level)
    {
        m_sw.Start();
    }

    ~BlockTimer()
    {
        if (FileLogger::CanLog(m_logLevel)) {
            BLOCK_TIMER_LOG().SetCurrentLogLevel(m_logLevel)
                << BLOCK_TIMER_LOG().Prefix() << "(" << m_label << ")"
                << " duration:" << m_sw.TimeInMicro() << "microseconds" << endl;
        }
    }

private:
    wxStopWatch m_sw;
    wxString m_label;
    FileLogger::LogLevel m_logLevel{FileLogger::Dbg};
};