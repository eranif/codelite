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
#ifndef _ProcessReaderThread_H_
#define _ProcessReaderThread_H_

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wx/event.h"
#include "wx/thread.h"
#include <atomic>
#include <deque>

class IProcess;
/**
 * Worker Thread class
 * usually user should define the ProcessRequest method
 */
class WXDLLIMPEXP_CL ProcessReaderThread : public wxThread
{
protected:
    wxEvtHandler* m_notifiedWindow = nullptr;
    IProcess* m_process = nullptr;
    std::atomic_bool m_suspend;
    std::atomic_bool m_is_suspended;

protected:
    void NotifyTerminated();

public:
    /**
     * Default constructor.
     */
    ProcessReaderThread();

    /**
     * Destructor.
     */
    virtual ~ProcessReaderThread();

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
     * Set the window to be notified when a change was done
     * between current source file tree and the actual tree.
     * \param evtHandler
     */
    void SetNotifyWindow(wxEvtHandler* evtHandler) { m_notifiedWindow = evtHandler; }

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

    void SetProcess(IProcess* proc) { m_process = proc; }

    /**
     * @brief suspend the reader thread
     */
    void Suspend();

    /**
     * @brief resume the reader thread
     */
    void Resume();
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_PROCESS_OUTPUT, clProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_PROCESS_STDERR, clProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ASYNC_PROCESS_TERMINATED, clProcessEvent);

#endif // _ProcessReaderThread_H_
