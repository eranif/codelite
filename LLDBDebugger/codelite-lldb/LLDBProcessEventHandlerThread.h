//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBProcessEventHandlerThread.h
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

#ifndef LLDBHANDLERTHREAD_H
#define LLDBHANDLERTHREAD_H

#include <wx/thread.h>
#include <lldb/API/SBListener.h>
#include <lldb/API/SBProcess.h>
#include "LLDBProtocol/LLDBEnums.h"

/**
 * @class LLDBHandlerThread
 * @author eran
 * @brief Wait for the process events and report any changes to the application
 */
class CodeLiteLLDBApp;
class LLDBProcessEventHandlerThread : public wxThread
{
    CodeLiteLLDBApp* m_app;
    lldb::SBProcess m_process;
    lldb::SBDebugger& m_debugger;
    lldb::SBListener m_listener;
    eLLDBDebugSessionType m_sessionType;

public:
    LLDBProcessEventHandlerThread(
        CodeLiteLLDBApp* app, lldb::SBDebugger& debugger, lldb::SBProcess process, eLLDBDebugSessionType sessionType);
    virtual ~LLDBProcessEventHandlerThread();

    void Start()
    {
        Create();
        Run();
    }

protected:
    virtual void* Entry();
};
#endif // LLDBHANDLERTHREAD_H
