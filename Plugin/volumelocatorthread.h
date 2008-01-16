#ifdef __WXMSW__

#ifndef VOLUMELOCATORTHREAD_H
#define VOLUMELOCATORTHREAD_H

#include "wx/thread.h"
#include "wx/event.h"

#define wxEVT_THREAD_VOLUME_COMPLETED 3453

class VolumeLocatorThread : public wxThread
{
	wxEvtHandler *m_owner;
	
protected:
	void* Entry();
	
public:
	VolumeLocatorThread(wxEvtHandler *owner);
	~VolumeLocatorThread();
};

#endif //VOLUMELOCATORTHREAD_H
#endif //__WXMSW__



