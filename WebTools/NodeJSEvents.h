//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSEvents.h
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

#ifndef NODEJSEVENTS_H
#define NODEJSEVENTS_H

#include "cl_command_event.h"
#include "clDebugCallFramesEvent.h"
#include "clDebugRemoteObjectEvent.h"

wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_INTERACT, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_MARK_LINE, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_UPDATE_CONSOLE, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_EVAL_RESULT, clDebugRemoteObjectEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_CREATE_OBJECT, clDebugRemoteObjectEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_OBJECT_PROPERTIES, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_LOCAL_OBJECT_PROPERTIES, clDebugEvent);


// Custom Node v8+ events
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_STARTED, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_STOPPED, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_FINISHED, clDebugEvent);
wxDECLARE_EVENT(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, clDebugCallFramesEvent);
#endif
