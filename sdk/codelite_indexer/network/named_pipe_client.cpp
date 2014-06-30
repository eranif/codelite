//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : named_pipe_client.cpp
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

#include "named_pipe_client.h"

#ifndef __WXMSW__
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/un.h>
# include <stdio.h>
#endif

clNamedPipeClient::clNamedPipeClient(const char* path)
		: clNamedPipe(path)
{
}


clNamedPipeClient::~clNamedPipeClient(void)
{
	disconnect();
}


bool clNamedPipeClient::connect(long )
{
#ifdef __WXMSW__
	this->setLastError(ZNP_OK);

	while (1) {
		_pipeHandle = CreateFileA(
		                  getPipePath(),        // pipe name
		                  GENERIC_READ |        // read and write access
		                  GENERIC_WRITE,
		                  0,                    // no sharing
		                  NULL,                 // default security attributes
		                  OPEN_EXISTING,        // opens existing pipe
		                  FILE_FLAG_OVERLAPPED, // default attributes
		                  NULL);                // no template file

		if (_pipeHandle != INVALID_PIPE_HANDLE)
			return true;

		// Exit if an error other than ERROR_PIPE_BUSY occurs.
		DWORD err = GetLastError();
		if (err != ERROR_PIPE_BUSY) {
			this->setLastError(ZNP_CONNECT_ERROR);
			return false;
		}

		// All pipe instances are busy, so wait for 20 seconds.

		if (!WaitNamedPipeA(getPipePath(), 20000)) {
			DWORD err = GetLastError();
			if (err == WAIT_TIMEOUT) {
				this->setLastError(ZNP_TIMEOUT);
			} else {
				this->setLastError(ZNP_CONNECT_WAIT_ERROR);
			}
			return false;
		}
	}
#else // MSW
	struct sockaddr_un server;

	_pipeHandle = socket(AF_UNIX, SOCK_STREAM, 0);
	if (_pipeHandle < 0) {
		perror("ERROR");
		return false;
	}

	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, getPipePath());

	if (::connect(_pipeHandle, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		perror("ERROR");
		disconnect();
		return false;
	}
	return true;
#endif
}

bool clNamedPipeClient::isConnected()
{
	return (_pipeHandle != INVALID_PIPE_HANDLE);
}


void clNamedPipeClient::disconnect()
{
#ifdef __WXMSW__
	this->setLastError(ZNP_OK);

	if (_pipeHandle != INVALID_PIPE_HANDLE) {
		CloseHandle(_pipeHandle);
		_pipeHandle = INVALID_PIPE_HANDLE;
	}
#else
	if ( _pipeHandle != INVALID_PIPE_HANDLE ) {
		close(_pipeHandle);
		shutdown(_pipeHandle, SHUT_RDWR);
		_pipeHandle = INVALID_PIPE_HANDLE;
	}
#endif
}
