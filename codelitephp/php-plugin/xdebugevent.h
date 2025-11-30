//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : xdebugevent.h
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

#ifndef XDEBUGEVENT_H
#define XDEBUGEVENT_H

#include "php_event.h"
#include "XVariable.h"

class XDebugEvent : public PHPEvent
{
    XVariable::List_t m_variables;
    bool m_evalSucceeded = false;
    wxString m_errorString;
    wxString m_evaluated;
    int m_evalReason = wxNOT_FOUND;

public:
    XDebugEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    XDebugEvent(const XDebugEvent&) = default;
    XDebugEvent& operator=(const XDebugEvent&) = delete;
    ~XDebugEvent() override = default;

    wxEvent* Clone() const override { return new XDebugEvent(*this); }
    void SetVariables(const XVariable::List_t& variables) { this->m_variables = variables; }
    const XVariable::List_t& GetVariables() const { return m_variables; }
    void SetEvalSucceeded(bool evalSucceeded) { this->m_evalSucceeded = evalSucceeded; }
    bool IsEvalSucceeded() const { return m_evalSucceeded; }
    void SetErrorString(const wxString& errorString) { this->m_errorString = errorString; }
    const wxString& GetErrorString() const { return m_errorString; }
    void SetEvaluated(const wxString& evaluated) { this->m_evaluated = evaluated; }
    const wxString& GetEvaluated() const { return m_evaluated; }
    void SetEvalReason(int evalReason) { this->m_evalReason = evalReason; }
    int GetEvalReason() const { return m_evalReason; }
};

wxDECLARE_EVENT(wxEVT_XDEBUG_IDE_GOT_CONTROL, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_STOPPED, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_STACK_TRACE, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_SESSION_STARTED, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_CONNECTED, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_SESSION_STARTING, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_SESSION_ENDED, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_LOCALS_UPDATED, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_EVAL_EXPRESSION, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_UNKNOWN_RESPONSE, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_PROPERTY_GET, XDebugEvent);

#endif // XDEBUGEVENT_H
