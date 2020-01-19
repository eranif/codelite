#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "SocketBase.hpp"
#include <string>

using namespace std;
namespace dap
{
class SocketServer : public SocketBase
{
public:
    SocketServer();
    virtual ~SocketServer();

protected:
    /**
     * @throw clSocketException
     * @return port number
     */
    int CreateServer(const string& address, int port);

public:
    /**
     * @brief Create server using connection string
     * @return port number on success
     * @throw clSocketException
     */
    int Start(const string& connectionString);
    SocketBase::Ptr_t WaitForNewConnection(long timeout = -1);
    /**
     * @brief same as above, however, return a pointer to the connection that should be freed by the caller
     */
    SocketBase* WaitForNewConnectionRaw(long timeout = -1);
};
};     // namespace dap
#endif // CLSOCKETSERVER_H
