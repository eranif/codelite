//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : named_pipe_client.h
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

#ifndef named_pipe_client_h__
#define named_pipe_client_h__

#include "named_pipe.h"

class clNamedPipeClient : public clNamedPipe
{
public:


	clNamedPipeClient(const char* path);
	virtual ~clNamedPipeClient(void);
	virtual bool connect(long timeToConnect = -1);
	virtual void disconnect();
	virtual bool isConnected();

};

#endif // named_pipe_client_h__
