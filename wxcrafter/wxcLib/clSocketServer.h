#ifndef CLSOCKETSERVER_H
#define CLSOCKETSERVER_H

#include "wxcLib/clSocketBase.h"

class clSocketServer : public clSocketBase
{
public:
    clSocketServer();
    virtual ~clSocketServer();

    /**
     * @brief On Windows, the 'pipePath' is a string containing the port number.
     * LocalSocket is implemented under Windows with TCP/IP with IP always set to 127.0.0.1
     * @param pipePath
     */
    void CreateServer(const std::string &pipePath) ;
    void CreateServer(const std::string &address, int port) ;
    clSocketBase::Ptr_t WaitForNewConnection( long timeout = -1 ) ;
};

#endif // CLSOCKETSERVER_H
