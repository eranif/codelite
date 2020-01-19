#include "SocketBase.hpp"
#include <cerrno>
#include <cstdio>
#include <memory>
#include <sstream>

#ifndef _WIN32
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif
namespace dap
{
SocketBase::SocketBase(socket_t sockfd)
    : m_socket(sockfd)
    , m_closeOnExit(true)
{
}

SocketBase::~SocketBase() { DestroySocket(); }

void SocketBase::Initialize()
{
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

int SocketBase::Read(string& content, long timeout)
{
    char buffer[4096];
    size_t bytesRead = 0;
    int rc = Read(buffer, sizeof(buffer) - 1, bytesRead, timeout);
    if(rc == kSuccess) {
        buffer[bytesRead] = 0;
        content = buffer;
    }
    return rc;
}

int SocketBase::Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout)
{
    if(SelectRead(timeout) == kTimeout) { return kTimeout; }
    memset(buffer, 0, bufferSize);
    const int res = recv(m_socket, buffer, bufferSize, 0);

    if(res < 0) {
        const int err = GetLastError();
        if(eWouldBlock == err) { return kTimeout; }

        throw SocketException("Read failed: " + error(err));
    } else if(0 == res) {
        throw SocketException("Read failed: " + error());
    }

    bytesRead = static_cast<size_t>(res);
    return kSuccess;
}

int SocketBase::SelectRead(long seconds)
{
    if(seconds == -1) { return kSuccess; }

    if(m_socket == INVALID_SOCKET) { throw SocketException("Invalid socket!"); }

    struct timeval tv = { seconds, 0 };

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_socket, &readfds);
    int rc = select(m_socket + 1, &readfds, NULL, NULL, &tv);
    if(rc == 0) {
        // timeout
        return kTimeout;

    } else if(rc < 0) {
        // an error occurred
        throw SocketException("SelectRead failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}

// Send API
void SocketBase::Send(const string& msg)
{
    if(m_socket == INVALID_SOCKET) { throw SocketException("Invalid socket!"); }
    if(msg.empty()) { return; }
    char* pdata = (char*)msg.c_str();
    int bytesLeft = msg.length();
    while(bytesLeft) {
        if(SelectWriteMS(1000) == kTimeout) continue;
        int bytesSent = ::send(m_socket, (const char*)pdata, bytesLeft, 0);
        if(bytesSent <= 0) throw SocketException("Send error: " + error());
        pdata += bytesSent;
        bytesLeft -= bytesSent;
    }
}

int SocketBase::GetLastError()
{
#ifdef _WIN32
    return ::WSAGetLastError();
#else
    return errno;
#endif
}

std::string SocketBase::error() { return error(GetLastError()); }

std::string SocketBase::error(const int errorCode)
{
    std::string err;
#ifdef _WIN32
    // Get the error message, if any.
    if(errorCode == 0) return "No error message has been recorded";

    LPSTR messageBuffer = nullptr;
    size_t size =
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    // Free the buffer.
    LocalFree(messageBuffer);
    err = message;
#else
    err = strerror(errorCode);
#endif
    return err;
}

void SocketBase::DestroySocket()
{
    if(IsCloseOnExit()) {
        if(m_socket != INVALID_SOCKET) {
#ifdef _WIN32
            ::shutdown(m_socket, 2);
            ::closesocket(m_socket);
#else
            ::shutdown(m_socket, 2);
            ::close(m_socket);
#endif
        }
    }
    m_socket = INVALID_SOCKET;
}

socket_t SocketBase::Release()
{
    int fd = m_socket;
    m_socket = INVALID_SOCKET;
    return fd;
}

void SocketBase::MakeSocketBlocking(bool blocking)
{
#ifndef _WIN32
    // set socket to non-blocking mode
    int flags;
    flags = ::fcntl(m_socket, F_GETFL);
    if(blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    ::fcntl(m_socket, F_SETFL, flags);
#else
    u_long iMode = blocking ? 0 : 1;
    ::ioctlsocket(m_socket, FIONBIO, &iMode);
#endif
}

int SocketBase::SelectWriteMS(long milliSeconds)
{
    if(milliSeconds == -1) { return kSuccess; }

    if(m_socket == INVALID_SOCKET) { throw SocketException("Invalid socket!"); }

    struct timeval tv;
    tv.tv_sec = milliSeconds / 1000;
    tv.tv_usec = (milliSeconds % 1000) * 1000;

    fd_set write_set;
    FD_ZERO(&write_set);
    FD_SET(m_socket, &write_set);
    errno = 0;
    int rc = select(m_socket + 1, NULL, &write_set, NULL, &tv);
    if(rc == 0) {
        // timeout
        return kTimeout;

    } else if(rc < 0) {
        // an error occurred
        throw SocketException("SelectWriteMS failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}

int SocketBase::SelectWrite(long seconds)
{
    if(seconds == -1) { return kSuccess; }

    if(m_socket == INVALID_SOCKET) { throw SocketException("Invalid socket!"); }

    struct timeval tv = { seconds, 0 };

    fd_set write_set;
    FD_ZERO(&write_set);
    FD_SET(m_socket, &write_set);
    errno = 0;
    int rc = select(m_socket + 1, NULL, &write_set, NULL, &tv);
    if(rc == 0) {
        // timeout
        return kTimeout;

    } else if(rc < 0) {
        // an error occurred
        throw SocketException("SelectRead failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}

int SocketBase::SelectReadMS(long milliSeconds)
{
    if(milliSeconds == -1) { return kSuccess; }

    if(m_socket == INVALID_SOCKET) { throw SocketException("Invalid socket!"); }

    int seconds = milliSeconds / 1000; // convert the number into seconds
    int ms = milliSeconds % 1000;      // the remainder is less than a second
    struct timeval tv = { seconds, ms * 1000 };

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_socket, &readfds);
    int rc = select(m_socket + 1, &readfds, NULL, NULL, &tv);
    if(rc == 0) {
        // timeout
        return kTimeout;

    } else if(rc < 0) {
        // an error occurred
        throw SocketException("SelectRead failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}
}; // namespace dap