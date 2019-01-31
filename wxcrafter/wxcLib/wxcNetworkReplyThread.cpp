#include "wxcNetworkReplyThread.h"
#include "wxcLib/wxcReplyEventData.h"
#include "wxcLib/wxcNetworkReply.h"

wxcNetworkReplyThread::wxcNetworkReplyThread(wxEvtHandler *parent, socket_t sock)
    : wxThread(wxTHREAD_JOINABLE)
    , m_parent(parent)
{
    m_socket.reset( new clSocketBase(sock) );
}

wxcNetworkReplyThread::~wxcNetworkReplyThread()
{
}

void* wxcNetworkReplyThread::Entry()
{
    try {
        while ( !TestDestroy() ) {
            // Wait for message
            wxString message;
            if ( m_socket->ReadMessage( message, 1 ) == clSocketBase::kSuccess ) {
                wxcNetworkReply reply( message );
                switch ( reply.GetReplyType() ) {
                case wxCrafter::kReplyTypeFilesGenerated: {
                    if ( m_parent ) {
                        
                        // File generation completed
                        wxcReplyEventData *data = new wxcReplyEventData;
                        data->SetFiles( reply.GetFiles() );
                        data->SetWxcpFile( reply.GetWxcpFile() );
                        wxCommandEvent event(wxcEVT_NET_REPLY_FILES_GENERATED);
                        event.SetClientObject( data );
                        m_parent->AddPendingEvent( event );
                        
                    }
                }
                break;
                default:
                    break;
                }
            }
        }

    } catch (clSocketException& e) {
        // got an exception!, this means that the other side closed the connection
        wxCommandEvent event(wxcEVT_NET_REPLY_CONN_TERMINATED);
        m_parent->AddPendingEvent( event );
    }
    return NULL;
}
