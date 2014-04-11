#include "CodeLiteLLDBApp.h"
#include <iostream>
#include <wx/sckaddr.h>
#include "LLDBProtocol/LLDBReply.h"
#include "LLDBProtocol/cl_socket_server.h"
#include "LLDBProtocol/LLDBEnums.h"
#include "clcommandlineparser.h"
#include <lldb/API/SBBreakpointLocation.h>
#include <lldb/API/SBFileSpec.h>


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static char** _wxArrayStringToCharPtrPtr(const wxArrayString &arr)
{
    char** argv = new char*[arr.size()+1]; // for the NULL
    for(size_t i=0; i<arr.size(); ++i) {
        argv[i] = strdup(arr.Item(i).mb_str(wxConvUTF8).data());
    }
    argv[arr.size()] = NULL;
    return argv;
}

static void DELETE_CHAR_PTR_PTR(char** argv)
{
    size_t i=0;
    while ( argv[i] ) {
        delete [] argv[i];
        ++i;
    }
    delete [] argv;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_APP_CONSOLE(CodeLiteLLDBApp)
CodeLiteLLDBApp::CodeLiteLLDBApp()
    : m_networkThread(NULL)
    , m_lldbProcessEventThread(NULL)
    , m_debuggeePid(wxNOT_FOUND)
    , m_interruptReason(kInterruptReasonNone)
{
    wxSocketBase::Initialize();
    wxPrintf("codelite-lldb: lldb initialized successfully\n");
}

CodeLiteLLDBApp::~CodeLiteLLDBApp()
{
    wxDELETE( m_networkThread );
    wxDELETE( m_lldbProcessEventThread );
    m_replySocket.reset(NULL);
}

int CodeLiteLLDBApp::OnExit()
{
    lldb::SBDebugger::Terminate();
    return TRUE;
}

bool CodeLiteLLDBApp::OnInit()
{
    // Start the network thread. This thread is waiting for commands coming
    // from codeite -> lldb

    // Start the socket server and wait for connection
    clSocketServer server;
    wxPrintf("codelite-lldb: waiting for new connection\n");
    server.CreateServer("127.0.0.1", LLDB_PORT);
    clSocketBase::Ptr_t conn(NULL);
    try {
        while( true ) {
            conn = server.WaitForNewConnection(1);
            if ( conn ) {
                break;
            }
        }

    } catch (clSocketException &e) {
        wxPrintf("Error occured while waiting for connection. %s\n", e.what().c_str());
        Cleanup();
        return false;
    }

    // release the fd from the connection
    socket_t socketFd = conn->Release();

    // handle the connection to the thread
    m_networkThread = new LLDBNetworkServerThread(this, socketFd);
    m_networkThread->Start();

    // Establish the replies connection
    m_replySocket.reset( new clSocketBase(socketFd) );

    lldb::SBDebugger::Initialize();
    m_debugger = lldb::SBDebugger::Create();

    // We got both ends connected
    wxPrintf("codelite-lldb: successfully established connection to codelite\n");
    // place your initialization code here
    return true;
}

void CodeLiteLLDBApp::StartDebugger(const LLDBCommand& command)
{
    if ( !command.GetWorkingDirectory().IsEmpty() ) {
        ::wxSetWorkingDirectory( command.GetWorkingDirectory() );
    }
    wxPrintf("codelite-lldb: working directory is set to %s\n", ::wxGetCwd());
#ifdef __WXMAC__
    // On OSX, debugserver executable must exists otherwise lldb will not work properly
    // we ensure that it exists by checking the environment variable LLDB_DEBUGSERVER_PATH
    wxString lldbDebugServer;
    if ( !::wxGetEnv("LLDB_DEBUGSERVER_PATH", &lldbDebugServer) || !wxFileName::Exists(lldbDebugServer) ) {
        std::cerr << "LLDB_DEBUGSERVER_PATH environment does not exist or contains a path to a non existent file" << std::endl;
        Cleanup();
        return;
    }
#endif

    m_debuggeePid = wxNOT_FOUND;
    m_debugger = lldb::SBDebugger::Create();
    m_target = m_debugger.CreateTarget(command.GetExecutable().mb_str().data());
    m_debugger.SetAsync(true);

    wxPrintf("codelite-lldb: created target for %s\n", command.GetExecutable());

    // Launch the thread that will handle the LLDB process events
    m_lldbProcessEventThread = new LLDBProcessEventHandlerThread(this, m_debugger.GetListener(), m_target.GetProcess());
    m_lldbProcessEventThread->Start();

    // In any case, reset the interrupt reason
    m_interruptReason = kInterruptReasonNone;

    // Notify codelite that the debugger started successfully
    NotifyStarted();
}

void CodeLiteLLDBApp::NotifyAllBreakpointsDeleted()
{
    LLDBReply reply;
    reply.SetReplyType( kTypeAllBreakpointsDeleted );
    SendReply( reply );
}

void CodeLiteLLDBApp::NotifyBacktrace()
{
}

void CodeLiteLLDBApp::NotifyBreakpointsUpdated()
{
    LLDBBreakpoint::Vec_t breakpoints;
    int num = m_target.GetNumBreakpoints();
    for(int i=0; i<num; ++i) {
        lldb::SBBreakpoint bp = m_target.GetBreakpointAtIndex(i);
        if ( bp.IsValid() ) {
            
            // Add the parent breakpoint
            LLDBBreakpoint::Ptr_t mainBreakpoint( new LLDBBreakpoint() );
            mainBreakpoint->SetId( bp.GetID() );
            if ( bp.GetNumLocations() >  1 ) {
                
                // add all the children locations to the main breakpoint
                for(size_t i=0; i<bp.GetNumLocations(); ++i) {
                    lldb::SBBreakpointLocation loc = bp.GetLocationAtIndex(i);
                    lldb::SBFileSpec fileLoc = loc.GetAddress().GetLineEntry().GetFileSpec();
                    wxFileName bpFile( fileLoc.GetDirectory(), fileLoc.GetFilename() );

                    // Create a breakpoint for this location
                    LLDBBreakpoint::Ptr_t new_bp(new LLDBBreakpoint());
                    new_bp->SetType( LLDBBreakpoint::kLocation );
                    new_bp->SetFilename( bpFile.GetFullPath() );
                    new_bp->SetLineNumber( loc.GetAddress().GetLineEntry().GetLine() );
                    new_bp->SetName( mainBreakpoint->GetName() );
                    mainBreakpoint->GetChildren().push_back( new_bp );
            }
                
            } else {
                lldb::SBBreakpointLocation loc = bp.GetLocationAtIndex(0);
                lldb::SBFileSpec fileLoc = loc.GetAddress().GetLineEntry().GetFileSpec();
                wxFileName bpFile( fileLoc.GetDirectory(), fileLoc.GetFilename() );
                
                mainBreakpoint->SetType( LLDBBreakpoint::kFileLine );
                mainBreakpoint->SetFilename( bpFile.GetFullPath() );
                mainBreakpoint->SetLineNumber( loc.GetAddress().GetLineEntry().GetLine() );
                
            }
            breakpoints.push_back( mainBreakpoint );
        }
    }
    
    LLDBReply reply;
    reply.SetReplyType( kTypeBreakpointsUpdated );
    reply.SetBreakpoints( breakpoints );
    SendReply( reply );
}

void CodeLiteLLDBApp::NotifyExited()
{
    LLDBReply reply;
    reply.SetReplyType( kTypeDebuggerExited );
    SendReply( reply );
    Cleanup();
}

void CodeLiteLLDBApp::NotifyRunning()
{
    LLDBReply reply;
    reply.SetReplyType( kTypeDebuggerRunning );
    SendReply( reply );
}

void CodeLiteLLDBApp::NotifyStarted()
{
    LLDBReply reply;
    reply.SetReplyType( kTypeDebuggerStartedSuccessfully );
    SendReply( reply );
}

void CodeLiteLLDBApp::NotifyStopped()
{
    LLDBReply reply;
    reply.SetReplyType( kTypeDebuggerStopped );
    reply.SetInterruptResaon( m_interruptReason );
    
    lldb::SBThread thread = m_target.GetProcess().GetSelectedThread();
    LLDBBacktrace bt( thread );
    reply.SetBacktrace( bt );
    
    // set the selected frame file:line
    if ( thread.IsValid() && thread.GetSelectedFrame().IsValid() ) {
        lldb::SBFrame frame = thread.GetSelectedFrame();
        lldb::SBLineEntry lineEntry = thread.GetSelectedFrame().GetLineEntry();
        if ( lineEntry.IsValid() ) {
            reply.SetLine(lineEntry.GetLine());
            lldb::SBFileSpec fileSepc = frame.GetLineEntry().GetFileSpec();
            reply.SetFilename( wxFileName(fileSepc.GetDirectory(), fileSepc.GetFilename()).GetFullPath() );
        }
    }
    SendReply( reply );

    // reset the interrupt reason
    m_interruptReason = kInterruptReasonNone;
}

void CodeLiteLLDBApp::NotifyStoppedOnFirstEntry()
{
    LLDBReply reply;
    reply.SetReplyType( kTypeDebuggerStoppedOnFirstEntry );
    SendReply( reply );
}

void CodeLiteLLDBApp::SendReply(const LLDBReply& reply)
{
    try {
        m_replySocket->WriteMessage( reply.ToJSON().format() );

    } catch (clSocketException &e) {
        wxPrintf("codelite-lldb: failed to send reply. %s. %s.\n", e.what().c_str(), strerror(errno));
    }
}

void CodeLiteLLDBApp::RunDebugger(const LLDBCommand& command)
{
    if ( m_debugger.IsValid() ) {

        // Construct char** arrays
        clCommandLineParser parser(command.GetCommandArguments());
        const char** argv = (const char**)_wxArrayStringToCharPtrPtr(parser.ToArray());

        std::string tty_c;
        if ( !command.GetRedirectTTY().IsEmpty() ) {
            tty_c = command.GetRedirectTTY().mb_str(wxConvUTF8).data();
        }
        const char *ptty = tty_c.empty() ? NULL : tty_c.c_str();

        lldb::SBError error;
        lldb::SBListener listener = m_debugger.GetListener();
        lldb::SBProcess process = m_target.Launch(
                                      listener,
                                      argv,
                                      NULL,
                                      ptty,
                                      ptty,
                                      ptty,
                                      NULL,
                                      lldb::eLaunchFlagLaunchInSeparateProcessGroup|lldb::eLaunchFlagStopAtEntry,
                                      true,
                                      error);

        //bool isOk = m_target.LaunchSimple(argv, envp, wd).IsValid();
        DELETE_CHAR_PTR_PTR( const_cast<char**>(argv) );

        if ( !process.IsValid() ) {
            NotifyExited();
            Cleanup();

        } else {
            m_debuggeePid = process.GetProcessID();
            NotifyRunning();
        }
    }
}

void CodeLiteLLDBApp::Cleanup()
{
    wxDELETE( m_networkThread );
    wxDELETE( m_lldbProcessEventThread );
    
    m_interruptReason = kInterruptReasonNone;
    m_debuggeePid = wxNOT_FOUND;
    
    if ( m_target.IsValid() ) {
        m_target.DeleteAllBreakpoints();
        m_target.DeleteAllWatchpoints();
        m_debugger.DeleteTarget( m_target );
    }

    if ( m_debugger.IsValid() ) {
        lldb::SBDebugger::Destroy( m_debugger );
    }
}

void CodeLiteLLDBApp::ApplyBreakpoints(const LLDBCommand& command)
{
    if ( m_target.GetProcess().GetState() == lldb::eStateStopped ) {
        // we can apply the breakpoints
        // Apply all breakpoints with an-invalid breakpoint ID
        LLDBBreakpoint::Vec_t breakpoints = command.GetBreakpoints();
        while( !breakpoints.empty() ) {
            
            LLDBBreakpoint::Ptr_t breakPoint = breakpoints.at(0);
            
            switch( breakPoint->GetType() ) {
            case LLDBBreakpoint::kFunction: {
                m_target.BreakpointCreateByName(breakPoint->GetName().mb_str().data(), NULL);
                break;
            }
            case LLDBBreakpoint::kFileLine: {
                m_target.BreakpointCreateByLocation(breakPoint->GetFilename().mb_str().data(), breakPoint->GetLineNumber());
                break;
            }
            }
            breakpoints.erase(breakpoints.begin());
        }
        NotifyBreakpointsUpdated();

    } else {
        
        // interrupt the process
        m_interruptReason = kInterruptReasonApplyBreakpoints;
        m_target.GetProcess().SendAsyncInterrupt();
    }
}

void CodeLiteLLDBApp::Continue(const LLDBCommand& command)
{
    wxUnusedVar( command );
    m_target.GetProcess().Continue();
}

void CodeLiteLLDBApp::StopDebugger(const LLDBCommand& command)
{
    NotifyExited();
    Cleanup();
}

void CodeLiteLLDBApp::DeleteAllBreakpoints(const LLDBCommand& command)
{
    wxUnusedVar(command);
    if ( m_target.GetProcess().GetState() == lldb::eStateStopped ) {
        m_target.DeleteAllBreakpoints();
        NotifyAllBreakpointsDeleted();
        
    } else {
        m_interruptReason = kInterruptReasonDeleteAllBreakpoints;
        m_target.GetProcess().SendAsyncInterrupt();
    }
}

void CodeLiteLLDBApp::DeleteBreakpoints(const LLDBCommand& command)
{
    const LLDBBreakpoint::Vec_t& bps = command.GetBreakpoints();
    if ( bps.empty() ) {
        return;
    }
    
    bool notifySuccess = false;
    if ( m_target.GetProcess().GetState() == lldb::eStateStopped ) {
        for(size_t i=0; i<bps.size(); ++i) {
            LLDBBreakpoint::Ptr_t breakpoint = bps.at(i);
            if ( breakpoint->IsApplied() ) {
                lldb::SBBreakpoint lldbBreakpoint = m_target.FindBreakpointByID(breakpoint->GetId());
                if ( lldbBreakpoint.IsValid() ) {
                    lldbBreakpoint.ClearAllBreakpointSites();
                    m_target.BreakpointDelete( lldbBreakpoint.GetID() );
                    notifySuccess = true;
                }
            }
        }
        
        if ( notifySuccess ) {
            NotifyBreakpointsUpdated();
        }
        
    } else {
        m_interruptReason = kInterruptReasonDeleteBreakpoint;
        m_target.GetProcess().SendAsyncInterrupt();

    }
}
