//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : clSocketServer.h
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

#ifndef CLSOCKETSERVER_H
#define CLSOCKETSERVER_H

#include "clSocketBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clSocketServer : public clSocketBase
{
public:
    clSocketServer();
    virtual ~clSocketServer();

    /**
     * @brief On Windows, the 'pipePath' is a string containing the port number.
     * LocalSocket is implemented under Windows with TCP/IP with IP always set to 127.0.0.1
     * @param pipePath
     */
    void CreateServer(const std::string &pipePath) throw (clSocketException);
    void CreateServer(const std::string &address, int port) throw (clSocketException);
    clSocketBase::Ptr_t WaitForNewConnection( long timeout = -1 ) throw (clSocketException);
};

#endif // CLSOCKETSERVER_H
