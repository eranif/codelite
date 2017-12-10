//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : np_connections_server.cpp
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

#include "np_connections_server.h"
#include "named_pipe_server.h"

#ifndef __WXMSW__
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/un.h>
#  include <stdio.h>
#else
# include <ctype.h>
# include <sys/time.h>
# include <fcntl.h>
#endif

#ifdef __WXMSW__
class HandleLockerServer
{
	HANDLE m_event;
public:
	HandleLockerServer(HANDLE event) : m_event(event) {}
	~HandleLockerServer() { 
		if(m_event != INVALID_PIPE_HANDLE) 
			CloseHandle(m_event); 
	}
};
#endif

#ifdef __WXMSW__
static PIPE_HANDLE createNamedPipe(const char* pipeName, SECURITY_ATTRIBUTES sa)
{
	return CreateNamedPipeA(	pipeName,
	                        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
	                        PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_READMODE_BYTE,
	                        PIPE_UNLIMITED_INSTANCES,
	                        8192, 8192, 0,
	                        &sa);
}
#endif

clNamedPipeConnectionsServer::clNamedPipeConnectionsServer(const char* pipeName)
		: _listenHandle(INVALID_PIPE_HANDLE)
{
	_pipePath = pipeName;
}

clNamedPipeConnectionsServer::~clNamedPipeConnectionsServer()
{
	_pipePath.clear();
	_listenHandle = INVALID_PIPE_HANDLE;
}

PIPE_HANDLE clNamedPipeConnectionsServer::initNewInstance()
{
#ifdef __WXMSW__
	SECURITY_ATTRIBUTES  sa = {0};
	memset(&sa, 0, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	HANDLE hPipe = createNamedPipe(_pipePath.c_str(), sa);
	if (!hPipe) {
		this->setLastError(NP_SERVER_UNKNOWN_ERROR);
		return INVALID_PIPE_HANDLE;
	}
	return hPipe;
#else
	if (_listenHandle == INVALID_PIPE_HANDLE) {

		unlink(_pipePath.c_str());
		struct sockaddr_un server;

		_listenHandle = socket(AF_UNIX, SOCK_STREAM, 0);
		if (_listenHandle < 0) {
			perror("ERROR: socket");
			return INVALID_PIPE_HANDLE;
		}

		server.sun_family = AF_UNIX;
		strcpy(server.sun_path, _pipePath.c_str());
		if (bind(_listenHandle, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
			perror("ERROR: binding stream socket");
			return INVALID_PIPE_HANDLE;
		}
	}
	listen(_listenHandle, 10);
	return _listenHandle;
#endif
}

bool clNamedPipeConnectionsServer::shutdown()
{
	_pipePath.clear();
#ifndef __WXMSW__
	close(_listenHandle);
#endif

	_listenHandle = INVALID_PIPE_HANDLE;
	return true;
}

clNamedPipe *clNamedPipeConnectionsServer::waitForNewConnection( int timeout )
{
	PIPE_HANDLE hConn = this->initNewInstance();

#ifdef __WXMSW__
	OVERLAPPED ov = {0};
	HANDLE ev = CreateEvent(NULL, TRUE, TRUE, NULL);
	ov.hEvent = ev;
	
	HandleLockerServer locker(ov.hEvent);
	
	bool fConnected = ConnectNamedPipe(hConn, &ov);
	if (fConnected != 0) {
		if(hConn != INVALID_PIPE_HANDLE) {
			CloseHandle(hConn);
		}
		this->setLastError(NP_SERVER_UNKNOWN_ERROR);
		return NULL;
	}

	switch (GetLastError()) {

		// The overlapped connection in progress.
	case ERROR_IO_PENDING: {
		DWORD res = WaitForSingleObject(ov.hEvent, timeout) ;
		switch (res) {
		case WAIT_OBJECT_0 : {
			clNamedPipeServer *conn = new clNamedPipeServer(_pipePath.c_str());
			conn->setHandle(hConn);
			return conn;
		}
		case WAIT_TIMEOUT : {
			if ( hConn != INVALID_PIPE_HANDLE ) {
				CloseHandle( hConn );
			}
			this->setLastError(NP_SERVER_TIMEOUT);
			return NULL;
		}
		default: {
			if ( hConn != INVALID_PIPE_HANDLE ) {
				CloseHandle( hConn );
			}

			this->setLastError(NP_SERVER_UNKNOWN_ERROR);
			return NULL;
		}

		}
	}

	case ERROR_PIPE_CONNECTED: {
		clNamedPipeServer *conn = new clNamedPipeServer(_pipePath.c_str());
		conn->setHandle(hConn);
		return conn;
	}
	// If an error occurs during the connect operation...
	default: {
		if(hConn != INVALID_PIPE_HANDLE) {
			CloseHandle(hConn);
		}

		this->setLastError(NP_SERVER_UNKNOWN_ERROR);
		return NULL;
	}
	}
#else
	// accept new connection
	if (hConn != INVALID_PIPE_HANDLE) {

		if ( timeout > 0 ) {
			fd_set fds;
			struct timeval tv;
			memset( (void*)&fds, 0, sizeof( fds ) );
			FD_SET( hConn, &fds );
			tv.tv_sec = 0;
			tv.tv_usec = timeout * 1000; // convert mili to micro

			int rc = select(hConn + 1, &fds, 0, 0, &tv);
			if ( rc == 0 || rc < 0 ) {
				// timeout or error
				setLastError(NP_SERVER_TIMEOUT);
				return NULL;
			}
		}

		PIPE_HANDLE fd = ::accept(hConn, 0, 0);
		if (fd > 0) {
			clNamedPipeServer *conn = new clNamedPipeServer(_pipePath.c_str());
			conn->setHandle(fd);
			return conn;
		} else {
			perror("ERROR: accept");
			return NULL;
		}

	}
	return NULL;
#endif
}
