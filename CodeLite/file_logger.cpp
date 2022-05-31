//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : file_logger.cpp
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

#include "file_logger.h"

#include "cl_standard_paths.h"

#include <sys/time.h>
#include <wx/crt.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

// the logbal log object
thread_local FileLogger LOG{ FileLogger::Error };

// critical section to sync writes to the file
static wxCriticalSection m_fileCS;

// We maintain 2 global variables that holds the default log file file + log verbosity
// they are both updated when the *main* thread calls for OpenLog(..)
// and they are used incase a thread calls clDEBUG() (or any debug macro)
// without calling first to FileLogger::Get().OpenLog(..)
static wxString global_log_file;
static int global_log_level = FileLogger::Error;

FileLogger& set_log_level(FileLogger& _LOG, FileLogger::LOG_LEVEL level)
{
    _LOG.SetRequestedLogLevel(level);
    return _LOG;
}

FileLogger::FileLogger(int requestedVerbo)
    : _requestedLogLevel(requestedVerbo)
    , m_fp(nullptr)
{
}

FileLogger::~FileLogger()
{
    // flush any content that remain
    Flush();
}

FileLogger& FileLogger::Get() { return LOG; }

void FileLogger::AddLogLine(const wxString& msg, int verbosity)
{
    if(msg.empty()) {
        return;
    }

    if(verbosity < GetRequestedLogLevel()) {
        wxString formattedMsg = Prefix(verbosity);
        formattedMsg << " " << msg;
        formattedMsg.Trim().Trim(false);
        formattedMsg << wxT("\n");
        if(!m_buffer.empty() && (m_buffer.Last() != '\n')) {
            m_buffer << "\n";
        }
        m_buffer << formattedMsg;
    }
}

void FileLogger::SetVerbosity(int level)
{
    if(level > FileLogger::Warning) {
        clSYSTEM() << "Log verbosity is now set to:" << FileLogger::GetVerbosityAsString(level) << clEndl;
    }

    wxCriticalSectionLocker locker{ m_fileCS };
    global_log_level = level;
}

int FileLogger::GetVerbosityAsNumber(const wxString& verbosity)
{
    if(verbosity == wxT("Debug") || verbosity == "DBG") {
        return FileLogger::Dbg;

    } else if(verbosity == wxT("Error") || verbosity == "ERR") {
        return FileLogger::Error;

    } else if(verbosity == wxT("Warning") || verbosity == "WARN") {
        return FileLogger::Warning;

    } else if(verbosity == wxT("System") || verbosity == "INFO" || verbosity == "SYS") {
        return FileLogger::System;

    } else if(verbosity == wxT("Developer") || verbosity == "TRACE") {
        return FileLogger::Developer;

    } else {
        return FileLogger::Error;
    }
}

wxString FileLogger::GetVerbosityAsString(int verbosity)
{
    switch(verbosity) {
    case FileLogger::Dbg:
        return wxT("Debug");

    case FileLogger::Error:
        return wxT("Error");

    case FileLogger::Warning:
        return wxT("Warning");

    case FileLogger::Developer:
        return wxT("Developer");

    case FileLogger::System:
        return wxT("System");

    default:
        return wxT("Error");
    }
}

void FileLogger::SetVerbosity(const wxString& verbosity) { SetVerbosity(GetVerbosityAsNumber(verbosity)); }

void FileLogger::OpenLog(const wxString& fullName, int verbosity)
{
    wxCriticalSectionLocker locker(m_fileCS);
    m_thread_logfile << clStandardPaths::Get().GetUserDataDir() << wxFileName::GetPathSeparator() << fullName;

    if(wxThread::IsMain()) {
        global_log_level = verbosity;
        global_log_file = m_thread_logfile;
    }
}

void FileLogger::AddLogLine(const wxArrayString& arr, int verbosity)
{
    for(size_t i = 0; i < arr.GetCount(); ++i) {
        AddLogLine(arr.Item(i), verbosity);
    }
}

void FileLogger::Flush()
{
    if(m_buffer.empty()) {
        return;
    }

    {
        wxCriticalSectionLocker locker(m_fileCS);
        if(m_thread_logfile.empty()) {
            // user did not call OpenLog for this thread
            m_thread_logfile = global_log_file;
        }

        // still empty?
        if(m_thread_logfile.empty()) {
            m_buffer.clear();
            return;
        }

        // we always open the file and then closing it
        // this is because on Windows, we cant really keep the file open in write open
        // an alternative is to create a writer thread that will handle all the writes
        // and other threads will just pass the buffer to it for writing
        if(!m_fp) {
            m_fp = wxFopen(m_thread_logfile, "a+");
        }

        if(m_fp) {
            wxFprintf(m_fp, "%s\n", m_buffer);
            fflush(m_fp);
            fclose(m_fp);
            m_fp = nullptr;
        }
    }
    m_buffer.Clear();
}

wxString FileLogger::Prefix(int verbosity)
{
    if(verbosity <= GetCurrentLogLevel()) {
        wxString prefix;
        timeval tim;
        gettimeofday(&tim, NULL);
        int ms = (int)tim.tv_usec / 1000.0;

        wxString msStr = wxString::Format(wxT("%03d"), ms);
        prefix << wxT("[") << wxDateTime::Now().FormatISOTime() << wxT(":") << msStr;
        switch(verbosity) {
        case System:
            prefix << wxT(" SYS]");
            break;

        case Error:
            prefix << wxT(" ERR]");
            break;

        case Warning:
            prefix << wxT(" WRN]");
            break;

        case Dbg:
            prefix << wxT(" DBG]");
            break;

        case Developer:
            prefix << wxT(" DVL]");
            break;
        }

        wxString thread_name = GetCurrentThreadName();
        if(!thread_name.IsEmpty()) {
            prefix << " [" << thread_name << "]";
        }
        return prefix;
    } else {
        return wxEmptyString;
    }
}

const wxString& FileLogger::GetCurrentThreadName()
{
    if(!m_thread_name.empty()) {
        return m_thread_name;
    }

    // auto assign name to the thread
    if(wxThread::IsMain()) {
        m_thread_name = "Main Thread";
        return m_thread_name;
    } else {
        m_thread_name << "Thread-" << wxThread::GetCurrentId();
    }
    return m_thread_name;
}

void FileLogger::SetThreadName(const wxString& name) { m_thread_name = name; }

int FileLogger::GetCurrentLogLevel() const { return global_log_level; }
