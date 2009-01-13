#if !defined(__WXMSW__)

#ifndef __equeue_unix_impl_h__
#define __equeue_unix_impl_h__

#include <pthread.h>
#include <list>
#include <errno.h>
#include <sys/time.h>

template <class T>
class eQueueImpl
{
protected:
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	std::list<T> m_queue;

public:
	eQueueImpl();
	virtual ~eQueueImpl();

	bool put ( const T& item );
	bool get ( T& item, long timeout );
};

//----------------------------
// Implementation
//----------------------------
template<class T>
eQueueImpl<T>::eQueueImpl()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init ( &attr );
	pthread_mutexattr_settype ( &attr, PTHREAD_MUTEX_NORMAL );
	pthread_mutex_init ( &m_mutex, &attr );

	pthread_condattr_t cattr;
	pthread_condattr_init ( &cattr );
	pthread_cond_init ( &m_cond, &cattr );
}

template<class T>
eQueueImpl<T>::~eQueueImpl()
{
}

template<class T>
bool eQueueImpl<T>::get ( T& item, long timeout )
{
	struct timespec	ts;
	struct timeval	tv;

	pthread_mutex_lock ( &m_mutex );

	while ( m_queue.empty() ) {
		gettimeofday ( &tv, NULL );

		/* Convert from timeval to timespec */
		ts.tv_sec  = tv.tv_sec;
		ts.tv_nsec = tv.tv_usec * 1000;

		// add our timeout
		ts.tv_sec  += ( timeout / 1000 );
		ts.tv_nsec += ( timeout % 1000 ) * 1000000;

		// nsec can not bre greater than 1000000000 (sec)
		if ( ts.tv_nsec >= 1000000000 ) {
			ts.tv_nsec -= 1000000000;
			++ts.tv_sec;
		}

		int rc = pthread_cond_timedwait ( &m_cond, &m_mutex, &ts );
		if ( rc == ETIMEDOUT ) {
			pthread_mutex_unlock ( &m_mutex );
			return false;
		}
	}

	// get copy of the data
	item = m_queue.front();
	m_queue.pop_front();

	pthread_mutex_unlock ( &m_mutex );

	return true;
}

template<class T>
bool eQueueImpl<T>::put ( const T& item )
{
	pthread_mutex_lock ( &m_mutex );
	bool was_empty = m_queue.empty();
	m_queue.push_back ( item );
	if ( was_empty ) {
		pthread_cond_signal ( &m_cond );
	}
	pthread_mutex_unlock ( &m_mutex );
	return true;
}

#endif // __equeue_unix_impl_h__

#endif // !defined(__WXMSW__)
