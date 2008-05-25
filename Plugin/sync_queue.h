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

#include "wx/thread.h"
#include <deque>


#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

/**
 * A thread safe queue implementation
 */
template <class T> class WXDLLIMPEXP_LE_SDK SyncQueue {
	std::deque<T> m_queue;
	wxCriticalSection m_cs;
public:
	SyncQueue(){}

	virtual ~SyncQueue(){
		wxCriticalSectionLocker lk(m_cs);
		m_queue.clear();
	}

	/**
	 * Add element to the top of the queue
	 */
	void PushFront(T element) {
		wxCriticalSectionLocker lk(m_cs);
		m_queue.push_front(element);
	}

	/**
	 * pop element from the top of the queue
	 */
	T PopFront(){
		wxCriticalSectionLocker lk(m_cs);
		if( m_queue.empty() ){
			return NULL;
		}
		T e = m_queue.front();
		m_queue.pop_front();
		return e;
	}

	/**
	 * Add element to the end of the queue
	 */
	void PushBack(T element) {
		wxCriticalSectionLocker lk(m_cs);
		m_queue.push_back(element);
	}

	/**
	 * pop element from the end of the queue
	 */
	T PopBack(){
		wxCriticalSectionLocker lk(m_cs);
		if( m_queue.empty() ){
			return NULL;
		}
		T e = m_queue.back();
		m_queue.pop_back();
		return e;
	}

	/**
	 * Return number of elements on the queue
	 */
	size_t GetCount() {
		wxCriticalSectionLocker lk(m_cs);
		return m_queue.size();
	}

	/**
	 * Clear the queue - remove all elements from the queue
	 */
	void Clear()  {
		wxCriticalSectionLocker lk(m_cs);
		return m_queue.clear();
	}
};
#endif // SYNC_QUEUE_H
