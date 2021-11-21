#include "Channel.hpp"
#include "LSP/Message.h"
#include "file_logger.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

Channel::Channel() {}

Channel::~Channel() {}

eReadSome Channel::read_some()
{
    wxString buffer;
    client->SelectRead();

    switch(client->Read(buffer)) {
    case clSocketBase::kSuccess:
        m_buffer.Append(buffer);
        return eReadSome::kSuccess;
    case clSocketBase::kTimeout:
        return eReadSome::kTimeout;
    default:
    case clSocketBase::kError:
        clERROR() << "Socket read error. " << client->error() << endl;
        return eReadSome::kError;
    }
}

bool Channel::write_reply(const wxString& message)
{
    auto cb = message.mb_str(wxConvUTF8);

    // Build the request header
    std::stringstream ss;
    std::string s;
    ss << "Content-Length: " << cb.length() << "\r\n";
    ss << "\r\n";
    s = ss.str();

    // append the data
    s.append(cb.data(), cb.length());
    clDEBUG() << "Sending reply:" << s << endl;
    client->Send(s);
    return true;
}

unique_ptr<JSON> Channel::read_message()
{
    while(true) {
        auto msg = LSP::Message::GetJSONPayload(m_buffer);
        if(msg) {
            return msg;
        }

        // attempt to read some more data from stdin
        bool cont = true;
        while(cont) {
            switch(read_some()) {
            case eReadSome::kError:
                return nullptr;
            case eReadSome::kSuccess:
                cont = false;
                break;
            case eReadSome::kTimeout:
                break;
            }
        }
    }
}

void Channel::open(const wxString& ip, int port)
{
    clSocketServer server;
    wxString connection_string;
    connection_string << "tcp://" << ip << ":" << port;

    server.Start(connection_string);
    clSYSTEM() << "Accepting connection on:" << connection_string << endl;

    client = server.WaitForNewConnection();
    if(!client) {
        clERROR() << "failed to accept new connection. Error code:" << server.GetLastError() << endl;
        exit(1);
    }
    clSYSTEM() << "Connection established successfully" << endl;
}
