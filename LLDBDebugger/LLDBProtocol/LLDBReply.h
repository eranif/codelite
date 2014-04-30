#ifndef LLDBREPLY_H
#define LLDBREPLY_H

#include "json_node.h"
#include "LLDBEnums.h"
#include "LLDBBreakpoint.h"
#include "LLDBBacktrace.h"
#include "LLDBVariable.h"
#include "LLDBThread.h"
#include "LLDBPivot.h"

class LLDBReply
{
protected:
    int                         m_replyType;
    int                         m_interruptResaon;
    int                         m_line;
    wxString                    m_filename;
    LLDBBreakpoint::Vec_t       m_breakpoints;
    LLDBBacktrace               m_backtrace;
    LLDBVariable::Vect_t        m_variables;
    LLDBThread::Vect_t          m_threads;
    int                         m_lldbId;
    wxString                    m_expression;
    int                         m_debugSessionType;

public:
    LLDBReply()
        : m_replyType(kReplyTypeInvalid)
        , m_interruptResaon(kInterruptReasonNone)
        , m_line(wxNOT_FOUND)
        , m_lldbId(wxNOT_FOUND)
        , m_debugSessionType(kDebugSessionTypeNormal)
    {}

    LLDBReply(const wxString &str);
    virtual ~LLDBReply();
    
    void UpdatePaths(const LLDBPivot& pivot);
    void SetDebugSessionType(int debugSessionType) {
        this->m_debugSessionType = debugSessionType;
    }
    int GetDebugSessionType() const {
        return m_debugSessionType;
    }
    void SetExpression(const wxString& expression) {
        this->m_expression = expression;
    }
    const wxString& GetExpression() const {
        return m_expression;
    }
    void SetThreads(const LLDBThread::Vect_t& threads) {
        this->m_threads = threads;
    }
    const LLDBThread::Vect_t& GetThreads() const {
        return m_threads;
    }
    void SetLldbId(int lldbId) {
        this->m_lldbId = lldbId;
    }
    int GetLldbId() const {
        return m_lldbId;
    }
    void SetVariables(const LLDBVariable::Vect_t& locals) {
        this->m_variables = locals;
    }
    const LLDBVariable::Vect_t& GetVariables() const {
        return m_variables;
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
