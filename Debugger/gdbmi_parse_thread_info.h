//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gdbmi_parse_thread_info.h
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

#ifndef GDBMIPARSETHREADINFO_H
#define GDBMIPARSETHREADINFO_H

#include <wx/string.h>
#include <vector>
#include "debugger.h"

class GdbMIThreadInfo
{
public:
    wxString threadId;
    wxString extendedName;
    wxString function;
    wxString file;
    wxString line;
    wxString active;
    
public:
    GdbMIThreadInfo() = default;
    virtual ~GdbMIThreadInfo() = default;

    ThreadEntry ToThreadEntry() const {
        ThreadEntry te;
        this->threadId.ToLong(&te.dbgid);
        te.active   = (this->active == "Yes");
        te.file     = this->file;
        te.function = this->function;
        te.line     = this->line;
        return te;
    }
};

typedef std::vector<GdbMIThreadInfo> GdbMIThreadInfoVec_t;

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class GdbMIThreadInfoParser
{
    GdbMIThreadInfoVec_t m_threads;
protected:
    bool ReadBlock(wxString &input, const wxString &pair, wxString& block);
    bool ReadKeyValuePair(const wxString &input, const wxString &key, wxString &value);

public:
    GdbMIThreadInfoParser() = default;
    virtual ~GdbMIThreadInfoParser() = default;

    void Parse(const wxString &info);
    
    const GdbMIThreadInfoVec_t& GetThreads() const {
        return m_threads;
    }
};

#endif // GDBMIPARSETHREADINFO_H
