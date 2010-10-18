//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : pluginsdata.h              
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
 #ifndef __pluginsdata__
#define __pluginsdata__

#include "plugindata.h"
#include "codelite_exports.h"
#include "map"

class WXDLLIMPEXP_SDK PluginsData : public SerializedObject
{
	std::map< wxString, PluginInfo > m_info;

public:
	PluginsData();
	virtual ~PluginsData();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	void SetInfo(const std::map<wxString, PluginInfo>& info) {
		this->m_info = info;
	}
	
	const std::map<wxString, PluginInfo>& GetInfo() const {
		return m_info;
	}
};

#endif // __pluginsdata__
