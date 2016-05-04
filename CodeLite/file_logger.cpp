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
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <sys/time.h>
#include <wx/log.h>
#include <wx/crt.h>
#include "cl_standard_paths.h"

static FileLogger theLogger;
static bool initialized = false;

FileLogger::FileLogger()
    : m_verbosity(FileLogger::Error)
    , _requestedLogLevel(FileLogger::Developer)
    , m_fp(NULL)
{
}

FileLogger::~FileLogger()
{
    if(m_fp) {
        fclose(m_fp);
        m_fp = NULL;
    }
}

void FileLogger::AddLogLine(const wxString& msg, int verbosity)
{
    if(msg.IsEmpty()) return;
    if((m_verbosity >= verbosity) && m_fp) {
        wxString formattedMsg = Prefix(verbosity);
        formattedMsg << " " << msg;
        formattedMsg.Trim().Trim(false);
        formattedMsg << wxT("\n");
        wxFprintf(m_fp, wxT("%s"), formattedMsg.c_str());
        fflush(m_fp);
    }
}

FileLogger& FileLogger::Get() { return theLogger; }

void FileLogger::SetVerbosity(int level)
{
    if(level > FileLogger::Warning) {
        clSYSTEM() << "Log verbosity is now set to:" << FileLogger::GetVerbosityAsString(level) << clEndl;
    }
    m_verbosity = level;
}

int FileLogger::GetVerbosityAsNumber(const wxString& verbosity)
{
    if(verbosity == wxT("Debug")) {
        return FileLogger::Dbg;

    } else if(verbosity == wxT("Error")) {
        return FileLogger::Error;

    } else if(verbosity == wxT("Warning")) {
        return FileLogger::Warning;

    } else if(verbosity == wxT("System")) {
        return FileLogger::System;

    } else if(verbosity == wxT("Developer")) {
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

    default:
        return wxT("Error");
    }
}

void FileLogger::SetVerbosity(const wxString& verbosity) { SetVerbosity(FileLogger::GetVerbosityAsNumber(verbosity)); }

void FileLogger::OpenLog(const wxString& fullName, int verbosity)
{
    if(!initialized) {
        wxString filename;
        filename << clStandardPaths::Get().GetUserDataDir() << wxFileName::GetPathSeparator() << fullName;
        theLogger.m_fp = wxFopen(filename, wxT("a+"));
        theLogger.m_verbosity = verbosity;
        initialized = true;
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
    if(m_buffer.IsEmpty()) {
        return;
    }
    wxFprintf(m_fp, "%s\n", m_buffer);
    fflush(m_fp);
    m_buffer.Clear();
}

wxString FileLogger::Prefix(int verbosity) const
{
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
    return prefix;
}
