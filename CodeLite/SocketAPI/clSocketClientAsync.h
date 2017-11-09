//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clSocketClientAsync.h
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

#ifndef CLSOCKETCLIENTASYNC_H
#define CLSOCKETCLIENTASYNC_H

#include "SocketAPI/clSocketClient.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "worker_thread.h"
#include <wx/event.h>
#include <wx/msgqueue.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_CONNECTED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_CONNECTION_LOST, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_INPUT, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_CONNECT_ERROR, clCommandEvent);

class WXDLLIMPEXP_CL clSocketClientAsyncHelperThread : public wxThread
{
public:
    enum eCommand {
        kSend,
        kDisconnect,
    };

    struct MyRequest {
        eCommand m_command;
        wxString m_buffer;
    };

protected:
    wxEvtHandler* m_sink;
    wxString m_keepAliveMessage;
    wxString m_connectionString;
    wxMessageQueue<MyRequest> m_queue;
    bool m_nonBlockingMode;

public:
    virtual void AddRequest(const MyRequest& req) { m_queue.Post(req); }
    virtual void* Entry();

    /**
     * @brief start the worker thread
     */
    void Start()
    {
        Create();
        Run();
    }

    /**
     * @brief stop and wait for the thread to terminate
     */
    void Stop()
    {
        if(IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);

        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }

    clSocketClientAsyncHelperThread(wxEvtHandler* sink, const wxString& connectionString, bool nonBlockingMode,
                                    const wxString& keepAliveMessage = "");
    virtual ~clSocketClientAsyncHelperThread();
};

class WXDLLIMPEXP_CL clSocketClientAsync
{
    wxEvtHandler* m_owner;
    clSocketClientAsyncHelperThread* m_thread;

public:
    clSocketClientAsync(wxEvtHandler* owner);
    ~clSocketClientAsync();

    /**
     * @brief connect using connection string
     */
    void Connect(const wxString& connectionString, const wxString& keepAliveMessage = "");
    void ConnectNonBlocking(const wxString& connectionString, const wxString& keepAliveMessage = "");
    void Send(const wxString& buffer);
    void Disconnect();
};

#endif // CLSOCKETCLIENTASYNC_H
