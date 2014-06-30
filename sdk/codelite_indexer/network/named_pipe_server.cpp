//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : named_pipe_server.cpp
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

#include "named_pipe_server.h"
#ifndef __WXMSW__
# include <sys/types.h>
# include <sys/unistd.h>
# include <sys/socket.h>
#endif
clNamedPipeServer::clNamedPipeServer(const char* pipePath)
		: clNamedPipe(pipePath)
{

}


clNamedPipeServer::~clNamedPipeServer(void)
{
	disconnect();
}

void clNamedPipeServer::disconnect()
{
	this->setLastError(ZNP_OK);
#ifdef __WXMSW__
	if (_pipeHandle != INVALID_PIPE_HANDLE) {
		FlushFileBuffers(_pipeHandle);
		DisconnectNamedPipe(_pipeHandle);
		CloseHandle(_pipeHandle);
		_pipeHandle = INVALID_PIPE_HANDLE;
	}
#else
	if( _pipeHandle != INVALID_PIPE_HANDLE ) {
		close(_pipeHandle);
		shutdown(_pipeHandle, SHUT_RDWR);
		_pipeHandle = INVALID_PIPE_HANDLE;
	}
#endif
}
