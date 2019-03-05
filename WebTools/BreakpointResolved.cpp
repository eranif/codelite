#include "BreakpointResolved.h"

BreakpointResolved::BreakpointResolved()
    : NodeMessageBase("Debugger.breakpointResolved")
{
}

BreakpointResolved::~BreakpointResolved()
{
}

NodeMessageBase::Ptr_t BreakpointResolved::Clone()
{
    return NodeMessageBase::Ptr_t(new BreakpointResolved());
}

void BreakpointResolved::Process(clWebSocketClient& socket, const JSONItem& json)
{
}
