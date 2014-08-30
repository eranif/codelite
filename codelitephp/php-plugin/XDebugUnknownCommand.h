#ifndef XDEBUGUNKNOWNCOMMAND_H
#define XDEBUGUNKNOWNCOMMAND_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugUnknownCommand : public XDebugCommandHandler
{
public:
    XDebugUnknownCommand(XDebugManager* mgr, int transcationId);
    virtual ~XDebugUnknownCommand();

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGUNKNOWNCOMMAND_H
