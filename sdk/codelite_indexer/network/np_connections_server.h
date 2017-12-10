//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : np_connections_server.h
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

#ifndef np_connections_server_h__
#define np_connections_server_h__

#include "named_pipe.h"
#include <string>

class clNamedPipeConnectionsServer
{
public:
	enum NP_SERVER_ERRORS {
		NP_SERVER_TIMEOUT = 1,
		NP_SERVER_UNKNOWN_ERROR
	};

	clNamedPipeConnectionsServer(const char* pipeName);
	virtual ~clNamedPipeConnectionsServer();
	bool shutdown();
	clNamedPipe *waitForNewConnection(int timeout);
	NP_SERVER_ERRORS getLastError() { return this->_lastError ; }

protected:
	void setLastError(NP_SERVER_ERRORS error) { this->_lastError = error; }

private:
	PIPE_HANDLE initNewInstance();
	NP_SERVER_ERRORS _lastError;
	std::string _pipePath;
	PIPE_HANDLE _listenHandle;
};

#endif // np_connections_server_h__


