#include "XDebugCommThread.h"

#include "XDebugManager.h"

#include <file_logger.h>
#include <string>
#include <wx/buffer.h>
#include <wx/log.h>

void XDebugComThread::SendMsg(const wxString& msg) { m_queue.Post(msg); }

XDebugComThread::~XDebugComThread() { Stop(); }

void XDebugComThread::Stop()
{
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);

    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void* XDebugComThread::Entry()
{
    clDEBUG() << "CodeLite >>> XDebugComThread started" << clEndl;
    //----------------------------------------------------------------
    // Start the debugger server and wait for XDebug to connect to us
    //----------------------------------------------------------------
    clSocketBase::Ptr_t client;
    int retry(0);
    try {

        wxString connectionString;
        connectionString << "tcp://" << m_host << ":" << m_port;
        clDEBUG() << "CodeLite >>> Creating server on:" << connectionString << clEndl;
        m_server.Start(connectionString);

        clDEBUG() << "CodeLite >>> Listening on" << m_host << ":" << m_port << clEndl;

        // Wait for new connection (up to m_waitForConnTimeout seconds, which defaults to 5 seconds )
        do {
            if((m_waitForConnTimeout > 0) && (retry > m_waitForConnTimeout)) {
                // Don't wait any longer for XDebug
                m_xdebugMgr->CallAfter(&XDebugManager::XDebugNotConnecting);
                return NULL;
            }
            client = m_server.WaitForNewConnection(1);
            ++retry;
            clDEBUG() << "CodeLite >>> Waiting for connection.." << clEndl;

        } while(!TestDestroy() && !client);

        clDEBUG() << "CodeLite >>> Successfully accepted connection from XDebug!" << endl;
        m_xdebugMgr->CallAfter(&XDebugManager::SetConnected, true);

        //----------------------------------------------------------------
        // Protocol:
        // First we read the init XML
        //----------------------------------------------------------------

        std::string initXML;
        if(DoReadReply(initXML, client)) {
            m_xdebugMgr->CallAfter(&XDebugManager::OnSocketInput, initXML);

        } else {
            // Something bad happened
            m_xdebugMgr->CallAfter(&XDebugManager::OnCommThreadTerminated);
            return NULL;
        }

        // The main loop: request-reply mode
        while(!TestDestroy()) {
            wxString command;
            if(m_queue.ReceiveTimeout(20, command) == wxMSGQUEUE_NO_ERROR) {
                DoSendCommand(command, client);

                // Wait for the reply
                std::string reply;
                if(!DoReadReply(reply, client)) {
                    // AN error occurred - close session
                    break;
                }

                // Notify XDebugManager
                m_xdebugMgr->CallAfter(&XDebugManager::OnSocketInput, reply);
            }
        }
    } catch(clSocketException& e) {
        clDEBUG() << "XDebugComThread caught an exception:" << e.what() << endl;
        m_xdebugMgr->CallAfter(&XDebugManager::OnCommThreadTerminated);
        return NULL;
    }

    m_xdebugMgr->CallAfter(&XDebugManager::OnCommThreadTerminated);
    return NULL;
}

bool XDebugComThread::DoReadReply(std::string& reply, clSocketBase::Ptr_t client)
{
    if(!client) {
        return false;
    }

    try {
        // Read the data length
        wxString length;
        while(true) {
            char c = 0;
            size_t count = 0;
            client->Read(&c, 1, count);
            if(c == 0) {
                break;
            }
            length << c;
        }

        long dataLengh(0);
        if(!length.ToCLong(&dataLengh)) {
            // session terminated!
            return false;
        }

        // Read the actual buffer
        ++dataLengh; // +1 for NULL
        char* buffer = new char[dataLengh];
        memset(buffer, 0, dataLengh);
        size_t actualSize(0);
        client->Read(buffer, dataLengh, actualSize);
        std::string content(buffer, dataLengh);
        reply.swap(content);
        wxDELETEA(buffer);

    } catch(clSocketException& e) {
        wxUnusedVar(e);
        return false;
    }
    return true;
}

void XDebugComThread::DoSendCommand(const wxString& command, clSocketBase::Ptr_t client)
{
    // got message, process it
    if(!client) {
        return;
    }
    clDEBUG() << "CodeLite >>> " << command << endl;

    wxMemoryBuffer buff;
    buff.AppendData(command.mb_str(wxConvISO8859_1), command.length());
    buff.AppendByte(0);
    std::string cmd((const char*)buff.GetData(), buff.GetDataLen());
    client->Send(cmd);
}
