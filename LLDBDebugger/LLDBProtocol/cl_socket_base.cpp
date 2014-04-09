#include "cl_socket_base.h"
#include <cerrno>
#include <cstdio>

#ifndef _WIN32
#   include <unistd.h>
#   include <sys/select.h>
#   include <string.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#endif

clSocketBase::clSocketBase(int sockfd)
    : m_socket(sockfd)
{
}

clSocketBase::~clSocketBase()
{
    CloseSocket();
}

void clSocketBase::Initialize()
{
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

int clSocketBase::Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout) throw (clSocketException)
{
    if ( SelectRead(timeout) == kTimeout ) {
        return kTimeout;
    }
    memset(buffer, 0, bufferSize);
    bytesRead = recv(m_socket, buffer, bufferSize, 0);
    return kSuccess;
}

int clSocketBase::SelectRead(long seconds) throw (clSocketException)
{
    if ( seconds == -1 ) {
        return kSuccess;
    }
    
    if ( m_socket == INVALID_SOCKET ) {
        throw clSocketException("Invalid socket!");
    }
    
    struct timeval tv = {seconds, 0};
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_socket, &readfds);
    int rc = select(m_socket+1, &readfds, NULL, NULL, &tv);
    if ( rc == 0 ) {
        // timeout
        return kTimeout;
        
    } else if ( rc < 0 ) {
        // an error occured
        throw clSocketException( "SelectRead failed: " + error() );
        
    } else {
        // we got something to read
        return kSuccess;
    }
}

void clSocketBase::Send(const std::string& msg) throw (clSocketException)
{
    if ( m_socket == INVALID_SOCKET ) {
        throw clSocketException("Invalid socket!");
    }
    ::send(m_socket, msg.c_str(), msg.length(), 0);
}

std::string clSocketBase::error() const
{
    std::string err;
#ifdef _WIN32
    char _buf[256];
    memset(_buf, 0, sizeof(_buf));
    sprintf(_buf, "WSAGetLastError returned: %d", WSAGetLastError());
    err = _buf;
#else
    err = strerror(errno);
#endif
    return err;
}

void clSocketBase::CloseSocket()
{
    if ( m_socket != INVALID_SOCKET ) {
#ifdef _WIN32
        ::closesocket( m_socket );
#else
        ::close(m_socket);
#endif
        ::shutdown(m_socket, 2);
    }
    
    m_socket = INVALID_SOCKET;
}
