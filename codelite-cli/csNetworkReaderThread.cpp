#include "csNetworkReaderThread.h"
#include "SocketAPI/clSocketBase.h"
#include "clNetworkMessage.h"
#include "csCommandHandlerBase.h"
#include "csListCommandHandler.h"
#include "json_node.h"
#include <file_logger.h>
#include <wx/dir.h>

wxDEFINE_EVENT(wxEVT_SOCKET_READ_ERROR, clCommandEvent);

csNetworkReaderThread::csNetworkReaderThread(wxEvtHandler* manager, clSocketBase* conn)
    : csJoinableThread(manager)
    , m_conn(conn)
{
    // m_handlers.Register("list", csCommandHandlerBase::Ptr_t(new csListCommandHandler()));
}

csNetworkReaderThread::~csNetworkReaderThread() { wxDELETE(m_conn); }

void* csNetworkReaderThread::Entry()
{
    FileLoggerNameRegistrar logName("Reader");
    clDEBUG() << "Reader thread started";
    while(true) {
        try {
            wxString message;
            if(m_conn->ReadMessage(message, 1) == clSocketBase::kTimeout) {
                if(TestDestroy()) { break; }
            } else {
                // Success
                if(!message.IsEmpty()) { ProcessCommand(message); }
            }
        } catch(clSocketException& e) {
            clERROR() << e.what();
            break;
        }
    }
    clDEBUG() << "Reader thread is going down";
    NotifyGoingDown();
    return nullptr;
}

void csNetworkReaderThread::ProcessCommand(const wxString& str)
{
    wxUnusedVar(str);
    //    clDEBUG() << "Read:" << str;
    //    clNetworkMessage message(str);
    //    const wxString& command = message["command"];
    //    csCommandHandlerBase::Ptr_t handler = m_handlers.FindHandler(command);
    //    if(handler) {
    //        handler->Process(message, m_conn);
    //    } else {
    //        clWARNING() << "Don't know how to handle command:" << command;
    //    }
}
