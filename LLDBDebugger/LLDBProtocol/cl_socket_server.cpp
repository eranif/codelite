#include "cl_socket_server.h"

#ifndef _WIN32
#   include <unistd.h>
#include <sys/stat.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <sys/un.h>
#   include <stdio.h>
#endif

clSocketServer::clSocketServer()
{
}

clSocketServer::~clSocketServer()
{
}

#ifndef __WXMSW__
void clSocketServer::CreateServer(const std::string &pipPath) throw (clSocketException)
{
    unlink(pipPath.c_str());

    // Create a socket
    if((m_socket = ::socket(AF_UNIX , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        throw clSocketException( "Could not create socket: " + error() );
    }
    
    // must set reuse-address
    int optval;
    
    // set SO_REUSEADDR on a socket to true (1):
    optval = 1;
    ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

    // Prepare the sockaddr_in structure
    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, pipPath.c_str());
    
    // Bind
    if( ::bind(m_socket ,(struct sockaddr *)&server , sizeof(server)) == -1) {
        throw clSocketException( "CreateServer: bind operation failed: " + error() );
    }
    
    char mode[] = "0777";
    int newMode = ::strtol(mode, 0, 8);
    ::chmod(pipPath.c_str(), newMode);
    
    // define the accept queue size
    ::listen(m_socket, 10);
}
#endif

clSocketBase::Ptr_t clSocketServer::WaitForNewConnection(long timeout) throw (clSocketException)
{
    if ( SelectRead( timeout ) == kTimeout ) {
        return clSocketBase::Ptr_t( NULL );
    }
    
    int fd = ::accept(m_socket , 0, 0);
    if ( fd < 0 ) {
        throw clSocketException( "accept error: " + error() );
    }
    return clSocketBase::Ptr_t(new clSocketBase(fd));
}

void clSocketServer::CreateServer(const std::string &address, int port) throw (clSocketException)
{
    // Create a socket
    if( (m_socket = ::socket(AF_INET , SOCK_STREAM , 0)) == INVALID_SOCKET) {
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
    server.sin_addr.s_addr = inet_addr( address.c_str() );
    server.sin_port = htons( port );

    // Bind
    if( ::bind(m_socket ,(struct sockaddr *)&server , sizeof(server)) == -1) {
        throw clSocketException( "CreateServer: bind operation failed: " + error() );
    }
    // define the accept queue size
    ::listen(m_socket, 10);
}
