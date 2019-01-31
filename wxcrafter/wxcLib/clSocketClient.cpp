#include "wxcLib/clSocketClient.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#endif

clSocketClient::clSocketClient()
{
}

clSocketClient::~clSocketClient()
{
}

bool clSocketClient::ConnectLocal(const wxString &socketPath)
{
    DestroySocket();
#ifndef __WXMSW__
    struct sockaddr_un server;
    m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, socketPath.mb_str(wxConvUTF8).data());
    if (::connect(m_socket, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
        return false;
    }
    return true;
#else
    return false;
#endif
}

bool clSocketClient::ConnectRemote(const wxString& address, int port)
{
    wxCharBuffer cb = address.mb_str(wxConvUTF8);
    return ConnectRemote(std::string(cb.data()), port);
}

bool clSocketClient::ConnectRemote(const std::string& address, int port)
{
    DestroySocket();
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    const char* ip_addr = address.c_str();
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

    int rc = ::connect(m_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    return rc == 0;
}
