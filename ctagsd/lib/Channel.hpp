#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "JSON.h"
#include "SocketAPI/clSocketServer.h"
#include <memory>
#include <wx/string.h>

using namespace std;

enum class eReadSome {
    kTimeout,
    kError,
    kSuccess,
};

class Channel
{
    wxString m_buffer;
    clSocketBase::Ptr_t client;

protected:
    eReadSome read_some();

public:
    Channel();
    ~Channel();

    void open(const wxString& ip, int port);

    bool write_reply(const wxString& message);
    bool write_reply(const JSONItem& response);
    bool write_reply(const JSON& response);
    unique_ptr<JSON> read_message();
};

#endif // Channel_HPP
