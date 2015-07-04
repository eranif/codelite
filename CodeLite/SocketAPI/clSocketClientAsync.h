#ifndef CLSOCKETCLIENTASYNC_H
#define CLSOCKETCLIENTASYNC_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "worker_thread.h"
#include <wx/msgqueue.h>
#include <wx/event.h>
#include "SocketAPI/clSocketClient.h"

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
    wxString m_host;
    wxString m_keepAliveMessage;
    int m_port;
    wxMessageQueue<MyRequest> m_queue;

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

    clSocketClientAsyncHelperThread(wxEvtHandler* sink,
                                    const wxString& host,
                                    int port,
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

    void Connect(const wxString& host, int port, const wxString& keepAliveMessage = "");
    void Send(const wxString& buffer);
    void Disconnect();
};

#endif // CLSOCKETCLIENTASYNC_H
