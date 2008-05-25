//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svncommitmsgsmgr.h              
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
 #ifndef __svncommitmsgsmgr__
#define __svncommitmsgsmgr__
#include "wx/arrstr.h"

class SvnCommitMsgsMgr {

	static SvnCommitMsgsMgr* ms_instance;
	wxArrayString m_messages;
	
public:
	static SvnCommitMsgsMgr* Instance();
	static void Release();

private:
	SvnCommitMsgsMgr();
	~SvnCommitMsgsMgr();
	
public:
	void AddMessage(const wxString &message);
	void GetAllMessages(wxArrayString &messages);
	
};
#endif // __svncommitmsgsmgr__
