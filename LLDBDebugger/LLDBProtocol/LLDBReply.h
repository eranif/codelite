#ifndef LLDBREPLY_H
#define LLDBREPLY_H

#include "json_node.h"
class LLDBReply
{
public:
    enum {
        kReasonInvalid,
        kReasonDebuggerStartedSuccessfully,
        kReasonDebuggerStopped,
        kReasonDebuggerExited,
        kReasonBacktraceReceived,
    };

protected:
    int m_transcationId;
    int m_reasonCode;

public:
    LLDBReply() : m_transcationId(0), m_reasonCode(kReasonInvalid) {}
    virtual ~LLDBReply();

    void SetReasonCode(int reasonCode) {
        this->m_reasonCode = reasonCode;
    }
    void SetTranscationId(int transcationId) {
        this->m_transcationId = transcationId;
    }
    int GetReasonCode() const {
        return m_reasonCode;
    }
    int GetTranscationId() const {
        return m_transcationId;
    }
    
    // Serialization API
    JSONElement ToJSON() const;
    void FromJSON(const JSONElement& json);
};

#endif // LLDBREPLY_H
