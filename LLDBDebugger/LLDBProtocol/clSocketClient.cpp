#include "clSocketClient.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#endif

clSocketClient::clSocketClient(const wxString& ip, int port)
    : m_ip(ip)
    , m_port(port)
{
}

clSocketClient::~clSocketClient()
{
}

bool clSocketClient::Connect()
{
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    const char* ip_addr = m_ip.mb_str(wxConvUTF8).data();
    struct sockaddr_in serv_addr; 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(m_port);
    
    if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
//        perror("inet_pton");
        return false;
    }
    
    int rc = ::connect(m_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if ( rc != 0 ) {
//        perror("connect");
    }
    return rc == 0;
}
