#ifndef XDEBUGSTACKGETCMDHANDLER_H
#define XDEBUGSTACKGETCMDHANDLER_H

#include "XDebugCommandHandler.h"
class XDebugStackGetCmdHandler : public XDebugCommandHandler
{
    int m_requestedStack;
public:
    XDebugStackGetCmdHandler(XDebugManager* mgr, int transcationId, int requestedStack = 0);
    virtual ~XDebugStackGetCmdHandler();

    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGSTACKGETCMDHANDLER_H
