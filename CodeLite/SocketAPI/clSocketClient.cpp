#include "clSocketClient.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#endif

static void MakeSocketBlocking(int sock, bool blocking) {
#ifndef __WXMSW__
    // set socket to non-blocking mode
    int flags;
    flags = ::fcntl( sock, F_GETFL );
    if ( blocking ) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    ::fcntl( sock, F_SETFL, flags );
#else
    u_long iMode = blocking ? 1 : 0;
    ::ioctlsocket(sock, FIONBIO, &iMode);
#endif

}

clSocketClient::clSocketClient()
{
}

clSocketClient::~clSocketClient()
{
}

bool clSocketClient::ConnectLocal(const wxString& socketPath, int num_secs)
{
    // Since we are about to allocate new socket, make sure
    // we close any previous connections
    Destroy();
    
#ifndef __WXMSW__
    struct sockaddr_un server;
    m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, socketPath.mb_str(wxConvUTF8).data());
    
    MakeSocketBlocking( m_socket, false );
    ::connect(m_socket, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
    return WaitForConnect(num_secs);
    
#else
    return false;
#endif
}

bool clSocketClient::ConnectRemote(const wxString& address, int port, int num_secs)
{
    // Since we are about to allocate new socket, make sure
    // we close any previous connections
    Destroy();
    
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    const char* ip_addr = address.mb_str(wxConvUTF8).data();
    struct sockaddr_in serv_addr; 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
#ifndef __WXMSW__
    if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
        return false;
    }
#else
    serv_addr.sin_addr.s_addr = inet_addr( ip_addr );
#endif
    
    // Make the socket non-blocking
    MakeSocketBlocking( m_socket, false );
    
    // This will fail immediately
    ::connect(m_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    return WaitForConnect(num_secs);
}

bool clSocketClient::WaitForConnect(int num_sesc)
{
    // wait for connect using select() call
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);
    
    timeval tv;
    tv.tv_sec = num_sesc;
    tv.tv_usec = 0;
    int ret = ::select(m_socket+1, NULL, &fds, NULL, &tv);
    if ( ret == 0 || ret < 0 ) {
        MakeSocketBlocking(m_socket, true);
        return true;
    }
    Destroy();
    return false;
}
