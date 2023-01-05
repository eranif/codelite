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

#include <vector>
#include <wx/colour.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/stopwatch.h>
#include <wx/thread.h>
#include <wxStringHash.h>

// manipulator function
class FileLogger;
typedef FileLogger& (*FileLoggerFunction)(FileLogger&);

class WXDLLIMPEXP_CL FileLogger
{
public:
    enum { System = -1, Error = 0, Warning = 1, Dbg = 2, Developer = 3 };

protected:
    static int m_verbosity;
    static wxString m_logfile;
    int _requestedLogLevel;
    FILE* m_fp;
    wxString m_buffer;
    static std::unordered_map<wxThreadIdType, wxString> m_threads;
    static wxCriticalSection m_cs;

protected:
    static wxString GetCurrentThreadName();

public:
    FileLogger(int requestedVerbo);
    ~FileLogger();

    FileLogger& SetRequestedLogLevel(int level)
    {
        _requestedLogLevel = level;
        return *this;
    }

    int GetRequestedLogLevel() const { return _requestedLogLevel; }
    static int GetCurrentLogLevel() { return m_verbosity; }

    /**
     * @brief return true if log_level is lower than the currently set log level
     */
    static bool CanLog(int log_level) { return log_level <= GetCurrentLogLevel(); }

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
    static void SetVerbosity(int level);

    // Set the verbosity as string
    static void SetVerbosity(const wxString& verbosity);

    ///----------------------------------
    /// Statics
    ///----------------------------------
    /**
     * @brief open the log file
     */
    static void OpenLog(const wxString& fullName, int verbosity);
    // Various util methods
    static wxString GetVerbosityAsString(int verbosity);
    static int GetVerbosityAsNumber(const wxString& verbosity);

    inline FileLogger& operator<<(FileLoggerFunction f)
    {
        Flush();
        return *this;
    }

    // special types printing
    inline FileLogger& operator<<(const std::vector<wxString>& arr)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
            return *this;
        }
        if(!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << "[";
        if(!arr.empty()) {
            for(size_t i = 0; i < arr.size(); ++i) {
                m_buffer << arr[i] << ", ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "]";
        return *this;
    }

    inline FileLogger& operator<<(const wxStringSet_t& S)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
            return *this;
        }
        if(!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << "{";
        if(!S.empty()) {
            for(const wxString& s : S) {
                m_buffer << s << ", ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "}";
        return *this;
    }

    inline FileLogger& operator<<(const wxStringMap_t& M)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
            return *this;
        }
        if(!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << "{";
        if(!M.empty()) {
            for(const auto& vt : M) {
                m_buffer << "{" << vt.first << ", " << vt.second << "}, ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "}";
        return *this;
    }

    inline FileLogger& operator<<(const wxArrayString& arr)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
            return *this;
        }
        std::vector<wxString> v{ arr.begin(), arr.end() };
        *this << v;
        return *this;
    }

    inline FileLogger& operator<<(const wxColour& colour)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
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
    inline FileLogger& operator<<(const wxString& str)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
            return *this;
        }
        if(!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << str;
        return *this;
    }

    /**
     * @brief special wxFileName printing
     */
    inline FileLogger& operator<<(const wxFileName& fn)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
            return *this;
        }
        if(!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << fn.GetFullPath();
        return *this;
    }

    /**
     * @brief append any type to the buffer, take log level into consideration
     */
    template <typename T> FileLogger& Append(const T& elem, int level)
    {
        if(level > m_verbosity) {
            return *this;
        }
        if(!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << elem;
        return *this;
    }

    /**
     * @brief flush the logger content
     */
    void Flush();
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

template <typename T> FileLogger& operator<<(FileLogger& logger, const T& obj)
{
    logger.Append(obj, logger.GetRequestedLogLevel());
    return logger;
}

// New API
#define clDEBUG() FileLogger(FileLogger::Dbg) << FileLogger::Prefix(FileLogger::Dbg)
#define clDEBUG1() FileLogger(FileLogger::Developer) << FileLogger::Prefix(FileLogger::Developer)

// alias
#define clTRACE() clDEBUG1()

#define clERROR() FileLogger(FileLogger::Error) << FileLogger::Prefix(FileLogger::Error)
#define clWARNING() FileLogger(FileLogger::Warning) << FileLogger::Prefix(FileLogger::Warning)
#define clSYSTEM() FileLogger(FileLogger::System) << FileLogger::Prefix(FileLogger::System)

#define LOG_IF_DEBUG if(FileLogger::CanLog(FileLogger::Dbg))
#define LOG_IF_TRACE if(FileLogger::CanLog(FileLogger::Developer))
#define LOG_IF_WARN if(FileLogger::CanLog(FileLogger::Developer))

// A replacement for wxLogMessage
#define clLogMessage(msg) clDEBUG() << msg

#endif // FILELOGGER_H
