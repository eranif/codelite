#include "cl_socket_server.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#endif

clSocketServer::clSocketServer()
{
}

clSocketServer::~clSocketServer()
{
}

void clSocketServer::CreateServer(const std::string& host, int port) throw (clSocketException)
{
    // Create a socket
    if((m_socket = ::socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        throw clSocketException( "Could not create socket: " + error() );
    }
    
    // must set reuse-address
    int optval;
    
    // set SO_REUSEADDR on a socket to true (1):
    optval = 1;
    ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

    // Prepare the sockaddr_in structure
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );

    // Bind
    if( ::bind(m_socket ,(struct sockaddr *)&server , sizeof(server)) == -1) {
        throw clSocketException( "CreateServer: bind operation failed: " + error() );
    }
    
    // define the accept queue size
    ::listen(m_socket, 10);
}

clSocketBase::Ptr_t clSocketServer::WaitForNewConnection(long timeout) throw (clSocketException)
{
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if ( SelectRead( timeout ) == kTimeout ) {
        return clSocketBase::Ptr_t( NULL );
    }
    int fd = ::accept(m_socket , (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if ( fd < 0 ) {
        throw clSocketException( "accept error: " + error() );
    }
    return clSocketBase::Ptr_t(new clSocketBase(fd));
}
