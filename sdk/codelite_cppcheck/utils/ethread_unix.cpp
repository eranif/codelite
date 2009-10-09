#if !defined(__WXMSW__)

#include "ethread_unix.h"
#include "ethread.h"

static void *startFunc(void* arg)
{
	eThread *thread = reinterpret_cast<eThread*>(arg);
	if(thread) {
		// call the thread main loop
		thread->start();
	}
	
	// terminate the thread
	pthread_exit(0);
 
	return 0;
}

eThreadImpl::eThreadImpl()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init( &m_cancelMutex, &attr );
	
	// lock the mutex, this will be used as flag in testDestroy()
	pthread_mutex_lock( &m_cancelMutex );
}

eThreadImpl::~eThreadImpl()
{
}

void eThreadImpl::run(eThread *thread)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	pthread_create(&m_tid, &attr,
	               &startFunc, 		// entry point
	               thread);
}

bool eThreadImpl::testDestroy()
{
	return pthread_mutex_trylock(&m_cancelMutex) == 0;
}

void eThreadImpl::requestStop()
{
	// unlock the cancellation mutex
	pthread_mutex_unlock(&m_cancelMutex);
}

void eThreadImpl::wait(long timeout)
{
	void *exit_code;
	pthread_join(m_tid, &exit_code);
}

#endif // __WXMSW__

