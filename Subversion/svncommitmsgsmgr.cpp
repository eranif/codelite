//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svncommitmsgsmgr.cpp              
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
 #include "svncommitmsgsmgr.h"

SvnCommitMsgsMgr* SvnCommitMsgsMgr::ms_instance = 0;

SvnCommitMsgsMgr::SvnCommitMsgsMgr()
{
}

SvnCommitMsgsMgr::~SvnCommitMsgsMgr()
{
}

SvnCommitMsgsMgr* SvnCommitMsgsMgr::Instance()
{
	if(ms_instance == 0){
		ms_instance = new SvnCommitMsgsMgr();
	}
	return ms_instance;
}

void SvnCommitMsgsMgr::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

void SvnCommitMsgsMgr::AddMessage(const wxString &message)
{
	if(message.IsEmpty()) {
		//dont add empty comments
		return;
	}
	
	//dont add duplicate messages
	if ( m_messages.Index( message, false ) == wxNOT_FOUND ) {
		m_messages.Add(message);
	}
}

void SvnCommitMsgsMgr::GetAllMessages(wxArrayString &messages)
{
	messages = m_messages;
}
