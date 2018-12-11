#ifndef CLWEBSOCKETCLIENT_H
#define CLWEBSOCKETCLIENT_H

#include "clJoinableThread.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include <memory>
#include <wx/event.h>

class WXDLLIMPEXP_CL clWebSocketClient : public wxEvtHandler
{
protected:
    void* m_client = 0;
    std::weak_ptr<void> m_connection_handle;
    wxEvtHandler* m_owner = nullptr;
    clJoinableThread* m_helperThread = nullptr;

protected:
    template <typename T> T* GetClient()
    {
        if(!m_client) { return nullptr; }
        return reinterpret_cast<T*>(m_client);
    }
    void DoCleanup();
    void DoInit();

public:
    void OnHelperThreadExit();
    
public:
    clWebSocketClient(wxEvtHandler* owner);
    virtual ~clWebSocketClient();
    
    /**
     * @brief prepare the socket before starting the main loop
     */
    void Initialise();
    
    /**
     * @brief connect to a remote URL. This function start a background thread to executes the main loop
     */
    void StartLoop(const wxString& url);

    /**
     * @brief send data over the websocket
     */
    void Send(const wxString& data);

    /**
     * @brief return the owner of this socket. The owner will receive events about new data etc in the form of events
     */
    wxEvtHandler* GetOwner() { return m_owner; }
    void SetConnectionHandle(std::weak_ptr<void> connection_handle) { this->m_connection_handle = connection_handle; }
    std::weak_ptr<void> GetConnectionHandle() const { return m_connection_handle; }

    /**
     * @brief close the connection
     */
    void Close();

    /**
     * @brief are we connected?
     */
    bool IsConnected() const { return m_helperThread != nullptr; }
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WEBSOCKET_CONNECTED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WEBSOCKET_DISCONNECTED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WEBSOCKET_ONMESSAGE, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WEBSOCKET_ERROR, clCommandEvent);

#endif // CLWEBSOCKETCLIENT_H
