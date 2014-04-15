#ifndef LLDBREPLY_H
#define LLDBREPLY_H

#include "json_node.h"
#include "LLDBEnums.h"
#include "LLDBBreakpoint.h"
#include "LLDBBacktrace.h"
#include "LLDBLocalVariable.h"

class LLDBReply
{
protected:
    int                         m_replyType;
    int                         m_interruptResaon;
    int                         m_line;
    wxString                    m_filename;
    LLDBBreakpoint::Vec_t       m_breakpoints;
    LLDBBacktrace               m_backtrace;
    LLDBLocalVariable::Vect_t   m_locals;
    int                         m_lldbId;

public:
    LLDBReply() : m_replyType(kReplyTypeInvalid), m_interruptResaon(kInterruptReasonNone), m_line(wxNOT_FOUND), m_lldbId(wxNOT_FOUND) {}
    LLDBReply(const wxString &str);
    virtual ~LLDBReply();

    void SetLldbId(int lldbId) {
        this->m_lldbId = lldbId;
    }
    int GetLldbId() const {
        return m_lldbId;
    }
    void SetLocals(const LLDBLocalVariable::Vect_t& locals) {
        this->m_locals = locals;
    }
    const LLDBLocalVariable::Vect_t& GetLocals() const {
        return m_locals;
    }
    void SetBacktrace(const LLDBBacktrace& backtrace) {
        this->m_backtrace = backtrace;
    }
    const LLDBBacktrace& GetBacktrace() const {
        return m_backtrace;
    }
    void SetBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints) {
        this->m_breakpoints = breakpoints;
    }
    void SetInterruptResaon(int interruptResaon) {
        this->m_interruptResaon = interruptResaon;
    }
    const LLDBBreakpoint::Vec_t& GetBreakpoints() const {
        return m_breakpoints;
    }
    void SetReplyType(int replyType) {
        this->m_replyType = replyType;
    }
    int GetReplyType() const {
        return m_replyType;
    }
    void SetFilename(const wxString& filename) {
        this->m_filename = filename;
    }
    void SetLine(int line) {
        this->m_line = line;
    }
    const wxString& GetFilename() const {
        return m_filename;
    }
    int GetLine() const {
        return m_line;
    }
    int GetInterruptResaon() const {
        return m_interruptResaon;
    }
    // Serialization API
    JSONElement ToJSON() const;
    void FromJSON(const JSONElement& json);
};

#endif // LLDBREPLY_H
