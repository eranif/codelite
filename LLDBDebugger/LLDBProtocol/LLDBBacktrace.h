//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBBacktrace.h
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

#ifndef LLDBBACKTRACE_H
#define LLDBBACKTRACE_H

#include <wx/string.h>
#include <vector>
#include "LLDBSettings.h"
#include "LLDBEnums.h"

#if BUILD_CODELITE_LLDB
#include "lldb/API/SBBlock.h"
#include "lldb/API/SBCompileUnit.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBFunction.h"
#include "lldb/API/SBModule.h"
#include "lldb/API/SBStream.h"
#include "lldb/API/SBSymbol.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBListener.h"
#endif

#include "JSON.h"

/**
 * @class LLDBBacktrace
 * Construct a human readable backtrace from lldb::SBThread object
 */
class LLDBBacktrace
{
public:
    struct Entry {
        int id;
        int line;
        wxString filename;
        wxString functionName;
        wxString address;

        JSONItem ToJSON() const;
        void FromJSON(const JSONItem& json);

        Entry()
            : id(0)
            , line(0)
        {
        }
    };
    typedef std::vector<LLDBBacktrace::Entry> EntryVec_t;

protected:
    int m_threadId;
    int m_selectedFrameId;
    LLDBBacktrace::EntryVec_t m_callstack;

public:

#if BUILD_CODELITE_LLDB
    LLDBBacktrace(lldb::SBThread& thread, const LLDBSettings& settings);
#endif

    LLDBBacktrace()
        : m_threadId(0)
        , m_selectedFrameId(0)
    {
    }
    virtual ~LLDBBacktrace();

    void Clear()
    {
        m_selectedFrameId = 0;
        m_threadId = 0;
        m_callstack.clear();
    }

    void SetSelectedFrameId(int selectedFrameId) { this->m_selectedFrameId = selectedFrameId; }
    int GetSelectedFrameId() const { return m_selectedFrameId; }
    void SetCallstack(const LLDBBacktrace::EntryVec_t& callstack) { this->m_callstack = callstack; }
    void SetThreadId(int threadId) { this->m_threadId = threadId; }
    const LLDBBacktrace::EntryVec_t& GetCallstack() const { return m_callstack; }
    int GetThreadId() const { return m_threadId; }

    wxString ToString() const;

    // Serialization API
    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);
};

#endif // LLDBBACKTRACE_H
