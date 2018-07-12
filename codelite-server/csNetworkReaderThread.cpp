#include "SocketAPI/clSocketBase.h"
#include "clNetworkMessage.h"
#include "csNetworkReaderThread.h"
#include "json_node.h"
#include <file_logger.h>
#include <wx/dir.h>

wxDEFINE_EVENT(wxEVT_SOCKET_READ_ERROR, clCommandEvent);

csNetworkReaderThread::csNetworkReaderThread(wxEvtHandler* manager, clSocketBase* conn)
    : csJoinableThread(manager)
    , m_conn(conn)
{
}

csNetworkReaderThread::~csNetworkReaderThread() { wxDELETE(m_conn); }

void* csNetworkReaderThread::Entry()
{
    clDEBUG() << "[csNetworkReaderThread] reader thread started";
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
                if(!message.IsEmpty() && !ProcessCommand(message)) {
                    break;
                }
            }
        } catch(clSocketException& e) {
            clERROR() << "[csNetworkReaderThread] " << e.what();
            break;
        }
    }
    clDEBUG() << "[csNetworkReaderThread] going down";
    NotifyGoingDown();
    return nullptr;
}

bool csNetworkReaderThread::ProcessCommand(const wxString& str)
{
    clDEBUG() << "[csNetworkReaderThread] read:" << str;
    clNetworkMessage message(str);
    const wxString& command = message["command"];
    if(command == "list") {
        const wxString& folder = message["folder"];
        wxDir dir(folder);
        wxString filename;
        bool cont = dir.GetFirst(&filename);
        JSONRoot json(cJSON_Array);
        JSONElement arr = json.toElement();
        while(cont) {
            wxFileName fn(folder, filename);
            JSONElement entry = JSONElement::createObject();
            wxString fullpath = fn.GetFullPath();
            entry.addProperty("path", fn.GetFullPath());
            entry.addProperty("type", wxFileName::DirExists(fullpath) ? "dir" : "file");
            arr.arrayAppend(entry);
            cont = dir.GetNext(&filename);
        }
        m_conn->WriteMessage(arr.FormatRawString(false));
    }
    return true;
}
