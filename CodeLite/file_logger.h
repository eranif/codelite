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
    enum LOG_LEVEL { System = -1, Error = 0, Warning = 1, Dbg = 2, Developer = 3 };

protected:
    int _requestedLogLevel;
    FILE* m_fp = nullptr;
    wxString m_buffer;
    wxString m_thread_name;
    wxString m_thread_logfile;

protected:
    const wxString& GetCurrentThreadName();

public:
    FileLogger(int requestedVerbo);
    ~FileLogger();

    /**
     * @brief return the global logger object
     * @return
     */
    static FileLogger& Get();

    FileLogger& SetRequestedLogLevel(int level)
    {
        _requestedLogLevel = level;
        return *this;
    }

    int GetRequestedLogLevel() const { return _requestedLogLevel; }
    int GetCurrentLogLevel() const;

    /**
     * @brief return true if log_level is lower than the currently set log level
     */
    bool CanLog(int log_level) const { return log_level <= GetCurrentLogLevel(); }

    /**
     * @brief give a name to the current thread
     */
    void SetThreadName(const wxString& name);

    /**
     * @brief create log entry prefix
     */
    wxString Prefix(int verbosity);

    void AddLogLine(const wxString& msg, int verbosity);
    /**
     * @brief print array into the log file
     * @param arr
     * @param verbosity
     */
    void AddLogLine(const wxArrayString& arr, int verbosity);
    void SetVerbosity(int level);

    // Set the verbosity as string
    void SetVerbosity(const wxString& verbosity);

    ///----------------------------------
    /// Statics
    ///----------------------------------

    /**
     * @brief sets the log file path and verbosity. when called from the main thread
     * this affects all threads. To provide custom thread path, call OpenLog again
     * from that thread.
     * @param fullpath path to the log file
     * @param verbosity when called from non main thread, this param is ignored
     */
    void OpenLog(const wxString& fullpath, int verbosity = FileLogger::Error);

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
        if(GetRequestedLogLevel() > GetCurrentLogLevel()) {
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
        if(GetRequestedLogLevel() > GetCurrentLogLevel()) {
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
        if(GetRequestedLogLevel() > GetCurrentLogLevel()) {
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
        if(GetRequestedLogLevel() > GetCurrentLogLevel()) {
            return *this;
        }
        std::vector<wxString> v{ arr.begin(), arr.end() };
        *this << v;
        return *this;
    }

    inline FileLogger& operator<<(const wxColour& colour)
    {
        if(GetRequestedLogLevel() > GetCurrentLogLevel()) {
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
        if(GetRequestedLogLevel() > GetCurrentLogLevel()) {
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
        if(GetRequestedLogLevel() > GetCurrentLogLevel()) {
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
        if(level > GetCurrentLogLevel()) {
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

WXDLLIMPEXP_CL FileLogger& set_log_level(FileLogger& LOG, FileLogger::LOG_LEVEL level);

// Log to custom LOG instance
#define LOG_DEBUG(LOG) set_log_level(LOG, FileLogger::Dbg) << LOG.Prefix(FileLogger::Dbg)
#define LOG_DEBUG1(LOG) set_log_level(LOG, FileLogger::Developer) << LOG.Prefix(FileLogger::Developer)
#define LOG_ERROR(LOG) set_log_level(LOG, FileLogger::Error) << LOG.Prefix(FileLogger::Error)
#define LOG_WARNING(LOG) set_log_level(LOG, FileLogger::Warning) << LOG.Prefix(FileLogger::Warning)
#define LOG_SYSTEM(LOG) set_log_level(LOG, FileLogger::System) << LOG.Prefix(FileLogger::System)

#define clTheLog FileLogger::Get()

// Log to the global logger
#define clDEBUG() LOG_DEBUG(clTheLog)
#define clDEBUG1() LOG_DEBUG1(clTheLog)
#define clERROR() LOG_ERROR(clTheLog)
#define clWARNING() LOG_WARNING(clTheLog)
#define clSYSTEM() LOG_SYSTEM(clTheLog)

// A replacement for wxLogMessage
#define clLogMessage(msg) clDEBUG() << msg

#endif // FILELOGGER_H
