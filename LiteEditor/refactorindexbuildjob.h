//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : refactorindexbuildjob.h              
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
 #ifndef __refactorindexbuildjob__
#define __refactorindexbuildjob__

#include "cpptoken.h"
#include <vector>
#include <wx/filename.h>
#include "job.h"

class RefactorIndexBuildJob : public Job
{
	std::vector<wxFileName> m_files;
	wxString m_word;
	
public:
	RefactorIndexBuildJob(const std::vector<wxFileName> &files, const wxChar* word);
	virtual ~RefactorIndexBuildJob();
	void Process(wxThread *thread);
	
public:
	void Parse(const wxString &word, CppTokensMap &l);
};
#endif // __refactorindexbuildjob__
