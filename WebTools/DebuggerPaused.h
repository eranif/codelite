#ifndef NODEEVENTDEBUGGERPAUSED_H
#define NODEEVENTDEBUGGERPAUSED_H

#include "NodeMessageBase.h" // Base class: NodeMessageBase

class DebuggerPaused : public NodeMessageBase
{
    wxString m_stopReason;

public:
    DebuggerPaused();
    virtual ~DebuggerPaused();

public:
    void Process(const JSONElement& json);
    NodeMessageBase::Ptr_t Clone();
};

#endif // NODEEVENTDEBUGGERPAUSED_H
