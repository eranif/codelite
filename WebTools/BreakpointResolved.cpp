#include "BreakpointResolved.h"

BreakpointResolved::BreakpointResolved()
    : NodeMessageBase("Debugger.breakpointResolved")
{
}

NodeMessageBase::Ptr_t BreakpointResolved::Clone()
{
    return NodeMessageBase::Ptr_t(new BreakpointResolved());
}

void BreakpointResolved::Process(clWebSocketClient& socket, const JSONItem& json)
{
}
