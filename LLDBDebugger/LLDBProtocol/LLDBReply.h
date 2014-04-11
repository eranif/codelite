#ifndef LLDBREPLY_H
#define LLDBREPLY_H

#include "json_node.h"
#include "LLDBEnums.h"
#include "LLDBBreakpoint.h"
#include "LLDBBacktrace.h"

class LLDBReply
{
protected:
    int      m_transcationId;
    int      m_replyType;
    int      m_interruptResaon;
    int      m_line;
    wxString m_filename;
    LLDBBreakpoint::Vec_t m_breakpoints;
    LLDBBacktrace m_backtrace;

public:
    LLDBReply() : m_transcationId(0), m_replyType(kTypeInvalid), m_interruptResaon(kInterruptReasonNone), m_line(wxNOT_FOUND) {}
    LLDBReply(const wxString &str);
    virtual ~LLDBReply();

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
    void SetTranscationId(int transcationId) {
        this->m_transcationId = transcationId;
    }
    int GetTranscationId() const {
        return m_transcationId;
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
