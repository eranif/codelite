//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBEvent.h
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

#ifndef LLDBEVENT_H
#define LLDBEVENT_H

#include "cl_command_event.h"
#include "LLDBBacktrace.h"
#include "LLDBBreakpoint.h"
#include "LLDBVariable.h"
#include "LLDBThread.h"

class LLDBEvent : public clCommandEvent
{
    LLDBBacktrace m_backtrace;
    wxString m_filename;
    int m_linenumber;
    int m_interruptReason;
    int m_frameId;
    int m_threadId;
    LLDBBreakpoint::Vec_t m_breakpoints;
    LLDBVariable::Vect_t m_variables;
    int m_variableId;
    LLDBThread::Vect_t m_threads;
    wxString m_expression;
    int m_sessionType;

public:
    LLDBEvent(wxEventType eventType, int winid = 0);
    virtual ~LLDBEvent();
    LLDBEvent(const LLDBEvent& src);
    LLDBEvent& operator=(const LLDBEvent& src);
    virtual wxEvent* Clone() const { return new LLDBEvent(*this); }

    bool ShouldPromptStopReason(wxString& message) const;

    void SetSessionType(int sessionType) { this->m_sessionType = sessionType; }
    int GetSessionType() const { return m_sessionType; }
    void SetExpression(const wxString& expression) { this->m_expression = expression; }
    const wxString& GetExpression() const { return m_expression; }
    void SetVariables(const LLDBVariable::Vect_t& locals)
    {
        this->m_variables.clear();
        this->m_variables.reserve(locals.size());
        this->m_variables.insert(this->m_variables.end(), locals.begin(), locals.end());
    }

    void SetThreads(const LLDBThread::Vect_t& threads) { this->m_threads = threads; }
    const LLDBThread::Vect_t& GetThreads() const { return m_threads; }
    void SetVariableId(int variableId) { this->m_variableId = variableId; }
    int GetVariableId() const { return m_variableId; }
    const LLDBVariable::Vect_t& GetVariables() const { return m_variables; }
    void SetBacktrace(const LLDBBacktrace& backtrace) { this->m_backtrace = backtrace; }
    const LLDBBacktrace& GetBacktrace() const { return m_backtrace; }
    void SetLinenumber(int linenumber) { this->m_linenumber = linenumber; }
    int GetLinenumber() const { return m_linenumber; }

    void SetFrameId(int frameId) { this->m_frameId = frameId; }
    void SetThreadId(int threadId) { this->m_threadId = threadId; }
    int GetFrameId() const { return m_frameId; }
    int GetThreadId() const { return m_threadId; }
    /**
     * @brief return the Interrupt reason (wxEVT_LLDB_STOPPED)
     */
    int GetInterruptReason() const { return m_interruptReason; }

    void SetInterruptReason(int stopReason) { this->m_interruptReason = stopReason; }
    void SetBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints) { this->m_breakpoints = breakpoints; }
    const LLDBBreakpoint::Vec_t& GetBreakpoints() const { return m_breakpoints; }
};

wxDECLARE_EVENT(wxEVT_LLDB_STOPPED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_BACKTRACE, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_EXITED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_STARTED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_RUNNING, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_BREAKPOINTS_UPDATED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_FRAME_SELECTED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_CRASHED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_LOCALS_UPDATED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_VARIABLE_EXPANDED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_EXPRESSION_EVALUATED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_INTERPERTER_REPLY, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_LAUNCH_SUCCESS, LLDBEvent);

typedef void (wxEvtHandler::*LLDBEventFunction)(LLDBEvent&);
#define LLDBEventHandler(func) wxEVENT_HANDLER_CAST(LLDBEventFunction, func)

#endif // LLDBEVENT_H
