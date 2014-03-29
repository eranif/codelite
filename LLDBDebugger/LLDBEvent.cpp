#include "LLDBEvent.h"

wxDEFINE_EVENT(wxEVT_LLDB_STOPPED,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BACKTRACE, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_EXITED,    LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_STARTED,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_RUNNING,   LLDBEvent);

LLDBEvent::LLDBEvent(wxEventType eventType, int windid)
    : clCommandEvent(eventType, windid)
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
    clCommandEvent::operator =(src);
    m_backtrace = src.m_backtrace;
    m_filename = src.m_filename;
    m_linenumber = src.m_linenumber;
    return *this;
}
