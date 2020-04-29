//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBBreakpoint.h
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

#ifndef LLDBBREAKPOINT_H
#define LLDBBREAKPOINT_H

#include <wx/string.h>
#include <vector>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include "debugger.h"
#include "JSON.h"

class LLDBBreakpoint
{
public:
    // Breakpoint type
    enum {
        kInvalid = -1,
        kFileLine,
        kFunction,
        kLocation,
    };
    typedef wxSharedPtr<LLDBBreakpoint> Ptr_t;
    typedef std::vector<LLDBBreakpoint::Ptr_t> Vec_t;

protected:
    int m_id;
    int m_type;
    wxString m_name;
    wxString m_filename;
    int m_lineNumber;
    LLDBBreakpoint::Vec_t m_children;

public:
    class Equal
    {
        LLDBBreakpoint::Ptr_t m_src;

    public:
        Equal(LLDBBreakpoint::Ptr_t src)
            : m_src(src)
        {
        }
        bool operator()(LLDBBreakpoint::Ptr_t other) const { return m_src->SameAs(other); }
    };

public:
    LLDBBreakpoint()
        : m_id(wxNOT_FOUND)
        , m_type(kInvalid)
        , m_lineNumber(wxNOT_FOUND)
    {
    }
    LLDBBreakpoint(const wxString& name);
    LLDBBreakpoint(const wxFileName& filename, int line);
    bool SameAs(LLDBBreakpoint::Ptr_t other) const;
    virtual ~LLDBBreakpoint();

    /**
     * @brief copy breakpoint from other
     * @param other
     */
    void Copy(LLDBBreakpoint::Ptr_t other);

    bool IsLocation() const { return m_type == kLocation; }

    size_t GetNumChildren() const { return m_children.size(); }

    LLDBBreakpoint::Vec_t& GetChildren() { return m_children; }

    const LLDBBreakpoint::Vec_t& GetChildren() const { return m_children; }

    /**
     * @brief convert list of gdb breakpoints into LLDBBreakpoint vector
     */
    static clDebuggerBreakpoint::Vec_t ToBreakpointInfoVector(const LLDBBreakpoint::Vec_t& breakpoints);
    /**
     * @brief convert list of lldb breakpoints into gdb's breakpoint list
     * @param breakpoints
     */
    static LLDBBreakpoint::Vec_t FromBreakpointInfoVector(const clDebuggerBreakpoint::Vec_t& breakpoints);

    /**
     * @brief return a string representation for this breakpoint
     */
    wxString ToString() const;

    bool IsApplied() const { return m_id != wxNOT_FOUND; }

    /**
     * @brief return true if this is a valid breakpoint
     */
    bool IsValid() const;

    void Invalidate();

    /**
     * @brief set the breakpoint filename.
     * @param filename the filename
     * @param normalise when set to 'true' CodeLite will attempt to remove . and .., and convert the path to fullpath.
     * This is the default
     */
    void SetFilename(const wxString& filename, bool normalise = true)
    {
        if(normalise) {
            wxFileName normalizedFilename(filename);
            normalizedFilename.Normalize();
            this->m_filename = normalizedFilename.GetFullPath();

        } else {
            this->m_filename = filename;
        }
    }
    void SetLineNumber(int lineNumber) { this->m_lineNumber = lineNumber; }
    void SetName(const wxString& name) { this->m_name = name; }
    void SetType(int type) { this->m_type = type; }
    const wxString& GetFilename() const { return m_filename; }
    int GetLineNumber() const { return m_lineNumber; }
    const wxString& GetName() const { return m_name; }
    int GetType() const { return m_type; }
    void SetId(int id) { this->m_id = id; }
    int GetId() const { return m_id; }

    // Serialization API
    void FromJSON(const JSONItem& json);
    JSONItem ToJSON() const;
};

#endif // LLDBBREAKPOINT_H
