#include "JsonRPC.hpp"
#include "SocketServer.hpp"

dap::JsonRPC::JsonRPC() {}

dap::JsonRPC::~JsonRPC() {}

void dap::JsonRPC::ServerStart(const string& connectString)
{
    m_socket.reset(new dap::SocketServer());
    m_socket->SetCloseOnExit(true);
    m_socket->As<dap::SocketServer>()->Start(connectString);
}

dap::SocketBase* dap::JsonRPC::ServerWaitForNewConnection()
{
    // wait for new connection for 1 second
    return m_socket->As<dap::SocketServer>()->WaitForNewConnectionRaw(1);
}
