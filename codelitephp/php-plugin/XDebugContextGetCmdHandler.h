#ifndef XDEBUGCONTEXTGETCMDHANDLER_H
#define XDEBUGCONTEXTGETCMDHANDLER_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugContextGetCmdHandler : public XDebugCommandHandler
{
    int m_requestedStack;
public:
    XDebugContextGetCmdHandler(XDebugManager* mgr, int transcationId, int requestedStack = 0);
    virtual ~XDebugContextGetCmdHandler();

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGCONTEXTGETCMDHANDLER_H
