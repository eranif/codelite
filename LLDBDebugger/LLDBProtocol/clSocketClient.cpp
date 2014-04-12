#include "clSocketClient.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#endif

clSocketClient::clSocketClient(const wxString& pipePath)
    : m_path(pipePath)
{
}

clSocketClient::~clSocketClient()
{
}

bool clSocketClient::Connect()
{
    // TCP / IP implementation
    // ============================================================
    // m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    // const char* ip_addr = m_ip.mb_str(wxConvUTF8).data();
    // struct sockaddr_in serv_addr; 
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(m_port);
    // 
    // if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
    //     return false;
    // }
    // 
    // int rc = ::connect(m_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    // if ( rc != 0 ) {
    // }
    // return rc == 0;
    
    struct sockaddr_un server;
    m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, m_path.mb_str(wxConvUTF8).data());
    if (::connect(m_socket, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
        return false;
    }
    return true;
}
