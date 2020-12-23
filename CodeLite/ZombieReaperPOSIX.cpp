//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ZombieReaperPOSIX.cpp
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

#include "ZombieReaperPOSIX.h"
#include "event_notifier.h"

#ifndef __WXMSW__

#include "file_logger.h"
#include <sys/wait.h>

wxDEFINE_EVENT(wxEVT_CL_PROCESS_TERMINATED, wxProcessEvent);

ZombieReaperPOSIX::ZombieReaperPOSIX()
    : wxThread(wxTHREAD_JOINABLE)
{
    CL_DEBUG("ZombieReaperPOSIX: started");
}

ZombieReaperPOSIX::~ZombieReaperPOSIX() {}

void* ZombieReaperPOSIX::Entry()
{
    while(!TestDestroy()) {
        int status(0);
        pid_t pid = ::waitpid((pid_t)-1, &status, WNOHANG);
        if(pid > 0) {
            // Notify about this process termination
            wxProcessEvent event(0, pid, status);
            event.SetEventType(wxEVT_CL_PROCESS_TERMINATED);
            EventNotifier::Get()->AddPendingEvent(event);
            clDEBUG() << "ZombieReaperPOSIX: process" << (int)pid << "exited with status code" << status << clEndl;
        }
        wxThread::Sleep(500);
    }

    clDEBUG() << "ZombieReaperPOSIX: going down" << clEndl;
    return NULL;
}

void ZombieReaperPOSIX::Stop()
{
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}
#endif
