//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : context_manager.cpp              
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
 #include "context_manager.h"
#include "context_cpp.h"
#include "context_text.h"
#include "context_base.h"
#include "generic_context.h"
#include "editor_config.h"
 
ContextManager::ContextManager()
{
	// register available context
	m_contextPool[wxT("C++")] = ContextBasePtr( new ContextCpp() );
	m_contextPool[wxT("Text")] = ContextBasePtr( new ContextText() );

	// load generic lexers
	EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
	 for(; iter != EditorConfigST::Get()->LexerEnd(); iter++){
		LexerConfPtr lex = iter->second;
		//skip hardcoded lexers
		if(lex->GetName() != wxT("C++") && lex->GetName() != wxT("Text")){
			std::pair<wxString, ContextBasePtr> entry(lex->GetName(), new ContextGeneric(lex->GetName()));
			m_contextPool.insert(entry);
		}
	}
}

ContextManager::~ContextManager()
{
}

ContextBasePtr ContextManager::NewContext(wxWindow *parent, const wxString &lexerName)
{
	// this function is actually a big switch ....
	std::map<wxString, ContextBasePtr>::iterator iter = m_contextPool.find(lexerName);
	if( iter == m_contextPool.end()){
		return m_contextPool[wxT("Text")]->NewInstance((LEditor*)parent);
	}

	return iter->second->NewInstance((LEditor*)parent);
}
