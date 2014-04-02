#ifndef LLDBEVENT_H
#define LLDBEVENT_H

#include "cl_command_event.h"
#include "LLDBBacktrace.h"

class LLDBEvent : public clCommandEvent
{
    LLDBBacktrace m_backtrace;
    wxString      m_filename;
    int           m_linenumber;
    int           m_stopReason;

public:
    LLDBEvent(wxEventType eventType, int winid = 0);
    virtual ~LLDBEvent();
    LLDBEvent(const LLDBEvent& src);
    LLDBEvent& operator=(const LLDBEvent& src);
    virtual wxEvent* Clone() const {
        return new LLDBEvent(*this);
    }
    void SetBacktrace(const LLDBBacktrace& backtrace) {
        this->m_backtrace = backtrace;
    }
    const LLDBBacktrace& GetBacktrace() const {
        return m_backtrace;
    }
    void SetLinenumber(int linenumber) {
        this->m_linenumber = linenumber;
    }
    int GetLinenumber() const {
        return m_linenumber;
    }
    
    /**
     * @brief return the Interrupt reason (wxEVT_LLDB_STOPPED)
     */
    int GetStopReason() const {
        return m_stopReason;
    }
    
    void SetStopReason(int stopReason) {
        this->m_stopReason = stopReason;
    }
};

wxDECLARE_EVENT(wxEVT_LLDB_STOPPED,   LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_BACKTRACE, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_EXITED,    LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_STARTED,   LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY,   LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_RUNNING,   LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_BREAKPOINTS_UPDATED, LLDBEvent);
wxDECLARE_EVENT(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, LLDBEvent);

typedef void (wxEvtHandler::*LLDBEventFunction)(LLDBEvent&);
#define LLDBEventHandler(func) \
    wxEVENT_HANDLER_CAST(LLDBEventFunction, func)

#endif // LLDBEVENT_H
