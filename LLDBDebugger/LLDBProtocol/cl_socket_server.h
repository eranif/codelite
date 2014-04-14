#ifndef CLSOCKETSERVER_H
#define CLSOCKETSERVER_H

#include "cl_socket_base.h"

class clSocketServer : public clSocketBase
{
public:
    clSocketServer();
    virtual ~clSocketServer();

#ifndef __WXMSW__
    void CreateServer(const std::string &pipPath) throw (clSocketException);
#endif

    void CreateServer(const std::string &address, int port) throw (clSocketException);
    clSocketBase::Ptr_t WaitForNewConnection( long timeout = -1 ) throw (clSocketException);
};

#endif // CLSOCKETSERVER_H
