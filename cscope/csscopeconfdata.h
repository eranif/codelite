//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : csscopeconfdata.h              
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

#ifndef __csscopeconfdata__
#define __csscopeconfdata__

#include "serialized_object.h"
#include <wx/string.h>

#define SCOPE_ENTIRE_WORKSPACE wxT("Entire Workspace")
#define SCOPE_ACTIVE_PROJECT   wxT("Active Project")

class CSscopeConfData : public SerializedObject
{
	wxString m_scanScope;

public:
	CSscopeConfData();
	virtual ~CSscopeConfData();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	void SetScanScope(const wxString& scanScope) {
		this->m_scanScope = scanScope;
	}
	const wxString& GetScanScope() const {
		return m_scanScope;
	}
};
#endif // __csscopeconfdata__
