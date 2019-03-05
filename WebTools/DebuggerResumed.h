#ifndef DEBUGGERRESUMED_H
#define DEBUGGERRESUMED_H

#include "NodeMessageBase.h"
class DebuggerResumed : public NodeMessageBase
{
    wxString m_stopReason;

public:
    DebuggerResumed();
    virtual ~DebuggerResumed();

public:
    void Process(clWebSocketClient& socket, const JSONItem& json);
    NodeMessageBase::Ptr_t Clone();
};

#endif // DEBUGGERRESUMED_H
