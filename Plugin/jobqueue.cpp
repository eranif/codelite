//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : jobqueue.cpp
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
 #include "jobqueue.h"
#include "job.h"

JobQueueWorker::JobQueueWorker(wxCriticalSection *cs, std::deque<Job*> *queue)
		: wxThread(wxTHREAD_JOINABLE)
		, m_cs(cs)
		, m_queue(queue)
{
}

JobQueueWorker::~JobQueueWorker()
{
}
void JobQueueWorker::Stop()
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

void JobQueueWorker::Start(int priority)
{
	Create();
	SetPriority(priority);
	Run();
}

Job *JobQueueWorker::GetJob()
{
	wxCriticalSectionLocker locker(*m_cs);
	Job *req(NULL);
	if ( !m_queue->empty() ) {
		req = m_queue->front();
		m_queue->pop_front();
	}
	return req;
}

void* JobQueueWorker::Entry()
{
	while ( true ) {
		// Did we get a request to terminate?
		if (TestDestroy())
			break;

		Job *job = GetJob();
		if ( job ) {
			// Call user's implementation for processing request
			ProcessJob( job );

			wxThread::Sleep(10);  // Allow other threads to work as well
			delete job;
			job = NULL;
			continue; // to avoid the sleep
		}

		// Sleep for 200ms , and then try again
		wxThread::Sleep(200);
	}
	return NULL;
}

void JobQueueWorker::ProcessJob(Job *job)
{
	if ( job ) {
		job->Process(this);
	}
}

//--------------------------------------------------------------
// JobQueue
//--------------------------------------------------------------

JobQueue::JobQueue()
{
}

JobQueue::~JobQueue()
{
	if ( !m_queue.empty() ) {
		std::deque<Job*>::iterator iter = m_queue.begin();
		for (; iter != m_queue.end(); iter++) {
			delete (*iter);
		}
		m_queue.clear();
	}
}

void JobQueue::PushJob(Job *job)
{
	wxCriticalSectionLocker locker(m_cs);
	m_queue.push_front(job);
}

void JobQueue::Start(size_t poolSize, int priority)
{
	size_t maxPoolSize = poolSize > 250 ? 250 : poolSize;
	for(size_t i=0; i<maxPoolSize; i++) {
		//create new thread
		JobQueueWorker *worker = new JobQueueWorker(&m_cs, &m_queue);
		worker->Start(priority);
		m_threads.push_back(worker);
	}
}

void JobQueue::Stop()
{
	//first loop and stop all running threads
	for(size_t i=0; i<m_threads.size(); i++){
		JobQueueWorker *worker = m_threads.at(i);
		//stop it
		worker->Stop();
		//delete it
		delete worker;
	}
	m_threads.clear();
}

//-----------------------------------------------------
// Job queue singleton
//-----------------------------------------------------
JobQueue* JobQueueSingleton::ms_instance = new JobQueue();

JobQueueSingleton::JobQueueSingleton()
{
}

JobQueueSingleton::~JobQueueSingleton()
{
}

JobQueue* JobQueueSingleton::Instance()
{
	return ms_instance;
}

void JobQueueSingleton::Release()
{
	if (ms_instance) {
		delete ms_instance;
	}
	ms_instance = 0;
}
