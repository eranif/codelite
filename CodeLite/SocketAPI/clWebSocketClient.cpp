#include "SocketAPI/clSocketBase.h"
#include "clWebSocketClient.h"

// No boost or other external libraries, just plain old good C++11
#define ASIO_STANDALONE 1
#define _WEBSOCKETPP_CPP11_THREAD_ 1

#include "file_logger.h"
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

wxDEFINE_EVENT(wxEVT_WEBSOCKET_CONNECTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_WEBSOCKET_DISCONNECTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_WEBSOCKET_ONMESSAGE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_WEBSOCKET_ERROR, clCommandEvent);

typedef websocketpp::client<websocketpp::config::asio_client> Client_t;
// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

class clWebSocketHelperThread : public clJoinableThread
{
    wxEvtHandler* m_owner = nullptr;
    wxString m_url;
    Client_t* m_client = nullptr;

public:
    clWebSocketHelperThread(wxEvtHandler* owner, const wxString& url, Client_t* c)
        : m_owner(owner)
        , m_url(url)
        , m_client(c)
    {
    }

    virtual ~clWebSocketHelperThread() {}

    void* Entry()
    {
        while(!TestDestroy()) {
            try {
                // Start the main loop
                // We use a second try/catch block here to make sure that the error reporting is done
                // Start the ASIO io_service run loop
                // this will cause a single connection to be made to the server. c.run()
                // will exit when this connection is closed.
                m_client->run();

            } catch(websocketpp::exception& e) {
                // Report an error
                clCommandEvent event(wxEVT_WEBSOCKET_ERROR);
                event.SetString(e.what());
                m_owner->AddPendingEvent(event);
            }
        }
        m_owner->CallAfter(&clWebSocketClient::OnHelperThreadExit);
        return NULL;
    }
};

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

// This message handler will be invoked once for each incoming message
static void on_ws_message(clWebSocketClient* c, websocketpp::connection_hdl hdl, message_ptr msg)
{
    clDEBUG1() << "<--" << msg->get_payload();
    clCommandEvent event(wxEVT_WEBSOCKET_ONMESSAGE);
    event.SetString(msg->get_payload());
    event.SetEventObject(c);
    c->GetOwner()->AddPendingEvent(event);
}

static void on_ws_open_handler(clWebSocketClient* c, websocketpp::connection_hdl hdl)
{
    clDEBUG1() << "<-- Connected!";
    c->SetConnectionHandle(hdl);
    clCommandEvent event(wxEVT_WEBSOCKET_CONNECTED);
    event.SetEventObject(c);
    c->GetOwner()->AddPendingEvent(event);
}

static void on_ws_fail_handler(clWebSocketClient* c, websocketpp::connection_hdl hdl)
{
    clDEBUG1() << "<-- Error!";
    clCommandEvent event(wxEVT_WEBSOCKET_ERROR);
    event.SetEventObject(c);
    c->GetOwner()->AddPendingEvent(event);
}

clWebSocketClient::clWebSocketClient(wxEvtHandler* owner)
    : m_owner(owner)
{
    DoInit();
}

clWebSocketClient::~clWebSocketClient()
{
    Close();
    DoCleanup();
}

void clWebSocketClient::StartLoop(const wxString& url)
{
    if(m_helperThread) { throw clSocketException("A websocket loop is already running"); }

    Client_t* c = GetClient<Client_t>();
    if(!c) { throw clSocketException("Invalid connection!"); }
    try {
        std::string uri = url.mb_str(wxConvUTF8).data();
        // Register our message handler
        websocketpp::lib::error_code ec;
        Client_t::connection_ptr con = c->get_connection(uri, ec);
        if(ec) { throw clSocketException(ec.message()); }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c->connect(con);
    } catch(websocketpp::exception& e) {
        m_connection_handle.reset();
        throw clSocketException(e.what());
    }

    // Run the main loop in background thread
    m_helperThread = new clWebSocketHelperThread(this, url, c);
    m_helperThread->Start();
}

void clWebSocketClient::Send(const wxString& data)
{
    Client_t* c = GetClient<Client_t>();
    if(!c) { throw clSocketException("Invalid connection!"); }
    if(m_connection_handle.expired()) { throw clSocketException("Invalid connection handle!"); }

    try {
        std::string str = data.mb_str(wxConvUTF8).data();
        websocketpp::lib::error_code ec;
        c->send(GetConnectionHandle(), str, websocketpp::frame::opcode::TEXT, ec);

    } catch(websocketpp::exception& e) {
        throw clSocketException(e.what());
    }
}

void clWebSocketClient::Close()
{
    Client_t* c = GetClient<Client_t>();
    if(!c) { return; }
    c->stop();
    DoCleanup();
}

void clWebSocketClient::DoCleanup()
{
    // Cleanup: first close the helper thread
    wxDELETE(m_helperThread);
    m_connection_handle.reset();
    Client_t* c = GetClient<Client_t>();
    wxDELETE(c);
    m_client = nullptr;
}

void clWebSocketClient::OnHelperThreadExit()
{
    DoCleanup();
    clCommandEvent event(wxEVT_WEBSOCKET_DISCONNECTED);
    event.SetEventObject(this);
    GetOwner()->AddPendingEvent(event);
}

void clWebSocketClient::DoInit()
{
    // Dont initialise again
    if(m_client) { return; }

    try {
        m_client = new Client_t();
        Client_t* c = GetClient<Client_t>();
        //c->set_access_channels(websocketpp::log::alevel::all);
        c->clear_access_channels(websocketpp::log::alevel::all);
        c->init_asio();
        c->set_message_handler(bind(&on_ws_message, this, ::_1, ::_2));
        c->set_open_handler(bind(&on_ws_open_handler, this, ::_1));
        c->set_fail_handler(bind(&on_ws_fail_handler, this, ::_1));
    } catch(websocketpp::exception& e) {
        clERROR() << e.what();
    }
}

void clWebSocketClient::Initialise() { DoInit(); }
