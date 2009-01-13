#if defined(__WXMSW__)

#include "ethread_win.h"
#include <process.h>
#include "ethread.h"

static unsigned __stdcall startFunc(void* arg)
{
	eThread *thread = reinterpret_cast<eThread*>(arg);
	if(thread) {
		// call the thread main loop
		thread->start();
	}
	
	// terminate the thread
	_endthreadex( 0 );

	return 0;
}

eThreadImpl::eThreadImpl()
: m_stopEvent(INVALID_HANDLE_VALUE)
, m_handle(INVALID_HANDLE_VALUE)
{
	m_stopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

eThreadImpl::~eThreadImpl()
{
	CloseHandle(m_stopEvent);
	m_stopEvent = INVALID_HANDLE_VALUE;
}

void eThreadImpl::run(eThread *thread)
{
	m_handle = (HANDLE)_beginthreadex
	           (
	               NULL,             // default security
	               0,				// default stack size
	               &startFunc, 		// entry point
	               thread,
	               0,
	               (unsigned int *)&m_tid
	           );
}

bool eThreadImpl::testDestroy()
{
	DWORD dwRet = WaitForSingleObject(m_stopEvent, 0);
	if(dwRet == WAIT_OBJECT_0) {
		// the event is signaled
		return true;
	}
	return false;
}

void eThreadImpl::requestStop()
{
	SetEvent(m_stopEvent);
}

void eThreadImpl::wait(long timeout)
{
	if(m_handle != INVALID_HANDLE_VALUE){
		if(WaitForSingleObject(m_handle, timeout) == WAIT_OBJECT_0){
			CloseHandle(m_handle);
			m_handle = INVALID_HANDLE_VALUE;
		}
	}
}

#endif // __WXMSW__

