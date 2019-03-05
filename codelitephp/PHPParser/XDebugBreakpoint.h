//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugBreakpoint.h
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

#ifndef XDEBUGBREAKPOINT_H
#define XDEBUGBREAKPOINT_H

#include <wx/string.h>
#include <list>
#include "JSON.h"

class XDebugBreakpoint
{
    wxString m_fileName;
    int      m_line;
    int      m_breakpointId;

public:
    typedef std::list<XDebugBreakpoint> List_t;
    
    /// A "Clear Id" for breakpoint functor
    struct ClearIdFunctor {
        void operator()(XDebugBreakpoint& bp) {
            bp.SetBreakpointId(wxNOT_FOUND);
        }
    };
    
    // Predicate class to be used with std::find_if
    struct Equal
    {
        wxString m_fileName;
        int      m_line;
    public:
        Equal(const wxString &filename, int line) : m_fileName(filename), m_line(line) {}
        bool operator()(const XDebugBreakpoint& bp) const {
            return m_fileName == bp.GetFileName() && m_line == bp.GetLine();
        }
    };

public:
    XDebugBreakpoint();
    XDebugBreakpoint(const wxString &filename, int line);
    virtual ~XDebugBreakpoint();
    bool operator==(const XDebugBreakpoint& other) const;

    bool IsApplied() const {
        return m_breakpointId != wxNOT_FOUND;
    }
    
    bool IsNull() const {
        return m_line == wxNOT_FOUND;
    }
    
    void SetBreakpointId(int breakpointId) {
        this->m_breakpointId = breakpointId;
    }
    int GetBreakpointId() const {
        return m_breakpointId;
    }
    void SetFileName(const wxString& fileName) {
        this->m_fileName = fileName;
    }
    void SetLine(int line) {
        this->m_line = line;
    }
    const wxString& GetFileName() const {
        return m_fileName;
    }
    int GetLine() const {
        return m_line;
    }
    
    /// Serialization
    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);
};

#endif // XDEBUGBREAKPOINT_H
