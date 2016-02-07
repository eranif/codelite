#if defined(__WXMSW__)

#ifndef __equeue_win_h__
#define __equeue_win_h__

#include <windows.h>
#include <list>

template <class T>
class eQueueImpl
{
protected:
	CRITICAL_SECTION m_cs;
	HANDLE m_event;
	std::list<T> m_queue;

public:
	eQueueImpl();
	virtual ~eQueueImpl();

	bool put(const T& item);
	bool get(T& item, long timeout);
};

//----------------------------
// Implementation
//----------------------------
template<class T>
eQueueImpl<T>::eQueueImpl()
{
	// Create a manual reset event initially signalled.
	// This event will be signalled and shall remain so whenever there is data in the queue and
	// it shall be reset as long as queue is empty
	InitializeCriticalSection(&m_cs);
	m_event = CreateEvent(NULL, TRUE, FALSE, NULL);
}

template<class T>
eQueueImpl<T>::~eQueueImpl()
{
	DeleteCriticalSection(&m_cs);
	CloseHandle(m_event);
	m_queue.clear();
}

template<class T>
bool eQueueImpl<T>::get(T& item, long timeout)
{
	bool cont ( true );
	while (cont) {

		DWORD dwRet = WaitForSingleObject(m_event, timeout);
		switch (dwRet) {
		case WAIT_OBJECT_0:
			// we have something on the queue, test it
			EnterCriticalSection( &m_cs );
			if (m_queue.empty()) {

				// probably another thread beat us to this portion of the code
				// keep on waiting
				cont = true;
				LeaveCriticalSection( &m_cs );
				break;

			} else {

				cont = false;
				// we got an item from the queue, copy and extract it
				item = m_queue.front();
				m_queue.pop_front();

				// if the queue is empty reset the event to 'non-signaled' state
				if (m_queue.empty()) {
					ResetEvent(m_event);
				}
				LeaveCriticalSection( &m_cs );
				return true;
			}
		default:
			// timeout or other error occurred here
			return false;
		}
	}
	return false;
}

template<class T>
bool eQueueImpl<T>::put(const T& item)
{
	EnterCriticalSection( &m_cs );
	m_queue.push_back(item);
	SetEvent(m_event);
	LeaveCriticalSection( &m_cs );
	return true;
}

#endif // __equeue_win_h__

#endif
