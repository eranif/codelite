#ifndef CLSOCKETSERVER_H
#define CLSOCKETSERVER_H

#include "clSocketBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clSocketServer : public clSocketBase
{
public:
    clSocketServer();
    virtual ~clSocketServer();

    /**
     * @brief On Windows, the 'pipePath' is a string containing the port number.
     * LocalSocket is implemented under Windows with TCP/IP with IP always set to 127.0.0.1
     * @param pipePath
     */
    void CreateServer(const std::string &pipePath) throw (clSocketException);
    void CreateServer(const std::string &address, int port) throw (clSocketException);
    clSocketBase::Ptr_t WaitForNewConnection( long timeout = -1 ) throw (clSocketException);
};

#endif // CLSOCKETSERVER_H
