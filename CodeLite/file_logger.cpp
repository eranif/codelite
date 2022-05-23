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

int FileLogger::m_verbosity = FileLogger::Error;
wxString FileLogger::m_logfile;
std::unordered_map<wxThreadIdType, wxString> FileLogger::m_threads;
wxCriticalSection FileLogger::m_cs;

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

void FileLogger::AddLogLine(const wxString& msg, int verbosity)
{
    if(msg.IsEmpty()) {
        return;
    }
    if((m_verbosity >= verbosity)) {
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
    m_verbosity = level;
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
    m_logfile.Clear();
    m_logfile << clStandardPaths::Get().GetUserDataDir() << wxFileName::GetPathSeparator() << fullName;
    m_verbosity = verbosity;
}

void FileLogger::AddLogLine(const wxArrayString& arr, int verbosity)
{
    for(size_t i = 0; i < arr.GetCount(); ++i) {
        AddLogLine(arr.Item(i), verbosity);
    }
}

void FileLogger::Flush()
{
    if(m_buffer.IsEmpty()) {
        return;
    }
    if(!m_fp) {
        m_fp = wxFopen(m_logfile, wxT("a+"));
    }

    if(m_fp) {
        wxFprintf(m_fp, "%s\n", m_buffer);
        fclose(m_fp);
        m_fp = nullptr;
    }
    m_buffer.Clear();
}

wxString FileLogger::Prefix(int verbosity)
{
    if(verbosity <= m_verbosity) {
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

wxString FileLogger::GetCurrentThreadName()
{
    if(wxThread::IsMain()) {
        return "Main";
    }
    wxCriticalSectionLocker locker(m_cs);
    std::unordered_map<wxThreadIdType, wxString>::iterator iter = m_threads.find(wxThread::GetCurrentId());
    if(iter != m_threads.end()) {
        return iter->second;
    }
    return "";
}

void FileLogger::RegisterThread(wxThreadIdType id, const wxString& name)
{
    wxCriticalSectionLocker locker(m_cs);
    std::unordered_map<wxThreadIdType, wxString>::iterator iter = m_threads.find(id);
    if(iter != m_threads.end()) {
        m_threads.erase(iter);
    }
    m_threads[id] = name;
}

void FileLogger::UnRegisterThread(wxThreadIdType id)
{
    wxCriticalSectionLocker locker(m_cs);
    std::unordered_map<wxThreadIdType, wxString>::iterator iter = m_threads.find(id);
    if(iter != m_threads.end()) {
        m_threads.erase(iter);
    }
}
