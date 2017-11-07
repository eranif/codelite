#include "SocketAPI/clSocketClient.h"
#include "clSocketClientAsync.h"

wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_CONNECTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_INPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_ERROR, clCommandEvent);

clSocketClientAsync::clSocketClientAsync(wxEvtHandler* owner)
    : m_owner(owner)
    , m_thread(NULL)
{
}

clSocketClientAsync::~clSocketClientAsync() { Disconnect(); }

void clSocketClientAsync::Connect(const wxString& connectionString, const wxString& keepAliveMessage)
{
    Disconnect();
    m_thread = new clSocketClientAsyncHelperThread(m_owner, connectionString, false, keepAliveMessage);
    m_thread->Start();
}

void clSocketClientAsync::ConnectNonBlocking(const wxString& connectionString, const wxString& keepAliveMessage)
{
    Disconnect();
    m_thread = new clSocketClientAsyncHelperThread(m_owner, connectionString, true, keepAliveMessage);
    m_thread->Start();
}

void clSocketClientAsync::Disconnect()
{
    if(m_thread) {
        m_thread->Stop();
        wxDELETE(m_thread);
    }
}

void clSocketClientAsync::Send(const wxString& buffer)
{
    if(m_thread) {
        clSocketClientAsyncHelperThread::MyRequest req;
        req.m_command = clSocketClientAsyncHelperThread::kSend;
        req.m_buffer = buffer;
        m_thread->AddRequest(req);
    }
}

//-----------------------------------------------------------------------------------------------
// The helper thread
//-----------------------------------------------------------------------------------------------
clSocketClientAsyncHelperThread::clSocketClientAsyncHelperThread(wxEvtHandler* sink, const wxString& connectionString,
                                                                 bool nonBlockingMode, const wxString& keepAliveMessage)
    : wxThread(wxTHREAD_JOINABLE)
    , m_sink(sink)
    , m_keepAliveMessage(keepAliveMessage)
    , m_connectionString(connectionString)
    , m_nonBlockingMode(nonBlockingMode)
{
}

clSocketClientAsyncHelperThread::~clSocketClientAsyncHelperThread() {}

void* clSocketClientAsyncHelperThread::Entry()
{
    // Connect
    clSocketClient* client = new clSocketClient();
    clSocketBase::Ptr_t socket(client);

    bool connected = false;

    // Try to connect and wait up to 5 seconds
    if(!m_nonBlockingMode) {
        for(size_t i = 0; i < 10; ++i) {
            connected = client->Connect(m_connectionString, false);
            if(connected) { break; }
            if(TestDestroy()) {
                // We were requested to go down during connect phase
                return NULL;
            }
            ::wxMilliSleep(500);
        }
    } else {
        bool wouldBlock = false;
        connected = client->ConnectNonBlocking(m_connectionString, wouldBlock);
        if(!connected && wouldBlock) {
            // We should select here (wait for the socket to be writable)
            for(size_t i = 0; i < 5; ++i) {
                int rc = client->SelectWrite(1);
                if(rc == clSocketBase::kSuccess) {
                    // we are connected
                    connected = true;
                    break;
                } else if(rc == clSocketBase::kError) {
                    // an error occured
                    break;
                } else {
                    // Timeout
                    // Loop again
                }
                
                // Test for thread shutdown before we continue
                if(TestDestroy()) { break; }
            }
        }
    }

    // Connected?
    if(!connected) {
        // report error and go out
        clCommandEvent event(wxEVT_ASYNC_SOCKET_CONNECT_ERROR);
        event.SetString(socket->error());
        m_sink->AddPendingEvent(event);
        return NULL;
    }

    // Notify about connection successful
    clCommandEvent event(wxEVT_ASYNC_SOCKET_CONNECTED);
    m_sink->AddPendingEvent(event);

    try {
        int counter = 0;
        while(!TestDestroy()) {
            // Wait for request from the user
            ++counter;
            if(!m_keepAliveMessage.IsEmpty() && (counter % 10) == 0) {
                // Fire the keep alive message
                // if we lost the socket, it will raise an exception
                socket->Send(m_keepAliveMessage);
            }

            MyRequest req;
            if(m_queue.ReceiveTimeout(1, req) == wxMSGQUEUE_NO_ERROR) {
                // got something
                if(req.m_command == kDisconnect) {
                    socket.reset(NULL);
                    return NULL;

                } else if(req.m_command == kSend) {
                    socket->Send(req.m_buffer);
                }
            }

            // timeout, test to see if we got something on the socket
            wxString buffer;
            if(socket->SelectReadMS(5) == clSocketBase::kSuccess) {
                int rc = socket->Read(buffer);
                if(rc == clSocketBase::kSuccess) {
                    clCommandEvent event(wxEVT_ASYNC_SOCKET_INPUT);
                    event.SetString(buffer);
                    m_sink->AddPendingEvent(event);

                } else if(rc == clSocketBase::kError) {
                    // Connection lost
                    clCommandEvent event(wxEVT_ASYNC_SOCKET_CONNECTION_LOST);
                    m_sink->AddPendingEvent(event);
                    return NULL;
                } else {
                    // Timeout
                }
            }
        }
    } catch(clSocketException& e) {
        clCommandEvent event(wxEVT_ASYNC_SOCKET_ERROR);
        event.SetString(e.what());
        m_sink->AddPendingEvent(event);
        return NULL;
    }
    return NULL;
}
