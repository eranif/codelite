#ifndef XDEBUGEVENT_H
#define XDEBUGEVENT_H

#include "php_event.h"
#include "XVariable.h"

class XDebugEvent : public PHPEvent
{
    XVariable::List_t m_variables;
    bool m_evalSucceeded;
    wxString m_errorString;
    wxString m_evaluted;
    int m_evalReason;

public:
    XDebugEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    XDebugEvent(const XDebugEvent& src);
    XDebugEvent& operator=(const XDebugEvent& src);
    virtual ~XDebugEvent();

    virtual wxEvent* Clone() const {
        return new XDebugEvent(*this);
    }
    void SetVariables(const XVariable::List_t& variables) {
        this->m_variables = variables;
    }
    const XVariable::List_t& GetVariables() const {
        return m_variables;
    }
    void SetEvalSucceeded(bool evalSucceeded) {
        this->m_evalSucceeded = evalSucceeded;
    }
    bool IsEvalSucceeded() const {
        return m_evalSucceeded;
    }
    void SetErrorString(const wxString& errorString) {
        this->m_errorString = errorString;
    }
    const wxString& GetErrorString() const {
        return m_errorString;
    }
    void SetEvaluated(const wxString& evaluted) {
        this->m_evaluted = evaluted;
    }
    const wxString& GetEvaluted() const {
        return m_evaluted;
    }
    void SetEvalReason(int evalReason) {
        this->m_evalReason = evalReason;
    }
    void SetEvaluted(const wxString& evaluted) {
        this->m_evaluted = evaluted;
    }
    int GetEvalReason() const {
        return m_evalReason;
    }
};

wxDECLARE_EVENT(wxEVT_XDEBUG_IDE_GOT_CONTROL,     XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_STOPPED,             XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_STACK_TRACE,         XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_SESSION_STARTED,     XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_SESSION_STARTING,    XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_SESSION_ENDED,       XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_LOCALS_UPDATED,      XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_EVAL_EXPRESSION,     XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_UNKNOWN_RESPONSE,    XDebugEvent);
wxDECLARE_EVENT(wxEVT_XDEBUG_PROPERTY_GET,        XDebugEvent);

#endif // XDEBUGEVENT_H
