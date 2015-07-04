#ifndef NODEJSSETBREAKPOINTHANDLER_H
#define NODEJSSETBREAKPOINTHANDLER_H

#include "NodeJSHandlerBase.h"
#include "NodeJSDebuggerBreakpoint.h"

class NodeJSDebugger;
class NodeJSSetBreakpointHandler : public NodeJSHandlerBase
{
    NodeJSBreakpoint m_breakpoint;

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
    NodeJSSetBreakpointHandler(const NodeJSBreakpoint& bp);
    virtual ~NodeJSSetBreakpointHandler();
};

#endif // NODEJSSETBREAKPOINTHANDLER_H
