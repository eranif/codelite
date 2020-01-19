#include "ConnectionString.hpp"
#include "SocketClient.hpp"

#ifndef _WIN32
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define RESET_ERRNO() WSASetLastError(0)
#else
#define RESET_ERRNO() errno = 0
#endif
namespace dap
{

SocketClient::SocketClient() {}

SocketClient::~SocketClient() {}

bool SocketClient::ConnectRemote(const string& address, int port)
{
    DestroySocket();
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    const char* ip_addr = address.c_str();
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

#ifndef _WIN32
    if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
        // restore socket to blocking mode
        return false;
    }
#else
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
#endif

    RESET_ERRNO();
    int rc = ::connect(m_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    // restore socket to blocking mode
    return rc == 0;
}

bool SocketClient::Connect(const string& connectionString)
{
    ConnectionString cs(connectionString);
    if(!cs.IsOK()) { return false; }
    if(cs.GetProtocol() == ConnectionString::kUnixLocalSocket) {
        throw SocketException("Unsupported protocol");
    } else {
        // TCP
        return ConnectRemote(cs.GetHost(), cs.GetPort());
    }
}
}; // namespace dap