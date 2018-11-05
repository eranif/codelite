#include "NodeJSWebSocketThread.h"
#include "SocketAPI/clSocketBase.h"
#include "file_logger.h"

wxDEFINE_EVENT(wxEVT_WEBSOCKET_DISCONNECTED, clCommandEvent);

NodeJSWebSocketThread::NodeJSWebSocketThread(wxEvtHandler* owner, const wxString& url)
    : m_owner(owner)
    , m_url(url)
    , m_socket(owner)
{
}

NodeJSWebSocketThread::~NodeJSWebSocketThread() {}

void* NodeJSWebSocketThread::Entry()
{
    while(!TestDestroy()) {
        try {
            // Start the main loop
            m_socket.Connect(m_url);
        } catch(clSocketException& e) {
            clDEBUG() << e.what();
            break;
        }
    }
    clCommandEvent endEvent(wxEVT_WEBSOCKET_DISCONNECTED);
    m_owner->AddPendingEvent(endEvent);
    return NULL;
}

void NodeJSWebSocketThread::Send(const wxString& payload)
{
    try {
        clDEBUG1() << "-->" << payload;
        m_socket.Send(payload);

    } catch(clSocketException& e) {
        clDEBUG() << e.what();
    }
}
