#ifndef XDEBUGBREAKPOINTCMDHANDLER_H
#define XDEBUGBREAKPOINTCMDHANDLER_H

#include "XDebugCommandHandler.h"
#include "XDebugBreakpoint.h"

class wxXmlNode;
class wxSocketBase;
class XDebugBreakpointCmdHandler : public XDebugCommandHandler
{
    XDebugBreakpoint &m_breakpoint;
    
public:
    XDebugBreakpointCmdHandler(XDebugManager* mgr, int transcationId, XDebugBreakpoint &breakpoint);
    virtual ~XDebugBreakpointCmdHandler();

    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGBREAKPOINTCMDHANDLER_H
