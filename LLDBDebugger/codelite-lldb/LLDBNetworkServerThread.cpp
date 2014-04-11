#include "LLDBNetworkServerThread.h"
#include "LLDBProtocol/cl_socket_server.h"
#include "LLDBProtocol/LLDBCommand.h"
#include "codelite-lldb/CodeLiteLLDBApp.h"

LLDBNetworkServerThread::LLDBNetworkServerThread(CodeLiteLLDBApp* app, int fd)
    : wxThread(wxTHREAD_JOINABLE)
    , m_app(app)
{
    m_socket.reset( new clSocketBase(fd) );
}

LLDBNetworkServerThread::~LLDBNetworkServerThread()
{
    if ( IsAlive() ) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
        
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
    wxPrintf("codelite-lldb: Network server thread exited\n");
}

void* LLDBNetworkServerThread::Entry()
{
    clSocketServer server;
    wxPrintf("codelite-lldb: successfully started network thread\n");
    wxPrintf("codelite-lldb: waiting for new connection\n");
    try {

        // we got connection, enter the main loop
        while ( !TestDestroy() ) {
            wxString str;
            if ( m_socket->ReadMessage( str, 1 ) == clSocketBase::kSuccess ) {
                // Process command
                LLDBCommand command(str);
                switch( command.GetCommandType() ) {
                case kCommandStart:
                    m_app->CallAfter( &CodeLiteLLDBApp::StartDebugger, command );
                    break;
                    
                case kCommandRun:
                    m_app->CallAfter( &CodeLiteLLDBApp::RunDebugger, command);
                    break;
                    
                case kCommandApplyBreakpoints:
                    m_app->CallAfter( &CodeLiteLLDBApp::ApplyBreakpoints, command);
                    break;
                    
                case kCommandContinue:
                    m_app->CallAfter( &CodeLiteLLDBApp::Continue, command);
                    break;
                    
                case kCommandStop:
                    m_app->CallAfter( &CodeLiteLLDBApp::StopDebugger, command);
                    break;
                
                case kCommandDeleteBreakpoint:
                    m_app->CallAfter( &CodeLiteLLDBApp::DeleteBreakpoints, command);
                    break;

                case kCommandDeleteAllBreakpoints:
                    m_app->CallAfter( &CodeLiteLLDBApp::DeleteAllBreakpoints, command);
                    break;

                default:
                    break;
                }
            }
        }

    } catch ( clSocketException& e ) {
        // make sure we don't close the descriptor upon exit
        m_socket->Release();
        wxPrintf("Error while reading message. %s\n", e.what().c_str());
        return NULL;
    }
    
    // make sure we don't close the descriptor upon exit
    m_socket->Release();
    return NULL;
}
