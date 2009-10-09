#ifndef __WXMSW__

#ifndef __ethread_unix_h__
#define __ethread_unix_h__

#include <pthread.h>

typedef pthread_t eThreadId;

class eThread;

/**
 * \class eThreadImpl 
 * \author Eran
 * \date 08/10/08
 * \file ethread.h
 * \brief Unix thread implemenation
 */
class eThreadImpl
{
	eThreadId m_tid;
	pthread_mutex_t m_cancelMutex;
	
public:
	eThreadImpl();

	virtual ~eThreadImpl();

	/**
	 * \brief user calls this function to start the thread execution
	 */
	void run(eThread *thread);
	
	// test the internal flag to see if a 'stop' request has been 
	// requested by caller
	bool testDestroy();
	
	// notify the running thread to termiante. note that this does not mean that the
	// thread terminates instantly
	void requestStop();
	
	// wait for thread to terminate (wait for timeout milliseconds
	void wait(long timeout);
	
	eThreadId getThreadId() {return m_tid;}
};

#endif // __ethread_unix_h__
#endif // __WXMSW__
