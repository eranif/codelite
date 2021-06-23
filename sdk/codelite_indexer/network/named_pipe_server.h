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

#include "wx/defs.h"
#include <stdlib.h>
#ifdef __WINDOWS__ // __WINDOWS__ defined in wx/defs.h
// includes windows.h and if wxUSE_WINSOCK2 is true includes winsock2.h
#include "wx/msw/wrapwin.h"
#else
#include <unistd.h>
#endif

#include "named_pipe.h"

class clNamedPipeServer : public clNamedPipe
{
public:
    clNamedPipeServer(const char* pipePath);
    virtual ~clNamedPipeServer(void);
    void setHandle(PIPE_HANDLE handle) { this->setPipeHandle(handle); }
    virtual void disconnect();
};

#endif // named_pipe_server_h__
