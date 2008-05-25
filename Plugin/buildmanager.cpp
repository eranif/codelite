//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : buildmanager.cpp              
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
 #include "buildmanager.h"
#include "builder.h"
#include "builder_gnumake.h"


BuildManager::BuildManager()
{
	// register all builders here
	AddBuilder(new BuilderGnuMake());
}

BuildManager::~BuildManager()
{
	m_builders.clear();
}

void BuildManager::AddBuilder(BuilderPtr builder)
{
	wxCriticalSectionLocker locker(m_cs);
	if(!builder){ 
		return;
	}

	m_builders[builder->GetName()] = builder;
}

void BuildManager::RemoveBuilder(const wxString &name)
{
	wxCriticalSectionLocker locker(m_cs);

	std::map<wxString, BuilderPtr>::iterator iter = m_builders.find(name);
	if(iter != m_builders.end()){
		m_builders.erase(iter);
	}
}

void BuildManager::GetBuilders(std::list<wxString> &list)
{
	wxCriticalSectionLocker locker(m_cs);

	std::map<wxString, BuilderPtr>::iterator iter = m_builders.begin();
	for(; iter != m_builders.end(); iter++){
		list.push_back(iter->second->GetName());
	}
}

BuilderPtr BuildManager::GetBuilder(const wxString &name)
{
	wxCriticalSectionLocker locker(m_cs);

	std::map<wxString, BuilderPtr>::iterator iter = m_builders.begin();
	for(; iter != m_builders.end(); iter++){
		if(iter->first == name){
			return iter->second;
		}
	}
	return NULL;
}

BuilderPtr BuildManager::GetSelectedBuilder()
{
	return m_builders.begin()->second;
}
