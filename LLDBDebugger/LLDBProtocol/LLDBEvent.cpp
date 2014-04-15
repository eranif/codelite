#include "LLDBEvent.h"

wxDEFINE_EVENT(wxEVT_LLDB_STOPPED,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BACKTRACE, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_EXITED,    LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_STARTED,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_RUNNING,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BREAKPOINTS_UPDATED,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_FRAME_SELECTED,   LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_CRASHED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_LOCALS_UPDATED, LLDBEvent);
wxDEFINE_EVENT(wxEVT_LLDB_VARIABLE_EXPANDED, LLDBEvent);

LLDBEvent::LLDBEvent(wxEventType eventType, int windid)
    : clCommandEvent(eventType, windid)
    , m_interruptReason(0)
    , m_frameId(0)
    , m_threadId(0)
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
    m_interruptReason = src.m_interruptReason;
    m_frameId = src.m_frameId;
    m_threadId = src.m_threadId;
    m_breakpoints = src.m_breakpoints;
    m_variableId = src.m_variableId;
    m_locals.clear();
    m_locals.reserve( src.m_locals.size() );
    this->m_locals.insert(m_locals.end(), src.m_locals.begin(), src.m_locals.end());
    m_threads = src.m_threads;
    return *this;
}
