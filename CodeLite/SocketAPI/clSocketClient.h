//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clSocketClient.h
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

#ifndef CLSOCKETCLIENT_H
#define CLSOCKETCLIENT_H

#include "clSocketBase.h"
#include <wx/string.h>

class WXDLLIMPEXP_CL clSocketClient : public clSocketBase
{
    wxString m_path;

public:
    clSocketClient();
    virtual ~clSocketClient();

    /**
     * @brief connect to a remote socket, using unix-domain socket
     */
    bool ConnectLocal(const wxString& socketPath);
    /**
     * @brief connect to a remote server using ip/port
     * when using non-blocking mode, wouldBlock will be set to true
     * incase the connect fails
     */
    bool ConnectRemote(const wxString& address, int port, bool& wouldBlock, bool nonBlockingMode = false);

    /**
     * @brief connect using connection string
     * @param connectionString in the format of unix:///path/to/unix/socket or tcp://127.0.0.1:1234
     * @return
     */
    bool Connect(const wxString& connectionString, bool nonBlockingMode = false);

    /**
     * @brief connect using non-blocking method
     */
    bool ConnectNonBlocking(const wxString& connectionString, bool& wouldBlock);
};

#endif // CLSOCKETCLIENT_H
