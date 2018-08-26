//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugCommThread.h
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

#ifndef XDEBUG_COMM_THREAD_H
#define XDEBUG_COMM_THREAD_H

#include <wx/thread.h>
#include <wx/msgqueue.h>
#include <wx/string.h>
#include <string>
#include "SocketAPI/clSocketBase.h"
#include "SocketAPI/clSocketServer.h"

#if defined(__WXMSW__)
#include <wx/msw/winundef.h>
#endif

class XDebugManager;
class XDebugComThread : public wxThread
{
    XDebugManager* m_xdebugMgr;
    wxMessageQueue<wxString> m_queue;
    int m_port;
    clSocketServer m_server;
    wxString m_host;
    int m_waitForConnTimeout;
    
protected:
    void DoSendCommand(const wxString& command, clSocketBase::Ptr_t client);
    bool DoReadReply(std::string& reply, clSocketBase::Ptr_t client);
    void Stop();

public:
    XDebugComThread(XDebugManager* xdebugMgr, int port, const wxString& host, int waitForConnTimeout = 5)
        : wxThread(wxTHREAD_JOINABLE)
        , m_xdebugMgr(xdebugMgr)
        , m_port(port)
        , m_host(host)
        , m_waitForConnTimeout(waitForConnTimeout)
    {
    }

    virtual ~XDebugComThread();

    void SendMsg(const wxString& msg);

    void* Entry();
    void Start()
    {
        Create();
        Run();
    }
};

#endif // XDEBUG_COMM_THREAD_H
