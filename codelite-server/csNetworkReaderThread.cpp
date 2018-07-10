#include "SocketAPI/clSocketBase.h"
#include "csNetworkReaderThread.h"
#include <file_logger.h>

wxDEFINE_EVENT(wxEVT_SOCKET_READ_ERROR, clCommandEvent);

csNetworkReaderThread::csNetworkReaderThread(wxEvtHandler* manager, clSocketBase* conn)
    : csJoinableThread(manager)
    , m_conn(conn)
{
}

csNetworkReaderThread::~csNetworkReaderThread() { wxDELETE(m_conn); }

void* csNetworkReaderThread::Entry()
{
    while(true) {
        try {
            wxString message;
            if(m_conn->ReadMessage(message, 1) == clSocketBase::kTimeout) {
                if(TestDestroy()) {
                    clDEBUG() << "[csNetworkReaderThread] going down";
                    break;
                }
            } else {
                // Success
                if(!ProcessCommand(message)) { break; }
            }
        } catch(clSocketException& e) {
            clERROR() << "[csNetworkReaderThread] " << e.what();
            break;
        }
    }

    NotifyGoingDown();
    return nullptr;
}

bool csNetworkReaderThread::ProcessCommand(const wxString& message) { return true; }
