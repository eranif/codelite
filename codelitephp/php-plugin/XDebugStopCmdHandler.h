#ifndef XDEBUGSTOPCMDHANDLER_H
#define XDEBUGSTOPCMDHANDLER_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugStopCmdHandler : public XDebugCommandHandler
{
public:
    XDebugStopCmdHandler(XDebugManager* mgr, int transcationId);
    virtual ~XDebugStopCmdHandler();

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGSTOPCMDHANDLER_H
