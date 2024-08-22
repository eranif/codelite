//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : stack_walker.h              
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
 #ifndef STACK_WALKER_H
#define STACK_WALKER_H

#include <wx/stackwalk.h>
#include <wx/string.h>

#if wxUSE_STACKWALKER
class wxTextOutputStream;
class StackWalker : public wxStackWalker
{
	wxTextOutputStream *m_output;
	
public:
	StackWalker(wxTextOutputStream *output);
	virtual ~StackWalker();
	void OnStackFrame(const wxStackFrame &frame);
};
#endif 

#endif //STACK_WALKER_H
