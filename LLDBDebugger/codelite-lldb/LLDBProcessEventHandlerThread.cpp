//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBProcessEventHandlerThread.cpp
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

#include "LLDBProcessEventHandlerThread.h"
#include "file_logger.h"
#include "CodeLiteLLDBApp.h"
#include <lldb/API/SBEvent.h>
#include <wx/wxcrtvararg.h>

LLDBProcessEventHandlerThread::LLDBProcessEventHandlerThread(CodeLiteLLDBApp* app, lldb::SBDebugger& debugger,
                                                             lldb::SBProcess process, eLLDBDebugSessionType sessionType)
    : wxThread(wxTHREAD_JOINABLE)
    , m_app(app)
    , m_process(process)
    , m_debugger(debugger)
    , m_sessionType(sessionType)
{
    m_listener = m_debugger.GetListener();
}

LLDBProcessEventHandlerThread::~LLDBProcessEventHandlerThread()
{
    wxPrintf("codelite-lldb: Process Event Handler thread is going down...\n");
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
    wxPrintf("codelite-lldb: Process Event Handler thread is going down...done\n");
}

#define DEBUG_MSG(s) wxPrintf("codelite-lldb: %s\n", s)

void* LLDBProcessEventHandlerThread::Entry()
{
    bool first_time_stopped = true;
    DEBUG_MSG("LLDB Thread: started");
    // m_listener.StartListeningForEventClass(m_debugger, lldb::SBTarget::GetBroadcasterClassName(),
    // lldb::SBTarget::eBroadcastBitBreakpointChanged);
    while(!TestDestroy()) {
        lldb::SBEvent event;
        if(m_listener.WaitForEvent(1, event) && event.IsValid()) {
            lldb::StateType state = m_process.GetStateFromEvent(event);
            switch(state) {
            case lldb::eStateInvalid:
                DEBUG_MSG("LLDBHandlerThread: eStateInvalid");
                break;

            case lldb::eStateUnloaded:
                DEBUG_MSG("LLDBHandlerThread: eStateUnloaded");
                break;

            case lldb::eStateStopped:
                if(first_time_stopped && m_sessionType != kDebugSessionTypeCore) {
                    first_time_stopped = false;
                    DEBUG_MSG("LLDBHandlerThread: eStateStopped - first time");
                    m_app->CallAfter(&CodeLiteLLDBApp::NotifyStoppedOnFirstEntry);

                } else {
                    DEBUG_MSG("LLDBHandlerThread: eStateStopped");
                    m_app->CallAfter(&CodeLiteLLDBApp::NotifyStopped);
                    m_app->CallAfter(&CodeLiteLLDBApp::NotifyBreakpointsUpdated);
                }
                break;

            case lldb::eStateConnected:
                DEBUG_MSG("LLDBHandlerThread: eStateConnected");
                break;

            case lldb::eStateLaunching:
                DEBUG_MSG("LLDBHandlerThread: eStateLaunching");
                break;

            case lldb::eStateRunning:
                m_app->CallAfter(&CodeLiteLLDBApp::NotifyRunning);
                DEBUG_MSG("LLDBHandlerThread: eStateRunning");
                break;

            case lldb::eStateExited:
                DEBUG_MSG("LLDBHandlerThread: eStateExited");
                m_app->CallAfter(&CodeLiteLLDBApp::NotifyExited);
                break;

            case lldb::eStateAttaching:
                DEBUG_MSG("LLDBHandlerThread: eStateAttaching");
                break;

            case lldb::eStateStepping:
                DEBUG_MSG("LLDBHandlerThread: eStateStepping");
                break;

            case lldb::eStateCrashed:
                DEBUG_MSG("LLDBHandlerThread: eStateCrashed");
                break;

            case lldb::eStateDetached:
                DEBUG_MSG("LLDBHandlerThread: eStateDetached");
                break;

            case lldb::eStateSuspended:
                DEBUG_MSG("LLDBHandlerThread: eStateSuspended");
                break;
            }
        }
    }
    return NULL;
}
