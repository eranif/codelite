//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : jobqueue.h
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
#ifndef __jobqueue__
#define __jobqueue__

#include <wx/thread.h>
#include <deque>
#include <vector>
#include "codelite_exports.h"
#include <wx/msgqueue.h>

class Job;

/**
 * @class JobQueueWorker
 * @author Eran
 * @date 05/09/08
 * @file jobqueue.h
 * @brief a thread class used internally by JobQueue
 * @sa JobQueue
 */
class WXDLLIMPEXP_SDK JobQueueWorker : public wxThread
{
    wxMessageQueue<Job*>* m_queue;

public:
    /// default ctor/dtor
    JobQueueWorker(wxMessageQueue<Job*>* queue);
    virtual ~JobQueueWorker();

private:
    //declare this class as non copyable
    JobQueueWorker(const JobQueueWorker& rhs);
    JobQueueWorker& operator=(const JobQueueWorker& rhs);

public:
    /**
     * @brief stop the running thread. usually called before calling JobQueue::Release()
     */
    void Stop();

    /**
     * @brief start the thread with given priority
     * @param priority thread priority (defaulted to WXTHREAD_DEFAULT_PRIORITY)
     */
    void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);


    /**
     * @brief implementation of the wxThread pure virtual function Entry()
     * @sa wxThread
     */
    virtual void* Entry();

    /**
     * @brief the main code that processes the job
     * @param job job to process
     * @sa Job
     */
    virtual void ProcessJob(Job *job);
};

/**
 * @class JobQueue
 * @author Eran
 * @date 05/09/08
 * @file jobqueue.h
 * @brief this class provides a convenient way of handling background tasks using Job objects
 *
 * @code
 * // somewhere in your application initialization
 * // start the job queue with 5 workers and default thread prioritization
 * m_jobQueue = new JobQueue();
 * m_jobQueue->Start(5);
 *
 * // whenever you need to add new job to be processed
 * // just add it
 * m_jobQueue->PushJob( new MyJob() );
 *
 * // Shutdown code:
 * m_jobQueue->Stop();
 * // free the resources allocated by the JobQueue and empty the queue
 * delete m_jobQueue;
 * @endcode
 *
 */
class JobQueue
{
    wxMessageQueue<Job*>         m_queue;
    std::vector<JobQueueWorker*> m_threads;

public:
    JobQueue();
    virtual ~JobQueue();

    /**
     * @brief add job to the queue. all jobs must be constructed on the heap.
     * note that the job will be freed when processing is done by the job queue
     * @param job job to process allocated on the heap
     */
    virtual void PushJob(Job *job);

    /**
     * @brief
     * @param poolSize
     * @param priority
     */
    virtual void Start(size_t poolSize = 1, int priority = WXTHREAD_DEFAULT_PRIORITY);

    /**
     * @brief stop all workers threads
     */
    virtual void Stop();
};

/**
 * @class JobQueueSingleton
 * @author Eran
 * @date 05/09/08
 * @file jobqueue.h
 * @brief a wrapper around the JobQueue class that provides the functionality of singleton
 */
class WXDLLIMPEXP_SDK JobQueueSingleton
{
private:
    static JobQueue *ms_instance;

private:
    JobQueueSingleton();
    virtual ~JobQueueSingleton();

public:
    /**
     * @brief
     * @return
     */
    static JobQueue *Instance();

    /**
     * @brief
     */
    static void Release();
};
#endif // __jobqueue__
