#include "clTernWorkerThread.h"
#include "SocketAPI/clSocketClient.h"
#include "clTernServer.h"
#include <wx/buffer.h>
#include <sstream>
#include "file_logger.h"

clTernWorkerThread::clTernWorkerThread(clTernServer* ternServer)
    : m_ternSerer(ternServer)
{
    m_port = m_ternSerer->GetPort();
}

clTernWorkerThread::~clTernWorkerThread() {}

struct MallocDeleter {
    char* m_ptr;
    MallocDeleter(char* p)
        : m_ptr(p)
    {
    }

    ~MallocDeleter()
    {
        if(m_ptr) {
            free(m_ptr);
            m_ptr = NULL;
        }
    }
};

void clTernWorkerThread::ProcessRequest(ThreadRequest* request)
{
    clTernWorkerThread::Request* r = dynamic_cast<clTernWorkerThread::Request*>(request);
    clSocketClient* client = new clSocketClient();
    clSocketBase::Ptr_t p(client);
    
    // Make sure that the jsonReuqest is destroyed
    MallocDeleter deleter(r->jsonRequest);
    
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

        wxMemoryBuffer buffer;
        std::stringstream ss;
        ss << "POST / HTTP/1.1\r\n";
        ss << "From: Eran Ifrah\r\n";
        ss << "User-Agent: CodeLite IDE\r\n";
        ss << "Content-Type: application/x-www-form-urlencoded\r\n";
        ss << "Content-Length: ";
        ss << strlen(r->jsonRequest) << "\r\n";
        ss << "\r\n";
        ss << r->jsonRequest;

        std::string strBuffer = ss.str();
        buffer.AppendData(strBuffer.c_str(), strBuffer.length());
        
        CL_DEBUG("[WebTools] %s", strBuffer.c_str());
        
        client->Send(buffer);
        wxMemoryBuffer output;
        client->Read(output, 5);

        // Strip the HTTP headers and send only the JSON reply back to the main thread
        wxString str = wxString::From8BitData((const char*)output.GetData(), output.GetDataLen());
        int where = str.Find("\r\n\r\n");
        if(where == wxNOT_FOUND) {
            CL_DEBUG("Tern invalid output:\n%s\n", str);
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
        reply.requestType = r->type;

        m_ternSerer->CallAfter(&clTernServer::OnTernWorkerThreadDone, reply);

    } catch(clSocketException& e) {
        m_ternSerer->CallAfter(&clTernServer::OnError, e.what().c_str());
    }
}
