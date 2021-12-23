//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : sync_queue.h
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
#ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H

#include "codelite_exports.h"
#include "wx/thread.h"

#include <deque>

/**
 * A thread safe queue implementation
 */
template <class T> class WXDLLIMPEXP_SDK SyncQueue
{
    std::deque<T> m_queue;
    wxMutex m_mutex;
    wxCondition m_cv;

public:
    SyncQueue()
        : m_cv(m_mutex)
    {
    }

    virtual ~SyncQueue() { clear(); }

    /**
     * Add element to the top of the queue
     */
    void push_front(T&& element)
    {
        wxMutexLocker lk(m_mutex);
        m_queue.emplace_front(std::move(element));
        m_cv.Broadcast();
    }

    /**
     * pop element from the top of the queue
     */
    T pop_front()
    {
        wxMutexLocker lk(m_mutex);
        // wait until 10 ms or something is in the queue
        m_cv.WaitTimeout(10);
        if(m_queue.empty()) {
            return nullptr;
        }
        T e = std::move(m_queue.front());
        m_queue.pop_front();
        return e;
    }

    /**
     * Add element to the end of the queue
     */
    void push_back(T&& element)
    {
        wxMutexLocker lk(m_mutex);
        m_queue.emplace_back(std::move(element));
        m_cv.Broadcast();
    }

    /**
     * pop element from the end of the queue
     */
    T pop_back()
    {
        wxMutexLocker lk(m_mutex);
        // wait until 10 ms or something is in the queue
        m_cv.WaitTimeout(10);
        if(m_queue.empty()) {
            return nullptr;
        }

        T e = std::move(m_queue.back());
        m_queue.pop_back();
        return std::move(e);
    }

    /**
     * Return number of elements on the queue
     */
    size_t size()
    {
        wxMutexLocker lk(m_mutex);
        return m_queue.size();
    }

    /**
     * Clear the queue - remove all elements from the queue
     */
    void clear()
    {
        wxMutexLocker lk(m_mutex);
        m_queue.clear();
    }
};
#endif // SYNC_QUEUE_H
