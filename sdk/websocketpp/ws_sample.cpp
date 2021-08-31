// No boost or other external libraries, just plain old good C++11
#define ASIO_STANDALONE 1
#define _WEBSOCKETPP_CPP11_THREAD_ 1

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include <iostream>
#include <sstream>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// This message handler will be invoked once for each incoming message
void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg)
{
    std::cout << "on_message called with hdl: " << hdl.lock().get() << " and message: " << msg->get_payload()
              << std::endl;
}

static void send_startup_command(
    client* c, websocketpp::connection_hdl hdl, const std::string& method, const std::string& params = "")
{
    static int i = 0;
    std::stringstream ss;
    ss << "{\"id\":" << (++i) << ",\"method\":\"" << method << "\"";
    if(!params.empty()) { ss << ", \"params\":\"" << params << "\""; }
    ss << "}";
    websocketpp::lib::error_code ec;
    c->send(hdl, ss.str(), websocketpp::frame::opcode::TEXT, ec);
}

void on_open_handler(client* c, websocketpp::connection_hdl hdl)
{
    std::cout << "on_open_handler called" << std::endl;

    // Send upgrade request
    send_startup_command(c, hdl, "Runtime.enable");
    send_startup_command(c, hdl, "Debugger.enable");
    send_startup_command(c, hdl, "Runtime.runIfWaitingForDebugger");
}

int main(int argc, char* argv[])
{
    // Create a client endpoint
    client c;
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <WebSocker URI>" << std::endl;
        return 1;
    }
    std::string uri = argv[1];

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));
        c.set_open_handler(bind(&on_open_handler, &c, ::_1));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if(ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    } catch(websocketpp::exception const& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
