//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : singleinstancethreadjob.cpp
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

#include "singleinstancethreadjob.h"

#include "JSON.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"

#include <wx/filename.h>

wxDEFINE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, clCommandEvent);

clSingleInstanceThread::clSingleInstanceThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

clSingleInstanceThread::~clSingleInstanceThread() { Stop(); }

void* clSingleInstanceThread::Entry()
{
    try {
        wxString connectionString;
        connectionString << "tcp://127.0.0.1:" << SINGLE_INSTANCE_PORT;
        m_serverSocket.Start(connectionString);
        clDEBUG() << "clSingleInstanceThread: starting on:" << connectionString << clEndl;

        while(!TestDestroy()) {
            // wait for a new connection
            clSocketBase::Ptr_t client = m_serverSocket.WaitForNewConnection(1);
            if(!client)
                continue;

            wxString message;
            if(client->ReadMessage(message, 3) == clSocketBase::kTimeout)
                continue;
            clDEBUG() << "clSingleInstanceThread: received new message:" << message << endl;

            JSON root(message);
            wxArrayString args = root.toElement().namedObject("args").toArrayString();

            if(args.IsEmpty()) {
                // just raise codelite
                clCommandEvent event(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP);
                EventNotifier::Get()->AddPendingEvent(event);
            } else {
                clCommandEvent event(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES);
                event.SetStrings(args);
                EventNotifier::Get()->AddPendingEvent(event);
            }
        }
    } catch(clSocketException& e) {
        clDEBUG() << "Failed to create single instance socket:" << e.what() << endl;
        clDEBUG() << "Another instance is already running" << endl;
    }
    return NULL;
}
