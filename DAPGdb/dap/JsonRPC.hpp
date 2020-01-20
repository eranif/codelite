#ifndef JSONRPC_HPP
#define JSONRPC_HPP

#include "SocketBase.hpp"

namespace dap
{
class JsonRPC
{
    SocketBase::Ptr_t m_socket;

public:
    JsonRPC();
    ~JsonRPC();
    
    /**
     * @brief start JSON RPC server on a connection string
     * @throws dap::SocketException
     */
    void ServerStart(const string& connectString);
    
    /**
     * @brief wait for new connection
     * @throws dap::SocketException
     */
    SocketBase* ServerWaitForNewConnection();
};
};     // namespace dap
#endif // JSONRPC_HPP
