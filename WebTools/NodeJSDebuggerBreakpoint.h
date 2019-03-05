//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSDebuggerBreakpoint.h
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

#ifndef NODEJSDEBUGGERBREAKPOINT_H
#define NODEJSDEBUGGERBREAKPOINT_H

#include "JSON.h"
#include "nSerializableObject.h"
#include <vector>
#include <wx/string.h>

class NodeJSBreakpoint : public nSerializableObject
{
    wxString m_filename;
    int m_line = wxNOT_FOUND;
    wxString m_nodeBpID; // If applied, this will contain V8 breakpoint ID

public:
    typedef std::vector<NodeJSBreakpoint> Vec_t;

public:
    NodeJSBreakpoint();
    virtual ~NodeJSBreakpoint();

    JSONItem ToJSON(const wxString& name) const;
    void FromJSON(const JSONItem& json);

    bool IsOk() const { return !m_filename.IsEmpty() && (m_line != wxNOT_FOUND); }

    /**
     * @brief is this breakpoint was already applied in NodeJS?
     */
    bool IsApplied() const { return !m_nodeBpID.empty(); }
    void SetFilename(const wxString& filename) { this->m_filename = filename; }
    void SetLine(int line) { this->m_line = line; }

    const wxString& GetFilename() const { return m_filename; }
    int GetLine() const { return m_line; }

    void SetNodeBpID(const wxString& nodeBpID) { this->m_nodeBpID = nodeBpID; }
    const wxString& GetNodeBpID() const { return m_nodeBpID; }
};

#endif // NODEJSDEBUGGERBREAKPOINT_H
