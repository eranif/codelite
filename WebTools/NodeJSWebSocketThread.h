#ifndef NODEJSWEBSOCKETTHREAD_H
#define NODEJSWEBSOCKETTHREAD_H

#include "SocketAPI/clWebSocketClient.h"
#include "clJoinableThread.h" // Base class: clJoinableThread
#include <wx/event.h>
#include <wx/string.h>

wxDECLARE_EVENT(wxEVT_WEBSOCKET_DISCONNECTED, clCommandEvent);

class NodeJSWebSocketThread : public clJoinableThread
{
    wxEvtHandler* m_owner = nullptr;
    wxString m_url;
    clWebSocketClient m_socket;

public:
    NodeJSWebSocketThread(wxEvtHandler* owner, const wxString& url);
    virtual ~NodeJSWebSocketThread();
    void* Entry();
    void Send(const wxString& payload);
};

#endif // NODEJSWEBSOCKETTHREAD_H
