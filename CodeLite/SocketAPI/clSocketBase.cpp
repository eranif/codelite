#include "clSocketBase.h"
#include <cerrno>
#include <cstdio>
#include <sstream>

#ifndef _WIN32
#include <fcntl.h>
#   include <unistd.h>
#   include <sys/select.h>
#   include <string.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#endif

clSocketBase::clSocketBase(int sockfd)
    : m_socket(sockfd)
    , m_closeOnExit(true)
{
}

clSocketBase::~clSocketBase()
{
    DestroySocket();
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

void clSocketBase::DestroySocket()
{
    if (IsCloseOnExit()) {
        if ( m_socket != INVALID_SOCKET ) {
#ifdef _WIN32
        ::shutdown(m_socket, 2);
        ::closesocket( m_socket );
#else
        ::shutdown(m_socket, 2);
        ::close(m_socket);
#endif
        }
    }
    m_socket = INVALID_SOCKET;
}

int clSocketBase::ReadMessage(wxString& message, int timeout) throw (clSocketException)
{
    // send the length in string form to avoid binary / arch differences between remote and local machine
    char msglen[11];
    memset(msglen, 0, sizeof(msglen));

    size_t message_len(0);
    size_t bytesRead(0);
    int rc = Read( (char*)msglen, sizeof(msglen)-1, bytesRead, timeout);
    if ( rc != kSuccess ) {
        // timeout
        return rc;
    }
    
    // convert the string to int
    message_len = ::atoi(msglen);
    
    bytesRead = 0;
    char *buff = new char[message_len+1];
    memset(buff, 0, message_len+1);
    
    // read the entire amount we need
    int bytesLeft = message_len;
    int totalRead = 0;
    while ( bytesLeft > 0 ) {
        rc = Read(buff + totalRead, bytesLeft, bytesRead, timeout);
        if ( rc != kSuccess ) {
            wxDELETEA( buff );
            return rc;

        } else if ( rc == 0 ) {
            // session was closed
            wxDELETEA( buff );
            throw clSocketException("connection closed by peer");

        } else {
            bytesLeft -= bytesRead;
            totalRead += bytesRead;
            bytesRead = 0;
        }
    }
    
    buff[message_len] = '\0';
    message = buff;
    return kSuccess;
}

void clSocketBase::WriteMessage(const wxString& message) throw (clSocketException)
{
    if ( m_socket == INVALID_SOCKET ) {
        throw clSocketException("Invalid socket!");
    }

    // Write the message length
    std::string c_str = message.mb_str(wxConvUTF8).data();
    int len = c_str.length();
    
    // send the length in string form to avoid binary / arch differences between remote and local machine
    char msglen[11];
    memset(msglen, 0, sizeof(msglen));
    sprintf(msglen, "%010d", len);
    ::send(m_socket, msglen, sizeof(msglen)-1, 0); // send it without the NULL byte

    // now send the actual data
    Send(c_str);
}

socket_t clSocketBase::Release()
{
    int fd = m_socket;
    m_socket = INVALID_SOCKET;
    return fd;
}

void clSocketBase::MakeSocketBlocking(bool blocking)
{
#ifndef __WXMSW__
    // set socket to non-blocking mode
    int flags;
    flags = ::fcntl( m_socket, F_GETFL );
    if ( blocking ) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    ::fcntl( m_socket, F_SETFL, flags );
#else
    u_long iMode = blocking ? 0 : 1;
    ::ioctlsocket(m_socket, FIONBIO, &iMode);
#endif
}

int clSocketBase::SelectWrite(long seconds) throw (clSocketException)
{
    if ( seconds == -1 ) {
        return kSuccess;
    }

    if ( m_socket == INVALID_SOCKET ) {
        throw clSocketException("Invalid socket!");
    }

    struct timeval tv = {seconds, 0};

    fd_set write_set;
    FD_ZERO(&write_set);
    FD_SET(m_socket, &write_set);
    errno = 0;
    int rc = select(m_socket+1, NULL, &write_set, NULL, &tv);
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
