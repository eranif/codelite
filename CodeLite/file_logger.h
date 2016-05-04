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

#include <wx/ffile.h>
#include "codelite_exports.h"
#include <wx/stopwatch.h>

// manipulator function
class FileLogger;
typedef FileLogger& (*FileLoggerFunction)(FileLogger&);

class WXDLLIMPEXP_CL FileLogger
{
public:
    enum { System = -1, Error = 0, Warning = 1, Dbg = 2, Developer = 3 };

protected:
    int m_verbosity;
    int _requestedLogLevel;
    FILE* m_fp;
    wxString m_buffer;

public:
    FileLogger();
    ~FileLogger();

    FileLogger& SetRequestedLogLevel(int level)
    {
        _requestedLogLevel = level;
        return *this;
    }

    int GetRequestedLogLevel() const { return _requestedLogLevel; }

    /**
     * @brief create log entry prefix
     */
    wxString Prefix(int verbosity) const;

    /**
     * @brief open the log file
     */
    static void OpenLog(const wxString& fullName, int verbosity);

    static FileLogger& Get();

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

    // Various util methods
    static wxString GetVerbosityAsString(int verbosity);
    static int GetVerbosityAsNumber(const wxString& verbosity);

    inline FileLogger& operator<<(FileLoggerFunction f)
    {
        Flush();
        return *this;
    }

    /**
     * @brief special wxArrayString printing
     */
    inline FileLogger& operator<<(const wxArrayString& arr)
    {
        if(GetRequestedLogLevel() > m_verbosity) {
            return *this;
        }
        if(!m_buffer.IsEmpty()) {
            m_buffer << " ";
        }
        m_buffer << "[";
        if(!arr.IsEmpty()) {
            for(size_t i = 0; i < arr.size(); ++i) {
                m_buffer << arr.Item(i) << ", ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "]";
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

template <typename T> FileLogger& operator<<(FileLogger& logger, const T& obj)
{
    logger.Append(obj, logger.GetRequestedLogLevel());
    return logger;
}

#define CL_SYSTEM(...) FileLogger::Get().AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::System);
#define CL_ERROR(...) FileLogger::Get().AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Error);
#define CL_WARNING(...) FileLogger::Get().AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Warning);
#define CL_DEBUG(...) FileLogger::Get().AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Dbg);
#define CL_DEBUGS(s) FileLogger::Get().AddLogLine(s, FileLogger::Dbg);
#define CL_DEBUG1(...) FileLogger::Get().AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Developer);
#define CL_DEBUG_ARR(arr) FileLogger::Get().AddLogLine(arr, FileLogger::Dbg);
#define CL_DEBUG1_ARR(arr) FileLogger::Get().AddLogLine(arr, FileLogger::Developer);

// New API
#define clDEBUG()                                           \
    FileLogger::Get().SetRequestedLogLevel(FileLogger::Dbg) \
        << FileLogger::Get().Prefix(FileLogger::Get().GetRequestedLogLevel())

#define clDEBUG1()                                                \
    FileLogger::Get().SetRequestedLogLevel(FileLogger::Developer) \
        << FileLogger::Get().Prefix(FileLogger::Get().GetRequestedLogLevel())

#define clERROR()                                             \
    FileLogger::Get().SetRequestedLogLevel(FileLogger::Error) \
        << FileLogger::Get().Prefix(FileLogger::Get().GetRequestedLogLevel())

#define clWARNING()                                             \
    FileLogger::Get().SetRequestedLogLevel(FileLogger::Warning) \
        << FileLogger::Get().Prefix(FileLogger::Get().GetRequestedLogLevel())

#define clSYSTEM()                                             \
    FileLogger::Get().SetRequestedLogLevel(FileLogger::System) \
        << FileLogger::Get().Prefix(FileLogger::Get().GetRequestedLogLevel())

#endif // FILELOGGER_H
