#include "LLDBConnector.h"
#include "LLDBNetworkListenerThread.h"
#include "LLDBEvent.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include <wx/tokenzr.h>
#include "file_logger.h"
#include <wx/filefn.h>
#include <wx/log.h>

wxBEGIN_EVENT_TABLE(LLDBConnector, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  LLDBConnector::OnProcessOutput)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, LLDBConnector::OnProcessTerminated)
wxEND_EVENT_TABLE()

LLDBConnector::LLDBConnector()
    : m_thread(NULL)
    , m_process(NULL)
    , m_isRunning(false)
    , m_canInteract(false)
{
    Bind(wxEVT_LLDB_EXITED,  &LLDBConnector::OnLLDBExited, this);
    Bind(wxEVT_LLDB_STARTED, &LLDBConnector::OnLLDBStarted, this);
}

LLDBConnector::~LLDBConnector()
{
    StopDebugServer();
    Unbind(wxEVT_LLDB_EXITED,  &LLDBConnector::OnLLDBExited, this);
    Unbind(wxEVT_LLDB_STARTED, &LLDBConnector::OnLLDBStarted, this);
    Cleanup();
}

bool LLDBConnector::ConnectToDebugger(int timeout)
{
    clSocketClient *client = new clSocketClient();
    m_socket.reset( client );
    client->SetPath( GetDebugServerPath() );
    
    long msTimeout = timeout * 1000;
    long retriesCount = msTimeout / 250; // We try every 250 ms to connect
    bool connected = false;
    for(long i=0; i<retriesCount; ++i) {
        if ( !client->Connect() ) {
            wxThread::Sleep(250);
            continue;
        }
        connected = true;
        break;
    }
    
    if ( !connected ) {
        return false;
    }
    
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
        if ( m_socket ) {
            m_socket->WriteMessage( command.ToJSON().format() );
        }

    } catch ( clSocketException &e ) {
        wxUnusedVar( e );
    }
}

void LLDBConnector::InvalidateBreakpoints()
{
    // mark all the breakpoints as "not-applied" (id=-1)
    LLDBBreakpoint::Vec_t updatedList;
    
    for(size_t i=0; i<m_breakpoints.size(); ++i) {
        m_breakpoints.at(i)->Invalidate();
        if ( wxFileName::Exists(m_breakpoints.at(i)->GetFilename()) ) {
            updatedList.push_back( m_breakpoints.at(i) );
        }
    }
    // we keep only breakpoints with valid filename
    m_breakpoints.swap( updatedList );
    ClearBreakpointDeletionQueue();
    
    CL_DEBUG("codelite: InvalidateBreakpoints called");
    m_pendingDeletionBreakpoints.clear();
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


void LLDBConnector::AddBreakpoint(LLDBBreakpoint::Ptr_t breakpoint, bool notify)
{
    if ( !IsBreakpointExists(breakpoint) ) {
        m_breakpoints.push_back( breakpoint );
        
        if ( notify ) {
            LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
            event.SetBreakpoints( GetAllBreakpoints() );
            ProcessEvent( event );
        }
    }
}

void LLDBConnector::ApplyBreakpoints()
{
    if ( !m_breakpoints.empty() ) {
        
        if ( IsCanInteract() ) {
            LLDBCommand command;
            command.SetCommandType( kCommandApplyBreakpoints );
            command.SetBreakpoints( GetUnappliedBreakpoints() );
            SendCommand( command );
            m_breakpoints.clear();
            
        } else {
            Interrupt( kInterruptReasonApplyBreakpoints );
        }
    }
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

void LLDBConnector::MarkBreakpointForDeletion(LLDBBreakpoint::Ptr_t bp)
{
    if ( !IsBreakpointExists(bp) ) {
        return;
    }
    
    LLDBBreakpoint::Vec_t::iterator iter = FindBreakpoint( bp );
    
    // add the breakpoint to the pending deletion breakpoints
    bp->SetId( (*iter)->GetId() );
    m_pendingDeletionBreakpoints.push_back( bp );
    m_breakpoints.erase( iter );
}

void LLDBConnector::DeleteBreakpoints()
{
    if ( IsCanInteract() ) {
        CL_DEBUG(wxString () << "codelite: deleting breakpoints (total of " << m_pendingDeletionBreakpoints.size() << " breakpoints)");
        LLDBCommand command;
        command.SetCommandType( kCommandDeleteBreakpoint );
        command.SetBreakpoints( m_pendingDeletionBreakpoints );
        SendCommand( command );
        CL_DEBUG(wxString () << "codelite: DeleteBreakpoints celar pending deletionbreakpoints queue");
        m_pendingDeletionBreakpoints.clear();
    
    } else {
        CL_DEBUG("codelite: interrupting codelite-lldb for kInterruptReasonDeleteBreakpoint");
        Interrupt( kInterruptReasonDeleteBreakpoint );
    }
}

void LLDBConnector::ClearBreakpointDeletionQueue()
{
    CL_DEBUG(wxString () << "codelite: ClearBreakpointDeletionQueue called");
    m_pendingDeletionBreakpoints.clear();
}

void LLDBConnector::Next()
{
    LLDBCommand command;
    command.SetCommandType(kCommandNext);
    SendCommand( command );
}

void LLDBConnector::StepIn()
{
    LLDBCommand command;
    command.SetCommandType(kCommandStepIn);
    SendCommand( command );
}

void LLDBConnector::StepOut()
{
    LLDBCommand command;
    command.SetCommandType(kCommandStepOut);
    SendCommand( command );
}

void LLDBConnector::DeleteAllBreakpoints()
{
    if ( !IsRunning() ) {
        m_pendingDeletionBreakpoints.clear();
        m_breakpoints.clear();
        
        LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
        event.SetBreakpoints( GetAllBreakpoints() );
        ProcessEvent( event );
        return;
     }

    // mark all breakpoints for deletion
    CL_DEBUG(wxString () << "codelite: DeleteAllBreakpoints called");
    m_pendingDeletionBreakpoints.swap( m_breakpoints );
    
    if ( !IsCanInteract() ) {
        Interrupt( kInterruptReasonDeleteAllBreakpoints );
        
    } else {
        LLDBCommand command;
        command.SetCommandType(kCommandDeleteAllBreakpoints);
        SendCommand( command );
        m_pendingDeletionBreakpoints.clear();

    }
}

void LLDBConnector::AddBreakpoints(const BreakpointInfo::Vec_t& breakpoints)
{
    LLDBBreakpoint::Vec_t bps = LLDBBreakpoint::FromBreakpointInfoVector( breakpoints );
    AddBreakpoints( bps );
}

void LLDBConnector::AddBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints)
{
    for(size_t i=0; i<breakpoints.size(); ++i) {
        AddBreakpoint( breakpoints.at(i), false );
    }
}

void LLDBConnector::Cleanup()
{
    // the order matters here, since both are using the same file descriptor
    // but only m_socket does the actual socket shutdown
    wxDELETE( m_thread );
    m_socket.reset(NULL);
    InvalidateBreakpoints();
    m_isRunning = false;
    m_canInteract = false;
    m_runCommand.Clear();
}

void LLDBConnector::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    m_isRunning = false;
}

void LLDBConnector::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    m_isRunning = true;
}

void LLDBConnector::Start(const LLDBCommand& runCommand)
{
    LLDBCommand startCommand;
    startCommand.SetExecutable( runCommand.GetExecutable() );
    startCommand.SetCommandType( kCommandStart );
    SendCommand( startCommand );

    // stash the runCommand for the future 'Run()' call
    m_runCommand.Clear();
    m_runCommand = startCommand;
    m_runCommand.SetCommandType( kCommandRun );

}

void LLDBConnector::Run()
{
    if ( m_runCommand.GetCommandType() == kCommandRun ) {
        SendCommand( m_runCommand );
        m_runCommand.Clear();
    }
}

void LLDBConnector::UpdateAppliedBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints)
{
    m_breakpoints = breakpoints;
}

const LLDBBreakpoint::Vec_t& LLDBConnector::GetAllBreakpoints() const
{
    return m_breakpoints;
}

void LLDBConnector::OnProcessOutput(wxCommandEvent& event)
{
    ProcessEventData* ped = (ProcessEventData*) event.GetClientData();
    wxString output = ped->GetData();
    wxDELETE(ped);
    
    wxArrayString lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(size_t i=0; i<lines.GetCount(); ++i) {
        CL_DEBUG(lines.Item(i).Trim());
    }
}

void LLDBConnector::OnProcessTerminated(wxCommandEvent& event)
{
    ProcessEventData* ped = (ProcessEventData*) event.GetClientData();
    if ( ped ) {
        wxDELETE(ped);
    }
    wxDELETE( m_process );
    
    // Notify debug-server terminated
    LLDBEvent lldb_event(wxEVT_LLDB_CRASHED);
    AddPendingEvent( lldb_event );
    LaunchDebugServer();
}

void LLDBConnector::Interrupt(eInterruptReason reason)
{
    LLDBCommand command;
    command.SetCommandType(kCommandInterrupt);
    command.SetInterruptReason( reason );
    SendCommand( command );
}

void LLDBConnector::LaunchDebugServer()
{
    CL_DEBUG("Launching codelite-lldb");
    // Start the debugger
    if ( m_process ) {
        // another debugger process is already running
        return;
    }

    wxFileName fnCodeLiteLLDB(wxStandardPaths::Get().GetExecutablePath());
    fnCodeLiteLLDB.SetFullName( "codelite-lldb" );
    
    wxString command;
    command << fnCodeLiteLLDB.GetFullPath() << " " << ::wxGetProcessId();
    
    // FIXME: 
    // On OSX, make sure we set the environment variable LLDB_DEBUGSERVER_PATH
    m_process = ::CreateAsyncProcess(this, command);
    if ( !m_process ) {
        CL_ERROR("LLDBConnector: failed to launch codelite-lldb: %s", fnCodeLiteLLDB.GetFullPath());
    }
}

void LLDBConnector::StopDebugServer()
{
    if ( m_process ) {
        m_process->Detach();
        m_process->SetHardKill(true); // kill -9
        m_process->Terminate();
        m_process = NULL;
    }
    
    wxLogNull noLog;
    wxRemoveFile(GetDebugServerPath());
}

LLDBBreakpoint::Vec_t LLDBConnector::GetUnappliedBreakpoints()
{
    LLDBBreakpoint::Vec_t unappliedBreakpoints;
    for(size_t i=0; i<m_breakpoints.size(); ++i) {
        if ( !m_breakpoints.at(i)->IsApplied() ) {
            unappliedBreakpoints.push_back( m_breakpoints.at(i) );
        }
    }
    return unappliedBreakpoints;
}

void LLDBConnector::RequestLocals()
{
    if ( IsCanInteract() ) {
        LLDBCommand command;
        command.SetCommandType( kCommandGetLocals );
        SendCommand( command );
    }
}

void LLDBConnector::RequestVariableChildren(int lldbId)
{
    if ( IsCanInteract() ) {
        LLDBCommand command;
        command.SetCommandType(kCommandExpandVariable);
        command.SetLldbId( lldbId );
        SendCommand( command );
    }
}

wxString LLDBConnector::GetDebugServerPath() const
{
    wxString path;
    path << "/tmp/codelite-lldb." << ::wxGetProcessId() << ".sock";
    return path;
}

