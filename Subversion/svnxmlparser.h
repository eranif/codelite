//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svnxmlparser.h              
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
 #ifndef SVNXMLPARSER_H
#define SVNXMLPARSER_H

#include "wx/string.h"
#include "wx/arrstr.h"

class SvnXmlParser 
{
public:
	enum FileState{
		//OK
		StateOK				= 0x00000000,
		
		//wc-status: conflict
		StateConflict		= 0x00000001,		

		//any state that is considered as modified, such:
		//wc-status: deleted, modified, added, merged
		StateModified		= 0x00000002,

		//wc-status: unversioned
		StateUnversioned	= 0x00000004
	};

public:
	SvnXmlParser();
	~SvnXmlParser();

	static void GetFiles(const wxString &input, wxArrayString &files, FileState state = StateModified);
	static wxString GetRevision(const wxString& input);
};

#endif //SVNXMLPARSER_H

