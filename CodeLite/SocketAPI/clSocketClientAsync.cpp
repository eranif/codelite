#include "SocketAPI/clSocketClient.h"
#include "clSocketClientAsync.h"
#include <wx/utils.h>
#include "fileutils.h"
#include "SocketAPI/clConnectionString.h"
#include "SocketAPI/clSocketServer.h"
#include "fileutils.h"

wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_CONNECTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_INPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_SOCKET_SERVER_READY, clCommandEvent);

clAsyncSocket::clAsyncSocket(const wxString& connectionString, size_t mode)
    : m_thread(NULL)
    , m_mode(mode)
    , m_connectionString(connectionString)
{
}

clAsyncSocket::~clAsyncSocket() { Stop(); }

void clAsyncSocket::Start()
{
    Stop();
    m_thread = new clSocketAsyncThread(this, m_connectionString, m_mode, wxEmptyString);
    m_thread->Start();
}

void clAsyncSocket::Stop() { wxDELETE(m_thread); }

void clAsyncSocket::Send(const std::string& buffer)
{
    if(m_thread) {
        clSocketAsyncThread::MyRequest req;
        req.m_command = clSocketAsyncThread::kSend;
        req.m_buffer = buffer;
        m_thread->AddRequest(req);
    }
}

void clAsyncSocket::Send(const wxString& buffer) { Send(FileUtils::ToStdString(buffer)); }

//-----------------------------------------------------------------------------------------------
// The helper thread
//-----------------------------------------------------------------------------------------------
clSocketAsyncThread::clSocketAsyncThread(wxEvtHandler* sink, const wxString& connectionString, size_t mode,
                                         const wxString& keepAliveMessage)
    : wxThread(wxTHREAD_JOINABLE)
    , m_sink(sink)
    , m_keepAliveMessage(keepAliveMessage)
    , m_connectionString(connectionString)
    , m_mode(mode)
{
}

clSocketAsyncThread::~clSocketAsyncThread() { Stop(); }

void* clSocketAsyncThread::Entry()
{
    if(m_mode & kAsyncSocketClient) {
        return ClientMain();
    } else {
        return ServerMain();
    }
    return nullptr;
}

void* clSocketAsyncThread::ServerMain()
{
    try {
        clSocketServer server;
        int port = server.Start(m_connectionString);
        
        // Notify that we are ready to accept connections
        clCommandEvent eventReady(wxEVT_ASYNC_SOCKET_SERVER_READY);
        eventReady.SetInt(port);
        m_sink->AddPendingEvent(eventReady);

        clSocketBase::Ptr_t conn;
        while(!TestDestroy()) {
            conn = server.WaitForNewConnection(1);
            if(conn) {
                clCommandEvent event(wxEVT_ASYNC_SOCKET_CONNECTED);
                m_sink->AddPendingEvent(event);
                break;
            }
        }

        if(m_mode & kAsyncSocketMessage) {
            MessageLoop(conn);
        } else {
            BufferLoop(conn);
        }
    } catch(clSocketException& e) {
        clCommandEvent event(wxEVT_ASYNC_SOCKET_CONNECT_ERROR);
        event.SetString(e.what());
        m_sink->AddPendingEvent(event);
    }
    return NULL;
}

void* clSocketAsyncThread::ClientMain()
{
    // Connect
    clSocketClient* client = new clSocketClient();
    clSocketBase::Ptr_t socket(client);

    bool connected = false;

    // Try to connect and wait up to 5 seconds
    if(m_mode & kAsyncSocketNonBlocking) {
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
    } else {
        for(size_t i = 0; i < 10; ++i) {
            connected = client->Connect(m_connectionString, false);
            if(connected) { break; }
            if(TestDestroy()) {
                // We were requested to go down during connect phase
                return NULL;
            }
            ::wxMilliSleep(500);
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

    if(m_mode & kAsyncSocketMessage) {
        MessageLoop(socket);
    } else {
        BufferLoop(socket);
    }
    return NULL;
}

void clSocketAsyncThread::MessageLoop(clSocketBase::Ptr_t socket)
{
    try {
        int counter = 0;
        while(!TestDestroy()) {
            MyRequest req;
            if(m_queue.ReceiveTimeout(100, req) == wxMSGQUEUE_NO_ERROR) {
                // got something
                if(req.m_command == kDisconnect) {
                    socket.reset(NULL);
                    return;

                } else if(req.m_command == kSend) {
                    socket->WriteMessage(req.m_buffer);
                    wxString replyMessage;
                    while(socket->ReadMessage(replyMessage, 1) == clSocketBase::kTimeout) {
                        if(TestDestroy()) {
                            // Going down
                            return;
                        }
                    }
                    // we got data
                    clCommandEvent event(wxEVT_ASYNC_SOCKET_INPUT);
                    event.SetString(replyMessage);
                    m_sink->AddPendingEvent(event);
                }
            } else {
                // Make sure that the socket is still alive
                if((counter % 10) == 0) {
                    socket->WriteMessage("");
                    counter = 0;
                }
            }
            ++counter;
        }
    } catch(clSocketException& e) {
        clCommandEvent event(wxEVT_ASYNC_SOCKET_ERROR);
        event.SetString(e.what());
        m_sink->AddPendingEvent(event);
        return;
    }
}

void clSocketAsyncThread::BufferLoop(clSocketBase::Ptr_t socket)
{
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
                    return;

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
                    return;
                } else {
                    // Timeout
                }
            }
        }
    } catch(clSocketException& e) {
        clCommandEvent event(wxEVT_ASYNC_SOCKET_ERROR);
        event.SetString(e.what());
        m_sink->AddPendingEvent(event);
    }
}
