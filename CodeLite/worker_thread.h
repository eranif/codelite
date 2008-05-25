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
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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


#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE


/**
 * Base class for thread requests,
 */
class WXDLLIMPEXP_CL ThreadRequest{
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
	wxCriticalSection m_cs;
	wxEvtHandler *m_notifiedWindow;
	std::deque<ThreadRequest*> m_queue;

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
	virtual void *Entry();

	/**
	 * Called when the thread exits
	 * whether it terminates normally or is stopped with Delete() (but not when it is Kill()'ed!)
	 */
	virtual void OnExit(){};

	/**
	 * Add a request to the worker thread
	 * \param request request to execute. 
	 */
	void Add(ThreadRequest *request);

	/**
	 * Set the window to be notified when a change was done
	 * between current source file tree and the actual tree.
	 * \param evtHandler
	 */
	void SetNotifyWindow( wxEvtHandler* evtHandler ) { m_notifiedWindow  = evtHandler; }

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
	virtual void ProcessRequest(ThreadRequest *request) = 0;

protected:
	/**
	 * Get next request from queue.
	 * \param file [output] source file that was updated
	 * \param project [output] project name where file belongs to
	 * \param dbfile [output] dataabase file name
	 * \return true if there is a request to process
	 */
	ThreadRequest* GetRequest();
};

#endif // WORKER_THREAD_H
