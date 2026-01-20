//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : file_logger.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "codelite_exports.h"
#include "macros.h"

#include <unordered_map>
#include <vector>
#include <wx/colour.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/thread.h>

// manipulator function
class FileLogger;
using FileLoggerFunction = FileLogger& (*)(FileLogger&);

class WXDLLIMPEXP_CL FileLogger final
{
public:
    enum LogLevel { System = -1, Error = 0, Warning = 1, Dbg = 2, Developer = 3 };

public:
    // construct a file logger entry with a given verbosity
    FileLogger(int verbosity, const char* filename = nullptr, int line_number = wxNOT_FOUND);
    ~FileLogger();

    FileLogger& SetLogEntryVerbosity(int verbosity)
    {
        m_logEntryVerbosity = verbosity;
        return *this;
    }

    int GetLogEntryVerbosity() const { return m_logEntryVerbosity; }
    static int GetGlobalLogVerbosity() { return m_globalLogVerbosity; }

    /**
     * @brief return true if log_level is lower or equal to the global log verbosity
     */
    static bool CanLog(int verbo) { return verbo <= GetGlobalLogVerbosity(); }

    /**
     * @brief give a thread-id a unique name which will be displayed in log
     */
    static void RegisterThread(wxThreadIdType id, const wxString& name);
    static void UnRegisterThread(wxThreadIdType id);

    /**
     * @brief create log entry prefix
     */
    static wxString Prefix(int verbosity);

    void AddLogLine(const wxString& msg, int verbosity);
    /**
     * @brief print array into the log file
     * @param arr
     * @param verbosity
     */
    void AddLogLine(const wxArrayString& arr, int verbosity);

    ///----------------------------------
    /// log verbosity manipulation
    ///----------------------------------
    static void SetGlobalLogVerbosity(int level);
    static void SetGlobalLogVerbosity(const wxString& verbosity);
    static int GetVerbosityAsNumber(const wxString& verbosity);
    static wxString GetVerbosityAsString(int verbosity);

    /**
     * @brief open the log file
     */
    static void OpenLog(const wxString& fullName, int verbosity);

    FileLogger& operator<<(FileLoggerFunction f)
    {
        Flush();
        return *this;
    }

    // special types printing
    FileLogger& operator<<(const std::vector<wxString>& arr)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }

        if (!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << "[";
        if (!arr.empty()) {
            for (size_t i = 0; i < arr.size(); ++i) {
                m_buffer << arr[i] << ", ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "]";
        return *this;
    }

    FileLogger& operator<<(const wxStringSet_t& S)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }
        if (!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << "{";
        if (!S.empty()) {
            for (const wxString& s : S) {
                m_buffer << s << ", ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "}";
        return *this;
    }

    FileLogger& operator<<(const wxStringMap_t& M)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }
        if (!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << "{";
        if (!M.empty()) {
            for (const auto& vt : M) {
                m_buffer << "{" << vt.first << ", " << vt.second << "}, ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "}";
        return *this;
    }

    FileLogger& operator<<(const wxArrayString& arr)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }
        std::vector<wxString> v{arr.begin(), arr.end()};
        *this << v;
        return *this;
    }

    FileLogger& operator<<(const wxRect& rect)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }

        wxString str;
        str << "(" << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height << ")";
        *this << str;
        return *this;
    }

    FileLogger& operator<<(const wxPoint& point)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }

        wxString str;
        str << "(" << point.x << ", " << point.y << ")";
        *this << str;
        return *this;
    }

    FileLogger& operator<<(const wxSize& size)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }

        wxString str;
        str << "(" << size.x << ", " << size.y << ")";
        *this << str;
        return *this;
    }

    FileLogger& operator<<(const wxColour& colour)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }

        *this << colour.GetAsString(wxC2S_HTML_SYNTAX);
        return *this;
    }

    /**
     * @brief special wxString printing
     * Without this overload operator, on some compilers, the "clDEBUG()<< wxString" might be "going" to the one
     * that handles wxFileName...
     */
    FileLogger& operator<<(const wxString& str)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }
        if (!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << str;
        return *this;
    }

    /**
     * @brief special wxFileName printing
     */
    FileLogger& operator<<(const wxFileName& fn)
    {
        if (!FileLogger::CanLog(GetLogEntryVerbosity())) {
            return *this;
        }
        if (!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << fn.GetFullPath();
        return *this;
    }

    /**
     * @brief append any type to the buffer, take log level into consideration
     */
    template <typename T>
    FileLogger& Append(const T& elem, int level)
    {
        if (level > m_globalLogVerbosity) {
            return *this;
        }
        if (!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << elem;
        return *this;
    }

    /**
     * @brief flush the logger content
     */
    void Flush();

protected:
    static wxString GetCurrentThreadName();

    static int m_globalLogVerbosity;
    static wxString m_logfile;
    int m_logEntryVerbosity;
    FILE* m_fp = nullptr;
    wxString m_buffer;
    static std::unordered_map<wxThreadIdType, wxString> m_threads;
    static wxCriticalSection m_cs;

    int m_lineNumber = wxNOT_FOUND;
    wxString m_fileName;
};

inline FileLogger& clEndl(FileLogger& d)
{
    d.Flush();
    return d;
}

inline FileLogger& endl(FileLogger& d)
{
    d.Flush();
    return d;
}

template <typename T>
FileLogger& operator<<(FileLogger& logger, const T& obj)
{
    if (!FileLogger::CanLog(logger.GetLogEntryVerbosity()))
        return logger;

    logger.Append(obj, logger.GetLogEntryVerbosity());
    return logger;
}

namespace
{
inline wxString GetLocation(const char* filename, int line)
{
    wxFileName fn{filename};
    return wxString() << "[" << fn.GetFullName() << ":" << line << "]";
}
} // namespace

#define LOCATION() GetLocation(__FILE__, __LINE__)

// New API
#define clDEBUG() FileLogger(FileLogger::Dbg) << FileLogger::Prefix(FileLogger::Dbg) << LOCATION()
#define clDEBUG1() FileLogger(FileLogger::Developer) << FileLogger::Prefix(FileLogger::Developer) << LOCATION()
#define clTRACE() clDEBUG1()

#define clERROR() FileLogger(FileLogger::Error) << FileLogger::Prefix(FileLogger::Error) << LOCATION()
#define clWARNING() FileLogger(FileLogger::Warning) << FileLogger::Prefix(FileLogger::Warning) << LOCATION()
#define clSYSTEM() FileLogger(FileLogger::System) << FileLogger::Prefix(FileLogger::System) << LOCATION()

#define LOG_IF_DEBUG if (FileLogger::CanLog(FileLogger::Dbg))
#define LOG_IF_TRACE if (FileLogger::CanLog(FileLogger::Developer))
#define LOG_IF_WARN if (FileLogger::CanLog(FileLogger::Developer))

// A replacement for wxLogMessage
#define clLogMessage(msg) clDEBUG() << msg

#endif // FILELOGGER_H
