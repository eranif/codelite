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

#include <chrono>
#include <wx/crt.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/utils.h>

int FileLogger::m_globalLogVerbosity = FileLogger::Error;
wxString FileLogger::m_logfile;
std::unordered_map<wxThreadIdType, wxString> FileLogger::m_threads;
wxCriticalSection FileLogger::m_cs;

FileLogger::FileLogger(int verbosity)
    : m_logEntryVersbosity(verbosity)
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
    if(msg.IsEmpty() || !CanLog(verbosity)) {
        return;
    }

    wxString formattedMsg = Prefix(verbosity);
    formattedMsg << " " << msg;
    formattedMsg.Trim().Trim(false);
    formattedMsg << wxT("\n");
    if(!m_buffer.empty() && (m_buffer.Last() != '\n')) {
        m_buffer << wxT("\n");
    }
    m_buffer << formattedMsg;
}

void FileLogger::SetGlobalLogVerbosity(int level)
{
    if(level > FileLogger::Warning) {
        clSYSTEM() << "Log verbosity is now set to:" << FileLogger::GetVerbosityAsString(level) << clEndl;
    }
    m_globalLogVerbosity = level;
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

void FileLogger::SetGlobalLogVerbosity(const wxString& verbosity)
{
    SetGlobalLogVerbosity(GetVerbosityAsNumber(verbosity));
}

void FileLogger::OpenLog(const wxString& fullName, int verbosity)
{
    m_logfile.Clear();
    wxFileName logfile{ clStandardPaths::Get().GetUserDataDir(), fullName };
    logfile.AppendDir("logs");
    logfile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    m_logfile = logfile.GetFullPath();
    SetGlobalLogVerbosity(verbosity);
}

void FileLogger::AddLogLine(const wxArrayString& arr, int verbosity)
{
    for(size_t i = 0; i < arr.GetCount(); ++i) {
        AddLogLine(arr.Item(i), verbosity);
    }
}

void FileLogger::Flush()
{
    m_fp = nullptr;
    if(m_buffer.IsEmpty()) {
        return;
    }
    wxFFile fp(m_logfile, "a+");
    if(fp.IsOpened()) {
        fp.Write(m_buffer + wxT("\n"), wxConvUTF8);
        fp.Close();
    }
    m_buffer.Clear();
}

wxString FileLogger::Prefix(int verbosity)
{
    if(!CanLog(verbosity)) {
        return wxEmptyString;
    }

    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const int ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count() % 1000;
    const wxString msStr = wxString::Format(wxT("%03d"), ms);
    wxString prefix;
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
