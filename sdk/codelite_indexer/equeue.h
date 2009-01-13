#ifndef __equeue__
#define __equeue__

#if defined(__WXMSW__)
#	include "equeue_win_impl.h"
#else
#	include "equeue_unix_impl.h"
#endif

template <class T>
class eQueue
{
protected:
	eQueueImpl<T> *m_impl;

public:
	eQueue();
	virtual ~eQueue();

	bool put(const T& item);
	bool get(T& item, long timeout);
};

//----------------------------
// Implementation
//----------------------------
template<class T>
eQueue<T>::eQueue()
{
	m_impl = new eQueueImpl<T>();
}

template<class T>
eQueue<T>::~eQueue()
{
	delete m_impl;
}

template<class T>
bool eQueue<T>::get(T& item, long timeout)
{
	return m_impl->get(item, timeout);
}

template<class T>
bool eQueue<T>::put(const T& item)
{
	return m_impl->put(item);
}

#endif // __equeue__
