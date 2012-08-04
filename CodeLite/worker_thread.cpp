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
, m_notifiedWindow( NULL )
, m_sleep(200)
{
}

WorkerThread::~WorkerThread()
{
	if( !m_queue.empty() ){
		std::deque<ThreadRequest*>::iterator iter = m_queue.begin();
		for(; iter != m_queue.end(); iter++){
			delete (*iter);
		}
		m_queue.clear();
	}
}

void* WorkerThread::Entry()
{
	while( true )
	{
		// Did we get a request to terminate?
		if(TestDestroy())
			break;

		ThreadRequest *request = GetRequest();
		if( request )
		{
			// Call user's implementation for processing request
			ProcessRequest( request );
			delete request;

			wxThread::Sleep(10);  // Allow other threads to work as well
			request = NULL;
			continue; // to avoid the sleep
		}

		// Sleep for 1 seconds, and then try again
		wxThread::Sleep(m_sleep);
	}
	return NULL;
}

void WorkerThread::Add(ThreadRequest *request)
{
	wxCriticalSectionLocker locker(m_cs);
	m_queue.push_back(request);
}

ThreadRequest *WorkerThread::GetRequest()
{
	wxCriticalSectionLocker locker(m_cs);
	ThreadRequest *req = NULL;
	if( !m_queue.empty() ){
		req = m_queue.front();
		m_queue.pop_front();
	}
	return req;
}

void WorkerThread::Stop()
{
#if wxVERSION_NUMBER < 2904
    if(IsAlive()) {
        Delete();
    }
    Wait();
#else    
    // Notify the thread to exit and 
    // wait for it
    if ( IsAlive() ) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
        
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
        
    }
#endif

}

void WorkerThread::Start(int priority)
{
	Create();
	SetPriority(priority);
	Run();
}

void WorkerThread::SetSleepInterval(size_t ms)
{
	m_sleep = ms;
}

