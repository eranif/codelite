#ifndef CLSOCKETCLIENT_H
#define CLSOCKETCLIENT_H

#include "SocketBase.hpp"
#include <string>
using namespace std;
namespace dap
{
class SocketClient : public SocketBase
{
    string m_path;

public:
    SocketClient();
    virtual ~SocketClient();

    /**
     * @brief connect to a remote server using ip/port
     * when using non-blocking mode, wouldBlock will be set to true
     * incase the connect fails
     */
    bool ConnectRemote(const string& address, int port);

    /**
     * @brief connect using connection string
     * @param connectionString in the format of tcp://127.0.0.1:1234
     * @return
     */
    bool Connect(const string& connectionString);
};
};     // namespace dap
#endif // CLSOCKETCLIENT_H
