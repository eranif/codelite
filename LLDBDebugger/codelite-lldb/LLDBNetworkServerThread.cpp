//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBNetworkServerThread.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "LLDBNetworkServerThread.h"
#include "SocketAPI/clSocketServer.h"
#include "LLDBProtocol/LLDBCommand.h"
#include "codelite-lldb/CodeLiteLLDBApp.h"
#include <wx/wxcrtvararg.h>

LLDBNetworkServerThread::LLDBNetworkServerThread(CodeLiteLLDBApp* app, socket_t fd)
    : wxThread(wxTHREAD_JOINABLE)
    , m_app(app)
{
    m_socket.reset(new clSocketBase(fd));
    // we don't own the socket, so don't close it when we are going down
    m_socket->SetCloseOnExit(false);
}

LLDBNetworkServerThread::~LLDBNetworkServerThread()
{
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void* LLDBNetworkServerThread::Entry()
{
    clSocketServer server;
    wxPrintf("codelite-lldb: successfully started network thread. Reading input on fd=%d\n",
             (int)m_socket->GetSocket());
    try {

        // we got connection, enter the main loop
        while(!TestDestroy()) {
            wxString str;
            if(m_socket->ReadMessage(str, 1) == clSocketBase::kSuccess) {
                // wxPrintf("codelite-lldb: received command\n%s\n", str);

                // Process command
                LLDBCommand command(str);
                switch(command.GetCommandType()) {
                case kCommandInterperterCommand:
                    m_app->CallAfter(&CodeLiteLLDBApp::ExecuteInterperterCommand, command);
                    break;

                case kCommandAddWatch:
                    m_app->CallAfter(&CodeLiteLLDBApp::AddWatch, command);
                    break;
                case kCommandDeleteWatch:
                    m_app->CallAfter(&CodeLiteLLDBApp::DeleteWatch, command);
                    break;

                case kCommandNextInstruction:
                    m_app->CallAfter(&CodeLiteLLDBApp::NextInstruction, command);
                    break;

                case kCommandCurrentFileLine:
                    m_app->CallAfter(&CodeLiteLLDBApp::ShowCurrentFileLine, command);
                    break;

                case kCommandStart:
                    m_app->CallAfter(&CodeLiteLLDBApp::StartDebugger, command);
                    break;

                case kCommandDebugCoreFile:
                    m_app->CallAfter(&CodeLiteLLDBApp::OpenCoreFile, command);
                    break;

                case kCommandAttachProcess:
                    m_app->CallAfter(&CodeLiteLLDBApp::AttachProcess, command);
                    break;

                case kCommandRun:
                    m_app->CallAfter(&CodeLiteLLDBApp::RunDebugger, command);
                    break;

                case kCommandApplyBreakpoints:
                    m_app->CallAfter(&CodeLiteLLDBApp::ApplyBreakpoints, command);
                    break;

                case kCommandContinue:
                    m_app->CallAfter(&CodeLiteLLDBApp::Continue, command);
                    break;

                case kCommandStop:
                    m_app->CallAfter(&CodeLiteLLDBApp::StopDebugger, command);
                    break;

                case kCommandDetach:
                    m_app->CallAfter(&CodeLiteLLDBApp::DetachDebugger, command);
                    break;

                case kCommandDeleteBreakpoint:
                    m_app->CallAfter(&CodeLiteLLDBApp::DeleteBreakpoints, command);
                    break;

                case kCommandDeleteAllBreakpoints:
                    m_app->CallAfter(&CodeLiteLLDBApp::DeleteAllBreakpoints, command);
                    break;

                case kCommandNext:
                    m_app->CallAfter(&CodeLiteLLDBApp::Next, command);
                    break;

                case kCommandStepIn:
                    m_app->CallAfter(&CodeLiteLLDBApp::StepIn, command);
                    break;

                case kCommandStepOut:
                    m_app->CallAfter(&CodeLiteLLDBApp::StepOut, command);
                    break;

                case kCommandInterrupt:
                    m_app->CallAfter(&CodeLiteLLDBApp::Interrupt, command);
                    break;

                case kCommandGetLocals:
                    m_app->CallAfter(&CodeLiteLLDBApp::LocalVariables, command);
                    break;

                case kCommandExpandVariable:
                    m_app->CallAfter(&CodeLiteLLDBApp::ExpandVariable, command);
                    break;

                case kCommandSelectFrame:
                    m_app->CallAfter(&CodeLiteLLDBApp::SelectFrame, command);
                    break;

                case kCommandSelectThread:
                    m_app->CallAfter(&CodeLiteLLDBApp::SelectThread, command);
                    break;

                case kCommandEvalExpression:
                    m_app->CallAfter(&CodeLiteLLDBApp::EvalExpression, command);
                    break;

                case kCommandRunTo:
                    m_app->CallAfter(&CodeLiteLLDBApp::RunTo, command);
                    break;

                case kCommandJumpTo:
                    m_app->CallAfter(&CodeLiteLLDBApp::JumpTo, command);
                    break;

                case kCommandSuspendThreads:
                    m_app->CallAfter(&CodeLiteLLDBApp::SuspendThreads, command);
                    break;

                case kCommandSuspendOtherThreads:
                    m_app->CallAfter(&CodeLiteLLDBApp::SuspendOtherThreads, command);
                    break;

                case kCommandResumeThreads:
                    m_app->CallAfter(&CodeLiteLLDBApp::ResumeThreads, command);
                    break;

                case kCommandResumeOtherThreads:
                    m_app->CallAfter(&CodeLiteLLDBApp::ResumeOtherThreads, command);
                    break;

                case kCommandResumeAllThreads:
                    m_app->CallAfter(&CodeLiteLLDBApp::ResumeAllThreads, command);
                    break;

                case kCommandSetVariableValue:
                    m_app->CallAfter(&CodeLiteLLDBApp::SetVariableValue, command);
                    break;

                case kCommandSetVariableDisplayFormat:
                    m_app->CallAfter(&CodeLiteLLDBApp::SetVariableDisplayFormat, command);
                    break;

                default:
                    break;
                }
            }
        }

    } catch(clSocketException& e) {
        wxPrintf("codelite-lldb: error while reading message. %s\n", e.what().c_str());
        m_app->CallAfter(&CodeLiteLLDBApp::NotifyAborted);
        return NULL;
    }

    wxPrintf("codelite-lldb: network thread: leaving main loop\n");
    return NULL;
}
