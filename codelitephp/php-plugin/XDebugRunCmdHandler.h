#ifndef XDEBUGRUNCMDHANDLER_H
#define XDEBUGRUNCMDHANDLER_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugManager;
class wxXmlNode;
class wxSocketBase;
class XDebugRunCmdHandler : public XDebugCommandHandler
{
public:
    XDebugRunCmdHandler(XDebugManager* mgr, int transcationId);
    virtual ~XDebugRunCmdHandler();

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGRUNCMDHANDLER_H
