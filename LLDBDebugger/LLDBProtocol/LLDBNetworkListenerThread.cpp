//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBNetworkListenerThread.cpp
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

#include "LLDBNetworkListenerThread.h"
#include "LLDBReply.h"
#include "LLDBEvent.h"
#include "file_logger.h"

LLDBNetworkListenerThread::LLDBNetworkListenerThread(wxEvtHandler* owner, const LLDBPivot& pivot, int fd)
    : wxThread(wxTHREAD_JOINABLE)
    , m_owner(owner)
{
    m_socket.reset(new clSocketBase(fd));
    m_pivot = pivot;
}

LLDBNetworkListenerThread::~LLDBNetworkListenerThread() { Stop(); }

void* LLDBNetworkListenerThread::Entry()
{
    while(!TestDestroy()) {
        wxString msg;
        try {
            if(m_socket->ReadMessage(msg, 1) == clSocketBase::kSuccess) {
                LLDBReply reply(msg);
                reply.UpdatePaths(m_pivot);
                switch(reply.GetReplyType()) {
                case kReplyTypeInterperterReply: {
                    LLDBEvent event(wxEVT_LLDB_INTERPERTER_REPLY);
                    event.SetString(reply.GetText());
                    m_owner->AddPendingEvent(event);
                    break;
                }
                case kReplyTypeDebuggerStartedSuccessfully: {
                    // notify debugger started successfully
                    LLDBEvent event(wxEVT_LLDB_STARTED);
                    event.SetSessionType(reply.GetDebugSessionType());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeDebuggerExited: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_EXITED);
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeDebuggerStopped: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_STOPPED);
                    event.SetFileName(reply.GetFilename());
                    event.SetLinenumber(reply.GetLine());
                    event.SetInterruptReason(reply.GetInterruptResaon());
                    event.SetBacktrace(reply.GetBacktrace());
                    event.SetThreads(reply.GetThreads());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeDebuggerRunning: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_RUNNING);
                    m_owner->AddPendingEvent(event);
                    break;
                }
                
                case kReplyTypeLaunchSuccess: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_LAUNCH_SUCCESS);
                    m_owner->AddPendingEvent(event);
                    break;
                }
                
                case kReplyTypeDebuggerStoppedOnFirstEntry: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY);
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeAllBreakpointsDeleted: {
                    LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL);
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeBreakpointsUpdated: {
                    LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
                    event.SetBreakpoints(reply.GetBreakpoints());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeLocalsUpdated: {
                    LLDBEvent event(wxEVT_LLDB_LOCALS_UPDATED);
                    event.SetVariables(reply.GetVariables());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeVariableExpanded: {
                    LLDBEvent event(wxEVT_LLDB_VARIABLE_EXPANDED);
                    event.SetVariables(reply.GetVariables());
                    event.SetVariableId(reply.GetLldbId());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeExprEvaluated: {
                    LLDBEvent event(wxEVT_LLDB_EXPRESSION_EVALUATED);
                    event.SetVariables(reply.GetVariables());
                    event.SetExpression(reply.GetExpression());
                    m_owner->AddPendingEvent(event);
                    break;
                }
                }
            }
        } catch(clSocketException& e) {
            CL_WARNING("Seems like we lost connection to codelite-lldb (probably crashed): %s", e.what().c_str());
            LLDBEvent event(wxEVT_LLDB_CRASHED);
            m_owner->AddPendingEvent(event);

            // exit the main loop
            break;
        }
    }
    return NULL;
}
