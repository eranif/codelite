//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : worker_thread.cpp
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
#include "worker_thread.h"

WorkerThread::WorkerThread()
    : wxThread(wxTHREAD_JOINABLE)
    , m_notifiedWindow(NULL)
{
}

WorkerThread::~WorkerThread() { Stop(); }

void* WorkerThread::Entry()
{
    while(true) {
        // Did we get a request to terminate?
        if(TestDestroy()) break;
        ThreadRequest* request = NULL;
        if(m_queue.ReceiveTimeout(50, request) == wxMSGQUEUE_NO_ERROR) {
            // Call user's implementation for processing request
            ProcessRequest(request);
            wxDELETE(request);
        }
    }
    return NULL;
}

void WorkerThread::Add(ThreadRequest* request)
{
    if(!request) { return; }
    m_queue.Post(request);
}

void WorkerThread::Stop()
{
    // Notify the thread to exit and
    // wait for it
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);

    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void WorkerThread::Start(int priority)
{
    Create();
    SetPriority(priority);
    Run();
}
