//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : filehistory.h              
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
 #ifndef FILE_HISTORY_H
#define FILE_HISTORY_H

#include <wx/docview.h>

class FileHistory : public wxFileHistory
{
public:
	FileHistory();
	virtual ~FileHistory();
	
	/**
	 * \brief 
	 * \param files
	 */
	void GetFiles(wxArrayString &files);

	/**
	 * \brief add files to the designated menu, first removing any extension
	 */
	void AddFilesToMenuWithoutExt();

protected:
	void AddFilesToMenuWithoutExt(wxMenu* menu);
};
#endif //FILE_HISTORY_H
