#include "xdebugevent.h"

wxDEFINE_EVENT(wxEVT_XDEBUG_IDE_GOT_CONTROL,     XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_STOPPED,             XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_STACK_TRACE,         XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_SESSION_STARTED,     XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_SESSION_STARTING,    XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_SESSION_ENDED,       XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_LOCALS_UPDATED,      XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_EVAL_EXPRESSION,     XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_UNKNOWN_RESPONSE,    XDebugEvent);
wxDEFINE_EVENT(wxEVT_XDEBUG_PROPERTY_GET,        XDebugEvent);

XDebugEvent::XDebugEvent(wxEventType commandType, int winid)
    : PHPEvent(commandType, winid)
    , m_evalSucceeded(false)
    , m_evalReason(wxNOT_FOUND)
{
}

XDebugEvent::XDebugEvent(const XDebugEvent& src)
{
    *this = src;
}

XDebugEvent::~XDebugEvent()
{
}

XDebugEvent& XDebugEvent::operator=(const XDebugEvent& src)
{
    PHPEvent::operator=(src);
    m_variables = src.m_variables;
    m_evalSucceeded = src.m_evalSucceeded;
    m_errorString = src.m_errorString;
    m_evaluted = src.m_evaluted;
    m_evalReason = src.m_evalReason;
    return *this;
}
