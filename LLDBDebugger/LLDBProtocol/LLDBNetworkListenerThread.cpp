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
            case kReplyTypeDebuggerStartedSuccessfully: {
                // notify debugger started successfully
                LLDBEvent event(wxEVT_LLDB_STARTED);
                m_owner->AddPendingEvent( event );
                break;
            }

            case kReplyTypeDebuggerExited: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_EXITED);
                m_owner->AddPendingEvent( event );
                break;
            }

            case kReplyTypeDebuggerStopped: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_STOPPED);
                event.SetFileName( reply.GetFilename() );
                event.SetLinenumber( reply.GetLine() );
                event.SetInterruptReason( reply.GetInterruptResaon() );
                event.SetBacktrace( reply.GetBacktrace() );
                m_owner->AddPendingEvent( event );
                break;
            }

            case kReplyTypeDebuggerRunning: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_RUNNING);
                m_owner->AddPendingEvent( event );
                break;
            }

            case kReplyTypeDebuggerStoppedOnFirstEntry: {
                // notify debugger exited
                LLDBEvent event(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY);
                m_owner->AddPendingEvent( event );
                break;
            }
            
            case kReplyTypeAllBreakpointsDeleted: {
                LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL);
                m_owner->AddPendingEvent( event );
                break;
            }
            
            case kReplyTypeBreakpointsUpdated: {
                LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
                event.SetBreakpoints( reply.GetBreakpoints() );
                m_owner->AddPendingEvent( event );
                break;
            }
            
            case kReplyTypeLocalsUpdated: {
                LLDBEvent event(wxEVT_LLDB_LOCALS_UPDATED);
                event.SetLocals( reply.GetLocals() );
                m_owner->AddPendingEvent( event );
                break;
            }
            
            case kReplyTypeVariableExpanded: {
                LLDBEvent event(wxEVT_LLDB_VARIABLE_EXPANDED);
                event.SetLocals( reply.GetLocals() );
                event.SetVariableId( reply.GetLldbId() );
                m_owner->AddPendingEvent( event );
                break;
            }
            }
        }
    }
    return NULL;
}

