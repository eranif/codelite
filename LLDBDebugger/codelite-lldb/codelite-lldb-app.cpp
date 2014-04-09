#include "codelite-lldb-app.h"
#include <iostream>
#include <wx/sckaddr.h>
#include "LLDBProtocol/LLDBReply.h"
#include "LLDBProtocol/cl_socket_server.h"

IMPLEMENT_APP_CONSOLE(CodeLiteLLDBApp)

CodeLiteLLDBApp::CodeLiteLLDBApp()
    : m_networkThread(NULL)
    , m_debuggeePid(wxNOT_FOUND)
{
    wxSocketBase::Initialize();
    wxPrintf("codelite-lldb: lldb initialized successfully\n");
}

CodeLiteLLDBApp::~CodeLiteLLDBApp()
{
    wxDELETE( m_networkThread );
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
    ::wxSetWorkingDirectory( command.GetWorkingDirectory() );
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
    
    // Notify codelite that the debugger started successfully
    LLDBReply reply;
    reply.SetReasonCode( LLDBReply::kReasonDebuggerStartedSuccessfully );
    reply.SetTranscationId( command.GetTranscationId() );
    m_replySocket->WriteMessage( reply.ToJSON().format() );
}
