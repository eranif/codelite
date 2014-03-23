#include "LLDBEvent.h"

wxDEFINE_EVENT(wxEVT_LLDB_STOPPED,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BACKTRACE, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_EXITED,    LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_STARTED,   LLDBEvent);

LLDBEvent::LLDBEvent(wxEventType eventType, int windid)
    : wxCommandEvent(eventType, windid)
{
}

LLDBEvent::~LLDBEvent()
{
}

LLDBEvent::LLDBEvent(const LLDBEvent& src)
{
    *this = src;
}

LLDBEvent& LLDBEvent::operator=(const LLDBEvent& src)
{
    m_id = src.m_id;
    m_eventType = src.m_eventType;
    m_eventObject = src.m_eventObject;
    m_commandInt = src.m_commandInt;
    m_cmdString = src.m_cmdString;
    m_extraLong = src.m_extraLong;
    m_backtrace = src.m_backtrace;
    m_filename = src.m_filename;
    m_linenumber = src.m_linenumber;
    return *this;
}
