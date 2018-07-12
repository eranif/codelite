#ifndef CSLISTCOMMANDHANDLER_H
#define CSLISTCOMMANDHANDLER_H

#include "csCommandHandlerBase.h"
class csListCommandHandler : public csCommandHandlerBase
{
public:
    csListCommandHandler();
    virtual ~csListCommandHandler();
    virtual void Process(const clNetworkMessage& message, clSocketBase* conn);
};

#endif // CSLISTCOMMANDHANDLER_H
