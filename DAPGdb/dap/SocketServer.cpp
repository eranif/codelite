#include "ConnectionString.hpp"
#include "SocketServer.hpp"

#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace dap
{
SocketServer::SocketServer() {}

SocketServer::~SocketServer() { DestroySocket(); }

int SocketServer::CreateServer(const string& address, int port)
{
    // Create a socket
    if((m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        throw SocketException("Could not create socket: " + error());
    }

    // must set reuse-address
    int optval;

    // set SO_REUSEADDR on a socket to true (1):
    optval = 1;
    ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

    // Prepare the sockaddr_in structure
    struct sockaddr_in server;
    server.sin_family = AF_INET;
#ifdef _WIN32
    server.sin_addr.s_addr = inet_addr(address.c_str());
#else
    inet_pton(AF_INET, address.c_str(), &server.sin_addr);
#endif
    server.sin_port = htons(port);

    // Bind
    if(::bind(m_socket, (struct sockaddr*)&server, sizeof(server)) != 0) {
        throw SocketException("CreateServer: bind() error: " + error());
    }

    if(port == 0) {
        struct sockaddr_in socket_name;
#ifdef _WIN32
        int name_len = sizeof(socket_name);
#else
        socklen_t name_len = sizeof(socket_name);
#endif
        if(::getsockname(m_socket, (struct sockaddr*)&socket_name, &name_len) != 0) {
            throw SocketException("CreateServer: getsockname() error: " + error());
        }
        port = ntohs(socket_name.sin_port);
    }
    // define the accept queue size
    if(::listen(m_socket, 10) != 0) { throw SocketException("CreateServer: listen() error: " + error()); }

    // return the bound port number
    return port;
}

int SocketServer::Start(const string& connectionString)
{
    ConnectionString cs(connectionString);
    if(!cs.IsOK()) { throw SocketException("Invalid connection string provided"); }
    if(cs.GetProtocol() == ConnectionString::kTcp) {
        return CreateServer(cs.GetHost(), cs.GetPort());
    } else {
        throw SocketException("Unsupported protocol");
    }
}

SocketBase::Ptr_t SocketServer::WaitForNewConnection(long timeout)
{
    return SocketBase::Ptr_t(WaitForNewConnectionRaw(timeout));
}

SocketBase* SocketServer::WaitForNewConnectionRaw(long timeout)
{
    if(SelectRead(timeout) == kTimeout) { return NULL; }
    int fd = ::accept(m_socket, 0, 0);
    if(fd < 0) { throw SocketException("accept error: " + error()); }
    return new SocketBase(fd);
}
}; // namespace dap