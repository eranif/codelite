#include "LLDBConnector.h"
#include "LLDBNetworkListenerThread.h"
#include "LLDBEvent.h"

LLDBConnector::LLDBConnector()
    : m_thread(NULL)
{
}

LLDBConnector::~LLDBConnector()
{
    // the order matters here, since both are using the same file descriptor
    // but only m_socket does the actual socket shutdown
    wxDELETE( m_thread );
    m_socket.reset(NULL);
}

bool LLDBConnector::ConnectToDebugger()
{
    clSocketClient *client = new clSocketClient();
    m_socket.reset( client );
    client->SetIp("127.0.0.1");
    client->SetPort( LLDB_PORT );
    if ( !client->Connect() )
        return false;

    // Start the lldb event thread

    // and start a listener thread which will read replies
    // from codelite-lldb and convert them into LLDBEvent
    socket_t fd = m_socket->GetSocket();
    m_thread = new LLDBNetworkListenerThread(this, fd);
    m_thread->Start();
    return true;
}

void LLDBConnector::SendCommand(const LLDBCommand& command)
{
    try {
        m_socket->WriteMessage( command.ToJSON().format() );

    } catch ( clSocketException &e ) {
        wxUnusedVar( e );
    }
}

void LLDBConnector::InvalidateBreakpoints()
{
    for(size_t i=0; i<m_breakpoints.size(); ++i) {
        m_breakpoints.at(i)->Invalidate();
    }
}

bool LLDBConnector::IsBreakpointExists(LLDBBreakpoint::Ptr_t bp) const
{
    return FindBreakpoint(bp) != m_breakpoints.end();
}

LLDBBreakpoint::Vec_t::iterator LLDBConnector::FindBreakpoint(LLDBBreakpoint::Ptr_t bp)
{
    LLDBBreakpoint::Vec_t::iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter ) {
        if ( (*iter)->SameAs( bp ) ) {
            return iter;
        }
    }
    return m_breakpoints.end();
}

LLDBBreakpoint::Vec_t::const_iterator LLDBConnector::FindBreakpoint(LLDBBreakpoint::Ptr_t bp) const
{
    LLDBBreakpoint::Vec_t::const_iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter ) {
        if ( (*iter)->SameAs( bp ) ) {
            return iter;
        }
    }
    return m_breakpoints.end();
}


void LLDBConnector::AddBreakpoint(LLDBBreakpoint::Ptr_t breakpoint)
{
    if ( !IsBreakpointExists(breakpoint) ) {
        m_breakpoints.push_back( breakpoint );
        LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
        event.SetBreakpoints( m_breakpoints );
        AddPendingEvent( event );
    }
}

void LLDBConnector::ApplyBreakpoints()
{
    LLDBCommand command;
    command.SetCommandType( kCommandApplyBreakpoints );
    command.SetBreakpoints( m_breakpoints );
    SendCommand( command );
}

void LLDBConnector::Continue()
{
    LLDBCommand command;
    command.SetCommandType( kCommandContinue );
    SendCommand( command );
}

void LLDBConnector::Stop()
{
    LLDBCommand command;
    command.SetCommandType( kCommandStop );
    SendCommand( command );
}
