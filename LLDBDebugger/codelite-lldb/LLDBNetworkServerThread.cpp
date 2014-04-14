#ifndef __WXMSW__

#include "LLDBNetworkServerThread.h"
#include "LLDBProtocol/cl_socket_server.h"
#include "LLDBProtocol/LLDBCommand.h"
#include "codelite-lldb/CodeLiteLLDBApp.h"
#include <wx/wxcrtvararg.h>

LLDBNetworkServerThread::LLDBNetworkServerThread(CodeLiteLLDBApp* app, socket_t fd)
    : wxThread(wxTHREAD_JOINABLE)
    , m_app(app)
{
    m_socket.reset( new clSocketBase(fd) );
    // we don't own the socket, so don't close it when we are going down
    m_socket->SetCloseOnExit(false);
}

LLDBNetworkServerThread::~LLDBNetworkServerThread()
{
    if ( IsAlive() ) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void* LLDBNetworkServerThread::Entry()
{
    clSocketServer server;
    wxPrintf("codelite-lldb: successfully started network thread. Reading input on fd=%d\n", (int)m_socket->GetSocket());
    try {

        // we got connection, enter the main loop
        while ( !TestDestroy() ) {
            wxString str;
            if ( m_socket->ReadMessage( str, 1 ) == clSocketBase::kSuccess ) {
                //wxPrintf("codelite-lldb: received command\n%s\n", str);
                
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

                case kCommandNext:
                    m_app->CallAfter( &CodeLiteLLDBApp::Next, command);
                    break;

                case kCommandStepIn:
                    m_app->CallAfter( &CodeLiteLLDBApp::StepIn, command);
                    break;

                case kCommandStepOut:
                    m_app->CallAfter( &CodeLiteLLDBApp::StepOut, command);
                    break;

                case kCommandInterrupt:
                    m_app->CallAfter( &CodeLiteLLDBApp::Interrupt, command);
                    break;
                
                case kCommandGetLocals:
                    m_app->CallAfter( &CodeLiteLLDBApp::LocalVariables, command);
                    break;
                
                case kCommandExpandVariable:
                    m_app->CallAfter( &CodeLiteLLDBApp::ExpandVariable, command);
                    break;
                    
                default:
                    break;
                }
            }
        }

    } catch ( clSocketException& e ) {
        wxPrintf("codelite-lldb: error while reading message. %s\n", e.what().c_str());
        return NULL;
    }

    wxPrintf("codelite-lldb: network thread: leaving main loop\n");
    return NULL;
}
#endif /// !__WXMSW__
