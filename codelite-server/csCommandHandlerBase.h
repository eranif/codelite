#ifndef CSCOMMANDHANDLERBASE_H
#define CSCOMMANDHANDLERBASE_H

#include <clNetworkMessage.h>
#include <SocketAPI/clSocketBase.h>
#include <wx/sharedptr.h>

class csCommandHandlerBase
{
public:
    typedef wxSharedPtr<csCommandHandlerBase> Ptr_t;
    
public:
    csCommandHandlerBase();
    virtual ~csCommandHandlerBase();
    
    /**
     * @brief process a network command
     * @param message the command message
     * @param conn connection for replying back
     */
    virtual void Process(const clNetworkMessage& message, clSocketBase* conn) = 0;
};

#endif // CSCOMMANDHANDLERBASE_H
