//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clSocketClient.cpp
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

#include "clConnectionString.h"
#include "clSocketClient.h"

#ifndef _WIN32
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#ifdef __WXMSW__
#define RESET_ERRNO() WSASetLastError(0)
#else
#define RESET_ERRNO() errno = 0
#endif

clSocketClient::clSocketClient() {}

clSocketClient::~clSocketClient() {}

bool clSocketClient::ConnectLocal(const wxString& socketPath)
{
    DestroySocket();
#ifndef __WXMSW__
    struct sockaddr_un server;
    m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, socketPath.mb_str(wxConvUTF8).data());
    if(::connect(m_socket, (struct sockaddr*)&server, sizeof(struct sockaddr_un)) < 0) { return false; }
    return true;
#else
    return false;
#endif
}

bool clSocketClient::ConnectRemote(const wxString& address, int port, bool& wouldBlock, bool nonBlockingMode)
{
    wouldBlock = false;
    DestroySocket();
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if(nonBlockingMode) { MakeSocketBlocking(false); }

    const char* ip_addr = address.mb_str(wxConvUTF8).data();
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

#ifndef __WXMSW__
    if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
        // restore socket to blocking mode
        if(nonBlockingMode) { MakeSocketBlocking(true); }
        return false;
    }
#else
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
#endif

    RESET_ERRNO();
    int rc = ::connect(m_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
#ifdef __WXMSW__
    wouldBlock = (WSAGetLastError() == WSAEWOULDBLOCK);
#else
    wouldBlock = (errno == EINPROGRESS);
#endif

    // restore socket to blocking mode
    if(nonBlockingMode) { MakeSocketBlocking(true); }
    return rc == 0;
}

bool clSocketClient::Connect(const wxString& connectionString, bool nonBlockingMode)
{
    clConnectionString cs(connectionString);
    if(!cs.IsOK()) { return false; }
    if(cs.GetProtocol() == clConnectionString::kUnixLocalSocket) {
        return ConnectLocal(cs.GetPath());
    } else {
        // TCP
        bool wouldBlock = false;
        return ConnectRemote(cs.GetHost(), cs.GetPort(), wouldBlock, nonBlockingMode);
    }
}

bool clSocketClient::ConnectNonBlocking(const wxString& connectionString, bool& wouldBlock)
{
    wouldBlock = false;
    clConnectionString cs(connectionString);
    if(!cs.IsOK()) { return false; }
    if(cs.GetProtocol() == clConnectionString::kUnixLocalSocket) {
        return ConnectLocal(cs.GetPath());
    } else {
        // TCP
        return ConnectRemote(cs.GetHost(), cs.GetPort(), wouldBlock, true);
    }
}
