//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : fileutils.cpp              
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
 #include "fileutils.h"
#include <wx/ffile.h> 
#include "wx/string.h"
#include <wx/strconv.h>

bool FileUtils::ReadFileUTF8(const wxFileName &fn, wxString &data)
{
	wxFFile file(fn.GetFullPath().GetData(), wxT("rb"));
	if(file.IsOpened() == false)
	{
		// Nothing to be done
		return false;
	}
	
	if( file.Length() <= 0){
		return false;
	}

	if( file.Length() > 0 ){
		data.Alloc(file.Length());
	}

	//read first two bytes to check if unicode detected
	bool isUnic(false);
	unsigned char bom[2];
	if(file.Length() >= 2){
		if(file.Read(bom, 2) == 2){
			if(bom[0] == 0xFE && bom[1] == 0xFF){
				//UTF16
				isUnic = true;
			}
		}
	}

	size_t len(file.Length());
	wxFileOffset off(0);
	if( isUnic ){
		//seek file to start to read the consumed two bytes
		len -= 2;
		off = 2;
	}

	file.Seek(off);

	char *pdata = new char[len + 1];
	file.Read(pdata, len);
	pdata[len] = 0;

	data = wxString::FromAscii(pdata);
	file.Close();
	delete [] pdata;
    return true;
}
