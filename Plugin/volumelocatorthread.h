//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : volumelocatorthread.h              
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



