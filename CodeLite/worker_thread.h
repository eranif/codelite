//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : worker_thread.h
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
#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <deque>
#include "wx/thread.h"
#include "wx/event.h"
#include "codelite_exports.h"
#include <wx/msgqueue.h>
#include <queue>
#include <condition_variable>
#include <mutex>

/**
 * Base class for thread requests,
 */
class WXDLLIMPEXP_CL ThreadRequest
{
public:
    ThreadRequest(){};
    virtual ~ThreadRequest(){};
};

/**
 * Worker Thread class
 * usually user should define the ProcessRequest method
 */
class WXDLLIMPEXP_CL WorkerThread : public wxThread
{
protected:
    wxEvtHandler* m_notifiedWindow;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<ThreadRequest*> m_Q;

public:
    /**
     * Default constructor.
     */
    WorkerThread();

    /**
     * Destructor.
     */
    virtual ~WorkerThread();

    /**
     * Thread execution point.
     */
    virtual void* Entry();

    /**
     * Called when the thread exits
     * whether it terminates normally or is stopped with Delete() (but not when it is Kill()'ed!)
     */
    virtual void OnExit(){};

    /**
     * Add a request to the worker thread
     * \param request request to execute.
     */
    void Add(ThreadRequest* request);

    /**
     * @brief clear the request queue
     */
    void ClearQueue();
    
    /**
     * Set the window to be notified when a change was done
     * between current source file tree and the actual tree.
     * \param evtHandler
     */
    void SetNotifyWindow(wxEvtHandler* evtHandler) { m_notifiedWindow = evtHandler; }

    wxEvtHandler* GetNotifiedWindow() { return m_notifiedWindow; }
    /**
     * Stops the thread
     * This function returns only when the thread is terminated.
     * \note This call must be called from the context of other thread (e.g. main thread)
     */
    void Stop();

    /**
     * Start the thread as joinable thread.
     * \note This call must be called from the context of other thread (e.g. main thread)
     */
    void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);

    /**
     * Process request from the other thread
     * \param request ThreadRequest object to process
     */
    virtual void ProcessRequest(ThreadRequest* request) = 0;
};

#endif // WORKER_THREAD_H
