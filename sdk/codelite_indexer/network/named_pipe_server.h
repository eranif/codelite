//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : named_pipe_server.h
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

#ifndef named_pipe_server_h__
#define named_pipe_server_h__

#include <stdlib.h>
#ifdef __WXMSW__
#include <Windows.h>
#ifndef _WIN64
#   include <WinSock2.h>
#endif
#else
#include <unistd.h>
#endif

#include "named_pipe.h"

class clNamedPipeServer : public clNamedPipe
{
public:
	clNamedPipeServer(const char* pipePath);
	virtual ~clNamedPipeServer(void);
	void setHandle(PIPE_HANDLE handle) { this->setPipeHandle(handle) ; }
	virtual void disconnect();
};

#endif // named_pipe_server_h__
