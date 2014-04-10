#include "CodeLiteLLDBApp.h"
#include <iostream>
#include <wx/sckaddr.h>
#include "LLDBProtocol/LLDBReply.h"
#include "LLDBProtocol/cl_socket_server.h"
#include "LLDBProtocol/LLDBEnums.h"
#include "clcommandlineparser.h"


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
    if ( command.GetWorkingDirectory().IsEmpty() ) {
    ::wxSetWorkingDirectory( command.GetWorkingDirectory() );
    }
    wxPrintf("codelite-lldb: working directory is set to %s\n", ::wxGetCwd());
    
#ifdef __WXMAC__
    // On OSX, debugserver executable must exists otherwise lldb will not work properly
    // we ensure that it exists by checking the environment variable LLDB_DEBUGSERVER_PATH
    wxString lldbDebugServer;
    if ( !::wxGetEnv("LLDB_DEBUGSERVER_PATH", &lldbDebugServer) || !wxFileName::Exists(lldbDebugServer) ) {
        std::cerr << "LLDB_DEBUGSERVER_PATH environment does not exist or contains a path to a non existent file" << std::endl;
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
}

void CodeLiteLLDBApp::NotifyBacktrace()
{
}

void CodeLiteLLDBApp::NotifyBreakpointsUpdated()
{
}

void CodeLiteLLDBApp::NotifyExited()
{
    LLDBReply reply;
    reply.SetReplyType( kTypeDebuggerExited );
    SendReply( reply );
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
    reply.SetReplyType( kTypeDebuggerStartedSuccessfully );
    reply.SetInterruptResaon( m_interruptReason );
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

    wxDELETE( m_networkThread );
    wxDELETE( m_lldbProcessEventThread );
}
