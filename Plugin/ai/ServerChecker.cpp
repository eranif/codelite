#include "ai/ServerChecker.hpp"

#include "file_logger.h"

#include <wx/url.h>

namespace
{
// Counter for server checker instance IDs
int SocketIdCounter{5500};
} // namespace

SockerHelper::SockerHelper(ServerChecker* owner, const wxString& server_url) : m_owner{owner}
{
    wxURL url{server_url};
    if (url.GetError() != wxURL_NOERR) {
        clERROR() << "Failed to parse server URL." << server_url << endl;
        return;
    }

    m_sock = std::make_unique<wxSocketClient>();
    long port{80};
    wxString host = url.GetServer();
    url.GetPort().ToCLong(&port);
    m_sock = std::make_unique<wxSocketClient>();

    m_socket_id = ++SocketIdCounter;
    m_owner->Bind(wxEVT_SOCKET, &ServerChecker::OnSocketEvent, m_owner, m_socket_id);

    // Setup the event handler and subscribe to most events
    m_sock->SetEventHandler(*m_owner, m_socket_id);
    m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG | wxSOCKET_OUTPUT_FLAG);
    m_sock->Notify(true);

    wxIPV4address addr4;

    // we connect asynchronously and will get a wxSOCKET_CONNECTION event when
    // the connection is really established
    //
    // if you want to make sure that connection is established right here you
    // could call WaitOnConnect(timeout) instead
    addr4.Hostname(host);
    addr4.Service(port);
    m_sock->SetTimeout(1);
    m_sock->Connect(addr4, false);
}

SockerHelper::~SockerHelper()
{
    if (m_socket_id == wxNOT_FOUND) {
        return;
    }
    m_owner->Unbind(wxEVT_SOCKET, &ServerChecker::OnSocketEvent, m_owner, m_socket_id);
    m_sock.reset();
}

ServerChecker::ServerChecker(std::atomic_bool& is_alive) : m_is_alive(is_alive)
{
    m_timer.SetOwner(this);
    Bind(wxEVT_TIMER, &ServerChecker::OnTimer, this, m_timer.GetId());
}

ServerChecker::~ServerChecker()
{
    m_helper.reset();
    m_timer.Stop();
    Unbind(wxEVT_TIMER, &ServerChecker::OnTimer, this, m_timer.GetId());
}

void ServerChecker::Check(const wxString& server_url)
{
    m_server_url = server_url;
    m_helper = std::make_unique<SockerHelper>(this, m_server_url);
}

void ServerChecker::OnSocketEvent(wxSocketEvent& event)
{
    switch (event.GetSocketEvent()) {

    case wxSOCKET_LOST:
        m_is_alive = false;
        m_timer.StartOnce(1000);
        break;

    case wxSOCKET_CONNECTION:
        m_is_alive = true;
        m_timer.StartOnce(1000);
        break;
    default:
        break;
    }
}

void ServerChecker::OnTimer([[maybe_unused]] wxTimerEvent& event)
{
    m_helper.reset();
    m_helper = std::make_unique<SockerHelper>(this, m_server_url);
    m_timer.StartOnce(1000);
}
