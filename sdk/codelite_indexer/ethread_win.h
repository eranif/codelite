#if defined(__WXMSW__)

#ifndef __ethread_win_h__
#define __ethread_win_h__

#include <windows.h>

typedef unsigned int eThreadId;

class eThread;

/**
 * \class eThreadImpl
 * \author Eran
 * \date 08/10/08
 * \file ethread.h
 * \brief Windows implemenation
 */
class eThreadImpl
{
	HANDLE m_stopEvent;
	HANDLE m_handle;

	eThreadId m_tid;

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

#endif // __ethread_win_h__
#endif // __WXMSW__

