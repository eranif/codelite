//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : exelocator.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #include "exelocator.h"
#include "procutils.h" 

bool ExeLocator::Locate(const wxString &name, wxString &where)
{
	wxString command;
	wxArrayString output;
	command << wxT("which \"") << name << wxT("\"");
	ProcUtils::ExecuteCommand(command, output);
	
	if(output.IsEmpty() == false){
		wxString interstingLine = output.Item(0);
		
		if(interstingLine.Trim().Trim(false).IsEmpty()){
			return false;
		}
		
		if(	!interstingLine.StartsWith(wxT("which: no ")) &&
			!interstingLine.Contains(wxT("command not found")) &&
			!interstingLine.StartsWith(wxT("no "))){
			where = output.Item(0);
			where = where.Trim().Trim(false);
			return true;
		}
	}
	return false;
}
