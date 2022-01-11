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
public:
    virtual bool write_reply(const wxString& message) = 0;
    virtual bool write_reply(const JSONItem& response) = 0;
    virtual bool write_reply(const JSON& response) = 0;
    virtual unique_ptr<JSON> read_message() = 0;
    virtual void open() = 0;
    typedef shared_ptr<Channel> ptr_t;
};

// socket based channel
class ChannelSocket : public Channel
{
    wxString m_buffer;
    wxString m_ip;
    int m_port = -1;
    clSocketBase::Ptr_t client;

protected:
    eReadSome read_some();

public:
    ChannelSocket(const wxString& ip, int port);
    virtual ~ChannelSocket();

    void open() override;
    bool write_reply(const wxString& message) override;
    bool write_reply(const JSONItem& response) override;
    bool write_reply(const JSON& response) override;
    unique_ptr<JSON> read_message() override;
};
#endif // Channel_HPP
