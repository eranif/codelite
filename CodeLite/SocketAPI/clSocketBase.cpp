//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clSocketBase.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "clSocketBase.h"
#include <cerrno>
#include <cstdio>
#include <sstream>

#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

clSocketBase::clSocketBase(socket_t sockfd)
    : m_socket(sockfd)
    , m_closeOnExit(true)
{
}

clSocketBase::~clSocketBase() { DestroySocket(); }

void clSocketBase::Initialize()
{
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

// Read API
int clSocketBase::Read(wxMemoryBuffer& content, long timeout) 
{
    content.Clear();

    char buffer[4096];
    timeout = (timeout * 1000); // convert to MS
    while(true && timeout) {
        int rc = SelectReadMS(10);
        timeout -= 10;
        if(rc == kSuccess) {
            memset(buffer, 0x0, sizeof(buffer));
            int bytesRead = recv(m_socket, buffer, sizeof(buffer), 0);
            if(bytesRead < 0) {
                // Error
                throw clSocketException("Read failed: " + error());

            } else if(bytesRead == 0) {
                // connection closed
                return kError;

            } else {
                content.AppendData(buffer, bytesRead);
                continue;
            }
        } else {
            if(content.IsEmpty())
                continue; // keep waiting until time ends
            else
                return kSuccess; // we already read our content
        }
    }
    return kTimeout;
}

int clSocketBase::Read(wxString& content, const wxMBConv& conv, long timeout) 
{
    wxMemoryBuffer mb;
    int rc = Read(mb, timeout);
    if(rc == kSuccess) {
        content = wxString((const char*)mb.GetData(), conv, mb.GetDataLen());
    }
    return rc;
}

int clSocketBase::Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout) 
{
    if(SelectRead(timeout) == kTimeout) {
        return kTimeout;
    }
    memset(buffer, 0, bufferSize);
    bytesRead = recv(m_socket, buffer, bufferSize, 0);
    return kSuccess;
}

int clSocketBase::SelectRead(long seconds) 
{
    if(seconds == -1) {
        return kSuccess;
    }

    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }

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
        throw clSocketException("SelectRead failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}

// Send API
void clSocketBase::Send(const wxString& msg, const wxMBConv& conv) 
{
    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }
    wxCharBuffer cb = msg.mb_str(conv).data();
    wxMemoryBuffer mb;
    mb.AppendData(cb.data(), cb.length());
    Send(mb);
}

void clSocketBase::Send(const std::string& msg) 
{
    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }
    wxMemoryBuffer mb;
    mb.AppendData(msg.c_str(), msg.length());
    Send(mb);
}

void clSocketBase::Send(const wxMemoryBuffer& msg) 
{
    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }
    char* pdata = (char*)msg.GetData();
    int bytesLeft = msg.GetDataLen();
    while(bytesLeft) {
        if(SelectWriteMS(1000) == kTimeout) continue;
        int bytesSent = ::send(m_socket, (const char*)pdata, bytesLeft, 0);
        if(bytesSent <= 0) throw clSocketException("Send error: " + error());
        pdata += bytesSent;
        bytesLeft -= bytesSent;
    }
}

std::string clSocketBase::error() const
{
    std::string err;
#ifdef _WIN32
    // Get the error message, if any.
    DWORD errorMessageID = ::WSAGetLastError();
    if(errorMessageID == 0) return "No error message has been recorded";

    LPSTR messageBuffer = nullptr;
    size_t size =
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       errorMessageID,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPSTR)&messageBuffer,
                       0,
                       NULL);

    std::string message(messageBuffer, size);

    // Free the buffer.
    LocalFree(messageBuffer);
    err = message;
#else
    err = strerror(errno);
#endif
    return err;
}

void clSocketBase::DestroySocket()
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

int clSocketBase::ReadMessage(wxString& message, int timeout) 
{
    // send the length in string form to avoid binary / arch differences between remote and local machine
    char msglen[11];
    memset(msglen, 0, sizeof(msglen));

    size_t message_len(0);
    size_t bytesRead(0);
    int rc = Read((char*)msglen, sizeof(msglen) - 1, bytesRead, timeout);
    if(rc != kSuccess) {
        // timeout
        return rc;
    }

    // convert the string to int
    message_len = ::atoi(msglen);

    bytesRead = 0;
    char* buff = new char[message_len + 1];
    memset(buff, 0, message_len + 1);

    // read the entire amount we need
    int bytesLeft = message_len;
    int totalRead = 0;
    while(bytesLeft > 0) {
        rc = Read(buff + totalRead, bytesLeft, bytesRead, timeout);
        if(rc != kSuccess) {
            wxDELETEA(buff);
            return rc;

        } else if(rc == 0) {
            // session was closed
            wxDELETEA(buff);
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

void clSocketBase::WriteMessage(const wxString& message) 
{
    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }

    // Write the message length
    std::string c_str = message.mb_str(wxConvUTF8).data();
    int len = c_str.length();

    // send the length in string form to avoid binary / arch differences between remote and local machine
    char msglen[11];
    memset(msglen, 0, sizeof(msglen));
    sprintf(msglen, "%010d", len);
    ::send(m_socket, msglen, sizeof(msglen) - 1, 0); // send it without the NULL byte

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
    flags = ::fcntl(m_socket, F_GETFL);
    if(blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    ::fcntl(m_socket, F_SETFL, flags);
#else
    u_long iMode = blocking ? 0 : 1;
    ::ioctlsocket(m_socket, FIONBIO, &iMode);
#endif
}

int clSocketBase::SelectWriteMS(long milliSeconds) 
{
    if(milliSeconds == -1) {
        return kSuccess;
    }

    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }

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
        throw clSocketException("SelectWriteMS failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}

int clSocketBase::SelectWrite(long seconds) 
{
    if(seconds == -1) {
        return kSuccess;
    }

    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }

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
        throw clSocketException("SelectRead failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}

int clSocketBase::SelectReadMS(long milliSeconds) 
{
    if(milliSeconds == -1) {
        return kSuccess;
    }

    if(m_socket == INVALID_SOCKET) {
        throw clSocketException("Invalid socket!");
    }

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
        throw clSocketException("SelectRead failed: " + error());

    } else {
        // we got something to read
        return kSuccess;
    }
}
