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

static ThreadRequest* QueueGet(std::mutex& m, std::queue<ThreadRequest*>& q, std::condition_variable& cv)
{
    std::unique_lock<std::mutex> lock{ m }; // acquiring the mutex
    while(q.empty()) {
        cv.wait(lock); // waiting to be notified of new data
    }

    ThreadRequest* req = q.front(); // moving the front element
    q.pop();                        // which is immediately deleted

    return req;
}

static void QueuePut(std::mutex& m, std::queue<ThreadRequest*>& q, std::condition_variable& cv, ThreadRequest* req)
{
    std::unique_lock<std::mutex> lock{ m }; // Acquires the mutex
    q.push(req);                            // Forward the param to the
    lock.unlock();                          // operation on the wrapped collection
    cv.notify_one();
}

void* WorkerThread::Entry()
{
    while(true) {
        // Did we get a request to terminate?
        if(TestDestroy()) break;

        // Get the next entry from the queue
        ThreadRequest* request = QueueGet(m_mutex, m_Q, m_cv);
        if(request == nullptr) {
            // this dummy message was sent to tell us to exit
            break;
        }
        // Call user's implementation for processing request
        ProcessRequest(request);
        wxDELETE(request);
    }
    return NULL;
}

void WorkerThread::Add(ThreadRequest* request)
{
    if(!request) { return; }
    QueuePut(m_mutex, m_Q, m_cv, request);
}

void WorkerThread::Stop()
{
    // Notify the thread to exit and
    // wait for it
    QueuePut(m_mutex, m_Q, m_cv, nullptr); // Make sure that the thread wakes up

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

void WorkerThread::ClearQueue()
{
    std::unique_lock<std::mutex> lk(m_mutex);
    while(!m_Q.empty()) {
        m_Q.pop();
    }
}
