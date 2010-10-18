//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : job.h              
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
 #ifndef __job__
#define __job__

#include <wx/event.h>
#include "codelite_exports.h"

extern WXDLLIMPEXP_SDK const wxEventType wxEVT_CMD_JOB_STATUS;
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_CMD_JOB_STATUS_VOID_PTR;

/**
 * \class Job
 * \author Eran
 * \date 05/09/08
 * \file job.h
 * \brief this class defines the interface to a job used by the JobQueue
 * To use the JobQueue, you should inherit from Job, and implement the Porcess() pure virtual function
 */
class WXDLLIMPEXP_SDK Job
{
protected:
	wxEvtHandler *m_parent;
	
public:
	/**
	 * @brief Construct Job object with optional parent event handler.
	 * @param parent event handler class which would like to receive notifications of progress
	 */
	Job(wxEvtHandler *parent = NULL);
	virtual ~Job();

public:
	/**
	 * @brief post string and int values to parent in a form of wxCommandEvent of type wxEVT_CMD_JOB_STATUS. the string can be accessed by using event.GetString() and the int
	 * by calling event.GetInt(). 
	 * this function has no affect if parnet is NULL
	 * @param i
	 * @param message
	 */
	void Post(int i, const wxString &message);
	/**
	 * @brief post void* to parent in a form of wxCommandEvent of type wxEVT_CMD_JOB_STATUS_VOID_PTR. the void* can be accessed by using event.GetClientData() method. 
	 * NB: User must free the void* 
	 * this function has no affect if parnet is NULL
	 * @param i integer to send to parent
	 */
	virtual void Post(void *ptr);
	/**
	 * @brief overridable Process() method. If the Process() method is performs a long computations, it is advised to 
	 * to call thread->TestDestroy() to allow the thread to exit when requested 
	 * @param thread the thread that is currently running the job. 
	 */
	virtual void Process(wxThread *thread) = 0;
};
#endif // __job__
