#ifndef BREAKPOINTRESOLVED_H
#define BREAKPOINTRESOLVED_H

#include "NodeMessageBase.h" // Base class: NodeMessageBase

class BreakpointResolved : public NodeMessageBase
{
public:
    BreakpointResolved();
    virtual ~BreakpointResolved();

public:
    virtual NodeMessageBase::Ptr_t Clone();
    virtual void Process(clWebSocketClient& socket, const JSONElement& json);
};

#endif // BREAKPOINTRESOLVED_H
