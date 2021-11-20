#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "JSON.h"
#include "SocketAPI/clSocketServer.h"
#include <memory>
#include <wx/string.h>

using namespace std;

class Channel
{
    wxString m_buffer;
    clSocketBase::Ptr_t client;

protected:
    bool read_some();

public:
    Channel();
    ~Channel();

    void open(const wxString& ip, int port);

    bool write_reply(const wxString& message);
    unique_ptr<JSON> read_message();
};

#endif // Channel_HPP
