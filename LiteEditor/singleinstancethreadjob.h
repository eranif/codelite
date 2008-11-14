//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : singleinstancethreadjob.h              
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

#ifndef __singleinstancethreadjob__
#define __singleinstancethreadjob__

#include <wx/event.h>
#include "job.h"

extern const wxEventType wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES;
extern const wxEventType wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP;

class SingleInstanceThreadJob : public Job {
	wxString m_path;
	
protected:	
	void ProcessFile(const wxString &fileContent);
	
public:
	SingleInstanceThreadJob(wxEvtHandler *parent, const wxString &path);
	virtual ~SingleInstanceThreadJob();
	
public:
	virtual void Process(wxThread *thread);
};
#endif // __singleinstancethreadjob__
