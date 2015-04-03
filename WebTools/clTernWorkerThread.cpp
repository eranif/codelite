#include "clTernWorkerThread.h"
#include "SocketAPI/clSocketClient.h"
#include "clTernServer.h"
#include <wx/buffer.h>

clTernWorkerThread::clTernWorkerThread(clTernServer* ternServer)
    : m_ternSerer(ternServer)
{
    m_port = m_ternSerer->GetPort();
}

clTernWorkerThread::~clTernWorkerThread()
{
}

void clTernWorkerThread::ProcessRequest(ThreadRequest* request)
{
    clTernWorkerThread::Request* r = dynamic_cast<clTernWorkerThread::Request*>(request);
    clSocketClient* client = new clSocketClient();
    clSocketBase::Ptr_t p(client);
    
    bool wouldBlock;
    try {
        if(!client->ConnectRemote("127.0.0.1", m_port, wouldBlock)) {
            m_ternSerer->CallAfter(&clTernServer::OnError, client->error().c_str());
            return;
        }

        // POST /path/script.cgi HTTP/1.0
        // From: frog@jmarshall.com
        // User-Agent: HTTPTool/1.0
        // Content-Type: application/x-www-form-urlencoded
        // Content-Length: 32

        wxString buffer;
        buffer << "POST / HTTP/1.1\r\n"
               << "From: Eran Ifrah\r\n"
               << "User-Agent: CodeLite IDE\r\n"
               << "Content-Type: application/x-www-form-urlencoded\r\n"
               << "Content-Length: " << r->jsonRequest.length() << "\r\n"
               << "\r\n" << r->jsonRequest;
        // PrintMessage("Sending: :" + buffer);
        client->Send(buffer);
        wxMemoryBuffer output;
        client->Read(output, 5);
        
        // Stip the HTTP headers and send only the JSON reply back to the main thread
        wxString str = wxString::From8BitData((const char*)output.GetData(), output.GetDataLen());
        int where = str.Find("\r\n\r\n");
        if(where == wxNOT_FOUND) {
            m_ternSerer->CallAfter(&clTernServer::OnError, "Invalid output");
            return;
        }
        
        
        wxString json = str.Mid(where + 4);
    
        // Skip the ID
        json = json.AfterFirst('{');
        json.Prepend("{");
        clTernWorkerThread::Reply reply;
        reply.json.swap(json);
        reply.filename.swap(r->filename);
        reply.isFunctionTip = r->isFunctionTip;
        
        m_ternSerer->CallAfter(&clTernServer::OnTernWorkerThreadDone, reply);
        
    } catch(clSocketException& e) {
        m_ternSerer->CallAfter(&clTernServer::OnError, e.what().c_str());
    }
}
