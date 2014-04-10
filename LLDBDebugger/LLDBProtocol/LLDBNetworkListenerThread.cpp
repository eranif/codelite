#include "LLDBNetworkListenerThread.h"
#include "LLDBReply.h"
#include "LLDBEvent.h"

LLDBNetworkListenerThread::LLDBNetworkListenerThread(wxEvtHandler* owner, int fd)
    : wxThread(wxTHREAD_JOINABLE)
    , m_owner(owner)
{
    m_socket.reset( new clSocketBase(fd) );
}

LLDBNetworkListenerThread::~LLDBNetworkListenerThread()
{
    Stop();
}

void* LLDBNetworkListenerThread::Entry()
{
    while ( !TestDestroy() ) {
        wxString msg;
        if ( m_socket->ReadMessage(msg, 1) == clSocketBase::kSuccess ) {
            LLDBReply reply(msg);
            switch( reply.GetReplyType() ) {
            case kTypeDebuggerStartedSuccessfully: {
                // notify debugger started successfully
                LLDBEvent event(wxEVT_LLDB_STARTED);
                m_owner->AddPendingEvent( event );
                break;
            }

            case kTypeDebuggerExited: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_EXITED);
                m_owner->AddPendingEvent( event );
                break;
            }

            case kTypeDebuggerStopped: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_STOPPED);
                event.SetFileName( reply.GetFilename() );
                event.SetLinenumber( reply.GetLine() );
                event.SetInterruptReason( reply.GetInterruptResaon() );
                m_owner->AddPendingEvent( event );
                break;
            }

            case kTypeDebuggerRunning: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_RUNNING);
                m_owner->AddPendingEvent( event );
                break;
            }

            case kTypeDebuggerStoppedOnFirstEntry: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY);
                m_owner->AddPendingEvent( event );
                break;
            }
            
            case kTypeAllBreakpointsDeleted: {
                LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL);
                m_owner->AddPendingEvent( event );
                break;
            }
            
            case kTypeBreakpointsUpdated: {
                LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
                event.SetBreakpoints( reply.GetBreakpoints() );
                m_owner->AddPendingEvent( event );
                break;
            }
            }
        }
    }
    return NULL;
}

