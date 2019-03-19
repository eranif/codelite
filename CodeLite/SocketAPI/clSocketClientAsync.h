//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clAsyncSocket.h
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

#include "SocketAPI/clSocketBase.h"
#include "SocketAPI/clSocketClient.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "worker_thread.h"
#include <wx/event.h>
#include <wx/msgqueue.h>
#include <wx/sharedptr.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_CONNECTED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_CONNECTION_LOST, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_INPUT, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_CONNECT_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_SOCKET_SERVER_READY, clCommandEvent);

enum eAsyncSocketMode {
    kAsyncSocketClient = (1 << 0),
    kAsyncSocketServer = (1 << 1),
    kAsyncSocketMessage = (1 << 2),
    kAsyncSocketBuffer = (1 << 3),
    kAsyncSocketNonBlocking = (1 << 4),
};

class WXDLLIMPEXP_CL clSocketAsyncThread : public wxThread
{
public:
    enum eCommand {
        kSend,
        kDisconnect,
    };

    struct MyRequest {
        eCommand m_command;
        std::string m_buffer;
    };

protected:
    wxEvtHandler* m_sink;
    wxString m_keepAliveMessage;
    wxString m_connectionString;
    wxMessageQueue<MyRequest> m_queue;
    size_t m_mode = (kAsyncSocketClient | kAsyncSocketBuffer);

protected:
    void MessageLoop(clSocketBase::Ptr_t socket);
    void BufferLoop(clSocketBase::Ptr_t socket);

    void* ClientMain();
    void* ServerMain();

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

    clSocketAsyncThread(wxEvtHandler* sink, const wxString& connectionString, size_t mode,
                        const wxString& keepAliveMessage = "");
    virtual ~clSocketAsyncThread();
};

class WXDLLIMPEXP_CL clAsyncSocket : public wxEvtHandler
{
    clSocketAsyncThread* m_thread;
    size_t m_mode = kAsyncSocketClient | kAsyncSocketBuffer;
    wxString m_connectionString;

public:
    typedef wxSharedPtr<clAsyncSocket> Ptr_t;

public:
    clAsyncSocket(const wxString& connectionString,
                  size_t mode = (kAsyncSocketClient | kAsyncSocketBuffer));
    virtual ~clAsyncSocket();

    /**
     * @brief connect using connection string
     */
    void Start();
    void Stop();
    void Send(const wxString& buffer);
    void Send(const std::string& buffer);
};

#endif // CLSOCKETCLIENTASYNC_H
