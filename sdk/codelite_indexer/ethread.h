#ifndef __ethread_h__
#define __ethread_h__

#ifdef __WXMSW__
#	include "ethread_win.h"
#define eThreadSleep(x) Sleep(x)
#else 
#	include "ethread_unix.h"
#define eThreadSleep(x) usleep(x*1000)
#endif

/**
 * \class eThread
 * \author Eran
 * \date 08/10/08
 * \file ethread.h
 * \brief
 */
class eThread
{
	eThreadImpl *m_impl;
	
protected:
	/**
	 * \brief test the thread to see if it was required to stop. User should call this method inside the thread 
	 * main loop (eThread::start())  
	 * \return true if the stop() method has been called by the caller
	 */
	bool testDestroy() {
		return m_impl->testDestroy();
	}

public:
	eThread(){
		m_impl = new eThreadImpl();
	}

	virtual ~eThread(){
		delete m_impl;
	}

	/**
	 * \brief user should call this function to start the thread execution
	 */
	void run() {
		m_impl->run(this);
	}
	
	void requestStop() {
		m_impl->requestStop();
	}
	
	void wait(long timeout) {
		m_impl->wait(timeout);
	}
	
	eThreadId getThreadId() {
		return m_impl->getThreadId();
	}
	
	/**
	 * \brief the thread entry point
	 */
	virtual void start() = 0;

};

#endif //__ethread_h__
