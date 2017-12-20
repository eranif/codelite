//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBEvent.cpp
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

#include "LLDBEvent.h"

wxDEFINE_EVENT(wxEVT_LLDB_STOPPED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BACKTRACE, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_EXITED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_STARTED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_RUNNING, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BREAKPOINTS_UPDATED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_FRAME_SELECTED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_CRASHED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_LOCALS_UPDATED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_VARIABLE_EXPANDED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_EXPRESSION_EVALUATED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_INTERPERTER_REPLY, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_LAUNCH_SUCCESS, LLDBEvent);

LLDBEvent::LLDBEvent(wxEventType eventType, int windid)
    : clCommandEvent(eventType, windid)
    , m_interruptReason(0)
    , m_frameId(0)
    , m_threadId(0)
    , m_sessionType(kDebugSessionTypeNormal)
{
}

LLDBEvent::~LLDBEvent() {}

LLDBEvent::LLDBEvent(const LLDBEvent& src) { *this = src; }

LLDBEvent& LLDBEvent::operator=(const LLDBEvent& src)
{
    clCommandEvent::operator=(src);
    m_sessionType = src.m_sessionType;
    m_backtrace = src.m_backtrace;
    m_filename = src.m_filename;
    m_linenumber = src.m_linenumber;
    m_interruptReason = src.m_interruptReason;
    m_frameId = src.m_frameId;
    m_threadId = src.m_threadId;
    m_breakpoints = src.m_breakpoints;
    m_variableId = src.m_variableId;
    m_variables = src.m_variables;
    m_threads = src.m_threads;
    m_expression = src.m_expression;
    return *this;
}

bool LLDBEvent::ShouldPromptStopReason(wxString& message) const
{
    LLDBThread::Vect_t::const_iterator iter = m_threads.begin();
    for(; iter != m_threads.end(); ++iter) {
        if(iter->GetStopReason() == kStopReasonSignal || iter->GetStopReason() == kStopReasonException) {
            message = iter->GetStopReasonString();
            return true;
        }
    }
    return false;
}
