#ifndef NODEEVENTDEBUGGERPAUSED_H
#define NODEEVENTDEBUGGERPAUSED_H

#include "NodeDbgEventBase.h" // Base class: NodeDbgEventBase

class NodeEventDebuggerPaused : public NodeDbgEventBase
{
    wxString m_stopReason;

public:
    NodeEventDebuggerPaused();
    virtual ~NodeEventDebuggerPaused();

public:
    void Process(const JSONElement& json);
    NodeDbgEventBase::Ptr_t Clone();
};

#endif // NODEEVENTDEBUGGERPAUSED_H
